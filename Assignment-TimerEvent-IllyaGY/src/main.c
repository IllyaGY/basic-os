/* 
 * main.c -- Timer Event
 */

#include <spede/flames.h>
#include <spede/machine/asmacros.h>
#include <spede/machine/io.h>
#include <spede/machine/pic.h>
#include <spede/machine/proc_reg.h>
#include <spede/machine/seg.h>
#include <spede/stdio.h>
#include <spede/string.h>

#include "events.h" // needs addr of TimerEvent

#define LOOP 16660000 // handy loop to time .6 microseconds

typedef void (* func_ptr_t)();
struct i386_gate *IDT_p; 

void Process(void){
	int i = 0;

	while(1){
		if(cons_kbhit()) {
			cons_printf("\nTimer Event is stopping!");
			break;
			
		} 
		while(i < LOOP){
			//Since asm("inb $0x80") takes ~ 0.6 microseconds we do LOOP x 0.6 microseconds == 1 second
			asm("inb $0x80"); 
			i++;
			
		}
		i = 0;
		cons_putchar('z');
	}
}

int main(){
	IDT_p = get_idt_base(); // get IDT location

	cons_printf("IDT located @ DRAM addr %x (%d).\n", IDT_p, IDT_p); // show IDT addr

    /* Fill a trap/interrupt/task/call gate with particular values.  */
    /* You can find this function at /opt/spede/include/spede/machine/seg.h */ 
	fill_gate(&IDT_p[TIMER_EVENT], (int)TimerEntry, get_cs(), ACC_INTR_GATE, 0);
	outportb(0x21, ~0x01); // 0x21 is PIC mask, ~1 is mask
	
	// Need to implement:
	// asm("set/enable interrupt in CPU EFLAGS register");
	asm("sti");
	

	// Need to implement:
	// call Process function here to run until a key in pressed 
	Process();



	return 0;
}
