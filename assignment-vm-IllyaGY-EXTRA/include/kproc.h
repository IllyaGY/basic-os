/**
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 *
 * Kernel Process Handling
 */
#ifndef KPROC_H
#define KPROC_H

#include "spede.h"
#include "trapframe.h"

#ifndef PROC_MAX
#define PROC_MAX        10   // maximum number of processes to support
#endif

#define PROC_NAME_LEN       32   // Maximum length of a process name
#define PROC_STACK_SIZE     8192 // Process stack size
#define PROC_PAGE_LIST_SIZE 10
#define PAGE_PRESENT        0x1
#define PAGE_WRITE          0x2
#define PAGE_USER           0x4
#define PAGE_SIZE           0x00001000

// Process types
typedef enum proc_type_t {
    PROC_TYPE_NONE,     // Undefined/none
    PROC_TYPE_KERNEL,   // Kernel process
    PROC_TYPE_USER      // User process
} proc_type_t;


// Process States
typedef enum state_t {
    NONE,               // Process has no state (doesn't exist)
    IDLE,               // Process is idle (not scheduled)
    ACTIVE              // Process is active (scheduled)
} state_t;


// Process control block
// Contains all details to describe a process
typedef struct proc_t {
    int pid;                  // Process id
    state_t state;            // Process state
    proc_type_t type;         // Process type (kernel or user)

    char name[PROC_NAME_LEN]; // Process name

    int start_time;           // Time started
    int run_time;             // Total run time of the process
    int cpu_time;             // Current CPU time the process has used

    size_t pg_lst[PROC_PAGE_LIST_SIZE]; // Array used for storing the page list address, used for collecting page when destroy a process.
    unsigned int *pagetable; // Pointer to the page table directory


    unsigned char *stack;     // Pointer to the process stack
    trapframe_t *trapframe;   // Pointer to the trapframe
} proc_t;


// Process table
extern proc_t proc_table[PROC_MAX];

// Process stacks
extern unsigned char proc_stack[PROC_MAX][PROC_STACK_SIZE];


/**
 * Process functions
 */

/**
 * Initializes all process related data structures
 * Additionall, performs the following:
 *  - Creates the first process (kernel_idle)
 *  - Registers a timer callback to display the process table/status
 */
void kproc_init(void);

/**
 * Creates a new process
 * @param proc_ptr - address of process to execute
 * @param proc_name - "friendly" process name
 * @param proc_type - process type (kernel or user)
 * @return process id of the created process, -1 on error
 */
int kproc_create(void *proc_ptr, char *proc_name, proc_type_t proc_type);

/**
 * Destroys a process
 * If the process is currently scheduled it must be unscheduled
 * @param proc - process entry
 * @return 0 on success, -1 on error
 */
int kproc_destroy(proc_t *proc);

/**
 * Looks up a process in the process table via the process id
 * @param pid - process id
 * @return pointer to the process entry, NULL or error or if not found
 */
proc_t *pid_to_proc(int pid);

/**
 * Looks up a process in the process table via the entry/index into the table
 * @param entry - entry/index value
 * @return pointer to the process entry, NULL or error or if not found
 */
proc_t *entry_to_proc(int entry);


void context_switch(trapframe_t *current); 


/**
* Test User Process
*/
void uproc_test(void);

/**
 * Test process
 */
void kproc_test(void);
#endif
