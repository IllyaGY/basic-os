/**
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 *
 * TTY Definitions
 */
#include <spede/machine/io.h>
#include <spede/stdarg.h>
#include <spede/string.h>

#include "kernel.h"
#include "timer.h"
#include "tty.h"
#include "vga.h"



//TTY Cursor
#define TTY_PORT_ADDR       0x3D4
#define TTY_PORT_DATA       0x3D5


// TTY Table
struct tty_t tty_table[TTY_MAX];

// Current Active TTY
struct tty_t *active_tty;

int tty_current = 0;
int tty_new_char = 0;       //<-- Helps keep the cursor at x = 0 when doing pressing Enter
int tty_cursor = 1;



void tty_cursor_enable(void){
    tty_cursor = 1;

    //Cursor Start
    outportb(TTY_PORT_ADDR, 0x0A);
    outportb(TTY_PORT_DATA, (inportb(TTY_PORT_DATA) & 0xC0) | 0xE);

    //Cursor End
    outportb(TTY_PORT_ADDR, 0x0B);
    outportb(TTY_PORT_DATA, (inportb(TTY_PORT_DATA) & 0xE0) | 0xF);


}

void tty_cursor_disable(void){
    tty_cursor = 0;

    //Disabling the cursor status
    outportb(TTY_PORT_ADDR, 0x0A);
    outportb(TTY_PORT_DATA, 0x20);
}

void tty_cursor_update(void){
    if(tty_cursor){
        unsigned short curs_pos = active_tty->pos_y * TTY_WIDTH + active_tty->pos_x;

        outportb(TTY_PORT_ADDR, 0x0F);
        outportb(TTY_PORT_DATA, (unsigned char)(curs_pos & 0xFF));

        outportb(TTY_PORT_ADDR, 0x0E);
        outportb(TTY_PORT_DATA, (unsigned char)((curs_pos >> 8) & 0xFF));

    }

}

void tty_putc(char c){
    
    switch (c) {
        case '\b':
            if (active_tty->pos_x != 0) {
                active_tty->pos_x--;
            } else if (active_tty->pos_y != 0) {
                active_tty->pos_y--;
                active_tty->pos_x=TTY_WIDTH-1; 
            }
            active_tty->buf[active_tty->pos_x + active_tty->pos_y * TTY_WIDTH] = 0x00;
            break;

        case '\t':
            active_tty->pos_x += 4;
            if (active_tty->pos_x >= TTY_WIDTH) {
                active_tty->pos_x-=TTY_WIDTH;
                active_tty->pos_y++; 
            }if (active_tty->pos_y >= TTY_HEIGHT) {
                active_tty->pos_y = 0;
                active_tty->pos_x -= TTY_WIDTH; 
            }
            break;

        case '\r':                                  //<--- USE CTRL + M 
            active_tty->pos_x = 0;
            break;

        case '\n':
            active_tty->pos_x = 0;
            active_tty->pos_y++;
            break;


        default:
            active_tty->buf[active_tty->pos_x + active_tty->pos_y * TTY_WIDTH] = c;
            tty_new_char = 1; 
            break;
    }



    //if (active_tty->pos_scroll) {
        // Handle end of rows
        // if (active_tty->pos_y >= TTY_HEIGHT) {
        //     // Scroll the screen up (copy each row to the previous)
        //     for (unsigned int i = 0; i < VGA_WIDTH * (VGA_HEIGHT - 1); i++) {
        //         vga_buf[i] = vga_buf[VGA_WIDTH + i];
        //     }

        //     // Clear the last line
        //     for (unsigned int i = 0; i < VGA_WIDTH; i++) {
        //         vga_buf[i + (VGA_WIDTH * (VGA_HEIGHT-1))] = VGA_CHAR(vga_color_bg, vga_color_fg, ' ');
        //     }

        //     vga_pos_y = VGA_HEIGHT - 1;
        // }

    //}


} 



/**
 * Sets the active TTY to the selected TTY number
 * @param tty - TTY number
 */
