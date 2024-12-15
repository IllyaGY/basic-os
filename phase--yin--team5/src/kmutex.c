/**
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 *
 * Kernel Mutexes
 */

#include <spede/string.h>

#include "kernel.h"
#include "kmutex.h"
#include "queue.h"
#include "scheduler.h"

// Table of all mutexes
mutex_t mutexes[MUTEX_MAX];

// Mutex ids to be allocated
queue_t mutex_queue;

/**
 * Initializes kernel mutex data structures
 * @return -1 on error, 0 on success
 */
int kmutexes_init() {
    kernel_log_info("Initializing kernel mutexes");

    // Initialize the mutex table
    memset(mutexes, 0, sizeof(mutexes));
    
    // Initialize the mutex queue
    queue_init(&mutex_queue);
    
    // Fill the mutex queue
    for (int i = 0; i < MUTEX_MAX; i++) {
        queue_in(&mutex_queue, i);
    }
    return 0;
}

/**
 * Allocates a mutex
 * @return -1 on error, otherwise the mutex id that was allocated
 */
int kmutex_init(void) {
    int id;
    // Obtain a mutex id from the mutex queue
    if (queue_out(&mutex_queue, &id) != 0) {
        return -1;
    }

    if((id < 0) || (id > MUTEX_MAX))
    {
        kernel_log_info("ID outside of the range");
        return -1; 
    }

    // Pointer to the mutex table entry
    mutex_t *mutex = &mutexes[id];
    memset(mutex, 0, sizeof(mutexes[id]));
    // Initialize the mutex data structure (mutex_t + all members)
    mutex->allocated = 1;
    mutex->locks = 0;
    mutex->owner = NULL;
    queue_init(&mutex->wait_queue);
    
    // return the mutex id
    return id;
}

/**
 * Frees the specified mutex
 * @param id - the mutex id
 * @return 0 on success, -1 on error
 */
int kmutex_destroy(int id) {
    // look up the mutex in the mutex table
    if (id < 0 || id >= MUTEX_MAX) {
        return -1;
    }

    mutex_t *mutex = &mutexes[id];

    // If the mutex is locked, prevent it from being destroyed (return error)
    if (mutex->locks > 0) {
        return -1;
    }

    // Add the id back into the mutex queue to be re-used later
    queue_in(&mutex_queue, id);
    
    // Clear the memory for the data structure
    memset(mutex, 0, sizeof(mutex_t));

    return 0;
}

/**
 * Locks the specified mutex
 * @param id - the mutex id
 * @return -1 on error, otherwise the current lock count
 */
int kmutex_lock(int id) {
    // look up the mutex in the mutex table
    if (id < 0 || id >= MUTEX_MAX) {
        return -1;
    }

    mutex_t *mutex = &mutexes[id];
    // If the mutex is already locked
    if (mutex->locks > 0) {
        //   1. Set the active process state to WAITING
        active_proc->state = WAITING;
        
        //   2. Add the process to the mutex wait queue (so it can take
        //      the mutex when it is unlocked)
        queue_in(&mutex->wait_queue, active_proc->pid);
        
        //   3. Remove the process from the scheduler, allow another
        //      process to be scheduled
        scheduler_remove(active_proc);
    }
    else{
    // If the mutex is not locked
    //   1. set the mutex owner to the active process
        mutex->owner = active_proc;
    }
    // Increment the lock count
    mutex->locks++;
    // Return the mutex lock count
    return mutex->locks;
}

/**
 * Unlocks the specified mutex
 * @param id - the mutex id
 * @return -1 on error, otherwise the current lock count
 */
int kmutex_unlock(int id) {
    // look up the mutex in the mutex table
    if (id < 0 || id >= MUTEX_MAX) {
        return -1;
    }

    mutex_t *mutex = &mutexes[id];    
    // If the mutex is not locked, there is nothing to do
    if (mutex->locks == 0) {
        return 0;
    }
    // If there are still locks held, and the owner of the lock is the active process
    else if (mutexes[id].owner == active_proc) {
        // Decrement the lock count
        mutex->locks--;

        // If there are no more locks held:
        if (mutexes[id].locks == 0)
            // 1. clear the owner of the mutex
            mutex->owner = NULL;
    
    
        // 1. Obtain a process from the mutex wait queue
        int next_proc; 
        if(queue_out(&mutex->wait_queue, &next_proc) == -1)
            return -1; 
        
        proc_t* process = pid_to_proc(next_proc);

        // 2. Add the process back to the scheduler
        if (process != NULL)
            scheduler_add(process); 
        
        // 3. set the owner of the of the mutex to the process
        mutex->owner = process;
    }
    else 
        return -1; 
    
    // return the mutex lock count
    return mutex->locks;

}
