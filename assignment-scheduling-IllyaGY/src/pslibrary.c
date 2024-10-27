#include <stdio.h>
#include <string.h>
#include "pslibrary.h"

#define READY  0
#define RUNNING  1
#define WAITING  2
#define DONE  3


static char stateChars[] = {'r','R','w',0};


/* *********************************fcfs ***************************************/
void fcfs(char *s1, char *s2, int x1, int y1, int z1,
                                            int x2, int y2, int z2) {                                         
   int i;                                   /* next string position (time) */
   int state1 = READY;                            /* start with both ready */
   int state2 = READY;
   int cpuLeft1 = x1;                       /* P1 next CPU burst remaining */
   int cpuLeft2 = x2;                       /* P2 next CPU burst remaining */
   int ioLeft1 = y1;        /* P1 next IO burst remaining, 0 if no more IO */
   int ioLeft2 = y2;        /* P2 next IO burst remaining, 0 if no more IO */
   for (i=0; (state1 != DONE) || (state2 != DONE); i++) {
      /* running process completes its CPU burst */
      if ((state1 == RUNNING) && (cpuLeft1== 0)) {
         if (ioLeft1 == 0) {
            state1 = DONE;
            s1[i] = stateChars[state1];            /* terminate the string */
         }
         else {
            /* Can the current process use I/O immediately? */
            if(state2 == WAITING) ioLeft1+=ioLeft2;  
            state1 = WAITING;
         }
      }  
      
      else if ((state2 == RUNNING) && (cpuLeft2 == 0) ) {
         if (ioLeft2 == 0) {
            state2 = DONE;
            s2[i] = stateChars[state2];            /* terminate the string */
         }
         else {
            /* Can the current process use I/O immediately? */
            if(state1 == WAITING) ioLeft2+=ioLeft1;  
            state2 = WAITING;
         }
      }  
      /* handle IO complete */
      if ((state1 == WAITING) && (ioLeft1 == 0)) {
         state1 = READY;
         cpuLeft1 = z1;
      }  
      if ((state2 == WAITING) && (ioLeft2 == 0)) {
         state2 = READY;
         cpuLeft2 = z2; 
      }  
      /* if both ready, depends on algorithm */
      if ( (state1 == READY) && (state2 == READY)) {
         state1 = RUNNING;  
      }  
      /* handle one ready and CPU available */
      else if ( (state1 == READY) && (state2 != RUNNING)) {
         state1 = RUNNING;
      }  
      else if ( (state2 == READY) && (state1 != RUNNING)) {
         state2 = RUNNING;
      }  
      /* insert chars in string, but avoid putting in extra string terminators */
      if (state1 != DONE)
         s1[i] = stateChars[state1];
      if (state2 != DONE)
         s2[i] = stateChars[state2];
      /* decrement counts */
      if (state1 == RUNNING)
         cpuLeft1--;
      if (state1 == WAITING && (ioLeft1 > 0))
         ioLeft1--;
      if (state2 == RUNNING)
         cpuLeft2--;
      if (state2 == WAITING && (ioLeft2 > 0))
         ioLeft2--;
   }                                
   
               
   /* end of main for loop */
}

/* *********************************sjf ***************************************/

void sjf(char *s1, char *s2, int x1, int y1, int z1,
                                            int x2, int y2, int z2) {
                                
   int i;                                   /* next string position (time) */
   int state1 = READY;                            /* start with both ready */
   int state2 = READY;
   int cpuLeft1 = x1;                       /* P1 next CPU burst remaining */
   int cpuLeft2 = x2;                       /* P2 next CPU burst remaining */
   int ioLeft1 = y1;        /* P1 next IO burst remaining, 0 if no more IO */
   int ioLeft2 = y2;        /* P2 next IO burst remaining, 0 if no more IO */

   for (i=0; (state1 != DONE) || (state2 != DONE); i++) {
      /* running process completes its CPU burst */
      if ((state1 == RUNNING) && (cpuLeft1== 0)) {
         if (ioLeft1 == 0) {
            state1 = DONE;
            s1[i] = stateChars[state1];            /* terminate the string */
         }
         else {
            /* Can the current process use I/O immediately? */
            if(state2 == WAITING) ioLeft1+=ioLeft2;  
            state1 = WAITING;
         }
      }  
      
      else if ((state2 == RUNNING) && (cpuLeft2 == 0) ) {
         if (ioLeft2 == 0) {
            state2 = DONE;
            s2[i] = stateChars[state2];            /* terminate the string */
         }
         else {
            /* Can the current process use I/O immediately? */
            if(state1 == WAITING) ioLeft2+=ioLeft1;  
            state2 = WAITING;
         }
      }  
      /* handle IO complete */
      if ((state1 == WAITING) && (ioLeft1 == 0)) {
         state1 = READY;
         cpuLeft1 = z1;
      }  
      if ((state2 == WAITING) && (ioLeft2 == 0)) {
         state2 = READY;
         cpuLeft2 = z2;
      }  
      /* if both ready, depends on algorithm */
      if ( (state1 == READY) && (state2 == READY)) {
         if(cpuLeft1 <= cpuLeft2) state1 = RUNNING;
         else state2 = RUNNING;  
      }  
      /* handle one ready and CPU available */
      else if ( (state1 == READY) && (state2 != RUNNING)) {
         state1 = RUNNING;
      }  
      else if ( (state2 == READY) && (state1 != RUNNING)) {
         state2 = RUNNING;
      }  
      /* insert chars in string, but avoid putting in extra string terminators */
      if (state1 != DONE)
         s1[i] = stateChars[state1];
      if (state2 != DONE)
         s2[i] = stateChars[state2];
      /* decrement counts */
      if (state1 == RUNNING)
         cpuLeft1--;
      if (state1 == WAITING && (ioLeft1 > 0))
         ioLeft1--;
      if (state2 == RUNNING)
         cpuLeft2--;
      if (state2 == WAITING && (ioLeft2 > 0))
         ioLeft2--;
   }              
                 

}