void tty_select(int n) {
    // Set the active tty to point to the entry in the tty table
    if(n >= 0 && n < TTY_MAX){
        active_tty = &tty_table[n];
        kernel_log_info("tty %i is selected", n);
    }

    else{
        if (n == 10 && tty_current < TTY_MAX-1){
            n = tty_current + 1;
            active_tty = &tty_table[n];
            kernel_log_info("Next tty is selected %i", n); 

        }
        else if(n == -1 && tty_current > 0){
            n = tty_current - 1;
            active_tty = &tty_table[n];
            kernel_log_info("Previous tty is selected %i", n); 
        }
        else kernel_log_error("No tty selected");
    }
    
    // if a new tty is selected, the tty should trigger a refresh
    if(tty_current != n){
        vga_clear();
        //First we need to ensure we load up all the text for our new current active_tty back to the screen
        int x = 0; 
        int y = 0; 
        //New current index
        tty_current = n;
        //Retrieve the old text before writing down the new character                <--- The issue with adding an extra space is probably here, due to a 1 added somewhere          
        vga_puts_at(x, y, active_tty->color_bg, active_tty->color_fg, active_tty->buf);
        tty_cursor_update();
        kernel_log_info("New tty");
        active_tty->refresh = 1; 
    }
    
    
}

/**
 * Refreshes the tty if needed
 */
void tty_refresh(void) {
    if (!active_tty) {
        kernel_panic("No TTY is selected!");
        return;
    }

    // If the TTY needs to be refreshed, copy the tty buffer
    // to the VGA output.
    // ** hint: use vga_putc_at() since you are setting specific characters
    //          at specific locations
    // Reset the tty's refresh flag so we don't refresh unnecessarily
    if(active_tty->refresh){          
        vga_putc_at(
        active_tty->pos_x, 
        active_tty->pos_y, 
        active_tty->color_bg, 
        active_tty->color_fg, 
        active_tty->buf[active_tty->pos_y * TTY_WIDTH + active_tty->pos_x]);      
        
        if(tty_new_char && active_tty->buf[active_tty->pos_y * TTY_WIDTH + active_tty->pos_x] != 0x00){                                             
            //   Adjust the x/y positions as necessary <------ I moved this part here instead of keeping that in the tty_update
            active_tty->pos_x++;
            if(active_tty->pos_x == TTY_WIDTH) {
                active_tty->pos_y++;
                active_tty->pos_x = 0;
            } 
            if(active_tty->pos_y == TTY_HEIGHT) {
                active_tty->pos_y = 0;
                active_tty->pos_x = 0;
            } 
            tty_new_char = 0; 
            //   Handle scrolling at the bottom                 <------ STILL NEEDS ADJUSTMENTS, SCROLLING NOT DONE YET
        }
        tty_cursor_update();
        active_tty->refresh = 0; 
    }
    
     
            
    
}

/**
 * Updates the active TTY with the given character
 */
void tty_update(char c) {
    if (!active_tty) {
        return;
    }

    // Since this is a virtual wrapper around the VGA display, treat each
    // input character as you would for the VGA output
  
    // Instead of writing to the VGA display directly, you will write
    // to the tty buffer.

    tty_putc(c);
    // If the screen should be updated, the refresh flag should be set
    // to trigger the the VGA update via the tty_refresh callback
    active_tty->refresh = 1;


    
    
    
    
}

/**
 * Initializes all TTY data structures and memory
 * Selects TTY 0 to be the default
 */
void tty_init(void) {
    kernel_log_info("tty: Initializing TTY driver");

    // Initialize the tty_table
    memset(tty_table, 0, sizeof(tty_table));

    for(int i = 0; i < TTY_MAX;i++){
        tty_table[i].color_fg = 0xF;                 // <-- White foreground for testing 
    }

    if(tty_cursor){
        tty_cursor_enable();
    }
    else 
        tty_cursor_disable();


    // Select tty 0 to start with
    tty_select(0);



    // Register a timer callback to update the screen on a regular interval

    //Since we have 100 milliseconds / 10 = 10 times per second
    timer_callback_register(tty_refresh, 10, -1); 
}

