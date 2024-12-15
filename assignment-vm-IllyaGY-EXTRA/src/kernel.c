/**
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 *
 * Kernel functions
 */
#include <spede/flames.h>
#include <spede/stdarg.h>
#include <spede/stdio.h>
#include <spede/string.h>

#include "spede.h"
#include "interrupts.h"
#include "kernel.h"
#include "scheduler.h"
#include "trapframe.h"
#include "vga.h"
#include "kalloc.h"
#include "handlers.h"
 
#ifndef KERNEL_LOG_LEVEL_DEFAULT
#define KERNEL_LOG_LEVEL_DEFAULT KERNEL_LOG_LEVEL_DEBUG
#endif

// Global pointer to the current active process entry
proc_t *active_proc = NULL;


struct i386_descriptor *GDT_p;      /*pointer to global descriptor table*/
struct i386_tss kernel_tss;              /* Kernel TSS definition */
unsigned int kernel_cr3; 
char global_page_table [100][4096]; 
page_t used_pages_list[GLOBAL_PAGE_LIST];
size_t page_delete[10]; 

// Current log level
int kernel_log_level = KERNEL_LOG_LEVEL_DEFAULT;

/**
 * Initializes any kernel internal data structures and variables
 */
void kernel_init(void) {
    // Display a welcome message on the host
    kernel_log_info("Welcome to %s!", OS_NAME);

    kernel_log_info("Initializing kernel...");

    //kinit(page_address, (unsigned int)page_address + 20 * PAGE_SIZE); it should be 100 since we ve got 100 pages
    kinit(global_page_table, (unsigned int)global_page_table + 100 * PAGE_SIZE);

    struct pseudo_descriptor pseudo_desc;  /*used to get base addr of GDT*/
    get_gdt(&pseudo_desc);
    GDT_p = (struct i386_descriptor *)pseudo_desc.linear_base;

    if (GDT_p == NULL)
    {   
        cons_printf("Kernel Panic: GDT table is null!\n");
        return;
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
    kernel_cr3 = get_cr3(); 

    printf("initialized kalloc!\n");
    

    memset(used_pages_list, 0, sizeof(used_pages_list));
    memset(page_delete, 0, sizeof(page_delete));
    
    
}

/**
 * Prints a kernel log message to the host with an error log level
 *
 * @param msg - string format for the message to be displayed
 * @param ... - variable arguments to pass in to the string format
 */
void kernel_log_error(char *msg, ...) {
    // Return if our log level is less than error
    if (kernel_log_level < KERNEL_LOG_LEVEL_ERROR) {
        return;
    }

    va_list args;

    printf("error: ");

    va_start(args, msg);
    vprintf(msg, args);
    va_end(args);

    printf("\n");
}

/**
 * Prints a kernel log message to the host with a warning log level
 *
 * @param msg - string format for the message to be displayed
 * @param ... - variable arguments to pass in to the string format
 */
void kernel_log_warn(char *msg, ...) {
    // Return if our log level is less than warn
    if (kernel_log_level < KERNEL_LOG_LEVEL_WARN) {
        return;
    }

    va_list args;

    printf("warn: ");

    va_start(args, msg);
    vprintf(msg, args);
    va_end(args);

    printf("\n");
}

/**
 * Prints a kernel log message to the host with an info log level
 *
 * @param msg - string format for the message to be displayed
 * @param ... - variable arguments to pass in to the string format
 */
void kernel_log_info(char *msg, ...) {
    // Return if our log level is less than info
    if (kernel_log_level < KERNEL_LOG_LEVEL_INFO) {
        return;
    }

    // Obtain the list of variable arguments
    va_list args;

    // Indicate this is an 'info' type of message
    printf("info: ");

    // Pass the message variable arguments to vprintf
    va_start(args, msg);
    vprintf(msg, args);
    va_end(args);

    printf("\n");
}

/**
 * Prints a kernel log message to the host with a debug log level
 *
 * @param msg - string format for the message to be displayed
 * @param ... - variable arguments to pass in to the string format
 */
void kernel_log_debug(char *msg, ...) {
    // Return if our log level is less than debug
    if (kernel_log_level < KERNEL_LOG_LEVEL_DEBUG) {
        return;
    }

    va_list args;

    printf("debug: ");

    va_start(args, msg);
    vprintf(msg, args);
    va_end(args);

    printf("\n");
}

/**
 * Prints a kernel log message to the host with a trace log level
 *
 * @param msg - string format for the message to be displayed
 * @param ... - variable arguments to pass in to the string format
 */
void kernel_log_trace(char *msg, ...) {
    // Return if our log level is less than trace
    if (kernel_log_level < KERNEL_LOG_LEVEL_TRACE) {
        return;
    }

    va_list args;

    printf("trace: ");

    va_start(args, msg);
    vprintf(msg, args);
    va_end(args);

    printf("\n");
}

/**
 * Triggers a kernel panic that does the following:
 *   - Displays a panic message on the host console
 *   - Triggers a breakpiont (if running through GDB)
 *   - aborts/exits the operating system program
 *
 * @param msg - string format for the message to be displayed
 * @param ... - variable arguments to pass in to the string format
 */
void kernel_panic(char *msg, ...) {
    va_list args;

    printf("panic: ");

    va_start(args, msg);
    vprintf(msg, args);
    va_end(args);

    printf("\n");

    breakpoint();
    exit(1);
}

/**
 * Returns the current log level
 * @return the kernel log level
 */
int kernel_get_log_level(void) {
    return kernel_log_level;
}

/**
 * Sets the new log level and returns the value set
 * @param level - the log level to set
 * @return the kernel log level
 */
int kernel_set_log_level(int level) {
    if (level < KERNEL_LOG_LEVEL_NONE) {
        kernel_log_level = KERNEL_LOG_LEVEL_NONE;
    } else if (level > KERNEL_LOG_LEVEL_ALL) {
        kernel_log_level = KERNEL_LOG_LEVEL_ALL;
    } else {
        kernel_log_level = level;
    }

    kernel_log_info("kernel log level set to %d", kernel_log_level);

    return kernel_log_level;
}

/**
 * Exits the kernel
 */
void kernel_exit(void) {
    // Print to the terminal
    printf("Exiting %s...\n", OS_NAME);

    // Print to the VGA display
    vga_set_bg(VGA_COLOR_RED);
    vga_set_fg(VGA_COLOR_WHITE);
    vga_set_xy(0, 0);
    vga_printf("%*s", 80, "");
    vga_set_xy(0, 0);
    vga_printf("Exiting %s...\n", OS_NAME);

    // Exit
    exit(0);
}

/**
 * Kernel context entry point
 * @param trapframe - pointer to the current process' trapframe
 */
 void kernel_context_enter(trapframe_t *trapframe) { 
    if (trapframe->event_type == 0xE){
        PageFaultHandler(); 
    } 
    else if (trapframe->event_type == 0xD){
        GeneralProtection();
    }
    if(active_proc)
        active_proc->trapframe = trapframe;
    
    interrupts_irq_handler(trapframe->event_type); 
    scheduler_run(); 

   
    kernel_tss.tss_esp0 = (unsigned)&proc_stack[active_proc->pid][PROC_STACK_SIZE];
    set_cr3((unsigned int)active_proc->pagetable);
    kernel_context_exit(active_proc->trapframe);
}
