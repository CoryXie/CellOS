#include <sys.h>
#include <arch.h>
#include <os.h>

/*
 * Local-APIC:
 *
 * The purpose of each processor's LAPIC is to allow the CPUs in a
 * multiprocessor system to send messages to one another and to manage
 * the delivery of the interrupt-requests from the various peripheral
 * devices to one (or more) of the CPUs in a dynamically programmable way.
 *
 * Each processor's Local-APIC has a variety of registers, all 'memory mapped'
 * to paragraph-aligned addresses within the 4KB page at physical-address
 * 0xFEE00000.
 *
 * Analogies with the PIC:
 *
 * Among the registers in a Local-APIC are these (which had analogues in the
 * older 8259 PIC's design:
 * - IRR: Interrupt Request Register (256-bits)
 * - ISR: In-Service Register (256-bits)
 * - TMR: Trigger-Mode Register (256-bits)
 * For each of these, its 256-bits are divided among eight 32-bit register
 * addresses.
 *
 * New way to do 'EOI':
 *
 * Instead of using a special End-Of-Interrupt command-byte, the Local-APIC
 * contains a dedicated 'write-only' register (named the EOI Register) which
 * an Interrupt Handler writes to when it is ready to signal an EOI.
 *
 * # issuing EOI to the Local-APIC
 * mov  $0xFEE00000, %ebx   # address of the cpu's Local-APIC
 * movl $0, %fs:0xB0(%ebx)  # write any value into EOI register
 * # Here we assume segment-register FS holds the selector for a
 * # segment-descriptor for a 'writable' 4GB-size expand-up data-segment
 * # whose base-address equals 0
 *
 * Each CPU has its own timer!
 *
 * Four of the Local-APIC registers are used to implement a programmable timer.
 * It can privately deliver a periodic interrupt (or one-shot interrupt) just
 * to its own CPU
 * - 0xFEE00320: Timer Vector register
 * - 0xFEE00380: Initial Count register
 * - 0xFEE00390: Current Count register
 * - 0xFEE003E0: Divider Configuration register
 * When the timer is programmed for 'periodic' mode, the Current Count is
 * automatically reloaded from the Initial Count register, then counts down
 * with each CPU bus-cycle, generating an interrupt when it reaches zero.
 *
 * Using the timer's interrupts:
 *
 * - Setup the desired Initial Count value
 * - Select the desired Divide Configuration
 * - Setup the APIC-timer's LVT register with the desired interrupt-ID
 *   number and counting mode ('periodic' or 'one-shot'), and clear the LVT
 *   register's 'Mask' bit to initiate the automatic countdown operation.
 */

uint64_t x64_lapic_reg_base = 0;
uint64_t lapic_freq_hz = 0;
uint32_t bsp_apic_init_done = 0;

extern uint8_t smp_IMCRP;


void lapic_write
(
    uint32_t offset,
    uint32_t value
)
    {
    *(volatile uint32_t*) (x64_lapic_reg_base + offset) = value;
    }

uint32_t lapic_read
(
    uint32_t offset
)
    {
    return *(volatile uint32_t*) (x64_lapic_reg_base + offset);
    }

void lapic_ipi
(
    uint32_t dest,
    uint32_t type,
    uint8_t vec
)
    {
    lapic_write(LAPIC_ICR_HIGH, dest << 24);
    lapic_write(LAPIC_ICR_LOW, (uint32_t)(0x4000 | type | vec));
    }

uint8_t lapic_id(void)
    {
    uint32_t reg32;

    reg32 = lapic_read(LAPIC_ID);

    return (uint8_t)(reg32 >> 24);
    }

static inline void lapic_eoi(void)
    {
    lapic_write(LAPIC_EOI, 0);
    }

static void lapic_timer_enable_one_shot(void)
    {
    /* unmasked one-shot !*/

    lapic_write(LAPIC_LVT_TIMER, INTR_LAPIC_TIMER);
    }

static void lapic_timer_enable_periodic(void)
    {
    /* unmasked periodic !*/

    lapic_write(LAPIC_LVT_TIMER, INTR_LAPIC_TIMER | LAPIC_LVT_TIMER_PERIODIC);
    }

static void lapic_timer_disable(void)
    {
    /* masked one-shot !*/
    lapic_write(LAPIC_LVT_TIMER, INTR_LAPIC_TIMER | LAPIC_LVT_MASKED);
    }

static void lapic_timer_count_init(uint32_t HZ)
    {
    /* 
     * 1/HZ = n * (div/lapic_freq_hz); 
     * => n=(lapic_freq_hz)/(div * HZ))
     */
    
    uint32_t count = (uint32_t)(lapic_freq_hz / (8 * HZ));


    lapic_write(LAPIC_TICR, count);
    }

void lapic_timer_irq_handler
    (
    uint64_t stack_frame
    )
    {
    lapic_eoi();

    sched_tick((stack_frame_t *)stack_frame);
    }

void lapic_spurious_handler
    (
    uint64_t stack_frame
    )
    {
    lapic_eoi(); /* spurious interrupt does not need EOI but... */
    printk("lapic_spurious_handler on cpu-%d\n", this_cpu());
    }

void lapic_ipi_handler
    (
    uint64_t stack_frame
    )
    {
    lapic_eoi();
    printk("lapic_ipi_handler on cpu-%d\n", this_cpu());
    }

