/**
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 *
 * Keyboard Functions
 */
#include "io.h"
#include "kernel.h"
#include "keyboard.h"

#define CHAR_BUFFER 52 // LETTER upper and lower


unsigned char scan_to_ascii[CHAR_BUFFER] = {};
unsigned char upper_case;     
unsigned char offset; 


/**
 * Initializes keyboard data structures and variables
 */
void keyboard_init() {
    upper_case = 0;


    kernel_log_info("Initializing keyboard driver");
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
    unsigned char c = KEY_NULL; 
    unsigned char i = inportb(KBD_PORT_STAT);
    if(i & 1){
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
    if(~(c & 0x80)){
        if(c == KEY_SHIFT_LEFT || c == KEY_SHIFT_RIGHT || c == KEY_CAPS_LOCK)
            upper_case = (upper_case) ? 0 : 0x20;
        else if(scan_to_ascii[c])
            return scan_to_ascii[c]-upper_case; 
        else if(c == KEY_SHIFT_LEFT_REL || c == KEY_SHIFT_RIGHT_REL)
            upper_case = (upper_case) ? 0 : 0x20;
    }
    return KEY_NULL;
}
