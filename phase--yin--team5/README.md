[![Open in Visual Studio Code](https://classroom.github.com/assets/open-in-vscode-2e0aaae1b6195c2367325f4f02e2d04e9abb55f0b24a779b69b11b9e10269abc.svg)](https://classroom.github.com/online_ide?assignment_repo_id=15820720&assignment_repo_type=AssignmentRepo)
# This is a start code for CSC/CPE 159 operating system pragmatics.

Team member:
* Illya Gordyy,
* Yousif Alrubaye,
* Nikolay Chkhaylo

## Feedback for Phase 5.0: 

#### Strength
* Implementation: Completed all required functions.
* The OS can pass all our test case. 


### Grading Phase 4
Sleep and Exit call don't work/are not implmeneted

#### issues for current work
* src/syscall.c:
    - incorrect implementation: \_syscall1, \_syscall2, \_syscall3.
    - For example, \_syscall3. You're currently using: "%eax", but you should also add "%ebx", "%ecx", "%edx" since you are modifying those registers in the assembly code. If you don't mark them as clobbered, the compiler will not know that it needs to acoid using those registers for other purposes after the assembly block. 
* Sleep function: You should convert sleep seconds to our system timer ticks. Our system timer tick is 10 ms. 
* src/scheduler.c: scheduler\_timer 
    - some logic bugs. Please check carefully. 
* src/ksyscall.c:
    - ksyscall\_irq\_handler function: you should return rc after you call sleep function because sleeping process is no longer active.
 
### Overall
* I submitted a PR to your repository. You can find the solution to fix your code issues.  
