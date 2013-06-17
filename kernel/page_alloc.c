#include <sys.h>
#include <arch.h>
#include <os.h>

#define MM_PAGE_STATUS_AVAIL         0x0
#define MM_PAGE_STATUS_CHAINED         0x1
#define MM_PAGE_STATUS_CHAINED_LAST 0x2
#define MM_PAGE_STATUS_ALLOCATED     0x3

typedef struct page
    {
    list_t          list;
    cpu_addr_t      phys_addr;
    uint64_t        status;
    } __attribute__((packed)) page_t;

spinlock_t  page_alloc_lock;
list_t      page_alloc_free_page_list;
page_t *    page_array;

cpu_addr_t     mm_lowest_addr = 0 ;

size_t         mm_pages_available = 0;
size_t         mm_bytes_allocated = 0;
int         track_free_page = 0;

static inline void page_add_to_free_list
    (
    page_t *pp
    )
    {
    pp->status = MM_PAGE_STATUS_AVAIL;
    
    /* The node should be initialized before appending it to the list */
    
    list_init(&pp->list);
    
    list_append(&page_alloc_free_page_list, &pp->list);

    if (track_free_page)
        printk("add free page %p\n", pp->phys_addr);
    }

static inline page_t * page_get_from_free_list(void)
    {
    page_t *pp = NULL;
    
    if (LIST_EMPTY(&page_alloc_free_page_list))
        {
        printk("NO free page!\n");
        
        return NULL;
        }
    else
        {
        pp = (page_t *)LIST_ENTRY(page_alloc_free_page_list.next, page_t, list);

        /* Remove it from the free list */
        
        list_remove(&pp->list);
        
        if (track_free_page)
            printk("get free page %p\n", pp->phys_addr);
        
        return pp;
        }
    }


void page_alloc_init
    (
    cpu_addr_t first_address,
    cpu_addr_t last_address
    )
    {
    page_t *p;
    size_t pages_available;
    ssize_t i;

    mm_bytes_allocated = 0;

    first_address = (first_address + PAGE_SIZE) & PAGE_MASK;

    pages_available = (last_address - first_address) / PAGE_SIZE;

    mm_lowest_addr = first_address + (pages_available * sizeof(page_t));
    mm_lowest_addr = (mm_lowest_addr + PAGE_SIZE) & PAGE_MASK;
    pages_available = (last_address - mm_lowest_addr) / PAGE_SIZE;
    mm_pages_available = pages_available;

    printk("page_alloc_init(): pages_available=%d lowest_addr=%p\n",
            pages_available,mm_lowest_addr);

    spinlock_init(&page_alloc_lock);

    list_init(&page_alloc_free_page_list);
    
    /* Carve the first address as the page map array */
    
    page_array = (page_t *)PA2VA(first_address);
    
    for (i = pages_available - 1 ; i >= 0 ; i --)
        {
        page_array[i].phys_addr = (mm_lowest_addr + (i * PAGE_SIZE));
        page_array[i].status = MM_PAGE_STATUS_AVAIL;
        
        p = (page_t *)&page_array[i];
        
        page_add_to_free_list((page_t*)p);
        }

    track_free_page = 1;

    printk("page_alloc_init(): done!\n");
    }

