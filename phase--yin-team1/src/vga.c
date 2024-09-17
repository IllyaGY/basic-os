#include <spede/stdarg.h>
#include <spede/stdio.h>
#include <spede/stdlib.h>

#include "bit.h"
#include "io.h"
#include "kernel.h"
#include "vga.h"

typedef unsigned int u32;
typedef unsigned char u4; 

/**
 * Forward Declarations
 */
void vga_cursor_update(void);
void vga_end_line(u32 *cursor); 

/**
 * Global variables in this file scope
 */

bool cursor_enabled; 
unsigned int cursor[2];
u4 *upper_lines; 
u4 *lower_lines; 
u32 upper_count; 
u32 lower_count = 0; 




/**
 * Initializes the VGA driver and configuration
 *  - Defaults variables
 *  - Clears the screen
 */
void vga_init(void) {
    kernel_log_info("Initializing VGA driver");

    cursor_enabled = 0; 

    //Row
    cursor[0] = 0; 
    //Column
    cursor[1] = 0; 

    upper_count = 0; 
    //Will keep the top lines in the buffer and reallocate if needed 
    upper_lines = (u4 *)malloc(sizeof(u4) * VGA_WIDTH); 


    //Will keep the bottom lines in the buffer and reallocate if needed 
    lower_lines = (u4 *)malloc(sizeof(u4) * VGA_WIDTH);


    // Clear the screen
    vga_clear(); 
}

/**
 * Clears the VGA output and sets the background and foreground colors
 */
void vga_clear(void) {
    

    vga_clear_bg(0);
    vga_clear_fg(0);  

    
    // Clear all character data, set the foreground and background colors
    // Set the cursor position to the top-left corner (0, 0)
}

/**
 * Clears the background color for all characters and sets to the
 * specified value
 *
 * @param bg background color value
 */
void vga_clear_bg(int bg) {
     // Iterate through all VGA memory and set only the background color bits
    unsigned short memoryOffset = 0; 
    while(memoryOffset < VGA_WIDTH*VGA_HEIGHT){
        VGA_BASE[memoryOffset] = (unsigned short)VGA_ATTR(bg, VGA_BASE[memoryOffset] >> 8); 
        memoryOffset++;

    }
   
}

/**
 * Clears the foreground color for all characters and sets to the
 * specified value
 *
 * @param fg foreground color value
 */
void vga_clear_fg(int fg) {
    // Iterate through all VGA memory and set only the foreground color bits.
    unsigned short memoryOffset = 0; 
    while(memoryOffset < VGA_WIDTH*VGA_HEIGHT){
        VGA_BASE[memoryOffset] = (unsigned short)VGA_ATTR(VGA_BASE[memoryOffset] >> 12, fg); 
        memoryOffset++;

    }
}

/**
 * Enables the VGA text mode cursor
 */
void vga_cursor_enable(void) {
    // All operations will consist of writing to the address port which
    // register should be set, followed by writing to the data port the value
    // to set for the specified register

    // The cursor will be drawn between the scanlines defined
    // in the following registers:
    //   0x0A Cursor Start Register
    //   0x0B Cursor End Register

    vga_cursor_update();

    unsigned char currStart = inportb(0x0A);
    //unsigned char currEnd = inportb(0x0B);

    outportb(0x0A, (unsigned char)bit_clear(currStart, 5));

    // for(u4 i = 0; i < 16; i+=2){
    //     outportb(0); 
    // }
    outportb(0x0A, 0x1); 
    outportb(0x0B, 0xC); 

    //vga_cursor_update();

    // Bit 5 in the cursor start register (0x0A) will enable or disable the cursor:
    //   0 - Cursor enabled
    //   1 - Cursor disabled

    // The cursor will be displayed as a series of horizontal lines that create a
    // "block" or rectangular symbol. The position/size is determined by the "starting"
    // scanline and the "ending" scanline. Scanlines range from 0x0 to 0xF.

    // In both the cursor start register and cursor end register, the scanline values
    // are specified in bits 0-4

    // To ensure that we do not change bits we are not intending,
    // read the current register state and mask off the bits we
    // want to save

    // Set the cursor starting scanline (register 0x0A, cursor start register)

    // Set the cursor ending scanline (register 0x0B, cursor end register)
    // Ensure that bit 5 is not set so the cursor will be enabled

    // Since we may need to update the vga text mode cursor position in
    // the future, ensure that we track (via software) if the cursor is
    // enabled or disabled

    // Update the cursor location once it is enabled
}

