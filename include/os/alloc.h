#ifndef __INCLUDE_ALLOC_H
#define __INCLUDE_ALLOC_H

#include <sys.h>
#include <os/list.h>
#include <os/tlsf.h>

void page_alloc_init
    (
    cpu_addr_t first_address,
    cpu_addr_t last_address
    );
void *page_alloc(void);
void page_free(void *addr);
void page_free(void *addr);
void page_free_contig(size_t index);
void *page_alloc_contig(int num_pages);

extern void *kmalloc(size_t size);
extern void kfree(void *ptr);
extern void *krealloc(void *ptr, size_t size);
extern void *kcalloc(size_t nelem, size_t elem_size);

#endif 
