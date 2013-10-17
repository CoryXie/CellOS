#include <sys.h>
#include <arch.h>
#include <os.h>

void defaul_sighandler(int sig)
    {
    printk("thread %s received signal %d at time %lld\n", 
        kurrent->name, sig, get_now_nanosecond());
    }

void itimer_set(void)
    {
    struct itimerval timerval;

    timerval.it_interval.tv_sec = timerval.it_interval.tv_usec = 0;
    timerval.it_value.tv_sec = 3;
    timerval.it_value.tv_usec = 100000;

    signal(SIGALRM, defaul_sighandler);
    
    printk("set timer on cpu %d at time %lld\n", 
        this_cpu(), get_now_nanosecond());
    
    setitimer(ITIMER_REAL, &timerval, NULL);

    }

int do_itimer (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
    {
    itimer_set();

    return OK;
    }

CELL_OS_CMD(
    itimer,   1,        1,    do_itimer,
    "test itimer",
    "test itimer and thread based signal handling\n"
    );

void cpu_heart_beat (int cpu)
    {
    volatile uint8_t *vidmem = (volatile uint8_t *)VGA_TEXT_MODE_KERN_BASE_ADDR;
    int      count = 0;

    /* Let's do a heart beat show! */

    count = 0;
    while (TRUE)
        {
        vidmem[86 + (cpu * 2)] = 'A' + count++;
        
        if (count >= 26) 
            count = 0;
        
        sched_yield();
        }
    }

void *sched_bsp_idle_thread (void *notused)
    {
    interrupts_disable();

    clockeventer_subsystem_init();
    
    pit_timer_init();

    lapic_bsp_pre_init();

    pci_init();

    rtc_init();
    
    keyboard_init();

    lapic_common_init();

#ifdef CONFIG_ACPICA
    acpica_sub_system_init ();
    pci_scan_devices();
#endif

    clockcounter_subsystem_init();

    timerchain_subsystem_init();

    real_wall_time_init();

    tick_eventer_init();
    
    lapic_bsp_post_init();

    interrupts_enable();

    thread_create_test();

    lapic_ipi(1, 0, INTR_LAPIC_RESCHEDULE);

    cpu_heart_beat(this_cpu());
    }

void *sched_ap_idle_thread (void *notused)
    {    
    interrupts_disable();
    
    lapic_common_init();

    interrupts_enable();

    thread_create_test();
    
    cpu_heart_beat(this_cpu());
    }

void main (uint32_t mboot_magic, uint32_t mboot_info)
    {
    multiboot_memmap_t *first_free;
    uint64_t *new_stack;

    asm volatile ("cli");

    cpu_early_init();

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

    mb_parse_kernel_image();
    
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
