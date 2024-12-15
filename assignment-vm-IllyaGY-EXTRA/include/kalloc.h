#ifndef __KALLOC_H_

#define __KALLOC_H_

#define PAGE_SIZE               0x00001000                  // 4096 is 0x1000
#define PHYSTOP                 0x08000000                  // 128 MB
#define PAGE_START              0x00e00000                  // 14 MB
#define PAGE_STOP               0x00fff000                  // 16 MB - 1 KB

#define PT_PAGE_MASK            (~(PAGE_SIZE-1))    /* Trim offset */
#define PAGE_ALIGN(addr)        (((uint32)(addr)+PAGE_SIZE-1)&PT_PAGE_MASK)


void freerange(void *vstart, void *vend);
void kinit();
void* kalloc();
void kfree(void *);
void kpage_add(size_t * page_list, void * address);
size_t kpagemake(void);
void manage_page_del(void *page);  
void manage_page(void *page, size_t va);
void get_first();

#endif
