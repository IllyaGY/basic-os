#include "spede.h"
#include "types.h"
#include "kalloc.h"

size_t uniq_id;

void NewUserProcHandler(void *func, void *func_end) {
    proc_t *proc = &p[uniq_id];
    proc->proc_type = PROC_TYPE_USER;
    proc->pid = uniq_id;
    uniq_id++;

    /*Initialize trapframe*/
    proc->trapframe = (trapframe_t *)((unsigned)&proc_kernel_stack[proc->pid][STACK_SIZE] - sizeof(trapframe_t));
    trapframe_t *trapframe = proc->trapframe;
    trapframe->ds = 0x3B;
    trapframe->gs = 0x3B;
    trapframe->fs = 0x3B;
    trapframe->es = 0x3B;
    trapframe->user_ss = 0x43; 
    trapframe->eflags = get_eflags() | EF_INTR;
    trapframe->cs = 0x33;
    trapframe->esp = (unsigned)proc_kernel_stack[proc->pid];
    trapframe->ebp = trapframe->esp;
    trapframe->eax = trapframe->esp;

    /* You need to complete the following part:*/

    /* User-mode stack segment points to virtual address 2 GB -sizeof (TF) */
    trapframe->user_esp = 0x80000000 - sizeof(trapframe_t);

    /*1 GB virtual address for text section*/
    trapframe->eip = 0x40000000; 

    // Setup page table
    /* We need five page for each process
     * page 1: page directory
     * page 2: page table for stack 
     * page 3: stack page for process
     * page 4: page table for code 
     * page 5: code page for process
     * */
    size_t *page_directory = (size_t*)kpagemake(); 
    size_t *stack_table = (size_t*)kpagemake(); 
    size_t *stack_page = (size_t*)kpagemake(); 
    size_t *code_table = (size_t*)kpagemake(); 
    size_t *code_page = (size_t*)kpagemake(); 
    
    size_t *kernel_MMU = (size_t*)get_cr3(); 

    // Copy the first four entries from the kernel page directory
    for(int i = 0; i < 4; i++){
        page_directory[i] = kernel_MMU[i]; 
    }


    // Map stack (2GB virtual address)
    page_directory[511] = (size_t)stack_table | PAGE_PRESENT | PAGE_USER | PAGE_WRITE;
    stack_table[1023] = (size_t)stack_page | PAGE_PRESENT | PAGE_USER | PAGE_WRITE; 

    // Copy trapframe to the stack page
    memcpy((void *)stack_page + PAGE_SIZE - sizeof(trapframe_t), trapframe, sizeof(trapframe_t));

    // Map code (1GB virtual address)
    page_directory[256] = (size_t)code_table | PAGE_PRESENT | PAGE_USER | PAGE_WRITE; 
    code_table[0] = (size_t)code_page | PAGE_PRESENT | PAGE_USER | PAGE_WRITE; 

    // Copy user process code to the code page
    memcpy((void *)code_page, func, (size_t)func_end - (size_t)func);

    // Assign the page directory to the process
    proc->pagetable = page_directory; 
}

/* Create a new kernel process 
* @param: func
* Return: None
*/
void NewKernelProcHandler(void *func) {
    proc_t *proc = &p[uniq_id];    
    proc->proc_type = PROC_TYPE_KERNEL;
    proc->pid = uniq_id;
    uniq_id++;

    proc->trapframe = (trapframe_t *)((unsigned)&proc_kernel_stack[proc->pid][STACK_SIZE] - sizeof(trapframe_t));
    proc->trapframe->ds = get_ds();
    proc->trapframe->es = get_es();
    proc->trapframe->fs = get_fs();
    proc->trapframe->gs = get_gs();
    proc->trapframe->eflags = get_eflags() | EF_INTR;
    proc->trapframe->cs = get_cs();
    proc->trapframe->eip = (unsigned)func;
    /* Get kernel mmu page directory address */
    proc->pagetable = (size_t *)get_cr3();
}

void PageFaultHandler(void){
    /* Get the Error Code*/
    size_t error_code = active_process->trapframe->error_code;
    
    /* Page Fault occurred in ring 3: User Mode*/
    if (error_code & PF_ERR_USER_SUP_BIT)
    {
        /* Page Fault caused by protection violation*/
        if (error_code & PF_ERR_PRES_BIT){
            cons_printf("Kernel Panic in PageFault Handler: PageFault occurred in protection violation by process[%d]\n", active_process->pid); 
            return; 
        } else {
            /* Process Page Fault */    
            size_t va; /* virtual address*/
            asm volatile ("mov %%cr2, %0" : "=r"(va)); /* Get the Virtual Address */

            printf("Page Fault: Error Code[0x%x], VA[0x%x], Process %d\n", error_code, va, active_process->pid);
            
            /* handle page fault at here*/
            
            //entry page into page directory
            size_t pd_entry = (va >> 22); 

            //page table entry
            size_t pt_entry = (va >> 12) & 0x03FF; //Get the middle 10 bits 
            
            size_t *page_directory = active_process->pagetable; 
            size_t *page_table;
            if(!page_directory[pd_entry]){
                page_table = (size_t*)kpagemake(); 
                page_directory[pd_entry] = (size_t)page_table | PAGE_PRESENT | PAGE_USER | PAGE_WRITE; 
            } 
            else{
                page_table = (size_t*)(page_directory[pd_entry] & 0xFFFFF000); 
            } 
            page_table[pt_entry] = kpagemake() | PAGE_PRESENT | PAGE_USER | PAGE_WRITE;

        }
    } 
    /* Page Fault occurred in ring 0: Protected mode*/
    else 
    { 
        cons_printf("Kernel Panic in PageFault Handler: PageFault occurred in ring 0 by process[%d]\n", active_process->pid); 
        return;
    }
}

int ksyscall_printf(char *str){
    cons_printf(str);
    printf(str);
    return 0;
}
/* System call handler function*/
void SyscallHandler(void){
    printf("*********************This is the syscall function\n");
    /* Default return value */
    int rc = -1;
    int syscall;

    unsigned int arg1;

    if (!active_process) {
        cons_printf("Kernel Panic: No active process");
        return;
    }
    if(!active_process->trapframe){
        cons_printf("Kernel Panic: invalid trapframe");
        return;
    }

    syscall = active_process->trapframe->eax;
    arg1 = active_process->trapframe->ebx;

    switch(syscall) {
        case SYSCALL_PRINT:
            rc = ksyscall_printf((char *)arg1);
            break;
        default:
            cons_printf("Invalid system call %d\n", syscall);
    }
    if (active_process) {
        active_process->trapframe->eax = (unsigned int)rc;
    }
}
