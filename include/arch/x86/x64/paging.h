#ifndef _ARCH_X86_X64_PAGING_H
#define _ARCH_X86_X64_PAGING_H

void paging_init
    (
    uint64_t lowest_addr, 
    uint64_t highest_addr
    );
void paging_late_init(void);

addr_t mem_get_low_addr(void);
addr_t mem_get_high_addr(void);

#endif /* _ARCH_X86_X64_PAGING_H */