/**
 * Disables the VGA text mode cursor
 */
void vga_cursor_disable(void) {
    // All operations will consist of writing to the address port which
    // register should be set, followed by writing to the data port the value
    // to set for the specified register

    // The cursor will be drawn between the scanlines defined
    // in the following registers:
    //   0x0A Cursor Start Register
    //   0x0B Cursor End Register

    // Bit 5 in the cursor start register (0x0A) will enable or disable the cursor:
    //   0 - Cursor enabled
    //   1 - Cursor disabled

    // Since we are disabling the cursor, we can simply set the bit of interest
    // as we won't care what the current cursor scanline start/stop values are

    // Since we may need to update the vga text mode cursor position in
    // the future, ensure that we track (via software) if the cursor is
    // enabled or disabled
}

/**
 * Indicates if the VGA text mode cursor is enabled or disabled
 */
bool vga_cursor_enabled(void) {
    return cursor_enabled;
}

/**
 * Sets the vga text mode cursor position to the current VGA row/column
 * position if the cursor is enabled
 */
void vga_cursor_update(void) {
    // All operations will consist of writing to the address port which
    // register should be set, followed by writing to the data port the value
    // to set for the specified register

    // The cursor position is represented as an unsigned short (2-bytes). As
    // VGA register values are single-byte, the position representation is
    // split between two registers:
    //   0x0F Cursor Location High Register
    //   0x0E Cursor Location Low Register

    unsigned short pos = 15*VGA_WIDTH+50; 


    outportb(0x0F, pos & 0x00FF);
    outportb(0x0E, (pos >> 8) & 0x00FF);


    // The Cursor Location High Register is the least significant byte
    // The Cursor Location Low Register is the most significant byte

    // If the cursor is enabled:
    //unsigned short cursor_position; 
    
        
    
        // Calculate the cursor position as an offset into
        // memory (unsigned short value)

        // Set the VGA Cursor Location High Register (0x0F)
        //   Should be the least significant byte (0x??<00>)

        // Set the VGA Cursor Location Low Register (0x0E)
        //   Should be the most significant byte (0x<00>??)
}

/**
 * Sets the current row/column position
 *
 * @param row position (0 to VGA_HEIGHT-1)
 * @param col position (0 to VGA_WIDTH-1)
 * @notes If the input parameters exceed the valid range, the position
 *        will be set to the range boundary (min or max)
 */
void vga_set_rowcol(int row, int col) {
    if(row < 0 || row >= VGA_HEIGHT){
        row = (row < 0) ? 0 : VGA_HEIGHT-1;
    }
    if(col < 0 || col >= VGA_HEIGHT){
        col = (col < 0) ? 0 : VGA_WIDTH-1; 
    }
    

    // Update the text mode cursor (if enabled)
    unsigned char pos = row * VGA_WIDTH + col; 

    outportb(0x0F, (pos>>8) & 0x00FF);
    outportb(0x0E, pos & 0x00FF);
}

/**
 * Gets the current row position
 * @return integer value of the row (between 0 and VGA_HEIGHT-1)
 */
int vga_get_row(void) {
    return cursor[0];
}

/**
 * Gets the current column position
 * @return integer value of the column (between 0 and VGA_WIDTH-1)
 */
int vga_get_col(void) {
    return cursor[1];
}

/**
 * Sets the background color.
 *
 * Does not modify any existing background colors, only sets it for
 * new operations.
 *
 * @param bg - background color
 */
void vga_set_bg(int bg) {
    
}

/**
 * Gets the current background color
 * @return background color value
 */
int vga_get_bg(void) {
    //getting the current value at that memory address  ---- ---- ---- ---- (4 bits each), moving 12 bits to the right, and gettin 
    //the 0x(BG) 4 bit
    return VGA_BASE[cursor[0] * VGA_WIDTH + cursor[1]] >> 12;
}

/**
 * Sets the foreground/text color.
 *
 * Does not modify any existing foreground colors, only sets it for
 * new operations.
 *
 * @param color - background color
 */
void vga_set_fg(int fg) {
}

/**
 * Gets the current foreground color
 * @return foreground color value
 */
int vga_get_fg(void) {
    return 0;
}

/**
 * Prints a character on the screen without modifying the cursor or other attributes
 *
 * @param c - Character to print
 */
void vga_setc(unsigned char c) {
    VGA_BASE[0] = VGA_CHAR(VGA_COLOR_BLACK, VGA_COLOR_LIGHT_GREY, c);
}

