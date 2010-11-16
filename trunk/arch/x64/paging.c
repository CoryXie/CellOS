#include <sys.h>
#include <arch.h>
#include <os.h>

/*
 * Theory of paging:
 *
 * PML4:
 *
 * A 4-KByte naturally aligned PML4 table is located at the physical address
 * specified in bits 51:12 of CR3. A PML4 table comprises 512 64-bit entries
 * (PML4Es). A PML4E is selected using the physical address defined as follows:
 * - Bits 51:12 are from CR3.
 * - Bits 11:3 are bits 47:39 of the linear address.
 * - Bits 2:0 are all 0.
 * Because a PML4E is identified using bits 47:39 of the linear address, it
 * controls access to a 512-GByte region of the linear-address space.
 *
 * PDPT:
 *
 * A 4-KByte naturally aligned page-directory-pointer table is located
 * at the physical address specified in bits 51:12 of the PML4E. A page-
 * directory-pointer table comprises 512 64-bit entries (PDPTEs). A PDPTE is
 * selected using the physical address defined as follows:
 * - Bits 51:12 are from the PML4E.
 * - Bits 11:3 are bits 38:30 of the linear address.
 * - Bits 2:0 are all 0.
 *
 * Because a PDPTE is identified using bits 47:30 of the linear address,
 * it controls access to a 1-GByte region of the linear-address space.
 *
 * PDIR:
 *
 * A 4-KByte naturally aligned page directory is located at the physical
 * address specified in bits 51:12 of the PDPTE. A page directory comprises
 * 512 64-bit entries (PDEs). A PDE is selected using the physical address
 * defined as follows:
 * - Bits 51:12 are from the PDPTE.
 * - Bits 11:3 are bits 29:21 of the linear address.
 * - Bits 2:0 are all 0.
 *
 * Because a PDE is identified using bits 47:21 of the linear address, it
 * controls access to a 2-MByte region of the linear-address space. Use
 * of the PDE depends on its PS flag (bit 7):
 * - If the PDE's PS flag is 1, the PDE maps a 2-MByte page.
 * - If the PDE's PS flag is 0, a 4-KByte naturally aligned page table
 * is located at the physical address specified in bits 51:12 of the PDE.
 * A page table comprises 512 64-bit entries (PTEs).
 *
 * PTBL:
 *
 * A PTE is selected using the physical address defined as follows:
 *
 * - Bits 51:12 are from the PDE.
 * - Bits 11:3 are bits20:12 of the linear address.
 * - Bits 2:0 are all 0.
 *
 * Because a PTE is identified using bits47:12 of the linear address, every PTE
 * maps a 4-KByte page (see Table4-17). The final physical address is computed
 * as follows:
 * - Bits 51:12 are from the PTE.
 * - Bits 11:0 are from the original linear address.
 */


extern volatile uint64_t _boot_pml4[];
extern volatile uint64_t _boot_pdpt[];
extern volatile uint64_t _boot_pd0[];
extern volatile uint64_t _kernel_pd0[];

extern void _end(void);

static uint64_t first_usable_phys_address,last_usable_phys_address;

static inline int x64_get_pml4_index(uint64_t address)
  {
  return (int) (address >> 39) & 511;
  }

static inline int x64_get_pdpt_index(uint64_t address)
  {
  return (int) (address >> 30) & 511;
  }

static inline int x64_get_pd_index(uint64_t address)
  {
  return (int) (address >> 21) & 511;
  }

static inline int x64_get_pte_index(uint64_t address)
  {
  return (int) (address >> 12) & 511;
  }

void x64_paging_debug()
  {
  uint64_t cr3;

  cr3 = sys_read_cr3();

  printk("cr3=0x%p\n_boot_pml4 @ %p\n",cr3,(KA2PA((uint64_t)_boot_pml4)));

  printk("_boot_pml4[0]=%p\n",_boot_pml4[0]);
  printk("_boot_pdpt @ %p\n",(KA2PA((uint64_t)_boot_pdpt)));
  printk("_boot_pdpt[0]=%p\n",_boot_pdpt[0]);
  }


