// example.c
// use SPEDE cons_xxx helper functions

#define LOOP 1666000      // handy LOOP times .6 microseconds

#include <spede/stdio.h>  // cons_xxx to debug runtime
#include <spede/flames.h> // cons_xxx needs

int main() {
   int i;
   char ch = 'x';

   printf("Hello, World! (on Linux PC.)\n");

   cons_printf("Hello, World, too! (on Target PC.)\n");

   cons_putchar('A'); // cons_putchar() prints one character
   cons_putchar('B');
   cons_putchar('C');
  
   while(1) {
      if( cons_kbhit() ) {                    // poll keyboard, returns 1 if pressed
         ch = cons_getchar();                 // read in pressed key
         if (ch == 'x') 
            break; // break while loop
      }
      else {
         for(i=0; i<LOOP; i++) asm("inb $0x80"); // delays .6 microsecond
         cons_putchar('z');
      }
   }

   return 0;
}
