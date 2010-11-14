#include <sys.h>
#include <arch.h>
#include <os.h>

extern uint8_t apboot[];
extern uint64_t _boot_pml4[];
extern uint64_t x64_lapic_reg_base;
extern void thread_create_test(void);
extern uint32_t bsp_apic_init_done;

static spinlock_t ap_heartbeat_lock;

struct x64_smp_cpu_info smp_cpus[CONFIG_NR_CPUS];

uint64_t smp_bsp_lapic_address;
uint32_t smp_bsp_idx = 0;
uint32_t smp_cpu_count = 0;
uint8_t smp_IMCRP = 0;

volatile int smp_ap_booted = 0;
volatile int offset = 0;

void smp_ap_entry_point(void);
void smp_parse_config(struct smp_config_table *config);

extern cpu_addr_t _binary_ap_boot_start, _binary_ap_boot_end;

uint32_t smp_total_cpu_count(void)
    {
    return smp_cpu_count;
    }

void smp_activiate_ap(int cpu)
    {
    uint8_t *code_ptr;
    uint64_t *pml4_address;
    uint64_t *entry_point;
    size_t ap_boot_size;
    
    code_ptr = (uint8_t*)((uint64_t)PA2KA(0x1000));


#if 1
    ap_boot_size = (cpu_addr_t)&_binary_ap_boot_end - (cpu_addr_t)&_binary_ap_boot_start;
    printk("_binary_ap_boot_start at %p, _binary_ap_boot_end at %p, ap_boot_size %p\n",
        &_binary_ap_boot_start,
        &_binary_ap_boot_end,
        ap_boot_size);

    memcpy(code_ptr, 
        &_binary_ap_boot_start, 
        ap_boot_size);
    printk("code copy done!\n");
#else
    /* copy bootstrap code to 0x1000 physical */
    
    memcpy(code_ptr,apboot,apboot_size);
#endif
    /* give AP its pml4 address */
    
    pml4_address = (uint64_t*)((uint64_t)PA2KA(0x1000 - 8));
    *pml4_address = (uint64_t) (KA2PA((uint64_t)_boot_pml4));

    /* give AP the kernel entry point */
    
    entry_point = (uint64_t*)((uint64_t)PA2KA(0x1000 - 16));
    *entry_point = (uint64_t) &smp_ap_entry_point;

    printk("smp_activiate_ap:_boot_pml4 %p entry %p\n",
        *pml4_address,*entry_point);

    printk("smp_activiate_ap: activiate AP cpu #%i......",cpu);

    /* Initalize the BSP LAPIC */
    
    lapic_write(LAPIC_SPURIOUS, INTR_LAPIC_SPURIOUS |
                              LAPIC_SPURIOUS_LAPIC_ENABLED |
                              LAPIC_SPURIOUS_FOCUS_DISABLED);
    lapic_write(LAPIC_TASKPRI,0);

    /* send INIT */
    
    lapic_ipi(smp_cpus[cpu].apic_id,0x500,0x01);

    int j;
    for(j = 0 ; j < 10000 ; j++)
         asm ("nop");

    /* Startt the AP executing the code at 0x1000 ......*/
    
    lapic_ipi(smp_cpus[cpu].apic_id,0x600,0x01);
    for(j = 0 ; j < 10000 ; j++)
         asm ("nop");

    lapic_ipi(smp_cpus[cpu].apic_id,0x600,0x01);

    }

/*
 * The operating system's first task is to determine whether the system
 * conforms to the MP specification. This is done by searching for the MP
 * floating pointer structure. If a valid floating pointer structure is
 * detected, it indicates that the system is MP-compliant, and the operating
 * system should continue to look for the MP configuration table. If the
 * system is not MP-compliant, the operating system may attempt other means
 * of MP system detection, if it is capable of doing so, or treat the system
 * as a uniprocessor system.
 *
 * The MP Floating Pointer Structure. This structure contains a physical
 * address pointer to the MP configuration table and other MP feature
 * information bytes. When present, this structure indicates that the system
 * conforms to the MP specification. This structure must be stored in at
 * least one of the following memory locations, because the operating system
 * searches for the MP floating pointer structure in the order described below:
 *
 * a. In the first kilobyte of Extended BIOS Data Area (EBDA), or
 * b. Within the last kilobyte of system base memory (e.g., 639K-640K for
 * systems with 640 KB of base memory or 511K-512K for systems with 512 KB
 * of base memory) if the EBDA segment is undefined, or
 * c. In the BIOS ROM address space between 0F0000h and 0FFFFFh.
 *
 * This structure may contain an ID-number for one a small number of standard
 * SMP system architectures, or may contain the memory address for a more
 * extensive "MP Configuration Table" having entries that specify a
 * "customized" system architecture.
 */

