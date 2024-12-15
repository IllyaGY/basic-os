#include "spede.h"
#include "types.h"
#include "kalloc.h"
#include "kernel.h"
#include "kproc.h"
#include "trapframe.h"



void PageFaultHandler(void){
    /* Get the Error Code*/
    size_t error_code = active_proc->trapframe->error_code;
    
    /* Page Fault occurred in ring 3: User Mode*/
    if (error_code & PF_ERR_USER_SUP_BIT)
    {
        /* Page Fault caused by protection violation*/
        if (error_code & PF_ERR_PRES_BIT){
            cons_printf("Kernel Panic in PageFault Handler: PageFault occurred in protection violation by process[%d]\n", active_proc->pid); 
            return; 
        } else {
            /* Process Page Fault */    
            size_t va; /* virtual address*/
            asm volatile ("mov %%cr2, %0" : "=r"(va)); /* Get the Virtual Address */

            printf("\nPage Fault: Error Code[0x%x], VA[0x%x], Process %d\n", error_code, va, active_proc->pid);
            
            /* handle page fault at here*/
            
            //entry page into page directory
            size_t pd_entry = (va >> 22); 

            //page table entry
            size_t pt_entry = (va >> 12) & 0x03FF; //Get the middle 10 bits 
            
            size_t *page_directory = active_proc->pagetable; 
            size_t *page_table;
            if(!(page_directory[pd_entry] & PAGE_PRESENT ) ){
                page_table = (size_t*)kpagemake(); 
                page_directory[pd_entry] = (size_t)page_table | PAGE_PRESENT | PAGE_USER | PAGE_WRITE; 
                printf("Creating a new page_table");
            } 
            else{
                page_table = (size_t*)(page_directory[pd_entry] & 0xFFFFF000); 
                printf("Using old page at 0x%x\n", page_table);
            }
            if (!(page_table[pt_entry] & PAGE_PRESENT) ){
                size_t new_page = (size_t)kpagemake(); 
                page_table[pt_entry] = new_page | PAGE_PRESENT | PAGE_USER | PAGE_WRITE;
                // When we allocate a new page to a process, we put this page to process->pg_list table. Once we destroy the process, we still can collect these page. 
                //Add the new page to the used_page_list
                
                if(new_page != 0) 
                    manage_page((void *)new_page, va);
                printf("Page is placed at 0x%x\n", new_page);
            } {
                printf("Page is placed at memory, need to refresh TLB\n");
            }

            //ISSUE: Crashes when a newly freed page is given to a process, trapframe seems corrupted
            set_cr3((unsigned int)active_proc->pagetable);
            //So I decided to pass in the same trapframe which seemed to have fixed the issue, 
            //however it seems more like a crutch rather than a normal solution
            kernel_context_exit(active_proc->trapframe);
 
        }
    } 
    /* Page Fault occurred in ring 0: Protected mode*/
    else 
    { 
        cons_printf("Kernel Panic in PageFault Handler: PageFault occurred in ring 0 by process[%d]\n", active_proc->pid); 
        return;
    }
}

void GeneralProtection(void){
    unsigned int error_code = active_proc->trapframe->error_code;
    if (error_code == 0) 
        printf("%d: general Protection violation\n", active_proc->pid);
        
    else 
        printf("%d: general protection fault 0x%x\n" ,active_proc->pid,error_code);
    
}