addr_t mem_get_low_addr(void)
  {
  return (addr_t)first_usable_phys_address;
  }

addr_t mem_get_high_addr(void)
  {
  return (addr_t)last_usable_phys_address;
  }

uint64_t x64_get_usable_size(void)
  {
  return last_usable_phys_address - first_usable_phys_address;
  }

/** x64_paging_map - map lower 4GB physical memory space to the kernel
 * virtual address
 */
void x64_paging_map(uint64_t addr)
    {
    uint64_t lowest_addr;
    uint64_t num_pdirs;
    uint64_t num_pdpt;

    uint64_t *first_pdpt, *current_pdpt;
    uint64_t *first_pdir, *current_pdir;

    uint64_t i,j,k;
    uint64_t address,highest_addr;

    int base_pml4, base_pdpt;

    highest_addr = addr;

    /* map the lower 4GB */

    if (highest_addr < KERNEL_PHYS_MAP_HIGH)
        highest_addr = KERNEL_PHYS_MAP_HIGH;

    base_pml4 = x64_get_pml4_index(KERNEL_VIRT_MAP_BASE); /* 256, in the middle */
    base_pdpt = x64_get_pdpt_index(KERNEL_VIRT_MAP_BASE); /* 0 */

    printk("base_pml4=%i base_pdpt=%i\n",base_pml4,base_pdpt);

    /* calculate where we can put the first PDPT */

    lowest_addr = (KA2PA((uint64_t)&_end)) + PAGE_SIZE;
    lowest_addr &= PAGE_MASK;

    printk("lowest_addr=%p, highest_addr=%p\n", lowest_addr, highest_addr);

    /* get the number of PDIR entries */

    num_pdirs = highest_addr / PG_PDIR_SIZE; /* 4, max 4GB */
    if (highest_addr & (PG_PDIR_SIZE - 1))
        num_pdirs ++;

    /* get the number of PDPT entries */

    num_pdpt = highest_addr / (PG_PDP_SIZE); /* 1, max 512 GB */
    if (highest_addr & (PG_PDP_SIZE - 1))
        num_pdpt ++;

    /* Save the actual physical memory heap range */

    first_usable_phys_address = lowest_addr
                           + (num_pdirs * PAGE_SIZE)
                           + (num_pdpt * PAGE_SIZE);
    last_usable_phys_address = addr;

    printk("user available physical RAM %p-%p\n",
        first_usable_phys_address, last_usable_phys_address);

    printk("x64_paging_map: page tables using %d KB\n",
        ((num_pdirs * PAGE_SIZE)+(num_pdpt * PAGE_SIZE)) / 1024);

    first_pdpt = (uint64_t*)(PA2KA((uint64_t)lowest_addr));
    first_pdir = (uint64_t*)(PA2KA((uint64_t)lowest_addr) +
                    (num_pdpt * PAGE_SIZE));

    printk("first_pdpt=%p, first_pdir=%p, num_pdpt = %d\n",
            first_pdpt, first_pdir, num_pdpt);

    address = KERNEL_PHYS_MAP_LOW; /* 0 */

    /* map each PDPT entry */

    for (i = 0 ; i < num_pdpt ; i++) /* 1 */
        {
        current_pdpt = (uint64_t*)((uint64_t)first_pdpt + (i * PAGE_SIZE));

        printk("current_pdpt = %p\n",current_pdpt);

        /* map each PDIR entry */

        for (j = 0 ; j < num_pdirs; j++) /* 4 */
            {
            current_pdir = (uint64_t*)((uint64_t)first_pdir + (j * PAGE_SIZE));

            /* each PDIR has 512 eightbyte PDEs each pointing to a PTBL */

            for (k = 0 ; k < PAGE_SIZE/sizeof(uint64_t); k++)
                current_pdir[k] = 0;

            for (k=0; k < PAGE_SIZE/sizeof(uint64_t); k++) /* 512 */
                {
                current_pdir[k] = (uint64_t)address +
                                (PG_LARGE | PG_PRESENT | PG_WRITE);

                address += PG_PTBL_SIZE; /* 2MB */

                if (address > highest_addr)
                    break;
                }
            current_pdpt[base_pdpt+j] =
                (uint64_t) (KA2PA((uint64_t)current_pdir)) +
                                (PG_PRESENT | PG_WRITE);

            printk("pdpt #%i %p -> %p\n",j,current_pdpt[j],current_pdir);
            }

        _boot_pml4[base_pml4+i] = /*_boot_pml4[256] */
            (uint64_t)(KA2PA((uint64_t)current_pdpt)) +
                                (PG_PRESENT | PG_WRITE);

        printk("just set %p %p\n",_boot_pml4[base_pml4+i],current_pdpt);

        }

    printk("end address %p\n",address);
    printk("num_pdirs=%i\n",num_pdirs);
    printk("num_pdpt=%i\n",num_pdpt);

    }

