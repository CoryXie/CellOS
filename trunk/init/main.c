#include <sys.h>
#include <arch.h>
#include <os.h>

void cpu_heart_beat (int cpu)
    {
    volatile uint8_t *vidmem = (volatile uint8_t *)VGA_TEXT_MODE_KERN_BASE_ADDR;
    int      count = 0;

    lapic_ipi(1, 0, INTR_LAPIC_RESCHEDULE);

    /* Let's do a heart beat show! */

    count = 0;
    while (TRUE)
        {
        vidmem[84 + (cpu * 2)] = 'A' + count++;
        
        if (count > 26) 
            count = 0;
        
        sched_yield();
        }
    }


void *sched_bsp_idle_thread (void *notused)
    {
    interrupts_disable();
    
    rtc_init();
    
    keyboard_init();

    lapic_init();

    interrupts_enable();

#ifdef CONFIG_ACPICA
    acpica_sub_system_init ();
#endif

    thread_create_test();

    cpu_heart_beat(this_cpu());
    }

void *sched_ap_idle_thread (void *notused)
    {
    interrupts_disable();
    
    lapic_init();

    interrupts_enable();

    thread_create_test();
    
    cpu_heart_beat(this_cpu());
    }

void main 
    (
    uint32_t mboot_magic, 
    uint32_t mboot_info
    )
    {
    multiboot_memmap_t *first_free;
    uint64_t *new_stack;

    asm volatile ("cli");

    x64_gdt_init();

    x64_idt_init();

    vga_console_init();

    printk("\n================================\n");
    printk("||==Welcome to CELLOS 64 bit==||");
    printk("\n================================\n");

    /* Read mboot header */

    first_free = mboot_init(mboot_info, mboot_magic);

    if (first_free == NULL)
        {
        panic("No free memory for use! STOP~!\n");
        }
    
    paging_init(first_free->base_addr,
                    first_free->base_addr + first_free->length);

    /* Init page allocator */

    page_alloc_init(mem_get_low_addr(),mem_get_high_addr());

    /* Initialize the memory pool */
    init_memory_pool(CONFIG_KHEAP_SIZE,
                    (void *)page_alloc_contig(CONFIG_KHEAP_SIZE/PAGE_SIZE));

    detect_cpu();

    acpi_init();

    smp_init();

    paging_late_init();

    sched_core_init();

    sched_init();

    reschedule();
    
    /* No reached */ 
    }
