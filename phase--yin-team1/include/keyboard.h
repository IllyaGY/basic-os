/**
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 *
 * Keyboard Functions
 */
#ifndef KEYBOARD_H
#define KEYBOARD_H

// Keyboard data port
#define KBD_PORT_DATA           0x60

// Keyboard status port
#define KBD_PORT_STAT           0x64

// Special key definitions
#define KEY_NULL                0x00
#define KEY_ESCAPE              0x01

// Directional Keys
#define KEY_HOME                0x47
#define KEY_END                 0x4F
#define KEY_UP                  0x48
#define KEY_DOWN                0x50
#define KEY_LEFT                0x4B
#define KEY_RIGHT               0x4D
#define KEY_PAGE_UP             0xE6
#define KEY_PAGE_DOWN           0xE7
#define KEY_INSERT              0x52
#define KEY_DELETE              0xE9
#define KEY_BACKSPACE           0x0E


//Ctrl, Alt, NumLock 
#define KEY_CTRL_LEFT           0x1D
#define KEY_ALT_LEFT            0x38
#define KEY_NUM_LOCK            0x45

//Shift and Caps Lock
#define KEY_SHIFT_LEFT          0x2A    
#define KEY_SHIFT_RIGHT         0x36     

#define KEY_CAPS_LOCK           0x3A

// Function Keys
#define KEY_F1                  0xF1
#define KEY_F2                  0xF2
#define KEY_F3                  0xF3
#define KEY_F4                  0xF4
#define KEY_F5                  0xF5
#define KEY_F6                  0xF6
#define KEY_F7                  0xF7
#define KEY_F8                  0xF8
#define KEY_F9                  0xF9
#define KEY_F10                 0xFA
#define KEY_F11                 0xFB
#define KEY_F12                 0xFC


#ifndef ASSEMBLER

/**
 * Initializes keyboard data structures and variables
 */
void keyboard_init(void);

/**
 * Scans for keyboard input and returns the raw character data
 * @return raw character data from the keyboard
 */
unsigned int keyboard_scan(void);

/**
 * Polls for a keyboard character to be entered.
 *
 * If a keyboard character data is present, will scan and return
 * the decoded keyboard output.
 *
 * @return decoded character or KEY_NULL (0) for any character
 *         that cannot be decoded
 */
unsigned int keyboard_poll(void);

/**
 * Blocks until a keyboard character has been entered
 * @return decoded character entered by the keyboard or KEY_NULL
 *         for any character that cannot be decoded
 */
unsigned int keyboard_getc(void);



unsigned char keyboard_get_insert();
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
 */
unsigned int keyboard_decode(unsigned int c);
#endif
#endif
