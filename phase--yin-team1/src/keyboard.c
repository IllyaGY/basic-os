/**
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 *
 * Keyboard Functions
 */
 


#include "io.h"
#include "vga.h"
#include "kernel.h"
#include "keyboard.h"

#define CTRL_TOGGLE             1
#define SHIFT_TOGGLE            (1 << 2)
#define ALT_TOGGLE              (1 << 2)
#define NUMPAD_TOGGLE           (1 << 3)
#define INSERT_TOGGLE           (1 << 4)
#define CAPSLOCK_TOGGLE         (1 << 5)



#define BUFFER_SIZE 0x52+1 // LETTER lower case only (largest letter scan code becomes our size) 
#define ARROW_SIZE 0x50+1 //

unsigned char status;
                            // 0 bit - ctrl
                            // 1 bit - shift
                            // 2 bit - alt 
                            // 3 bit - numpad
                            // 4 bit - insert
                            // 5 bit - caps lock





//Buffer for our normal keys
unsigned char scan_to_ascii[BUFFER_SIZE] = {
    [0x1E] = 0x61, // a
    [0x30] = 0x62, // b
    [0x2E] = 0x63, // c
    [0x20] = 0x64, // d
    [0x12] = 0x65, // e
    [0x21] = 0x66, // f
    [0x22] = 0x67, // g
    [0x23] = 0x68, // h
    [0x17] = 0x69, // i
    [0x24] = 0x6A, // j
    [0x25] = 0x6B, // k
    [0x26] = 0x6C, // l
    [0x32] = 0x6D, // m
    [0x31] = 0x6E, // n
    [0x18] = 0x6F, // o
    [0x19] = 0x70, // p
    [0x10] = 0x71, // q
    [0x13] = 0x72, // r
    [0x1F] = 0x73, // s
    [0x14] = 0x74, // t
    [0x16] = 0x75, // u
    [0x2F] = 0x76, // v
    [0x11] = 0x77, // w
    [0x2D] = 0x78, // x
    [0x15] = 0x79, // y
    [0x2C] = 0x7A, // z
    [0x0C] = '-',
    [0x1A] = '[',
    [0x1B] = ']', 
    [0x27] = ';',
    [0x28] = '\'',
    [0x29] = '`', 
    [0x33] = ',',
    [0x34] = '.',
    [0x35] = '/',
    [0x2B] = '\\',
    [0x39] = ' ',
    [KEY_ALT_LEFT] = '\0',
    [KEY_INSERT] = '\0',
    [KEY_ESCAPE] = 0x1B,


    //Numbers
    [0x0B] = 0x30, // 0
    [0x02] = 0x31, // 1
    [0x03] = 0x32, // 2
    [0x04] = 0x33, // 3
    [0x05] = 0x34, // 4 
    [0x06] = 0x35, // 5
    [0x07] = 0x36, // 6
    [0x08] = 0x37, // 7
    [0x09] = 0x38, // 8
    [0x0A] = 0x39  // 9



};


unsigned char scan_to_hidden_ascii[BUFFER_SIZE] = {
    [0x1E] = 0x41, // a
    [0x30] = 0x42, // b
    [0x2E] = 0x43, // c
    [0x20] = 0x44, // d
    [0x12] = 0x45, // e
    [0x21] = 0x46, // f
    [0x22] = 0x47, // g
    [0x23] = 0x48, // h
    [0x17] = 0x49, // i
    [0x24] = 0x4A, // j
    [0x25] = 0x4B, // k
    [0x26] = 0x4C, // l
    [0x32] = 0x4D, // m
    [0x31] = 0x4E, // n
    [0x18] = 0x4F, // o
    [0x19] = 0x50, // p
    [0x10] = 0x51, // q
    [0x13] = 0x52, // r
    [0x1F] = 0x53, // s
    [0x14] = 0x54, // t
    [0x16] = 0x55, // u
    [0x2F] = 0x56, // v
    [0x11] = 0x57, // w
    [0x2D] = 0x58, // x
    [0x15] = 0x59, // y
    [0x2C] = 0x5A, // z
    [0x0C] = '_',
    [0x1A] = '{',
    [0x1B] = '}', 
    [0x27] = ':',
    [0x28] = '"',
    [0x29] = '~', 
    [0x33] = '<',
    [0x34] = '>',
    [0x35] = '?',
    [0x2B] = '|',
    [0x39] = ' ',
    [KEY_ALT_LEFT] = '\0',
    [KEY_INSERT] = '\0',
    [KEY_ESCAPE] = 0x1B,

    [0x29] = '~',
    [0x0B] = ')', 
    [0x02] = '!', 
    [0x03] = '@', 
    [0x04] = '#', 
    [0x05] = '$', 
    [0x06] = '%',
    [0x07] = '^', 
    [0x08] = '&',
    [0x09] = '*', 
    [0x0A] = '('  
};


unsigned char function_keys[BUFFER_SIZE] = {
    [KEY_CTRL] = CTRL_TOGGLE, 
    [KEY_ALT_LEFT] = ALT_TOGGLE, 
    [KEY_NUM_LOCK] = NUMPAD_TOGGLE,
    [KEY_SHIFT_LEFT] = SHIFT_TOGGLE, 
    [KEY_SHIFT_RIGHT] = SHIFT_TOGGLE,
    [KEY_CAPS_LOCK] = CAPSLOCK_TOGGLE,
    [KEY_INSERT] = INSERT_TOGGLE

};
//Arrow key movements
int arrows[ARROW_SIZE][2]={
    [KEY_UP][0]=-1,
    [KEY_UP][1]=0,

    [KEY_RIGHT][0]=0,
    [KEY_RIGHT][1]=1,

    [KEY_DOWN][0] = 1,
    [KEY_DOWN][1] = 0,
    
    [KEY_LEFT][0] = 0,
    [KEY_LEFT][1] = -1
};





