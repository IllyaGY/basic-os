#include <spede/stdarg.h>
#include <spede/stdio.h>


#include "bit.h"
#include "io.h"
#include "kernel.h"
#include "vga.h"
#include "keyboard.h"

typedef unsigned char u8_t;


// Cursor position variables
static int vga_row = 0;
static int vga_col = 0;
static int vga_fg_color = VGA_COLOR_LIGHT_GREY;
static int vga_bg_color = VGA_COLOR_BLACK;
static bool cursor_enabled = false;

/**
 * Forward Declarations
 */
void vga_cursor_update(void);

/**
 * Initializes the VGA driver and configuration
 */
void vga_init(void) {
    kernel_log_info("Initializing VGA driver");

    // Clear the screen
    vga_clear();
}

/**
 * Clears the VGA output and sets the background and foreground colors
 */
void vga_clear(void) {
    for (int row = 0; row < VGA_HEIGHT; row++) {
        for (int col = 0; col < VGA_WIDTH; col++) {
            VGA_BASE[row * VGA_WIDTH + col] = VGA_CHAR(vga_bg_color, vga_fg_color, '\0');
        }
    }
    vga_row = 0;
    vga_col = 0;
    vga_cursor_update();
}

/**
 * Clears the background color for all characters and sets to the specified value
 */
void vga_clear_bg(int bg) {
    vga_bg_color = bg;
    vga_clear();
}

/**
 * Clears the foreground color for all characters and sets to the specified value
 */
void vga_clear_fg(int fg) {
    vga_fg_color = fg;
    vga_clear();
}

/**
 * Enables the VGA text mode cursor
 */
void vga_cursor_enable(void) {
    u8_t cursor_start = inportb(0x0A); // Mask off unused bits
    outportb(0x3D5, cursor_start & ~0x20); // Clear bit 5 to enable the cursor

    cursor_enabled = true;
    vga_cursor_update();
}

/**
 * Disables the VGA text mode cursor
 */
void vga_cursor_disable(void) {
    u8_t cursor_start = inportb(0x0A);
    outportb(0x0A, cursor_start | 0x20); // Set bit 5 to disable the cursor

    cursor_enabled = false;
    vga_cursor_update();
}

/**
 * Updates the VGA text mode cursor position
 */
void vga_cursor_update(void) {
    if (!cursor_enabled) {
        return;
    }

    unsigned short cursor_pos = vga_row * VGA_WIDTH + vga_col;
    outportb(0x0F,(u8_t)(cursor_pos & 0xFF)); // Low byte
    outportb(0x0E,(u8_t)((cursor_pos >> 8) & 0xFF)); // High byte
}

/**
 * Prints a character on the screen at the current cursor position
 */
void vga_putc(unsigned char c) {
    if (c == '\n') {
        vga_row++;
        vga_col = 0;
    } else if (c == '\b') {
        if (vga_col > 0) vga_col--;
    } else {
        if(!keyboard_get_insert()){
            int index = vga_row * VGA_WIDTH + vga_col;
            while(VGA_BASE[index] != '\0'){
                index++;
            }
            int max = index;
            index = vga_row * VGA_WIDTH + vga_col; 
            while(max > index) {
                VGA_BASE[max] = VGA_BASE[max-1];
                max--;
            }    
                

        }
        VGA_BASE[vga_row * VGA_WIDTH + vga_col] = VGA_CHAR(vga_bg_color, vga_fg_color, c);
        vga_col++;
    }

    if (vga_col >= VGA_WIDTH) {
        vga_col = 0;
        vga_row++;
    }
    if (vga_row >= VGA_HEIGHT) {
        vga_row = 0; // Wrap around
    }

    vga_cursor_update();
}

/**
 * Prints a string on the screen at the current cursor position
 */
void vga_puts(char *s) {
    while (*s) {
        vga_putc(*s++);
    }
}

/**
 * Prints a character on the screen at the specified row/column position
 * with the specified background/foreground colors
 */
void vga_putc_at(int row, int col, int bg, int fg, unsigned char c) {
    if (row >= 0 && row < VGA_HEIGHT && col >= 0 && col < VGA_WIDTH) {
        VGA_BASE[row * VGA_WIDTH + col] = VGA_CHAR(bg, fg, c);
    }
}

/**
 * Prints a string on the screen at the specified row/column position
 * with the specified background/foreground colors
 */
void vga_puts_at(int row, int col, int bg, int fg, char *s) {
    while (*s) {
        vga_putc_at(row, col++, bg, fg, *s++);
    }
}

/**
 * Sets the background color for future print operations
 */
void vga_set_bg(int bg) {
    vga_bg_color = bg;
}

/**
 * Gets the current background color
 * @return background color value
 */
int vga_get_bg(void) {
    return vga_bg_color;
}

/**
 * Sets the foreground color for future print operations
 */
void vga_set_fg(int fg) {
    vga_fg_color = fg;
}

/**
 * Gets the current foreground color
 * @return foreground color value
 */
int vga_get_fg(void) {
    return vga_fg_color;
}

/**
 * Gets the current column position
 */
int vga_get_col(void) {
    return vga_col;
}

/**
 * Gets the current row position
 */
int vga_get_row(void) {
    return vga_row;
}


void vga_delete_char(void) {
    // Calculate the index of the current cursor position
    int index = vga_row * VGA_WIDTH + vga_col;

    // Make sure we don't delete past the last character on the screen
    if (index < VGA_WIDTH * VGA_HEIGHT - 1) {
        // Move all characters after the cursor position forward by one
        for (int i = index; i < VGA_WIDTH * VGA_HEIGHT - 1; i++) {
            VGA_BASE[i] = VGA_BASE[i + 1];
        }

        // Clear the last character in the buffer
        VGA_BASE[VGA_WIDTH * VGA_HEIGHT - 1] = VGA_CHAR(vga_bg_color, vga_fg_color, ' ');
    }

    // Update the cursor position (keep it where it is)
    vga_cursor_update();
}


//Sets row and column
void vga_set_rowcol(int row, int col){
    if(row < 0 || row >= VGA_HEIGHT)
        row = row < 0 ? 0 : VGA_HEIGHT-1; 
    if(col < 0 || col >= VGA_WIDTH)
        col = col < 0 ? 0 : VGA_WIDTH-1;
    vga_row=row;
    vga_col=col; 
    vga_cursor_update();
}