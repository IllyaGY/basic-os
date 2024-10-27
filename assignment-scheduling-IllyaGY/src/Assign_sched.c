#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pslibrary.h"

char *your_name = "Illya Gordyy";

int main(int argc, char **argv) {
    int i;

    printf(" \n Assignment scheduling program was written by %s \n", your_name);
    
    if (argc != 8) {
        printf("\n \t \t Error: Please enter 7 arguments. Program Terminated !! \n ");
        return 1;
    }
    else
    {
        printf("inputs: ");
        for (i = 1; i < 8; i++) {
            printf("%s ",argv[i]);
        }

        printf("\n");
        int size=0;
        for (i = 2; i < 8; i++) {
            size = size + atoi(argv[i]);
        }


        int x1, y1, z1, x2, y2, z2;
        int quatum = atoi(argv[1]);
        x1 = atoi(argv[2]); 
        y1 = atoi(argv[3]);
        z1 = atoi(argv[4]);
        x2 = atoi(argv[5]);
        y2 = atoi(argv[6]);
        z2 = atoi(argv[7]);

        char s1[size], s2[size];
        // Generate the original string for process 1 and process 2
        generate_original_schedule(s1, s2, x1, y1, z1, x2, y2, z2);

        /* test fcfs */
        fcfs(s1, s2, x1, y1, z1, x2, y2, z2);
        display("Fcfs", s1, s2);

        /* test sjf */
        sjf(s1, s2, x1, y1, z1, x2, y2, z2);
        display("SJF", s1, s2);

        /* test psjf */
        psjf(s1, s2, x1, y1, z1, x2, y2, z2);
        display("PSJF", s1, s2);

        /* test rr */
        rr(s1, s2, quatum, x1, y1, z1, x2, y2, z2);
        display("RR", s1, s2);

        return 0;
    }
}
