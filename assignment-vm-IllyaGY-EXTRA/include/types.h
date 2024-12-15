#ifndef _TYPES_H_
#define _TYPES_H_

#define STACK_SIZE      16384
#define TIMER_EVENT     32 // 32-bit long
#define NUM_ENTRIES     1024        // Each Page directory and table has 1024 entries

// 4096 is 0x1000

#define PF_ERR_USER_SUP_BIT     4   /* User/Supervisor bit position in PF error code*/
#define PF_ERR_PRES_BIT         1   /* Page present bit position in PF error code*/

typedef unsigned short seg_type_t;  // 16-bit segment value

typedef enum {
     SYSCALL_NONE,
     SYSCALL_PRINT,
}syscall_t;

#endif