smp_floating_pointer_t * smp_probe(void)
    {
    cpu_addr_t min_address, max_address;
    cpu_addr_t address, found_address;
    uint8_t * ptr;

    /* Scan for the MP floating pointer structure */

    min_address = PA2KA(0x0);
    max_address = PA2KA(0x8000);

smp_scan_again:

    found_address = 0;

    for (address = min_address; address < max_address; address += 16)
        {

        ptr = (uint8_t*) address;

        /* printk("scanning %p\n",ptr); */

        if (ptr[0] == '_' && ptr[1] == 'M' && ptr[2] == 'P' && ptr[3] == '_')
            {
            /* Verify the checksum */

            int checksum = 0;
            int i = 0 ;

            for (i = 0; i < sizeof(smp_floating_pointer_t); i++)
                checksum += ptr[i];

            if ((checksum & 255) != 0)
                continue;

            found_address = address;

            printk("Found MP Floating Pointer Structure @%p\n", found_address);

            return (smp_floating_pointer_t *)found_address;
            }
        }

    if (found_address == 0)
        {
        if (max_address == (PA2KA(0x8000)))
            {
            min_address = PA2KA(0x9f000);
            max_address = PA2KA(0xa0000);
            goto smp_scan_again;
            }
        else if (max_address == (PA2KA(0xa0000)))
            {
            min_address = PA2KA(0xf0000);
            max_address = PA2KA(0x100000);
            goto smp_scan_again;
            }
        }

    return NULL;
    }

void smp_init(void)
    {
    smp_floating_pointer_t *smp_fp;
    smp_config_table_t *smp_config;
    int i;

    /* Clear the SMP CPU infomation structures */
    
    memset(&smp_cpus[0], 0, 
           sizeof(smp_cpu_info_t) * CONFIG_NR_CPUS);

    smp_fp = (smp_floating_pointer_t *) smp_probe();

    if (smp_fp == 0)
        {
        printk( "smp_init: no MP structure found\n");
        return;
        }

    /*
     * Bit 7: IMCRP. When the IMCR presence bit is set, the IMCR is present and
     * PIC Mode is implemented; otherwise, Virtual Wire Mode is implemented.
     */

    smp_IMCRP = smp_fp->smp_features2 >> 7;

   /*
    * The physical address pointer field contains the address of the beginning
    * of the MP configuration table. If it is nonzero, the MP configuration table
    * can be accessed at the physical address provided in the pointer structure.
    * This field must be all zeros if the MP configuration table does not exist.
    */

    smp_config = (smp_config_table_t *) (PA2KA(smp_fp->smp_config_table));

    if (smp_config->cfg_signature[0] != 'P' ||
        smp_config->cfg_signature[1] != 'C' ||
        smp_config->cfg_signature[2] != 'M' ||
        smp_config->cfg_signature[3] != 'P')
        {
        printk("smp_init: bad signature in MP config table\n");
        return;
        }

    switch (smp_config->cfg_revision)
        {
        case 1    :
            printk( "smp_init: MP Specification rev. 1.1\n");
            break;
        case 4  :
            printk( "smp_init: MP Specification rev. 1.4\n");
            break;
        default    :
            printk( "smp_init: MP Specification rev. unknown\n");
            break;
        }

    smp_parse_config(smp_config);

    i = 0;

    while (i < smp_cpu_count)
        {
        printk("smp_init: cpu %i, lapic_id = %i, %s\n",
                smp_cpus[i].cpu_no,
                smp_cpus[i].apic_id,
                smp_cpus[i].cpu_info & MP_CFG_CPU_FLAGS_BSP? "BSP":"AP");

        i++;
        }

    /* Initalize the BSP LAPIC */
    
    lapic_write(LAPIC_SPURIOUS, INTR_LAPIC_SPURIOUS |
                              LAPIC_SPURIOUS_LAPIC_ENABLED |
                              LAPIC_SPURIOUS_FOCUS_DISABLED);
    lapic_write(LAPIC_TASKPRI,0);

    spinlock_init(&ap_heartbeat_lock);

    offset = 0;

    for (i = 0 ; i < smp_cpu_count; i++)
        {
        /* spin up all APs */
        
        if (!(smp_cpus[i].cpu_info & MP_CFG_CPU_FLAGS_BSP))
            {
            offset += 2;

            smp_ap_booted = 0;

            smp_activiate_ap(i);

            /* Once AP starts up it will set smp_ap_booted to none zero */
            
            while (smp_ap_booted == 0);
            }
        }
    }

void smp_test_func (void)
    {
    uint8_t *p = (uint8_t*)((uint64_t)PA2KA(0xb8000));
    int i,j;

    again:
         spinlock_lock(&ap_heartbeat_lock);
         for(i = 0 ; i < 10000;i++)
             p[11]++;

         spinlock_unlock(&ap_heartbeat_lock);
         for(j = 0 ; j < 1000 ; j++)
             asm ("nop");

         goto again;

    }


