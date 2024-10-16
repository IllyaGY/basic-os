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

#define COLOR_GREY 0x7

// TTY Table
struct tty_t tty_table[TTY_MAX];

// Current Active TTY
struct tty_t *active_tty;

int tty_current = -1;
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

void update_info_display() {

    for(int i = 0; i < TTY_WIDTH; i++)
        vga_putc_at(i, 0, COLOR_GREY, VGA_COLOR_BLUE, ' ');          //Clearing the first line 
    char os_info[] = "basicOS"; 
    for (int i = 0; os_info[i] != '\0' ; i++) {
        vga_putc_at(i, 0, COLOR_GREY, VGA_COLOR_BLUE, os_info[i]);
        
    }
    
    // Clear the last line
    for (int x = 0; x < TTY_WIDTH; x++) {
        vga_putc_at(x, TTY_HEIGHT - 1, COLOR_GREY, active_tty->color_fg, ' '); // Clear last line
    }

    // Display current row and column
    char position_info[70]; 
    snprintf(position_info, sizeof(position_info), "TTY(%d) Column(%d) Row(%d) Scrollback(%d), use Page UP or DOWN", tty_current, active_tty->pos_x, active_tty->pos_y, active_tty->pos_scroll);
    
    // Display the position info on the last line
    for (int i = 0; position_info[i] != '\0' && i < TTY_WIDTH; i++) {
        vga_putc_at(i, TTY_HEIGHT - 1, COLOR_GREY, active_tty->color_fg, position_info[i]);
    }

    
}

void tty_scroll_up(){
    if(!active_tty->pos_scroll) return; 
    active_tty->pos_scroll--; 

    vga_clear();
     //Move everything one line down in the vga
    for(unsigned int i = TTY_WIDTH; i < TTY_WIDTH * (TTY_HEIGHT - 1) ; i++){
        char c = active_tty->buf[i+active_tty->pos_scroll * TTY_WIDTH];
        vga_putc_at(i%TTY_WIDTH, i/TTY_WIDTH, active_tty->color_bg, active_tty->color_fg, c);
    }

    update_info_display();  //Used when called from keyboard.c
}

void tty_scroll_down(void){
    if(active_tty->pos_scroll >= 10) return;
    active_tty->pos_scroll++;
    
    vga_clear();

    //Move everything one line up in the vga
    for(unsigned int i = TTY_WIDTH; i < TTY_WIDTH * (TTY_HEIGHT - 1); i++){
        char c = active_tty->buf[i+active_tty->pos_scroll * TTY_WIDTH];
        vga_putc_at(i%TTY_WIDTH, i/TTY_WIDTH, active_tty->color_bg, active_tty->color_fg, c);
    }
          
    update_info_display();  //Used when called from keyboard.c

}

void tty_putc(unsigned char c){
    switch (c) {
        case '\b':
            if (active_tty->pos_x > 0) {
                active_tty->pos_x--;
            } else if (active_tty->pos_y > 1) {
                active_tty->pos_y--;
                active_tty->pos_x=TTY_WIDTH-1; 
            }
            else if (active_tty->pos_scroll > 0){
                tty_scroll_up();
                active_tty->pos_x=TTY_WIDTH-1; 
            }
            vga_putc_at(active_tty->pos_x, active_tty->pos_y, active_tty->color_bg,active_tty->color_fg,' ');
            break;

        case '\t':
            active_tty->pos_x += 4;
            if (active_tty->pos_x >= TTY_WIDTH) {
                active_tty->pos_x-=TTY_WIDTH;
                active_tty->pos_y++; 
            }if (active_tty->pos_y >= TTY_HEIGHT-1) {
                active_tty->pos_y--;
                tty_scroll_down();

            }
            break;

        case '\r':                                  //<--- USE CTRL + M 
            active_tty->pos_x = 0;
            break;

        case '\n':
            active_tty->pos_x = 0;
            active_tty->pos_y++;
            if(active_tty->pos_y >=TTY_HEIGHT-1){
                active_tty->pos_y--;
                tty_scroll_down();
                active_tty->pos_x = 0;
            }
            
            break;

        default:
            vga_putc_at(active_tty->pos_x, active_tty->pos_y, active_tty->color_bg,active_tty->color_fg,c);
            
            active_tty->pos_x++;
            if (active_tty->pos_x >= TTY_WIDTH) {
                active_tty->pos_x = 0;
                active_tty->pos_y++;
            }
            if(active_tty->pos_y >= TTY_HEIGHT-1){
                active_tty->pos_y--;
                tty_scroll_down();
                active_tty->pos_x = 0; 
            }
            break;
            
    }

   
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
        else{
            n = tty_current;
            kernel_log_error("No tty selected");
        }
    }
    
    if(tty_current != n){
        
        vga_clear();
        //New current index
        tty_current = n;
        //Retrieve the old text before writing down the new character 
        int i = TTY_WIDTH;
        while(i < TTY_WIDTH*(TTY_HEIGHT-2+TTY_SCROLLBACK)){
            vga_putc_at(i%80, i/80, active_tty->color_bg, active_tty->color_fg, active_tty->buf[i+active_tty->pos_scroll * TTY_WIDTH]);
            i++;
        }
        tty_cursor_update();
        update_info_display();
        kernel_log_info("New tty");
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
        tty_putc(active_tty->buf[active_tty->pos_x + (active_tty->pos_y + active_tty->pos_scroll) * TTY_WIDTH]);
         // Update current cursor position display
        update_info_display();
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

    if(c != 0x00)
        active_tty->buf[active_tty->pos_x + (active_tty->pos_y+active_tty->pos_scroll) * TTY_WIDTH] = c;

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
        tty_table[i].color_bg = VGA_COLOR_BLUE;
        tty_table[i].color_fg = VGA_COLOR_WHITE;
        tty_table[i].pos_y = 1; 
       
    }  

    if(tty_cursor){
        tty_cursor_enable();
    }
    else 
        tty_cursor_disable();

    
    

    // Select tty 0 to start with
    tty_select(0);  

    
    


    // Register a timer callback to update the screen on a regular interval
    //Since we have 100 milliseconds / 5 = 20 times per second
    timer_callback_register(tty_refresh, 5, -1); 
}