void x64_debug()
  {
  int i ;

  for (i = 0 ; i < 512; i++)
      if (_boot_pml4[i] != 0)
         printk("pml4[%i]=%p\n",i,_boot_pml4[i]);
  }

void dump_stack(struct stack_frame * stack);

#define ERR_PF_PRESENT          (1 << 0)
#define ERR_PF_READ_WRITE       (1 << 1)
#define ERR_PF_USER_SUPERVISOR  (1 << 2)
#define ERR_PF_RESERVED         (1 << 3)
#define ERR_PF_INSTRUCTION      (1 << 4)

void x64_page_fault
    (
    addr_t stack_frame
    )
    {
    addr_t fault_addr;
    uint64_t fault_code;
    stack_frame_t *frame;
    
    frame = (stack_frame_t *)stack_frame;

    fault_addr = sys_read_cr2();
    fault_code = frame->error;

    printk("x64_page_fault() address=%p error code=%i\n",
            fault_addr, fault_code);

    printk("x64_page_fault() offending PC=%p\n",frame->rip);

    printk("x64_page_fault() %s page fault due to %s%s, while in %s mode%s\n",
           fault_code & ERR_PF_READ_WRITE ? "write" : "read",
           fault_code & ERR_PF_PRESENT ? "access violation" : "page not present",
           fault_code & ERR_PF_RESERVED ? ", reserved bits set in page table"
           : "",
           fault_code & ERR_PF_USER_SUPERVISOR ? "user" : "supervisor",
           fault_code & ERR_PF_INSTRUCTION ? ", by instruction fetch" : "");


    dump_stack(frame);

    printk("Exception thread is %s\n", kurrent->name);
    sched_context_dump(&kurrent->saved_context);
    
    sched_thread_global_show();

    for(;;);

    }

void paging_init(uint64_t lowest_addr, uint64_t highest_addr)
    {
    uint64_t address;

    printk("Free physical area range [%p - %p]\n",lowest_addr, highest_addr);

    irq_register(14, "paging", (addr_t)x64_page_fault);

    x64_paging_map(highest_addr);

    sys_write_cr3(KA2PA((uint64_t)_boot_pml4));

    address = sys_read_cr3();

    printk("new CR3 %p\n", address);
    }

void paging_late_init(void)
    {

    /*
     * Remove the identity mapping used when we first enabled long mode.
     * This has to happen after the SMP APs are all on so they do not need to
     * use identity mapping!Otherwise there will be reboot when bring up APs!
     */

    _boot_pml4[0] = 0;
    }

/* Initialise a page map structure.
 *
 * Initialises a userspace page map structure.
 *
 * @param map  Page map to initialise.
 *
 * @return  0 on success, negative error code on failure.
 */
 