/**
 * Initializes keyboard data structures and variables
 */
void keyboard_init() {
    
    //Status is 0 by default
    status = 0x00 | INSERT_TOGGLE;  //Insert will be ON by default
    //No upper_case by default 
    kernel_log_info("Initializing keyboard driver");
    
}

/**
 * Scans for keyboard input and returns the raw character data
 * @return raw character data from the keyboard
 */
unsigned int keyboard_scan(void) {
    return inportb(KBD_PORT_DATA);
}

/**
 * Polls for a keyboard character to be entered.
 *
 * If a keyboard character data is present, will scan and return
 * the decoded keyboard output.
 *
 * @return decoded character or KEY_NULL (0) for any character
 *         that cannot be decoded
 */
unsigned int keyboard_poll(void) {
    //Accessing the status
    unsigned char c = KEY_NULL; 
    unsigned char status = inportb(KBD_PORT_STAT);
    //If available
    if(status & 1){
        //scan and decode
        c = keyboard_scan();
        c = keyboard_decode(c); 
    }
    return c;
}

/**
 * Blocks until a keyboard character has been entered
 * @return decoded character entered by the keyboard or KEY_NULL
 *         for any character that cannot be decoded
 */
unsigned int keyboard_getc(void) {
    unsigned int c = KEY_NULL;
    while ((c = keyboard_poll()) == KEY_NULL);
    return c;
}

/**
 * Processes raw keyboard input and decodes it.
 *
 * Should keep track of the keyboard status for the following keys:
 *   SHIFT, CTRL, ALT, CAPS, NUMLOCK
 *
 * For all other characters, they should be decoded/mapped to ASCII
 * or ASCII-friendly characters.
 *
 * For any character that cannot be mapped, KEY_NULL should be returned.
 *
 * If *all* of the status keys defined in KEY_KERNEL_DEBUG are pressed,
 * while another character is entered, the kernel_debug_command()
 * function should be called.
 */
unsigned int keyboard_decode(unsigned int c) { 
    switch(c){
        //Released keys
        case (KEY_SHIFT_LEFT | 0x80): 
        case (KEY_SHIFT_RIGHT | 0x80):
        case (KEY_CTRL | 0x80):
            status ^= function_keys[c & ~0x80];
            break;

        //Pressed keys
        case KEY_CAPS_LOCK: 
        case KEY_SHIFT_LEFT: 
        case KEY_SHIFT_RIGHT:
        case KEY_CTRL:  
        case KEY_NUM_LOCK: 
        case KEY_INSERT: 
            status ^= function_keys[c];
            break;    

        //Arrows that the cursor around         
        case KEY_LEFT:
        case KEY_RIGHT:
        case KEY_UP:
        case KEY_DOWN:
            vga_set_rowcol(vga_get_row()+arrows[c][0],vga_get_col()+arrows[c][1]); 
            break;

        case KEY_DELETE:
            vga_delete_char();  // Implement this in vga.c to handle the delete operation
            break;
        case KEY_BACKSPACE:
            // Move the cursor one position back and delete the character
            // Check if the cursor is at the beginning of the current row
            if (vga_get_col() == 0) {
                // Move to the previous row if not at the top
                if (vga_get_row() > 0) {
                    vga_set_rowcol(vga_get_row() - 1, VGA_WIDTH - 1); // Move to the last column of the previous row
                }
            } else {
                // Move the cursor one position back
                vga_set_rowcol(vga_get_row(), vga_get_col() - 1);
            }
            // Delete the character
            vga_delete_char();
            break;
        //Set to the beginning of the line or the end
        case KEY_HOME:
            vga_set_rowcol(0, 0);  
            break;


        
        default:  
            if ((status & CTRL_TOGGLE) && (status & SHIFT_TOGGLE) && 
                (status & ALT_TOGGLE) && (status & CAPSLOCK_TOGGLE) && 
                (status & NUMPAD_TOGGLE)) {
                kernel_panic("test panic"); //causes panic when all of the keys are ON
                break;
            }


            if(status & CTRL_TOGGLE){
                kernel_command(scan_to_ascii[c]);
                break;
            }

            if (status & SHIFT_TOGGLE) {
                // If Shift is active, return hidden ASCII map for symbols and numbers
                if ((status & CAPSLOCK_TOGGLE) && (c >= 0x1E && c <= 0x2C)) {
                    // If both Shift and Caps Lock are active, return lowercase letters
                    return scan_to_ascii[c];  // Lowercase letter due to Caps + Shift
                }
                return scan_to_hidden_ascii[c];  // Return hidden map (symbols, etc.)
            }

            // If only Caps Lock is active, affect letters (uppercase)
            if (status & CAPSLOCK_TOGGLE) {
                if (scan_to_ascii[c] >= 0x41 && scan_to_ascii[c] <= 0x7A) {
                    return scan_to_hidden_ascii[c];  // Return uppercase letters
                }
            }

            // If SHIFT is active, return the shifted version
            if (status & SHIFT_TOGGLE) {
                return scan_to_hidden_ascii[c];
            }
                        
            return scan_to_ascii[c];
               
    }
    
    return KEY_NULL;
}
unsigned char keyboard_get_insert(){
    return status & INSERT_TOGGLE;
}