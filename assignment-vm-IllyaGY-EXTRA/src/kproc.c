/**
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 *
 * Kernel Process Handling
 */

#include <spede/stdio.h>
#include <spede/string.h>
#include <spede/machine/proc_reg.h>

#include "kernel.h"
#include "trapframe.h"
#include "kproc.h"
#include "scheduler.h"
#include "timer.h"
#include "queue.h"
#include "vga.h"
#include "kalloc.h"
#include "spede.h"

// Next available process id to be assigned
int next_pid;

// Process table allocator
queue_t proc_allocator;

proc_t proc_table[PROC_MAX];             /* Define process table */
unsigned char proc_stack[PROC_MAX][PROC_STACK_SIZE]; /* Define process stacks */

/**
 * Looks up a process in the process table via the process id
 * @param pid - process id
 * @return pointer to the process entry, NULL or error or if not found
 */
proc_t *pid_to_proc(int pid) { 
    // Iterate through the process table and return a pointer to the valid entry where the process id matches
    for(int process_entry = 0; process_entry < PROC_MAX; process_entry++){
        // i.e. if proc_table[8].pid == pid, return pointer to proc_table[8]
        if(pid == proc_table[process_entry].pid){
            return &proc_table[process_entry];
        }
    
    // Ensure that the process control block actually refers to a valid process
    }
    return NULL;
}

/**
 * Translates a process pointer to the entry index into the process table
 * @param proc - pointer to a process entry
 * @return the index into the process table, -1 on error
 */
int proc_to_entry(proc_t *proc) {
    // For a given process entry pointer, return the entry/index into the process table
    if(proc == NULL) return -1; 
    //  i.e. if proc -> proc_table[3], return 3
    for(int process_entry = 0; process_entry < PROC_MAX; process_entry++){
        // i.e. if proc_table[8].pid == pid, return pointer to proc_table[8]
        if(proc == &proc_table[process_entry]){
            return process_entry;
        }
    
    // Ensure that the process control block actually refers to a valid process
    }
    return -1;
}

/**
 * Returns a pointer to the given process entry
 */
proc_t * entry_to_proc(int entry) {
    // For the given entry number, return a pointer to the process table entry
    // Ensure that the process control block actually refers to a valid process
    if(entry >= PROC_MAX || entry < 0) 
        return NULL; 

    return &proc_table[entry]; 
}

/**
 * Creates a new process
 * @param proc_ptr - address of process to execute
 * @param proc_name - "friendly" process name
 * @param proc_type - process type (kernel or user)
 * @return process id of the created process, -1 on error
 */