int pmap_init(pmap_t *map, bool user, cpu_addr_t start, cpu_addr_t end) 
    {
    int ret;
    pthread_mutex_t mutex;
    pthread_mutexattr_t attr;

    mutex = kmalloc(sizeof(pthread_mutex_t));

    if (!mutex)
        return ENOMEM;
    
    pthread_mutexattr_init(&attr);

    if ((ret = pthread_mutex_init(&mutex, &attr)) != OK)
        {
        return ret;
        }
    
    /* Create the mutex to protect this page mapping */
    
    map->lock = mutex;
    
    /* Allocate a page for the pml4 */
    
    map->pml4v = page_alloc();
    
    if (!map->pml4v)
        {
        printk("pmap_init - pml4v allocation fail\n");
        
        return -ENOMEM;
        }
    
    /* Get the phsical address of this pml4 */
    
    map->pml4 = VA2PA(map->pml4v);
    
    map->user = user;
    map->start = start;
    map->end = end;

    /* Get the kernel mappings into the new PML4 */
        
    map->pml4v[511] = _boot_pml4[511] & ~PG_ACCESSED;
    
    return 0;
    }

/* 
 * Destroy a page map.
 *
 * Destroys all mappings in a page map and frees up anything it has allocated.
 *
 * @param map Page map to destroy.
 */
 
void pmap_destroy (pmap_t *map) 
    {
    if (map->lock)
        {
        pthread_mutex_destroy(map->lock);
        kfree(map->lock);
        }
    
    page_free(map->pml4v);
    }

/* 
 * Switch to a different page map.
 *
 * Switches to a different page map.
 *
 * @param map  Page map to switch to.
 */
 
void pmap_switch (pmap_t *map) 
    {
    sys_write_cr3(map->pml4);
    }

    
/* 
 * Get the page table containing an virtual address in the specified page map
 *
 * @param map       Page map to get from.
 * @param virt      Address to get page table for.
 * @param alloc     Whether to allocate structures if not found.
 * @param mmflag    Allocation flags.
 * @param ptblp     Where to store pointer to page table (virtual address).
 *
 * @return  0 on success, negative error code on failure. 
 */
 
static int pmap_get_ptbl
    (
    pmap_t *    map, 
    ptr_t       virt, 
    bool        alloc, 
    int         mmflag, 
    uint64_t ** ptblp
    ) 
    {
    uint64_t *pml4, *pdp, *pdir;
    int pml4e, pdpe, pde;
    cpu_addr_t page;

    /* Get the virtual address of the PML4 */
    
    pml4 = map->pml4v;

    /* Get the page directory pointer number. A PDP covers 512GB. */
    
    pml4e = x64_get_pml4_index(virt);

    /* If the PDP is not present */
    
    if (!(pml4[pml4e] & PG_PRESENT)) 
        {
        /*
         * Allocate a new PDP if required.
         */
         
        if (alloc) 
            {
            page = (cpu_addr_t)page_alloc();
            
            if (!page) 
                {
                return -ENOMEM;
                }

            /* Map it into the PML4. */
            
            pml4[pml4e] = (uint64_t)(VA2PA(page) | 
                                    PG_PRESENT |
                                    PG_WRITE | 
                                    ((map->user) ? PG_USER : 0));
            } 
        else 
            {
            return -ENOENT;
            }
        }

    pdp = (uint64_t *)(PA2VA((cpu_addr_t)(pml4[pml4e] & PAGE_MASK)));

    /* Get the page directory number. A page directory covers 1GB. */
    
    pdpe = x64_get_pdpt_index(virt);

    /* If the pdpt entry is not present */
    
    if (!(pdp[pdpe] & PG_PRESENT)) 
        {
        /* Allocate a new page directory if required. */
        
        if (alloc) 
            {
            page = (cpu_addr_t)page_alloc();
            if (!page) 
                {
                return -ENOMEM;
                }

            /* Map it into the PDP. */
            
            pdp[pdpe] = (uint64_t)(VA2PA(page) | 
                                    PG_PRESENT | 
                                    PG_WRITE | 
                                    ((map->user) ? PG_USER : 0));
            } 
        else
            {
            return -ENOENT;
            }
        }

    pdir = (uint64_t *)(PA2VA((cpu_addr_t)(pdp[pdpe] & PAGE_MASK)));

    /* Get the page table number. A page table covers 2MB. */
    
    pde = x64_get_pd_index(virt);
    
    if (!(pdir[pde] & PG_PRESENT)) 
        {
        /* Allocate a new page table if required. */
        
        if (alloc) 
            {
            page = (cpu_addr_t)page_alloc();
            
            if (!page) 
                {
                return -ENOMEM;
                }

            /* Map it into the page directory. */
            
            pdir[pde] = (uint64_t)(VA2PA(page) |
                                    PG_PRESENT | 
                                    PG_WRITE | 
                                    ((map->user) ? PG_USER : 0));
            } 
        else 
            {
            return -ENOENT;
            }
        }

    ASSERT(!(pdir[pde] & PG_LARGE));
    
    *ptblp = (uint64_t *)(PA2VA((cpu_addr_t)(pdir[pde] & PAGE_MASK)));
    
    return 0;
    }

