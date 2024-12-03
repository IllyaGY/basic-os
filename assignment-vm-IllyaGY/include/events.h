/* 
 * events.h, Assignment -- Timer Event
 */

#ifndef __EVENTS_H__
#define __EVENTS_H__

#include "types.h"

#ifndef ASSEMBLER  // skip if ASSEMBLER defined (in assembly code)

void TimerEntry(); 
void ProcLoader(trapframe_t *);
void PageFaultEntry();
void SyscallEntry();
void KernelTssFlush();

#endif

#endif
