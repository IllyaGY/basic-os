/* 
 * handlers.c, Assignment -- Timer Event
 */
#include <spede/flames.h>
#include <spede/machine/asmacros.h>
#include <spede/machine/io.h>
#include <spede/machine/pic.h>
#include <spede/machine/proc_reg.h>
#include <spede/machine/seg.h>
#include <spede/stdio.h>
#include <spede/string.h>

#define PIC_CONTROL 0x20 	// programmable Interrupt Controller (PIC).
#define TIMER_DONE 	0x60	//	Set to PIC control when timer interrupt service done. 

#define VGA_BASE	(unsigned short *)0xB8000// 2-byte (unsigned short) ptr points to vga location
#define VGA_CENTER		VGA_BASE + 12 * 80 + 34


// assume screen has 24 rows, 80 cols, upper-left corner address is 0xb8000.

char my_name[] = "Illya Gordyy";
int length = sizeof(my_name)/sizeof(char); //Length of the my_name
int i = 0;  // which char in my name
int tick_count = 0;  // count number of timer events

// position vga location to show 1st char in name
unsigned short *char_p = (unsigned short *)0xB8000 + 12 * 80 + 34; // center address for vga

// Timer handler function, evoked from TimerEvent
void TimerHandler(){ 
	// Implement:
	if(!tick_count){
		*char_p = (unsigned short)(my_name[i] + (0x0f << 8)); 
	}
	tick_count++; 

	if(tick_count == 75){
		tick_count = 0; 
		i++; 
		char_p++;
		if(i == length){
			i=0;
			*char_p = ' ';
			while(char_p > VGA_CENTER){	
				char_p--;
				*char_p = ' ';
			}			
		}
	}
	

	// dismiss timer event (IRQ 0), otherwise, new event from timer won't be recognized by CPU
	// because hardware uses edge-trigger flipflop
	outportb(PIC_CONTROL, TIMER_DONE);
}