/* 
 * Convert page map flags to PTE flags.
 *
 * @param flags   Flags to convert.
 *
 * @return        Converted flags.
 */
 
static inline uint64_t pmap_flags_to_pte(int prot) 
    {
    uint64_t ret = 0;

    ret |= ((prot & PMAP_WRITE) ? PG_WRITE : 0);
    
#if CONFIG_X86_NX
    ret |= ((!(prot & PMAP_EXEC) && CPU_HAS_XD(curr_cpu)) ? PG_NOEXEC : 0);
#endif

    return ret;
    }


/** Insert a mapping in a page map.
 *
 * Maps a virtual address to a physical address with the given protection
 * settings in a page map.
 *
 * @param map        Page map to insert in.
 * @param virt        Virtual address to map.
 * @param phys        Physical address to map to.
 * @param prot        Protection flags.
 * @param mmflag    Page allocation flags.
 *
 * @return        0 on success, negative error code on failure. Can only
 *            fail if MM_SLEEP is not set.
 */
int pmap_insert(pmap_t *map, ptr_t virt, cpu_addr_t phys, int prot, int mmflag) {
    uint64_t *ptbl;
    int pte, ret;

    ASSERT(!(virt % PAGE_SIZE));
    ASSERT(!(phys % PAGE_SIZE));

    pthread_mutex_lock((pthread_mutex_t *)map->lock);

    /* Check that we can map here. */
    
    if (virt < map->start || virt > map->end)
        {
        panic("Map on %p outside allowed area", map);
        }

    /* Find the page table for the entry. */
    
    if ((ret = pmap_get_ptbl(map, virt, true, mmflag, &ptbl)) != 0) 
        {
        pthread_mutex_unlock((pthread_mutex_t *)map->lock);
        
        return ret;
        }

    /* Check that the mapping doesn't already exist. */
    
    pte = x64_get_pte_index(virt);
    
    if (ptbl[pte] & PG_PRESENT) 
        {
        panic("Mapping %p which is already mapped", virt);
        }

    /* Map the address in. */
    
    ptbl[pte] = phys | 
                PG_PRESENT |
                ((map->user) ? PG_USER : PG_GLOBAL) | 
                pmap_flags_to_pte(prot);

    memory_barrier();
    
    pthread_mutex_unlock((pthread_mutex_t *)map->lock);
    
    return 0;
    }

/* 
 * Remove a mapping from a page map.
 *
 * Removes the mapping at a virtual address from a page map.
 *
 * @param map    Page map to unmap from.
 * @param virt   Virtual address to unmap.
 * @param physp  Where to store mapping's physical address prior to
 *               unmapping (can be NULL).
 *
 * @return       0 on success, -ERR_NOT_FOUND if mapping missing.
 */
 
