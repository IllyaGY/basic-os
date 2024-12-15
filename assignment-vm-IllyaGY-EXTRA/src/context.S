/**
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 *
 * Context Switch Functions
 */
#include <spede/machine/asmacros.h>
#include "kernel.h"
#include "interrupts.h"

// define kernel stack space
.comm kstack, KSTACK_SIZE, 1
.text

// Keyboard ISR Entry
ENTRY(isr_entry_keyboard)
    // Indicate which interrupt occured
    /*Not sure why, but after copying this pushl $0x0 from assignment 5's event.S file it works now*/
    pushl $0x0
    pushl $IRQ_KEYBOARD
    // Enter into the kernel context for processing
    jmp kernel_enter

// Timer ISR Entry
ENTRY(isr_entry_timer)
    // Indicate which interrupt occured
    /*Not sure why, but after copying this pushl $0x0 from assignment 5's event.S file it works now*/
    pushl $0x0
    pushl $IRQ_TIMER
    // Enter into the kernel context for processing
    jmp kernel_enter


ENTRY(isr_entry_global_protection)
    //0xD = 13 
    pushl $0xD
    jmp kernel_enter


// Page Fault
ENTRY(isr_entry_page_fault)  // push eflag, cs, eip (by hardware)
    // Push error code (by hardware)
    // 0xE = 14
    pushl $0xE
    // Enter into the kernel context for processing
    jmp kernel_enter



// LoadKernelTSS
ENTRY(KernelTssFlush)
    movw $0x4B, %ax  
    ltr %ax
    ret


/**
 * Enter the kernel context
 *  - Save register state
 *  - Load the kernel stack
 *  - Trigger entry into the kernel
 */
kernel_enter:
    // Save register state
    pusha
    pushl %ds
    pushl %es
    pushl %fs
    pushl %gs
    // Load the kernel stack
    movl %esp, %edx
    cld
    movw $(KDATA_SEG), %ax
    mov %ax, %ds
    mov %ax, %es
    leal kstack + KSTACK_SIZE, %esp
    pushl %edx
    // Trigger entry into the kernel
    call CNAME(kernel_context_enter)

/**
 * Exit the kernel context
 *   - Load the process stack
 *   - Restore register state
 *   - Return from the previous interrupt
 */
ENTRY(kernel_context_exit)
    // Load the stack pointer
    movl 4(%esp), %eax
    movl %eax, %esp
    // Restore register state
    popl %gs
    popl %fs
    popl %es
    popl %ds
    popa
    // When kernel context was entered, the interrupt number
    // was pushed to the stack, so adjust the stack pointer
    // before returning
    add $8, %esp
    iret

