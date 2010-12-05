#include <sys.h>
#include <arch.h>
#include <os.h>
#include <os/elf.h>

static multiboot_info_t *mb_info;
static multiboot_memmap_t *mb_mmap;


void mb_show_capability(void)
    {
      if (mb_info->flags & MB_FLAG_MEMINFO) 
        printk("*We know how much memory we have\n");
      else 
        printk("*We don't know how much memory we have\n");
      
      if (mb_info->flags & MB_FLAG_BOOTDEV)
        printk("*We know where we boot from\n");
      else 
        printk("*We don't know where we boot from\n"); 
      
      if (mb_info->flags & MB_FLAG_CMDLINE) 
        printk("*We know the command line\n");
      else 
        printk("*We don't know the command line\n");
      
      if (mb_info->flags & MB_FLAG_MODULES) 
        printk("*We know about modules\n");
      else 
        printk("*We don't know about modules\n");
      
      if (mb_info->flags & MB_FLAG_AOUTSYMS) 
        printk("*HORROR ! GRUB has mistaken us for an a.out kernel !\n");
      else 
        printk("*Grub knows we're not an a.out kernel\n");
      
      if (mb_info->flags & MB_FLAG_ELFSYMS) 
        printk("*We know anything about our symbol table\n");
      else 
        printk("*We don't know anything about our symbol table\n");
      
      if (mb_info->flags & MB_FLAG_MMAP) 
        printk("*We have a map of memory\n");
      else 
        printk("*We don't have a map of memory\n");  
      
      if (mb_info->flags & MB_FLAG_DRIVES) 
        printk("*We know about physical drives\n");
      else 
        printk("*We don't know about physical drives\n");
      
      if (mb_info->flags & MB_FLAG_CONFTABLE) 
        printk("*We know about ROM configuration\n");
      else 
        printk("*We don't know about ROM configuration\n");
      
      if (mb_info->flags & MB_FLAG_LDRNAME) 
        printk("*We know our bootloader by name\n");
      else 
        printk("*We don't know our bootloader by name\n");
      
      if (mb_info->flags & MB_FLAG_APMTABLE) 
        printk("*We know about the APM table\n");
      else 
        printk("*We don't know about the APM table\n");  
      
      if (mb_info->flags & MB_FLAG_VBEINFO) 
        printk("*We know about VBE graphics capabilities\n");
      else 
        printk("*We don't know about VBE graphics capabilities\n");

    }

void mb_parse_kernel_image(void)
    {
    int                     i;
    struct Elf64_Shdr *     s;
    struct Elf64_Shdr *     elfshdr_addr;

    if (!(mb_info->flags & MB_FLAG_ELFSYMS))
        return;
    
    elfshdr_addr = (struct Elf64_Shdr *)PA2KA(mb_info->syms.elf.addr);

    printk("Number of ELF sections: %u (entry size %u), addr %p, shndx %d\n",
           mb_info->syms.elf.num, mb_info->syms.elf.size, elfshdr_addr,
           mb_info->syms.elf.shndx);

    if (mb_info->syms.elf.size == sizeof(struct Elf64_Shdr))
        {
        char *strtab = (char *)(PA2KA(elfshdr_addr[mb_info->syms.elf.shndx].sh_addr));

        for (s = elfshdr_addr, i = 0; i < mb_info->syms.elf.num; s++, i++)
            {
            struct Elf64_Shdr *shdr = (struct Elf64_Shdr *)(s);

            printk("Section %d", i);
            if (shdr->sh_type != SHT_NULL)
                {
                printk(" <%s>: type 0x%x\n", strtab + shdr->sh_name,
                       shdr->sh_type);

                if (shdr->sh_type == SHT_RELA)
                    {
                    struct Elf64_Rela *rela =
                        (struct Elf64_Rela *)PA2KA(shdr->sh_addr);
                    int j;

                    for (j = 0; j < shdr->sh_size / sizeof(struct Elf64_Rela); j++)
                        {
                        struct Elf64_Rela *r = &rela[j];

                        printk("Rela %d: offset 0x%llx, info %llu, addend %llx\n",
                               j, r->r_offset, r->r_info, r->r_addend);
                        }
                    }
                }
            else
                printk(": Unused\n");
            }
        }
    else
        printk("ELF section header entry size mismatch! Cannot process.\n");
    }

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

    mb_show_capability();

    while ((uint64_t)mb_mmap <
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