int pmap_remove(pmap_t *map, ptr_t virt, cpu_addr_t *physp) 
    {
    uint64_t *ptbl;
    int pte, ret;

    ASSERT(!(virt % PAGE_SIZE));

    pthread_mutex_lock((pthread_mutex_t *)map->lock);

    /* Check that we can unmap here. */
    
    if (virt < map->start || virt > map->end) 
        {
        panic("Unmap on %p outside allowed area", map);
        }

    /* Find the page table for the entry. */
    
    if ((ret = pmap_get_ptbl(map, virt, false, 0, &ptbl)) != 0) 
        {
        pthread_mutex_unlock((pthread_mutex_t *)map->lock);
        
        return ret;
        }

    pte = x64_get_pte_index(virt);
    
    if (ptbl[pte] & PG_PRESENT) 
        {
        /* Store the physical address if required. */
        
        if (physp != NULL) 
            {
            *physp = ptbl[pte] & PAGE_MASK;
            }

        /* Clear the entry. */
        
        ptbl[pte] = 0;
        
        memory_barrier();
        
        pthread_mutex_unlock((pthread_mutex_t *)map->lock);
        
        return 0;
        } 
    else
        {
        pthread_mutex_unlock((pthread_mutex_t *)map->lock);
        
        return -ENOENT;
        }
    }

/* 
 * Get the value of a mapping in a page map.
 *
 * Gets the physical address, if any, that a virtual address is mapped to in
 * a page map.
 *
 * @param map        Page map to lookup in.
 * @param virt        Address to find.
 * @param physp        Where to store mapping's value.
 *
 * @return        True if mapping is present, false if not.
 */
 
bool pmap_find(pmap_t *map, ptr_t virt, cpu_addr_t *physp) 
    {
    uint64_t *ptbl;
    int pte;

    ASSERT(!(virt % PAGE_SIZE));
    
    ASSERT(physp);

    pthread_mutex_lock((pthread_mutex_t *)map->lock);

    /* Find the page table for the entry. */
    
    if (pmap_get_ptbl(map, virt, false, 0, &ptbl) == 0) 
        {
        pte = x64_get_pte_index(virt);
        
        if (ptbl[pte] & PG_PRESENT) 
            {
            *physp = ptbl[pte] & PAGE_MASK;
            
            pthread_mutex_unlock((pthread_mutex_t *)map->lock);
            
            return true;
            }
        }

    pthread_mutex_unlock((pthread_mutex_t *)map->lock);
    
    return false;
    }

/* 
 * Modify protection flags of a range.
 *
 * Modifies the protection flags of a range of pages in a page map. If any of
 * the pages in the range are not mapped, then the function will ignore it and
 * move on to the next.
 *
 * @param map        Map to modify in.
 * @param start      Start of page range.
 * @param end        End of page range.
 * @param prot       New protection flags.
 *
 * @return        0 on success, negative error code on failure.
 */
 
int pmap_protect(pmap_t *map, ptr_t start, ptr_t end, int prot) 
    {
    uint64_t *ptbl;
    ptr_t i;
    int pte;

    ASSERT(!(start % PAGE_SIZE));
    ASSERT(!(end % PAGE_SIZE));

    pthread_mutex_lock((pthread_mutex_t *)map->lock);

    for (i = start; i < end; i++) 
        {
        pte = x64_get_pte_index(i);

        if (pmap_get_ptbl(map, i, false, 0, &ptbl) != 0 || 
            !(ptbl[pte] & PG_PRESENT)) 
            {
            continue;
            }

        /* Clear out original flags, and set the new flags. */
        
        ptbl[pte] = (ptbl[pte] & ~(PG_WRITE | PG_NOEXEC)) |
                    pmap_flags_to_pte(prot);
        }

    pthread_mutex_unlock((pthread_mutex_t *)map->lock);
    
    return 0;
    }

