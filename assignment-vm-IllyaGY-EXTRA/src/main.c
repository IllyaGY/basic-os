/**
 * CPE/CSC 159 Operating System Pragmatics
 * California State University, Sacramento
 *
 * Operating system entry point
 */


#include <spede/stdbool.h>
#include "spede.h"
#include "interrupts.h"
#include "kernel.h"
#include "keyboard.h"
#include "timer.h"
#include "tty.h"
#include "vga.h"
#include "scheduler.h"
#include "kproc.h"
#include "test.h"
#include "handlers.h"

int main(void) {
    // Always iniialize the kernel
    kernel_init();

    // Initialize interrupts
    interrupts_init();
    
    interrupts_irq_register(0xE,isr_entry_page_fault, PageFaultHandler);
    interrupts_irq_register(0xD,isr_entry_global_protection,GeneralProtection);   

    
    // Initialize timers
    timer_init();    

    // Initialize the TTY
    tty_init();

    // Initialize the VGA driver
    vga_init();

    // Initialize the keyboard driver
    keyboard_init();

    // Initialize processes
    kproc_init();

    // Initialize the scheduler
    scheduler_init();
    
    // Test initialization
    test_init();

    // //Print a welcome message
    // vga_printf("Welcome to %s!\n", OS_NAME);
    // vga_puts("Press a key to continue...\n");

    // // Wait for a key to be pressed
    // keyboard_getc();

    // // Clear the screen
    // vga_clear();

    // Enable interrupts
    interrupts_enable();


    // Switch to idle process
    active_proc = &proc_table[0];
    kernel_tss.tss_esp0 = (unsigned)&proc_stack[0][PROC_STACK_SIZE];
    kernel_context_exit(active_proc->trapframe);

    //while(1); 

    // Should never get here!
    return 0;
}