void *page_alloc_contig
    (
    int num_pages
    )
    {
    page_t *page;
    size_t i, found,curr;
    void *addr;

    spinlock_lock(&page_alloc_lock);
    
    found = curr = i = 0;
    
    while (curr < mm_pages_available)
        {
        page = (page_t *)&page_array[curr];
        /* 
             * If we found one free page then we need to check if the 
             * next num_pages are also free 
             */
                
        if (page->status == MM_PAGE_STATUS_AVAIL)
            {
            found = curr;
            while (page->status == MM_PAGE_STATUS_AVAIL && i < num_pages)
                {
                curr ++;
                i++;
                page = (page_t *)&page_array[curr];
                }
            }
        /* Found num_pages contiguous pages */
        
        if (i == num_pages)
            {
            #if 0
            printk("page_alloc_contig(): found %i pages at index #%i mem %p start\n",
                i, found, addr);
            #endif
            for (i = 0; i < num_pages; i++)
                {
                page = (page_t *)&page_array[i + found];
                
                list_remove(&page->list);
                
                page->status = MM_PAGE_STATUS_CHAINED;
                }

            page = (page_t*)&page_array[found];

            /* Return the corresponding kernel physical map address */
            
            addr = (void*)PA2VA(page->phys_addr);
            
            spinlock_unlock(&page_alloc_lock);

            #if 0
            printk("page_alloc_contig(): found %i pages at index #%i mem %p done\n",
                i, found, addr);
            #endif
            
            return (void*)addr;
            }
        else
            {
            found = i = 0;
            curr ++;
            }
        }

    spinlock_unlock(&page_alloc_lock);
    
    return NULL;
    }


void *page_alloc(void)
    {
    page_t *page;
    cpu_addr_t addr;

    spinlock_lock(&page_alloc_lock);

    page = (page_t *) page_get_from_free_list();
    
    if (!page)
        {
        spinlock_unlock(&page_alloc_lock);
        printk("page_get_from_free_list NULL!\n");
        return NULL;
        }

    if (page->status != MM_PAGE_STATUS_AVAIL)
        printk("WARNING:page_alloc() page %p status=%i\n",
                page->phys_addr, page->status);

    page->status = MM_PAGE_STATUS_ALLOCATED;

    /* Return the corresponding kernel physical map address */

    addr = (cpu_addr_t) PA2VA(page->phys_addr);
    
    spinlock_unlock(&page_alloc_lock);
    
    return (void*)(addr);
    }

void page_free_contig
    (
    size_t index
    )
    {
    page_t *page;
    size_t curr;

    /* kmalloc lock should already be held... */

    curr = index;
    page = (page_t*)&page_array[curr];

    if (page->status != MM_PAGE_STATUS_CHAINED && 
        page->status != MM_PAGE_STATUS_CHAINED_LAST)
        {
        printk("WARNING: page_free_contig() first page %p not chained!\n", page->phys_addr);
        return;
        }
    
    /* Set the page status as free */
    
    while (page->status == MM_PAGE_STATUS_CHAINED)
        {
        /* If this page has been popped off, stick it back on now */
        
        page_add_to_free_list(page);

        curr ++;
        
        page = (page_t*)&page_array[curr];
        }


    printk("page_free_contig() freed %i pages\n",curr - index);
    
    return;
    }

/* page_free - free pages that were allocated by page_alloc or page_alloc_contig
  */
void page_free
    (
    void *addr
    )
    {
    page_t *p;
    cpu_addr_t offset;
    size_t index;
    
    if (!addr)
        {
        printk("WARNING: page_free() addr is NULL");
        return;
        }
        
    /* Calculate the page index in the page map array */
    
    offset = ((cpu_addr_t)addr - PA2VA(mm_lowest_addr));
    
    index = (offset & PAGE_MASK) / PAGE_SIZE;

    if (index >= mm_pages_available)
        {
        printk("WARNING: page_free() out of range address %p, index=%i\n", 
                addr, index);
        return ;
        }

    spinlock_lock(&page_alloc_lock);

    p = (page_t *)&page_array[index];
    
    if (p->status != MM_PAGE_STATUS_ALLOCATED &&
        p->status != MM_PAGE_STATUS_CHAINED)
        {
        printk("WARNING: page_free() bad address %p, status=%i\n",
                addr, p->status);
                
        spinlock_unlock(&page_alloc_lock);
        
        return;
        }

    if (p->status == MM_PAGE_STATUS_ALLOCATED)
        {
        page_add_to_free_list(p);
        }
    else
        {
        page_free_contig(index);
        }
    
    spinlock_unlock(&page_alloc_lock);
    
    return;
    }
