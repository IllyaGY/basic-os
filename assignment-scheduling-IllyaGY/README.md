### Please update your name at here
Illya Gordyy

### This is the assignment for CPU scheduling.
using proto.txt as example to finish the four scheduling algorithm, including First come first served (FCFS),
Shortest job first (SJF),Preemptive shortest job first (PSJF), Round Robin (RR). 

### how to compile your code
```
make
```

### how to test your code
```
make test
```

Example output:
```
./build/Assign_sched 2 10 3 5 5 2 3
 
 Assignment scheduling program was written by First Middle Family 
inputs: 2 10 3 5 5 2 3 

 Original String S1: RRRRRRRRRRwwwRRRRR
 Original String S2: RRRRRwwRRR

Simulating Fcfs and print scheduled sequence:
 RRRRRRRRRRwwwrrRRRRR
 rrrrrrrrrrRRRRRwwrrrRRR
Average Ready Time: 7.5 
Average Running Time: 1.00000


Simulating SJF and print scheduled sequence:
 rrrrrRRRRRRRRRRwwwRRRRR
 RRRRRwwrrrrrrrrRRR
Average Ready Time: 6.5 
Average Running Time: 1.00000


Simulating PSJF and print scheduled sequence:
 rrrrrRRrrrRRRRRRRRwwwRRRRR
 RRRRRwwRRR
Average Ready Time: 4.0 
Average Running Time: 0.88462


Simulating RR and print scheduled sequence:
 RRrrRRrrRRrRRRRwwwRRRRR
 rrRRrrRRrrRwwrrRRR
Average Ready Time: 6.5 
Average Running Time: 1.00000
```

### how to generate your result
```
make run
```


### submission
Before submitting your code, please clean your folder

```
make clean
```

You only need to update the file src/pslibrary.c, and the result folder. The result folder should have 5 text files.  
