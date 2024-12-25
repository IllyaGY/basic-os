[![Open in Visual Studio Code](https://classroom.github.com/assets/open-in-vscode-2e0aaae1b6195c2367325f4f02e2d04e9abb55f0b24a779b69b11b9e10269abc.svg)](https://classroom.github.com/online_ide?assignment_repo_id=15820720&assignment_repo_type=AssignmentRepo)
# This is a start code for CSC/CPE 159 operating system pragmatics.

# Please update your teammate information, it will be easier for lecturer to grade and put assignment score to Canvas. 

Team member:
* Illya Gordyy,
* Yousif Alrubaye,
* Nikolay Chkhaylo

### Grading Phase 3

#### Functionality
* All function is implemented.

#### Trivial things and Need to improve
* src/kproc.c: kproc\_create function
    - 1. I noticed you use malloc and free function. If you don't have these two functions, how can you implement this funtions. Note: I Don't suggest to use malloc function at here. 
    - 2. Why don't you just use proc\_table[next\_pid] directly to initialize process? 
    - 3. line 149: proc\_table[id] = \*proc.
        * Please carefully think this code, if you can find error.
        * This is a hidden bug. 
    - 4. what is the difference between id and next\_pid in your process?
* I hope your team has more peer review. The contributor shouldn't be a merger.  
    - Illya's pull request should be handled by Nikolay or Yousif. 
* I hope Nicolay and Yousif can contribute more code at Phase 4 and Phase 5.
    - Code review is also a part of contribution. 
    - Or what kind of discussion do they involve? Please mention at next Phase. 

#### Overall
* Your team show some progress. Please keep a good work. Thanks. 
