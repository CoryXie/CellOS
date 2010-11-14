#ifndef __INCLUDE_ARCH_X64_VM_H
#define __INCLUDE_ARCH_X64_VM_H

/*
 * System memory mapping as follows:
 *
 * 1. The kernel itself is built at 0xFFFFFFFF80100000 (see cell64.ld), it will  
 * be loaded at physical address 0x100000 by GRUB. On entry, the ASM code will
 * map the first 4MB with two 2MB pages to be both as 0 (identity mapping) and 
 * 0xFFFFFFFF80000000 (high half mapping). So in the boot process it is possible
 * to access the first 4MB in two different address ranges :
 *
 * - 0~0x400000 
 * - 0xFFFFFFFF80000000~0xFFFFFFFF80400000
 *
 * 2. In the paging code, we will map the first 4GB onto 0xFFFF800000000000, 
 * using pml4[256] that is just around the middle of the 64 bit address sapce. 
 * And then the identity mapping is removed after the SMP APs are booted (during 
 * the AP bootting process the identity mapping is still needed). So after that, 
 * we have again two address ranges to access the kernel area (we assume kernel 
 * will take up to 4MB for now):
 *
 * - 0xFFFFFFFF80000000 ~ 0xFFFFFFFF80400000
 *    (this is only 4MB mapped using pml4[511],pdpt[0],pdir[0~1], the remaining 
 *    range of this 2GB is not mapped yet)
 * - 0xFFFF800000000000 ~ 0xFFFF800100000000
 *    (this is only 4GB mapped using pml4[256],pdpt[0~3], the remiaing range of 
 *    this 512GB area is not mapped yet)
 *
 * 3. Accessing the unmapped address ranges will trigger page fault!For example, 
 * if you try to access 0xFFFFFFFF80500000, which is 1MB above the current 
 * kernel virtual address mapping (and not mapped yet), it will page fault.
 */
  
#define KERNEL_SYMB_MAP_BASE  0xFFFFFFFF80000000  /* kernel symbols built here */
#define KERNEL_VIRT_MAP_BASE  0xFFFF800000000000  /* map 0~4GB up here */
#define KERNEL_PHYS_BASE      0

#define KERNEL_PHYS_MAP_LOW   0 /* 4GB */
#define KERNEL_PHYS_MAP_HIGH  0xFFFFFFFF /* 4GB */

/* Kernal symbol address and physical address conversion - for symbols only */
#ifndef __ASM__
#define KA2PA(a)        ((cpu_addr_t)(a) - KERNEL_SYMB_MAP_BASE)
#define PA2KA(a)        ((cpu_addr_t)(a) + KERNEL_SYMB_MAP_BASE)
#else
#define KA2PA(a)        ((a) - KERNEL_SYMB_MAP_BASE)
#define PA2KA(a)        ((a) + KERNEL_SYMB_MAP_BASE)
#endif

/* Virtual address and physical address conversion - for registers etc... */
#ifndef __ASM__
#define VA2PA(a)        ((cpu_addr_t)(a) - KERNEL_VIRT_MAP_BASE)
#define PA2VA(a)        ((cpu_addr_t)(a) + KERNEL_VIRT_MAP_BASE)
#else
#define VA2PA(a)        ((a) - KERNEL_VIRT_MAP_BASE)
#define PA2VA(a)        ((a) + KERNEL_VIRT_MAP_BASE)
#endif

#define PG_PTBL_SIZE    0x200000        /* a PDE points to a PTBL referring 2MB */
#define PG_PDIR_SIZE    0x40000000      /* a PDPTE points to a PDIR referring 1GB */
#define PG_PDP_SIZE     0x8000000000    /* a PML4E points to a PDP referring 512 GB */

/** Page size definitions. */
#define PAGE_SHIFT      12                /* Width of a page in bits. */
#define PAGE_SIZE       (1 << PAGE_SHIFT)  /* Size of a page (4KB). */
#define PAGE_MASK       0xFFFFFFFFFF000    /* Mask to clear the page offset 
                                         * from a virtual address. */

/** Definitions of paging structure bits. */
#define PG_PRESENT      (1<<0)            /* Page is present. */
#define PG_WRITE        (1<<1)            /* Page is writable. */
#define PG_USER         (1<<2)            /* Page is accessible in CPL3. */
#define PG_PWT          (1<<3)            /* Page has write-through caching. */
#define PG_NOCACHE      (1<<4)            /* Page has caching disabled. */
#define PG_ACCESSED     (1<<5)            /* Page has been accessed. */
#define PG_DIRTY        (1<<6)            /* Page has been written to. */
#define PG_LARGE        (1<<7)            /* Page is a large page. */
#define PG_GLOBAL       (1<<8)            /* Page won't be cleared in TLB. */
#ifndef __ASM__
# define PG_NOEXEC      ((uint64_t)1<<63) /* Page is not executable */
#else
# define PG_NOEXEC      (1<<63)           /* (requires NX support). */
#endif

#define PAGE_ALIGN(s) (((s)+PAGE_SIZE-1)&~(PAGE_SIZE-1))

#ifndef __ASM__
#include <sys.h>
typedef struct pmap 
    {
    void *          lock;  /* Mutex to protect this page map */
    uint64_t *      pml4v; /* PML4 virtual address */
    cpu_addr_t      pml4;  /* PML4 physical address */
    bool            user;  /* Pages mapped is userspace accessible */
    cpu_addr_t      start; /* start page of the range covered by this page map */
    cpu_addr_t      end;   /* end page of the range covered by this page map */
    } pmap_t;

/* Page mapping protection flags. */

#define PMAP_READ        (1<<0)    /* Mapping should be readable. */
#define PMAP_WRITE       (1<<1)    /* Mapping should be writable. */
#define PMAP_EXEC        (1<<2)    /* Mapping should be executable. */
#endif

#endif /* __INCLUDE_ARCH_X64_VM_H */
