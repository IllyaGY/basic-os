## Assignment 5: Virtual Memory

### Introduction

* Since we have successfully set up different processes within a designated DRAM memory segment, we will now allocate runtime space in the virtual address space, allowing a process to expand from 1 GB to 2 GB-1 (0x40000000 to 0x7FFFFFFF) bytes.

### Objectives & Outcomes

#### The objectives for this assignment are to ensure that you:

* Can relocate the process’s data section and stack section to a virtual address space.
* Can use protected mode to prevent user processes from directly accessing hardware resources.
* Can apply page allocation to assign new pages to a process.
* Can practice page translation.

#### Once this assignment is completed, you should be able to explain:

* Why each user process can start running from the same virtual memory address.
* Why user processes don’t have permission to access hardware directly.
* The principles of page translation.

### Background Knowledge

* During process runtime, the virtual address space is backed by actual DRAM pages. The CPU component responsible for translating virtual addresses is the Memory Management Unit (MMU), which dynamically translates each address requested by the CPU—whether for instructions or data—during each cycle of instruction fetch and execution.
* The MMU continuously relies on translation tables. A set of these tables, initialized by the SPEDE setup code (the "boot-loader"), has been in use by the kernel since bootstrap. Some of these tables must be incorporated into the translation tables for a new process to enable it to make kernel service calls.

### Address Translation

* The Intel MMU for 32-bit virtual addresses uses a two-tier memory paging system:
    - The leftmost 10 bits are used as the entry index into the page directory to locate a page table address.
    - The next 10 bits serve as the entry index into the page table to find a physical address that points to a code or stack page.
    - The remaining 12 bits are used for control and permission settings for the code or stack page.

* The Process Control Block (PCB) needs a new field: `pagetable`
    - This field contains the address of the main address-translation table, which is set to configure the CPU’s MMU before the `Loader()` call, allowing the process to run in virtual memory. During the bootstrap phase, this field is initialized to zero and later updated by `NewUserProcHandler()` when creating a process in virtual space.

* `UserHandler()` will begin by allocating five memory pages to set up:
    - One page directory
    - Two page tables (one for code pages, another for stack pages)
    - A code page
    - A stack page  
* Each page directory or table has 1,024 entries, with each entry being 4 bytes in size.

* Two entries in the page directory point to the code and stack page tables, each containing the actual addresses of two allocated memory pages. Since the virtual process space starts at 1 GB (0x40000000), the first 10 bits yield 256 (decimal), which serves as the index in the main table for the real address of the code page table. Similarly, the stack ends at 2 GB - 1, with its first 10 bits equating to 511 (decimal), which is the index for the stack page table’s real address in the main directory.

* The main address translation table must also copy its first four entries from the current kernel translation table. The `get_cr3()` function (from SPEDE) can retrieve the address of the kernel MMU (`kernel_MMU`) to facilitate this copying during the bootstrap phase. `NewUserProcHandler()` can then use this data to enable processes to make kernel service calls.

* In the code page table, the first entry should point to the real address of a memory page containing the executable loaded from file storage.
* In the stack page table, the last entry should be set to the real address of another memory page for housing the initial trapframe of a new process.
* The `EIP` in the trapframe will be initialized with the virtual address 1 GB (0x40000000). The trapframe address for the PCB of this new process will be at 2 GB - 1, offset by the size of the trapframe type.

* To set the MMU, use the SPEDE function `set_cr3(int)`. This function should be called before loading a new process to execute, as well as upon process exit (to revert to the kernel's translation table).

### Setting page table entry
* Since memory pages are intervals of 4K bytes, only the first 20 bits in the addresses actually change. The last 12 bits are always zeros. So these bits are used as flags to notify the MMU the attributes of the memory page, during the hardware address translation. The right most seven flags (bits) are important to a OS:
    - PRESENT, 0: page not present, 1: page present.
    - RO / RW , 0: page is read-only, 1: read-writable
    - USER MODE, 0: superuser to access, 1: user can access
    - CACHE POLICY, 0: write back; 1: write-through (if cache-use set)
    - CACHE USE, 0: page content not to be cached, 1: to be cached
    - ACCESSED, 0: entry never looked up by MMU, 1: accessed
    - DIRTY, 0: page content not changed/written, 1: changed/written

* In each valid entry, we will set both flags RW and Present to 1 (others 0). This must be done correctly while setting new table entries.

* The target PC would reboot whenever an address translation cannot be arried out (due to any conflicts from these flags). The page being "not present," RW on RO pages, user/super mode, etc., are all possible causes. Use GDB to verify both the PCB and tables are correctly done at the end of the NewUserProcHandler().

### Grading Metrics

#### No credit will be given for the following:
* Source code is submitted past the due data
* Source code cannpt be compiled with the original/included Makefile
* Source code has not been materially modified from the base source code or provide template
* Source code has been plagiarized, duplicated, or otherwise demonstrated to not be originally developed. 

#### The following deductions may be made:
* Up to 20 points for each missing requirement
* Up to 10 points for modification of header files (unless indicated otherwise).
* Minimum 5 points for each incorrectly implemented functional requirement
* Minimum 5 points for each occurrence of programming errors
    - Invalide use of pointers
    - Invalide memory access
    - Buffer overflows
    - Segment Error
    - Compile Error

### Required Checklist
* src/handler.c
    - complete NewUserProcHandler function
    - complete PageFaultHandler function
 
### Submission
* You only need to modify the src/handler.c file. 