void smp_ap_ready (void)
    {
    uint8_t *p = (uint8_t*)VGA_TEXT_MODE_KERN_BASE_ADDR;
    int i, off,j;

    uint64_t lapic_base;

    lapic_base = read_msr(MSR_IA32_APICBASE);

    printk("AP cpu-%d is now ready, lapic_base %p\n",
        lapic_id(),
        lapic_base);

    printk("RIP = %p\n",get_ip());

    off = offset;
    smp_ap_booted = 1;

    again:

        if (bsp_apic_init_done == 1)
            goto create_ap_task;

        spinlock_lock(&ap_heartbeat_lock);
        for(i = 0 ; i < 10000 ; i ++)
            {
            for(j = 0 ; j < 5 ; j++)
                 asm ("nop");

            p[1+off]+=off;
            p[off] ++;
            }
        spinlock_unlock(&ap_heartbeat_lock);

        for(j = 0 ; j < 1000 ; j++)
             asm ("nop");

        goto again;

create_ap_task:
        printk("AP cpu-%d creating tasks\n", this_cpu());
        thread_create_test();
#if CONFIG_SCHED_USE_APIC
        lapic_init();
#else
        pit_timer_init(CONFIG_HZ);
#endif
    cpu_heart_beat(this_cpu());
    }

void smp_ap_entry_point(void)
    {
    size_t *new_stack;

    printk("ok\n");

    /* load the GDT */
    x64_gdt_ap_init();

    /* load an IDT */
    x64_idt_ap_init();

    new_stack = (size_t*) page_alloc();
    if (!new_stack)
        {
        printk("x64_smp_ap_entry(): unable to alloc new stack!!");
        for(;;)
             asm ("cli; hlt");
        }

    memset(new_stack,0,PAGE_SIZE);

    /* switch to this new stack */

    asm volatile(
        "movq %0, %%rsp\n\t"
        :
        :"r"((uint64_t)new_stack + ((PAGE_SIZE)-100))
        :"rcx"
        );

    smp_ap_ready();
    }

/*
 * The MP Configuration Table. This table is optional. The table is composed
 * of a base section and an extended section. The base section contains entries
 * that are completely backwards compatible with previous versions of this
 * specification. The extended section contains additional entry types. The
 * MP configuration table contains explicit configuration information about
 * APICs, processors, buses, and interrupts. The table consists of a header,
 * followed by a number of entries of various types. The format and length of
 * each entry depends on its type. When present, this configuration table must
 * be stored either in a non-reported system RAM or within the BIOS read-only
 * memory space.
 *
 * The following is a list of the suggested memory spaces for the MP
 * configuration table:
 * a. In the first kilobyte of Extended BIOS Data Area (EBDA), or
 * b. Within the last kilobyte of system base memory if the EBDA segment is
 *    undefined, or
 * c. At the top of system physical memory, or
 * d. In the BIOS read-only memory space between 0E0000h and 0FFFFFh.
 */

void smp_parse_config
    (
    struct smp_config_table *config
    )
    {
    uint8_t * p;
    uint8_t entry_type;
    int i;

    /* Go to the config entries area */

    p = (void*)((cpu_addr_t)config + sizeof(struct smp_config_table));

    x64_lapic_reg_base = PA2VA(config->cfg_local_apic_base);

    printk("smp_parse_config: LAPIC @%p\n", config->cfg_local_apic_base);

    for (i = 0; i < config->cfg_entry_count; i++)
        {
        entry_type = (uint8_t)p[0];

        switch (entry_type)
            {
            case MP_CONFIG_ENTRY_TYPE_PROCESSOR:
                {
                mp_cfg_entry_processor_t * smp_proc =
                                         (mp_cfg_entry_processor_t*)p;

                if (smp_proc->cpu_flags & MP_CFG_CPU_FLAGS_EN)
                    {
                    smp_cpus[smp_cpu_count].cpu_no = smp_cpu_count;
                    smp_cpus[smp_cpu_count].apic_id = smp_proc->lapic_id;
                    smp_cpus[smp_cpu_count].cpu_info = smp_proc->cpu_flags;
                    smp_cpu_count ++;

                    printk("smp_parse_config:found cpu-%d enabled.\n",
                        smp_proc->lapic_id);

                    /* Remember which processor is the BSP */

                    if (smp_proc->cpu_flags & MP_CFG_CPU_FLAGS_BSP)
                        smp_bsp_idx = i;
                    }
                else
                    {
                    /*
                    printk("smp_parse_config:found cpu-%d disabled.\n",
                        smp_proc->local_apic_id);
                    */
                    }

                p += MP_CONFIG_ENTRY_PROCESSOR_LEN;
                break;
                }

            case MP_CONFIG_ENTRY_TYPE_BUS:
                {
                p += MP_CONFIG_ENTRY_BUS_LEN;

                break;
                }

            case MP_CONFIG_ENTRY_TYPE_IO_APIC:
                {

                p += MP_CONFIG_ENTRY_IOAPIC_LEN;

                break;
                }

            default:
                {
                p += 8;
                /* printk("x64_: unknown MP entry\n"); */
                break;
                }
            }
        }

    return;
    }
