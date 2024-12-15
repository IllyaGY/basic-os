// Physical memory allocator, for user processes, kernel stacks, page-table pages
#include "spede.h"
#include "types.h"
#include "kalloc.h"
#include "kernel.h"
#include "queue.h"

uint32_t end = PAGE_STOP;





struct run {
    struct run *next;
};

struct {
    struct run *freelist;
}kmem;

// Process table allocator
queue_t page_allocator;


void kinit(void *vstart, void *vend){
    freerange(vstart, vend);
    for(int i = 0; i < GLOBAL_PAGE_LIST; i++)
    {
        used_pages_list[i].va_addr = 0;
        used_pages_list[i].pa_addr = 0;
        used_pages_list[i].pid = -1;
    } 
}

void freerange(void *vstart, void *vend){
    uint32_t p;
    p = PAGE_ALIGN((uint32_t)vstart);
    
    for(; p+PAGE_SIZE <= (uint32_t)vend; p+=PAGE_SIZE)
        kfree((void *)p);
}

// Free the page of physical memory pointed at by v

void kfree(void *va){
    struct run *r;

    if (((uint32_t)va % PAGE_SIZE != 0) || (uint32_t)va >= end){
        printf("kfree error!");
        return;
    }
    printf("put page back to kmem %x\n", (uint32_t)va); 
    memset(va, 1, PAGE_SIZE);

    r = (struct run *)va;
    r->next = kmem.freelist;
    kmem.freelist = r;
}

void* kalloc(void){
    struct run *r;
    if (!kmem.freelist->next) {
        printf("Error allocating a page, using FIFO \n");     
        get_first(0); 
    }
    r = kmem.freelist; 
    kmem.freelist = kmem.freelist->next;
    memset((char*)r, 0, PAGE_SIZE);
    printf("Kalloc a new page: 0x%x, kmem is %x\n", r, kmem.freelist, kmem.freelist->next);    
    return r;
}

void kpage_add(size_t * page_list, void * address){
    
    if(!address){
        return;
    }
    for(int i = 0; i < PROC_PAGE_LIST_SIZE; i++){
        if(!page_list[i]){
            page_list[i] = (size_t)address;
            return;
        }
    }

    // Todo: Here I don't think the logic is correct.
    // You should define a global varabile page_list to host the whole page. If you try to ask page from current process, I don't think it will be a good idea. Since we don't linked list, so we have a limited page_list at here. The process can give unlimited page theoritically. 
    // Our question is if I can't get page from kpagemake(), what should we do?    
}


size_t kpagemake(void){
    size_t kpgtbl = 0;

    kpgtbl = (size_t)kalloc();
    if(kpgtbl == 0) return 0; 
    memset((void *)kpgtbl, 0, PAGE_SIZE);
    return kpgtbl;
}




void manage_page(void *page, size_t va) {
    int i = 0; 
    while(i < GLOBAL_PAGE_LIST){
        if(used_pages_list[i].pa_addr == 0){
            printf("\nAdding page to global page table 0x0%x at %i\n", (void *)page, i); 
            used_pages_list[i].pa_addr = (size_t)page;
            used_pages_list[i].va_addr = (size_t)va;
            used_pages_list[i].pid = active_proc->pid;
            break; 
        }
        i++; 
    }
       
}

void manage_page_del(void *page) {
    int i = 0; 
    while(i < 10){
        if(page_delete[i] == 0){
            printf("\nAdding page for removal 0x0%x\n", (void *)page_delete); 
            page_delete[i] = (size_t)page; 
            break; 
        }
        i++; 
    }
       
}


void get_first(int t){
    //We need to allocate at least two pages for the kalloc to work properly 
    //ISSUE: might crash if there is not a lot pages
    //TODO: Rewrite recursion into something less heavy
    //Todo: You can use our queue to reimplement this part.
    if(t > 1) return;
    size_t page = used_pages_list[0].pa_addr;

    if (used_pages_list[0].pid == -1) {
        printf("Page replacement error\n");
        return;
    }
    /*before free this page, we need to reset the pagetable as non-present*/
    size_t va = used_pages_list[0].va_addr; 
    size_t pd_entry = (va >> 22);
    size_t pt_entry = (va >> 12) & 0x03FF; //Get the middle 10 bits
    size_t *page_dir = proc_table[used_pages_list[0].pid].pagetable;
    size_t *page_table;
    page_table = (size_t*)(page_dir[pd_entry] & 0xFFFFF000);
    page_table[pt_entry] = 0;

    kfree((void*)page);

    int i = 1;
    while(i < 100){
        if (used_pages_list[i].pa_addr == 0) break; 
        used_pages_list[i-1] = used_pages_list[i]; 
        i++;
    }
    used_pages_list[i-1].pa_addr = 0;
    used_pages_list[i-1].va_addr = 0;
    used_pages_list[i-1].pid = -1;
    //get_first(t+1); 
}