int kproc_create(void *proc_ptr, char *proc_name, proc_type_t proc_type) {
    proc_t *proc; 

    int proc_entry = 0; 
    // Allocate an entry in the process table via the process allocator
    if(queue_out(&proc_allocator, &proc_entry) == -1){
        kernel_log_info("Error");
        return -1;
    }


    // Initialize the process control block
    proc = &proc_table[proc_entry];
    memset(proc, 0, sizeof(proc_t));

    // Initialize the process stack via proc_stack
    proc->stack = proc_stack[next_pid]; 
    memset(proc->stack, 0, sizeof(proc->stack));

    // Initialize the trapframe pointer at the bottom of the stack
    proc->trapframe = (trapframe_t *)(&proc->stack[PROC_STACK_SIZE - sizeof(trapframe_t)]);

    // Set each of the process control block structure members to the initial starting values
    // as each new process is created, increment next_pid
    // proc->pid, state, type, run_time, cpu_time, start_time, etc.
    proc->pid = next_pid; 
    proc->state = IDLE; 
    proc->type = proc_type;
    proc->start_time = timer_get_ticks();  
    proc->run_time = 0; 
    proc->cpu_time = 0; 
   

    next_pid++;
    // Copy the passed-in name to the name buffer in the process control block
    strcpy(proc->name, proc_name);

    // Set the instruction pointer in the trapframe
    proc->trapframe->eip = (unsigned int)proc_ptr;

    // Set INTR flag
    proc->trapframe->eflags = EF_DEFAULT_VALUE | EF_INTR;

    if (proc->type != PROC_TYPE_USER){
        // Set each segment in the trapframe
        proc->trapframe->cs = get_cs();
        proc->trapframe->ds = get_ds();
        proc->trapframe->es = get_es();
        proc->trapframe->fs = get_fs();
        proc->trapframe->gs = get_gs();

        proc->pagetable = (unsigned int*)kernel_cr3;
    }
    else {
        trapframe_t *trapframe = proc->trapframe;
        trapframe->ds = 0x3B;
        trapframe->gs = 0x3B;
        trapframe->fs = 0x3B;
        trapframe->es = 0x3B;
        trapframe->user_ss = 0x43; 
        trapframe->eflags = get_eflags() | EF_INTR;
        trapframe->cs = 0x33;
        trapframe->esp = (unsigned int)proc->stack;
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

       //I use kpage_add just in case I have time to do part
        size_t *page_directory = (size_t*)kpagemake(); 
        kpage_add(proc->pg_lst, page_directory);

        size_t *stack_table = (size_t*)kpagemake(); 
        kpage_add(proc->pg_lst, stack_table);

        size_t *stack_page = (size_t*)kpagemake(); 
        kpage_add(proc->pg_lst, stack_page);

        size_t *code_table = (size_t*)kpagemake();
        kpage_add(proc->pg_lst, code_table); 
        
        size_t *code_page = (size_t*)kpagemake(); 
        kpage_add(proc->pg_lst, code_page);

        size_t *kernel_MMU = (size_t*)kernel_cr3; 

        // Copy the first four entries from the kernel page directory
        for(int i = 0; i < 5; i++){
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
        memcpy((void *)code_page, uproc_test, 512);

        // Assign the page directory to the process
        proc->pagetable = (unsigned int *)page_directory; 
    }
    // Add the process to the scheduler
    scheduler_add(proc);

    kernel_log_info("Created process %s (%d) entry=%d", proc->name, proc->pid, -1);
   
    return proc_table[proc_entry].pid;
}

/**
 * Destroys a process
 * If the process is currently scheduled it must be unscheduled
 * @param proc - process control block
 * @return 0 on success, -1 on error
 */
int kproc_destroy(proc_t *proc) {

    if(proc->pid == 0)
        return -1;
    // Here you got error, when you remove proc from scheduler
    // we will memset proc_t as 0, which means we can't collect page from pg_list because pg_list is zero
    //
    
    int entry;
    if((entry = proc_to_entry(proc)) == -1)
        return -1;
    // Collecting the page we allocated for user process.
    if(proc->type == PROC_TYPE_USER){
        /* We need to switch pagetable to kernel pagetable*/
        set_cr3(kernel_cr3);
        for(int i = 0; i < 10; i++){
            if(proc->pg_lst[i] > 0){
                manage_page_del((void*)(proc->pg_lst[i]));
                proc->pg_lst[i] = 0;
            }
        }
    }
    
    memset(&proc_stack[entry], 0, sizeof(proc_stack[entry]));
    
    // Add the process entry/index value back into the process allocator
    queue_in(&proc_allocator, entry);
    
    // Remove the process from the scheduler
    scheduler_remove(proc);

    //This is to make the kernel process free all the pages from the removed process.
    active_proc = &proc_table[0];
    active_proc->state = ACTIVE; 

    return 0;
}

/**
 * Idle Process
 */
void kproc_idle(void) {
    while (1) {
        // Ensure interrupts are enabled
        asm("sti");

        int i = 0; 
        while(i < 10){
            if(page_delete[i]){
                printf("\nFreeing memory 0x0%x\n", (void *)page_delete[i]);
                kfree((void *)page_delete[i]); 
                page_delete[i] = 0; 
                i++; 
            }
            else break; 
            
        } 

        // Halt the CPU
        asm("hlt");
    }
}


/**
* Test User Process
*/
void uproc_test(void){
    uint32_t *x = (uint32_t *)0x7FFF0000 ;
    while (1){
        for(uint32_t i = 0; i < 16660000; i++)
            asm("inb $0x80");
        *x = 20;
        // I limit the page usage for each process for similifying test.
        if ((int)x > 0x7FFC0000)
            x = (uint32_t *)((uint32_t)x - 0x4000);
        else
            x = (uint32_t *)0x7FFF0000;
    }
}

/**
 * Test process
 */
void kproc_test(void) {
    // Loop forever
    while (1);
}


/**
 * Initializes all process related data structures
 * Creates the first process (kernel_idle)
 * Registers the callback to display the process table/status
 */
void kproc_init(void) {
    kernel_log_info("Initializing process management");

    // Initialize all data structures and variables
    //   - process table
    memset(proc_table, 0, sizeof(proc_table));
    //   - process stack
    memset(proc_stack, 0, sizeof(proc_stack));
    

    //   - process allocator
    queue_init(&proc_allocator); 
    for (int i = 0; i < PROC_MAX; i++) {
        if (queue_in(&proc_allocator, i) != 0) {
            kernel_log_warn("Unable to allocate a proccess %d", i);
        }
    }


    

    //First available pid
    next_pid = 0; 
    
    
    // Create the idle process (kproc_idle) as a kernel process
    kproc_create(&kproc_idle, "idle", PROC_TYPE_KERNEL);

    

   
}