/**
 * Prints a character on the screen at the current cursor (row/column) position
 *
 * When a character is printed, will do the following:
 *  - Update the row and column positions
 *  - If needed, will wrap from the end of the current line to the
 *    start of the next line
 *  - If the last line is reached, the cursor position will reset to the top-left (0, 0) position
 *  - Special characters are handled as such:
 *    - tab character (\t) prints 'tab_stop' spaces
 *    - backspace (\b) character moves the character back one position,
 *      prints a space, and then moves back one position again
 *    - new-line (\n) should move the cursor to the beginning of the next row
 *    - carriage return (\r) should move the cursor to the beginning of the current row
 *
 * @param c - character to print
 */
void vga_putc(unsigned char c) { 
    // Handle special characters
    switch (c)
    {
    case (unsigned short)('\b'):
        if(cursor[1]){      //Check if column is greater than 0 
            cursor[1]--;   
        }
        //Else if column == 0 
        else if(cursor[0]){      //If Row is greater than 0
            cursor[0]--; 
            cursor[1] = VGA_WIDTH-1; 
        } 
        return; 
    
    case (unsigned short)('\t'): 
        //Move 4 cells foward
        unsigned short cellsLeft = 4;
        while(cellsLeft){      
            VGA_BASE[cursor[0] * VGA_WIDTH + cursor[1]] = VGA_CHAR(VGA_COLOR_BLACK, VGA_COLOR_LIGHT_GREY, 0x20); 
            cursor[1]++; 
            cellsLeft--; 
        }
        return;
        
    case (unsigned short)('\n'): 
        //Move 4 cells foward
        if(cursor[0] < VGA_HEIGHT-1){
            cursor[0]++;
            cursor[1] = 0; 
        }
        return;
    
    case (unsigned short)('\r'):
        cursor[1] = 0; 
        return;         

    default: 
        VGA_BASE[cursor[0] * VGA_WIDTH + cursor[1]] = VGA_CHAR(VGA_COLOR_BLACK, VGA_COLOR_LIGHT_GREY, c);
        break;
    }
   
    cursor[1]++;

    // Handle end of lines
    vga_end_line(cursor);

    // Wrap-around to the top/left corner
    
    // Update the text mode cursor, if enabled
    vga_cursor_update(); 
    
}

/**
 * Prints a string on the screen at the current cursor (row/column) position
 *
 * @param s - string to print
 */
void vga_puts(char *s) {
    if(s){  //Checking if not null pointer
        while(*s != 0) {
            vga_putc((unsigned char)(*s)); 
            s+=1; 
        }   
    }
}

/**
 * Prints a character on the screen at the specified row/column position and
 * with the specified background/foreground colors
 *
 * Does not modify the current row or column position
 * Does not modify the current background or foreground colors
 *
 * @param row the row position (0 to VGA_HEIGHT-1)
 * @param col the column position (0 to VGA_WIDTH-1)
 * @param bg background color
 * @param fg foreground color
 * @param c character to print
 */
void vga_putc_at(int row, int col, int bg, int fg, unsigned char c) {
    if(row < 0 || row >= VGA_HEIGHT){
        kernel_log_error("Row is out of range %d <> %d", 0, VGA_HEIGHT );
        return; 
    }
    if(col < 0 || col >= VGA_WIDTH){
        kernel_log_error("Column is out of range %d <> %d", 0, VGA_WIDTH );
        return; 
    }

    VGA_BASE[row * VGA_WIDTH + col] = VGA_CHAR(bg, fg, c);

}

/**
 * Prints a string on the screen at the specified row/column position and
 * with the specified background/foreground colors
 *
 * Does not modify the current row or column position
 * Does not modify the current background or foreground colors
 *
 * @param row the row position (0 to VGA_HEIGHT-1)
 * @param col the column position (0 to VGA_WIDTH-1)
 * @param bg background color
 * @param fg foreground color
 * @param s string to print
 */
void vga_puts_at(int row, int col, int bg, int fg, char *s) {
}


void vga_end_line(u32 *cursor){
    if(cursor[1] >= VGA_WIDTH){
        //Here, if the row counter reached 24(the last row) we are simply keeping it at 24, since the top line will be 
        //placed into the buffer, and all the other lines will be moved to the top
        if(cursor[0] < VGA_WIDTH-1) 
            cursor[0]=+1;
        else{ 
        

        }
        cursor[1]=0; 
    }


}