/* *********************************psjf ***************************************/
void psjf(char *s1, char *s2, int x1, int y1, int z1,
                                            int x2, int y2, int z2) {
                                          
   int i;                                   /* next string position (time) */
   int state1 = READY;                            /* start with both ready */
   int state2 = READY;
   int cpuLeft1 = x1;                       /* P1 next CPU burst remaining */
   int cpuLeft2 = x2;                       /* P2 next CPU burst remaining */
   int ioLeft1 = y1;        /* P1 next IO burst remaining, 0 if no more IO */
   int ioLeft2 = y2;        /* P2 next IO burst remaining, 0 if no more IO */

   for (i=0; (state1 != DONE) || (state2 != DONE); i++) {
      /* running process completes its CPU burst */
      if ((state1 == RUNNING) && (cpuLeft1== 0)) {
         if (ioLeft1 == 0) {
            state1 = DONE;
            s1[i] = stateChars[state1];            /* terminate the string */
         }
         else {
            /* Can the current process use I/O immediately? */
            if(state2 == WAITING) ioLeft1+=ioLeft2;  
            state1 = WAITING;
         }
      }  
      
      else if ((state2 == RUNNING) && (cpuLeft2 == 0) ) {
         if (ioLeft2 == 0) {
            state2 = DONE;
            s2[i] = stateChars[state2];            /* terminate the string */
         }
         else {
            /* Can the current process use I/O immediately? */
            if(state1 == WAITING) ioLeft2+=ioLeft1;  
            state2 = WAITING;
         }
      }  
      /* handle IO complete */
      if ((state1 == WAITING) && (ioLeft1 == 0)) {
         state1 = READY;
         cpuLeft1 = z1;
         if(state2 == RUNNING && cpuLeft1 < cpuLeft2) {
            state1 = RUNNING;
            state2 = READY; 
         }
      }  
      if ((state2 == WAITING) && (ioLeft2 == 0)) {
         state2 = READY;
         cpuLeft2 = z2;
         if(state1 == RUNNING && cpuLeft2 < cpuLeft1) {
            state2 = RUNNING;
            state1 = READY; 
         }
      }  
      /* if both ready, depends on algorithm */
      if ( (state1 == READY) && (state2 == READY)) {
         if(cpuLeft1 <= cpuLeft2) state1 = RUNNING;
         else state2 = RUNNING;  
      }  
      /* handle one ready and CPU available */
      else if ( (state1 == READY) && (state2 != RUNNING)) {
         state1 = RUNNING;
      }  
      else if ( (state2 == READY) && (state1 != RUNNING)) {
         state2 = RUNNING;
      }  
      /* insert chars in string, but avoid putting in extra string terminators */
      if (state1 != DONE)
         s1[i] = stateChars[state1];
      if (state2 != DONE)
         s2[i] = stateChars[state2];
      /* decrement counts */
      if (state1 == RUNNING)
         cpuLeft1--;
      if (state1 == WAITING && (ioLeft1 > 0))
         ioLeft1--;
      if (state2 == RUNNING)
         cpuLeft2--;
      if (state2 == WAITING && (ioLeft2 > 0))
         ioLeft2--;
   }  
}                                             
/* ********************************* RR ***************************************/
void rr(char *s1, char *s2, int quantum, int x1, int y1, int z1, int x2, int y2, int z2) {
   
   int i;                                   /* next string position (time) */
   int state1 = READY;                            /* start with both ready */
   int state2 = READY;
   int cpuLeft1 = x1;                       /* P1 next CPU burst remaining */
   int cpuLeft2 = x2;                       /* P2 next CPU burst remaining */
   int ioLeft1 = y1;        /* P1 next IO burst remaining, 0 if no more IO */
   int ioLeft2 = y2;        /* P2 next IO burst remaining, 0 if no more IO */
   int quant = quantum; 

   for (i=0; (state1 != DONE) || (state2 != DONE); i++) {
      /* running process completes its CPU burst */
      if ((state1 == RUNNING) && (cpuLeft1== 0)) {
         if (ioLeft1 == 0) {
            state1 = DONE;
            s1[i] = stateChars[state1];            /* terminate the string */
         }
         else {
            /* Can the current process use I/O immediately? */
            if(state2 == WAITING) ioLeft1+=ioLeft2;  
            state1 = WAITING;
         }
      }  
      
      else if ((state2 == RUNNING) && (cpuLeft2 == 0) ) {
         if (ioLeft2 == 0) {
            state2 = DONE;
            s2[i] = stateChars[state2];            /* terminate the string */
         }
         else {
            /* Can the current process use I/O immediately? */
            if(state1 == WAITING) ioLeft2+=ioLeft1;  
            state2 = WAITING;
         }
      }  

      
      /* handle IO complete */
      if ((state1 == WAITING) && (ioLeft1 == 0)) {
         state1 = READY;
         cpuLeft1 = z1;
      }  
      if ((state2 == WAITING) && (ioLeft2 == 0)) {
         state2 = READY;
         cpuLeft2 = z2;
      }  

      //quantum expired
      if ((state1 == RUNNING) && (quant <= 0) ) {
         if(state2 == READY){
            state2 = RUNNING;
            state1 = READY;
            quant = quantum;
         }
         else if(state2 == WAITING){
            quant+=quantum;

         }

          
      }  
      if ((state2 == RUNNING) && (quant <= 0) ) {
         if(state1 == READY){
            state1 = RUNNING;
            state2 = READY;
            quant = quantum;
         }
         else if(state1 == WAITING){
            quant+=quantum;

         }
      }  

      /* if both ready, depends on algorithm */
      if ( (state1 == READY) && (state2 == READY)) {
         state1 = RUNNING; 
      }  
      /* handle one ready and CPU available */
      else if ( (state1 == READY) && (state2 != RUNNING)) {
         state1 = RUNNING;
         quant = quantum; 
      }  
      else if ( (state2 == READY) && (state1 != RUNNING)) {
         state2 = RUNNING;
         quant = quantum; 
      }  
      /* insert chars in string, but avoid putting in extra string terminators */
      if (state1 != DONE)
         s1[i] = stateChars[state1];
      if (state2 != DONE)
         s2[i] = stateChars[state2];
      /* decrement counts */
      
      quant--; 
      if (state1 == RUNNING){
         cpuLeft1--;
      }
      if (state1 == WAITING && (ioLeft1 > 0))
         ioLeft1--;
      if (state2 == RUNNING){
         cpuLeft2--;
      }
      if (state2 == WAITING && (ioLeft2 > 0))
         ioLeft2--;
   }  
}
/* ********************************* generate_original_schedule ******************/
void generate_original_schedule(char *s1, char *s2, int x1, int y1, int z1, int x2, int y2, int z2){
   int i;
   for (i=0; i < x1; i++)
      s1[i] = 'R';
   for (i=0; i < y1; i++)
      s1[i+x1] = 'w';
   for (i=0; i < z1; i++)
      s1[i+x1+y1] = 'R';
   s1[x1+y1+z1] = '\0';
   for (i=0; i < x2; i++)
      s2[i] = 'R';
   for (i=0; i < y2; i++)
      s2[i+x2] = 'w';
   for (i=0; i < z2; i++)
      s2[i+x2+y2] = 'R';
   s2[x2+y2+z2] = '\0';
   
   printf("\n Original String S1: %s", s1);
   printf("\n Original String S2: %s", s2);
   printf("\n");
}

/* ********************************* display ****************************************/
void display(char *heading, char *s1, char *s2){
   int lens1 = strlen(s1);
   int lens2 = strlen(s2);
   int i;
   int longest;
   int countr1=0;
   int countr2=0;
   int countR1=0;
   int countR2=0;

   for (i = 0; i < lens1; i++)
   {
      if (s1[i]=='r')
      {
         countr1++;
      }
   }
   for (i = 0; i < lens2; i++) {
      if (s2[i]=='r')
      {
         countr2++;
      }
   }

   float avgr = (float)(countr1+countr2)/2;

   for (i = 0; i < lens1; i++) {
      if (s1[i]=='R')
      {
         countR1++;
      }
   }

   for (i = 0; i < lens2; i++) {
      if (s2[i]=='R')
      {
         countR2++;
      }
   }
   longest = lens1;
   if (lens2 > lens1)
   {
      longest = lens2;
   }

   float avgR =(float)(countR1+countR2)/longest;
   printf("\nSimulating %s and print scheduled sequence:\n %s\n %s\n", heading, s1, s2);
   printf("Average Ready Time: %.1f \nAverage Running Time: %.5f\n\n", avgr, avgR);
}
