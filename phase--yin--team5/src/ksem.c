/**
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 *
 * Kernel Semaphores
 */

#include <spede/string.h>

#include "kernel.h"
#include "ksem.h"
#include "queue.h"
#include "scheduler.h"

// Table of all semephores
sem_t semaphores[SEM_MAX];

// semaphore ids to be allocated
queue_t sem_queue;

/**
 * Initializes kernel semaphore data structures
 * @return -1 on error, 0 on success
 */
int ksemaphores_init() {
    kernel_log_info("Initializing kernel semaphores");

    // Initialize the semaphore table
    memset(semaphores, 0, sizeof(semaphores[SEM_MAX]));
    // Initialize the semaphore queue
    queue_init(&sem_queue);

    // Fill the semaphore queue
    for (int i = 0; i < SEM_MAX; i++) {
        queue_in(&sem_queue, i);
    }

    return 0;
}

/**
 * Allocates a semaphore
 * @param value - initial semaphore value
 * @return -1 on error, otherwise the semaphore id that was allocated
 */
int ksem_init(int value) {
    // Obtain a semaphore id from the semaphore queue
    int id;
    if (queue_out(&sem_queue, &id) == -1) {
        kernel_log_error("semaphore not available");
        return -1;
    }
    // Ensure that the id is within the valid range
    if (id < 0 || id >= SEM_MAX) {
        kernel_log_error("semaphore id out of range");
        return -1;
    }

    // Initialize the semaphore data structure
    // sempohare table + all members (wait queue, allocated, count)
    // set count to initial value
    memset(&semaphores[id], 0, sizeof(semaphores[id]));
    semaphores[id].count = value; // Set the initial semaphore value (e.g., 1 for a lock)
    queue_init(&semaphores[id].wait_queue); // No processes are waiting initially
    semaphores[id].allocated = true; // Mark the semaphore as allocated

    return id;
}

/**
 * Frees the specified semaphore
 * @param id - the semaphore id
 * @return 0 on success, -1 on error
 */
int ksem_destroy(int id) {
    // look up the sempaphore in the semaphore table
    if (id < 0 || id >= SEM_MAX) {
        kernel_log_error("semaphore id out of range");
        return -1;
    }
    // If the semaphore is locked, prevent it from being destroyed
    if (semaphores[id].count > 0) {
        kernel_log_error("Cant destroy semaphore: waiting processes exist");
        return -1;
    }
    // Add the id back into the semaphore queue to be re-used later
    // Clear the memory for the data structure
    
    // semaphores[id].allocated = false;
    // semaphores[id].count = 0;
    if (queue_in(&sem_queue, id) == -1) 
        return -1; 
    //I guess we can just do memset instead of doing it manually like that code above
    memset(&semaphores[id], 0, sizeof(semaphores[id])); 

    return 0;
}

/**
 * Waits on the specified semaphore if it is held
 * @param id - the semaphore id
 * @return -1 on error, otherwise the current semaphore count
 */
int ksem_wait(int id) {
    // look up the sempaphore in the semaphore table
    if (id < 0 || id >= SEM_MAX) {
        kernel_log_error("semaphore id out of range");
        return -1;
    }
    // If the semaphore count is 0, then the process must wait
    if (semaphores[id].count == 0) {
        // Set the state to WAITING
        active_proc->state = WAITING;
        // add to the semaphore's wait queue
        if(queue_in(&semaphores[id].wait_queue, active_proc->pid) == -1)
            kernel_log_info("error when adding process pid to semaphore waiting queue");
        // remove from the scheduler
        scheduler_remove(active_proc);
        //We also have to clear the pointer to let scheduler_run assign a new process
        active_proc = NULL; 
    }
    // If the semaphore count is > 0
    else{// MY OWN COMMENT: there may be a case it seems where else is a better option than and if statment
        // Decrement the count
        semaphores[id].count--;
    }
    // Return the current semaphore count
    return semaphores[id].count;
}

/**
 * Posts the specified semaphore
 * @param id - the semaphore id
 * @return -1 on error, otherwise the current semaphore count
 */
int ksem_post(int id) {

    // look up the semaphore in the semaphore table
    if (id < 0 || id >= SEM_MAX) {
        kernel_log_error("semaphore id out of range");
        return -1;
    }
    // incrememnt the semaphore count
    semaphores[id].count++;
    
    // check if any processes are waiting on the semaphore (semaphore wait queue)
    if (semaphores[id].wait_queue.size > 0) {   
        // if so, queue out and add to the scheduler
        int pid;
        if (queue_out(&semaphores[id].wait_queue, &pid) == 0) {
            proc_t *waiting_proc = pid_to_proc(pid);
            if (waiting_proc != NULL) {
                scheduler_add(waiting_proc);
                // decrement the semaphore count
                semaphores[id].count--;
            }
        }
        //Since we did nesting if statements here, we better then do the else statement otherwise it might crash
        else
            return -1;
    }

    // return current semaphore count
    return semaphores[id].count;
    
}