void lapic_reschedule_handler
    (
    uint64_t stack_frame
    )
    {
    lapic_eoi();
    //printk("lapic_reschedule_handler on cpu-%d\n", this_cpu());
    //reschedule();
    }

void lapic_dump(void)
    {
    uint64_t reg32 = x64_lapic_reg_base;

    int i = 0;

    while (i < 1024)
        {
        printk("reg %p val %p\n", reg32, lapic_read(i));

        i += 0x10;

        reg32 += 0x10;
        }
    }

status_t lapic_init(void)
    {
    cpu_addr_t base;
    uint32_t reg32;

    if (!has_apic())
        {
        printk("NO LAPIC\n");

        return ERROR;
        }

    if (has_x2apic())
        {
        printk("CPU supports x2APIC\n");
        }
    else
        {
        printk("CPU does not support x2APIC\n");
        }
    /*
     * The operating system should switch over to Symmetric I/O Mode to start
     * multiprocessor operation. If the IMCRP bit of the MP feature information
     * bytes is set, the operating system must set the IMCR to APIC mode. The
     * operating system should not write to the IMCR unless the IMCRP bit
     * is set.
     */

    if (smp_IMCRP)
        {
        /* Switch to SYMMETRIC_IO mode from PIC/VIRTUAL_WIRE mode */

        printk("Switch to SYMMETRIC_IO mode\n");

        ioport_out8 (IMCR_ADRS, IMCR_REG_SEL);

        ioport_out8 (IMCR_DATA, IMCR_IOAPIC_ON);
        }

    base = read_msr(MSR_IA32_APICBASE);

    base |= MSR_IA32_APICBASE_ENABLE;

    write_msr(MSR_IA32_APICBASE, base);

    /* If bit 11 is 0, the LAPIC is disabled */

    if (!(base & MSR_IA32_APICBASE_ENABLE))
        {
        printk("LAPIC disabled\n");

        return ERROR;
        }

    if (base & MSR_IA32_APICBASE_BSP)
        {
        printk("This is BSP\n");
        }
    else
        {
        printk("This is AP\n");
        }

    base &= MSR_IA32_APICBASE_BASE;

    /*
     * If the LAPIC base register map is not set, then we have not done
     * the MP config scan, but we can get it here!
     */

    if (!x64_lapic_reg_base)
        {
        /* Map it into the kernel space */

        x64_lapic_reg_base = KERNEL_VIRT_MAP_BASE + base;
        }

    irq_register(INTR_LAPIC_TIMER,
                 "LAPIC_TIMER",
                 (addr_t)lapic_timer_irq_handler);

    irq_register(INTR_LAPIC_SPURIOUS,
                 "LAPIC_SPURIOUS",
                 (addr_t)lapic_spurious_handler);

    irq_register(INTR_LAPIC_IPI,
                 "LAPIC_IPI",
                 (addr_t)lapic_ipi_handler);

    irq_register(INTR_LAPIC_RESCHEDULE,
                 "LAPIC_RESCHEDULE",
                 (addr_t)lapic_reschedule_handler);

    uint32_t lvr = lapic_read(LAPIC_LVR);

    /* Version - The version numbers of the local LAPIC */
    uint32_t vers = GET_LAPIC_VERSION(lvr);

    /* Max LVT Entry - Shows the number of LVT entries minus 1 */

    uint32_t maxlvt = GET_LAPIC_MAXLVT(lvr) + 1;

    printk("LAPIC: version 0x%x, %d LVTs\n", vers, maxlvt);

    lapic_write(LAPIC_TASKPRI, 0);

    lapic_write(LAPIC_EOI, 0);

    if (maxlvt >= 4)
        lapic_write(LAPIC_LVTPC, 0);

    lapic_write(LAPIC_ESR, 0);
    lapic_write(LAPIC_ESR, 0);

    /*
     * To initialise the BSP's LAPIC, set the enable bit in the spurious
     * interrupt vector register and set the error interrupt vector in the
     * local vector table.
     */

    lapic_write(LAPIC_SPURIOUS, INTR_LAPIC_SPURIOUS |
                LAPIC_SPURIOUS_LAPIC_ENABLED |
                LAPIC_SPURIOUS_FOCUS_DISABLED);

    lapic_write(LAPIC_TDCR, LAPIC_TDIV_8);

    reg32 = lapic_read(LAPIC_SPURIOUS);

    /* Send an Init Level De-Assert to synchronise arbitration ID's. */

    lapic_write(LAPIC_ICR_HIGH, 0);

    lapic_write(LAPIC_ICR_LOW, LAPIC_DEST_ALLINC | LAPIC_DM_INIT |
                LAPIC_INT_LEVELTRIG);

    while (lapic_read(LAPIC_ICR_LOW) & LAPIC_ICR_BUSY);

    /* Figure out the CPU bus frequency only for BSP and apply for AP */

    if (this_cpu() == 0)
        {  
        printk("cpu%d - calculate lapic frequency...", this_cpu());
        
        lapic_freq_hz = calculate_lapic_frequency();

        printk("done! lapic_freq_hz %lld\n", lapic_freq_hz);
        }

    lapic_timer_count_init(CONFIG_HZ);

    lapic_timer_enable_periodic();

    disable_pit_intr();

    lapic_write(LAPIC_TASKPRI, 0);
    lapic_write(LAPIC_EOI, 0);

    printk("lapic_init done for cpu-%d!\n", this_cpu());

    if (this_cpu() == 0)
        bsp_apic_init_done = 1;

    return OK;
    }

