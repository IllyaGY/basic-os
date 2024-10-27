/*
 * pslibrary.h
 *
 */

#ifndef SRC_PSLIBRARY_H_
#define SRC_PSLIBRARY_H_

/* Generate the original input schedule sequence */
void generate_original_schedule(char *s1, char *s2, int x1, int y1, int z1, int x2, int y2, int z2);

/* display the performance result */
void display(char *heading, char *s1, char *s2);

/* First Come First Served */
void fcfs(char *s1, char *s2, int x1, int y1, int z1, int x2, int y2, int z2);

/* Shortest Job First */
void sjf(char *s1, char *s2, int x1, int y1, int z1, int x2, int y2, int z2);

/* PREEMPTIVE Shortest Job First*/
void psjf(char *s1, char *s2, int x1, int y1, int z1, int x2, int y2, int z2);

/* Round-Robin Scheduling */
void rr(char *s1, char *s2, int quantum, int x1, int y1, int z1,int x2, int y2, int z2);

#endif /* SRC_PSLIBRARY_H_ */
