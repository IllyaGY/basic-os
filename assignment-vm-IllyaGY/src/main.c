/* 
 * main.c -- Page Management 
 */
#include "spede.h"
#include "events.h"
#include "process.h"
#include "handlers.h"
#include "types.h"
#include "kalloc.h"

struct i386_gate *IDT_p; 
struct i386_tss kernel_tss;         /*Kernel TSS */
struct i386_descriptor *GDT_p = NULL;      /*pointer to global descriptor table*/

proc_t p[10];
char proc_kernel_stack[10][STACK_SIZE];
// Current running process
proc_t *active_process = NULL; 

int main(){
    struct pseudo_descriptor pseudo_desc;  /*used to get base addr of GDT*/
    get_gdt(&pseudo_desc);
    GDT_p = (struct i386_descriptor *)pseudo_desc.linear_base;

    if (GDT_p == NULL)
    {   
        cons_printf("Kernel Panic: GDT table is null!\n");
        return 0;
    }
    /* Build Kernel TSS */ 
    kernel_tss.tss_ss0 = 0x18;
    kernel_tss.tss_cs = 0x8;
    kernel_tss.tss_ds = 0x10;
    kernel_tss.tss_es = 0x10;
    kernel_tss.tss_fs = 0x10;
    kernel_tss.tss_ss = 0x18;
    
    /* Global Descriptor Table in Spede 
     * 0. NULL
     * 1. Kernel Code   0x08
     * 2. Kernel Data   0x10
     * 3. Kernel Stack  0x18
     * 4. Unknown       0x20
     * 5. Unknown       0x28 
     * 6. User Code     0x30
     * 7. User Data     0x38
     * 8. User Stack    0x40
     * */
    /* Add Kernel TSS to entry 9 */
    fill_descriptor(&GDT_p[9], (unsigned)&kernel_tss, sizeof(kernel_tss), TSS_PRESENT | ACC_TSS, 0x0); 
    KernelTssFlush(); 


    printf("initialized kalloc!\n");
    //kinit(page_address, (unsigned int)page_address + 20 * PAGE_SIZE);
    kinit(PAGE_START, PAGE_STOP);
    
    // Set up interrupt for Page Fault
    IDT_p = get_idt_base(); // get IDT location
    /* Register Page Fault to interrupt table*/
    fill_gate(&IDT_p[14], (int)PageFaultEntry, get_cs(), ACC_INTR_GATE, 0);
    /* Register PiC Timer to interrupt table: 0x20 */
    fill_gate(&IDT_p[32], (int)TimerEntry, get_cs(), ACC_INTR_GATE, 0);
    /* Register System Call to interrupt table*/
    fill_gate(&IDT_p[128], (int)SyscallEntry, get_cs(), ACC_INTR_GATE | ACC_PL_U, 0); 

    NewKernelProcHandler(SystemProc);
    NewUserProcHandler(&Process1, &Process1_code_end);
    NewUserProcHandler(&Process2, &Process2_code_end);
 
    /* Make the kernel process as the first process */
    active_process = &p[0];
    kernel_tss.tss_esp0 = (unsigned)&proc_kernel_stack[0][STACK_SIZE];
    ProcLoader(active_process->trapframe);    
    return 0;
}

void context_switch(trapframe_t *current) {
    if (current->event_type == 0x80){
        SyscallHandler(); 
    } 
    else if (current->event_type == 0xD){
        PageFaultHandler();
    }
    // dismiss timer event (IRQ 0), otherwise, new event from timer won't be recognized by CPU
	// because hardware uses edge-trigger flipflop
    outportb(0x20, 0x60);
    
    active_process->trapframe = current;
    if (active_process == &p[0]){
        // kernel is running
        active_process = &p[1];
    } else {
        // Iteratively switch process 1 and process 2
        if (active_process == &p[1]) 
            active_process = &p[2];
        else
            active_process = &p[0];
    }
   
    kernel_tss.tss_esp0 = (unsigned)&proc_kernel_stack[active_process->pid][STACK_SIZE];
    set_cr3((unsigned int)active_process->pagetable);
    ProcLoader(active_process->trapframe);
}
