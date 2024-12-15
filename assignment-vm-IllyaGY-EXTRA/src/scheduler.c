/**
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 *
 * Kernel Process Handling
 */

#include <spede/string.h>
#include <spede/stdio.h>
#include <spede/time.h>
#include <spede/machine/proc_reg.h>

#include "kernel.h"
#include "kproc.h"
#include "scheduler.h"
#include "timer.h"

#include "queue.h"


#define TIME_SLICE 200

// Process Queues
queue_t run_queue;


/**
 * Scheduler timer callback
 */
void scheduler_timer(void) {
    if(active_proc == NULL) return; 
    active_proc->cpu_time++;
    active_proc->run_time++;
}

/**
 * Executes the scheduler
 * Should ensure that `active_proc` is set to a valid process entry
 */
void scheduler_run(void) {
    // Ensure that processes not in the active state aren't still scheduled

    // Check if we have an active process and check if the current process has exceeded it's time slice
    if(active_proc != NULL && active_proc->cpu_time >= TIME_SLICE){
        // Reset the active time
        active_proc->cpu_time = 0; 

        // If the process is not the idle task, add it back to the scheduler
        if(active_proc->pid > 0){ 
            queue_in(&run_queue, active_proc->pid); 
        }

        //The current active process 
        active_proc->state = IDLE; 
        active_proc = NULL; 
    }
    if(active_proc != NULL) return; 

    int new_proc = 0;
    // Get the proces id from the run queue
    if (queue_out(&run_queue, &new_proc) == -1) {
        new_proc = 0;  // Default to idle task if no other process is available
    }
    
    // Update the active proc pointer
    active_proc = pid_to_proc(new_proc);

    // Make sure we have a valid process at this point
    if (active_proc != NULL) {
        // Ensure that the process state is set
        active_proc->state = ACTIVE; 
        kernel_log_info("Switched to process %d", active_proc->pid);
    }
}

/**
 * Adds a process to the scheduler
 * @param proc - pointer to the process entry
 */
void scheduler_add(proc_t *proc) {
    //Process 0 cannot be added to the queue 
    if(proc->pid == 0) return; 
    queue_in(&run_queue, proc->pid); 
}

/**
 * Removes a process from the scheduler
 * @param proc - pointer to the process entry
 */
void scheduler_remove(proc_t *proc) {

    int pid;

    if (!proc) {
        kernel_panic("Invalid process!");
        exit(1);
    }


    for (int i = 0; i < run_queue.size; i++) {
        if (queue_out(&run_queue, &pid) != 0) {
            kernel_panic("Unable to queue out the process entry");
        }

        if (proc->pid == pid) {
            // Found the process
            // continue iterating so the run queue order is maintained
            continue;
        }

        // Add the item back to the run queue
        if (queue_in(&run_queue, pid) != 0) {
            kernel_panic("Unable to queue process back to the run queue");
        }
    }

    memset(active_proc,0,sizeof(proc_t));

    
    // If the process is the current process, ensure that the current
    // process is reset so a new process will be scheduled
    if (proc == active_proc) {
        active_proc = NULL;
    }
}

/**
 * Initializes the scheduler, data structures, etc.
 */
void scheduler_init(void) {
    kernel_log_info("Initializing scheduler");
    
    // Initialize any data structures or variables
    queue_init(&run_queue);
        
    // Register the timer callback (scheduler_timer) to run every tick
    timer_callback_register(&scheduler_timer, 1 , -1);
}

