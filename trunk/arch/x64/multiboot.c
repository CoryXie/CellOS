#include <sys.h>
#include <arch.h>
#include <os.h>

/* 
 * Scan through the multi boot memory map and return the biggest free map!
 * This is a hack for now that we only support one free area to be used!
 */
 
multiboot_memmap_t * mboot_init
    (
    uint32_t mboot_addr, 
    uint32_t mboot_magic
    )
    {
    multiboot_info_t *mb_info;
    multiboot_memmap_t *mb_mmap;
    multiboot_memmap_t *biggest_free = NULL;
    uint64_t size;
    char * type;
    uint64_t base, length, biggest = 0;
    
    if (mboot_magic != MB_LOADER_MAGIC)
        {
        printk("mboot_init: bad multiboot magic: 0x%x\n",mboot_magic);
        return 0;
        }
    
    mb_info = (multiboot_info_t *) ((uint64_t)PA2KA(mboot_addr));
    mb_mmap = (multiboot_memmap_t *)((uint64_t)PA2KA(mb_info->mmap_addr));

    while((uint64_t)mb_mmap <
           ((uint64_t)PA2KA(mb_info->mmap_addr + mb_info->mmap_length)))
        {

        base = (uint64_t)mb_mmap->base_addr;
        length = (uint64_t)mb_mmap->length;
        
        switch (mb_mmap->type)
            {
            case E820_TYPE_FREE:
                type = "FREE";
                biggest = (mb_mmap->length > biggest) ? mb_mmap->length : biggest;
                if (biggest == mb_mmap->length)
                    biggest_free = mb_mmap;
                break;
            case E820_TYPE_RESERVED:
                type = "RESERVED";
                break;
            case E820_TYPE_ACPI_RECLAIM:
                type = "ACPI_RECLAIM";
                break;
            case E820_TYPE_ACPI_NVS:
                type = "ACPI_NVS";
                break;
            case E820_TYPE_BAD:
                /* fall through */
            default:
                type = "BAD";
                break;                
            }
        printk("mmap: %p - %p (%dKb) %s\n",base,base+length,length/1024,type);

        size = mb_mmap->size + sizeof(uint32_t);
        mb_mmap = (multiboot_memmap_t *) ((uint64_t)mb_mmap + size);
        }
    return biggest_free;
    }
