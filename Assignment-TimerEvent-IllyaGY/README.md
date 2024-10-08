[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/mmtD-63g)
### Please update your name at here

Illya Gordyy

<h3>Assignment - Timer Event</h3>

<p>
The objective of this assigment is to practice how to
program to use the timer interrupt events.
</p>

Write your own code following class lectures and this pseudo code:
```
test
```
Compile into <i>MyOS.dli</i>
```
make
```
Start the target window, and you can see the file spede.sock in current folder:
```
spede-target
```
Run it under GDB.
```
spede-run -d
```

### Assignment requirement
<ol type=a>
<li>The timer-driven routine should output your name, character by character.
<li>The output sequence is at every 3/4 seconds one character is shown.
<li>The output location is at the center of the screen on the target PC.
<li>After the full name is shown, it then erases it and restart.
</ol>

### Example code

See "example.c" which shows a few useful given SPEDE I/O functions you can use.

### Submission

After getting it to work as described above, you only need to modify these two files: <b>src/main.c, src/handlers.c</b>. 
