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

#define CHAR_BUFFER 0x39+1 // LETTER lower case only (largest letter scan code becomes our size) 
#define NUM_BUFFER 0x0B+1 // NUMBERS  (largest number scan code becomes our size)

#define EXIT_LIMIT 3 // Number of CTRL + ESC needed to exit the spede-target

//Buffers for our letter and numbers
unsigned char scan_to_ascii[CHAR_BUFFER] = {};
unsigned char scan_to_num[NUM_BUFFER] = {};  

//booleans
unsigned char upper_case;
unsigned char ctrl_pressed;
unsigned char num_lock; 
unsigned char exit_state; 



/**
 * Initializes keyboard data structures and variables
 */
void keyboard_init() {
    //No upper_case by default 
    upper_case = 0;
    //ctrl released by default
    ctrl_pressed = 0;
    //Num Lock
    num_lock = 0; 
    //if exit_state == 0, exist the spede-target
    exit_state = EXIT_LIMIT; 




    kernel_log_info("Initializing keyboard driver");

    //Letters
    scan_to_ascii[0x1E] = 0x61; // a
    scan_to_ascii[0x30] = 0x62; // b
    scan_to_ascii[0x2E] = 0x63; // c
    scan_to_ascii[0x20] = 0x64; // d
    scan_to_ascii[0x12] = 0x65; // e
    scan_to_ascii[0x21] = 0x66; // f
    scan_to_ascii[0x22] = 0x67; // g
    scan_to_ascii[0x23] = 0x68; // h
    scan_to_ascii[0x17] = 0x69; // i
    scan_to_ascii[0x24] = 0x6A; // j
    scan_to_ascii[0x25] = 0x6B; // k
    scan_to_ascii[0x26] = 0x6C; // l
    scan_to_ascii[0x32] = 0x6D; // m
    scan_to_ascii[0x31] = 0x6E; // n
    scan_to_ascii[0x18] = 0x6F; // o
    scan_to_ascii[0x19] = 0x70; // p
    scan_to_ascii[0x10] = 0x71; // q
    scan_to_ascii[0x13] = 0x72; // r
    scan_to_ascii[0x1F] = 0x73; // s
    scan_to_ascii[0x14] = 0x74; // t
    scan_to_ascii[0x16] = 0x75; // u
    scan_to_ascii[0x2F] = 0x76; // v
    scan_to_ascii[0x11] = 0x77; // w
    scan_to_ascii[0x2D] = 0x78; // x
    scan_to_ascii[0x15] = 0x79; // y
    scan_to_ascii[0x2C] = 0x7A; // z
    scan_to_ascii[0x39] = ' '; 

    //Numbers
    scan_to_num[0x0B] = 0x30; // 0
    scan_to_num[0x02] = 0x31; // 1
    scan_to_num[0x03] = 0x32; // 2
    scan_to_num[0x04] = 0x33; // 3
    scan_to_num[0x05] = 0x34; // 4 
    scan_to_num[0x06] = 0x35; // 5
    scan_to_num[0x07] = 0x36; // 6
    scan_to_num[0x08] = 0x37; // 7
    scan_to_num[0x09] = 0x38; // 8
    scan_to_num[0x0A] = 0x39; // 9
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
    unsigned char i = inportb(KBD_PORT_STAT);
    //If available
    if(i & 1){
        //scan and decode
        c = keyboard_decode(keyboard_scan()); 
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
    //If key pressed
    if(~(c & 0x80)){    
        switch(c){
            //Either one of them sets upper_Case to 1
            case KEY_SHIFT_LEFT: 
                upper_case ^= 1; 
                return 0; 
            case KEY_SHIFT_RIGHT:
                upper_case ^= 1; 
                return 0; 
            case KEY_CAPS_LOCK: 
                upper_case ^= 1; 
                return 0; 
            case KEY_CTRL_LEFT: 
                ctrl_pressed ^= 1;  
                return 0; 
            case KEY_NUM_LOCK: 
                num_lock ^= 1; 
                return 0; 
            case KEY_HOME:
                vga_set_rowcol(vga_get_row(), 0);  
                return 0;
            case KEY_END: 
                vga_cursor_end();
                return 0; 
                
            default: 
                if(!ctrl_pressed){
                    if(scan_to_ascii[c])
                    //if upper_case == 1 multiply 0x20 by 1 and we get the upper case letter instead of a lower
                        return scan_to_ascii[c] - upper_case*0x20; 

                    //Numbers
                    else if(scan_to_num[c])
                        return scan_to_num[c];
                }

                //ELSE we r continuing to the second switch statement(for release keys)
            
        }
    }
    //If key released
    switch(c){
        case KEY_ESCAPE | 0x80:
            if(!exit_state) kernel_exit(); 
            exit_state--; 
            return 0; 
        exit_state = EXIT_LIMIT;
        //if ctrl_pressed stay on upper case checking else go to functions
        //if 'k' released and CTRL pressed do clear screen
        case K | 0x80: 
            if(ctrl_pressed) vga_clear();
            return 0; 
        case B | 0x80: 
            if(ctrl_pressed) kernel_break();
            return 0; 
        case P | 0x80: 
            if(ctrl_pressed) kernel_panic("Panic\0");
            return 0; 

        //Either one of them sets upper_Case to 1
        case KEY_SHIFT_LEFT | 0x80: 
            upper_case ^= 1; 
            return 0; 
        case KEY_SHIFT_RIGHT | 0x80:
            upper_case ^= 1; 
            return 0; 
        case KEY_CTRL_LEFT | 0x80: 
            ctrl_pressed ^= 1; 
            return 0; 
    }
    
    return KEY_NULL;
}
