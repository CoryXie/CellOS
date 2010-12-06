#include <sys.h>
#include <arch.h>
#include <os.h>

#include <acpi.h>
#include <accommon.h>
#include <amlcode.h>
#include <acparser.h>
#include <acdebug.h>
#include <acnamesp.h>

/*
I/O port     Usage

0x40         Channel 0 data port (read/write)
0x41         Channel 1 data port (read/write)
0x42         Channel 2 data port (read/write)
0x43         Mode/Command register (write only, a read is ignored)
*/

#define PIT_8254_CHAN0  0x40
#define PIT_8254_CHAN1  0x41
#define PIT_8254_CHAN2  0x42
#define PIT_8254_MCR    0x43

/*
  The Mode/Command register at I/O address 0x43 contains the following:
  
   Bits         Usage
   6 and 7      Select channel :
                   0 0 = Channel 0
                   0 1 = Channel 1
                   1 0 = Channel 2
                   1 1 = Read-back command (8254 only)
   4 and 5      Access mode :
                   0 0 = Latch count value command
                   0 1 = Access mode: lobyte only
                   1 0 = Access mode: hibyte only
                   1 1 = Access mode: lobyte/hibyte
   1 to 3       Operating mode :
                   0 0 0 = Mode 0 (interrupt on terminal count)
                   0 0 1 = Mode 1 (hardware re-triggerable one-shot)
                   0 1 0 = Mode 2 (rate generator)
                   0 1 1 = Mode 3 (square wave generator)
                   1 0 0 = Mode 4 (software triggered strobe)
                   1 0 1 = Mode 5 (hardware triggered strobe)
                   1 1 0 = Mode 2 (rate generator, same as 010b)
                   1 1 1 = Mode 3 (square wave generator, same as 011b)
   0            BCD/Binary mode: 0 = 16-bit binary, 1 = four-digit BCD
*/

/* BCD/Binary mode */
#define PIT_8254_MCR_BIN                (0 << 0)
#define PIT_8254_MCR_BCD                (1 << 0)

/* Operating mode */
#define PIT_8254_MCR_OP_INTR            (0 << 1)
#define PIT_8254_MCR_OP_ONE_SHOT        (1 << 1)
#define PIT_8254_MCR_OP_RATE_GEN        (2 << 1)
#define PIT_8254_MCR_OP_SQUARE          (3 << 1)
#define PIT_8254_MCR_OP_SOFT_STROBE     (4 << 1)
#define PIT_8254_MCR_OP_HARD_STROBE     (5 << 1)

/* Access mode */
#define PIT_8254_MCR_AC_LATCH           (0 << 4)
#define PIT_8254_MCR_AC_LO              (1 << 4)
#define PIT_8254_MCR_AC_HI              (2 << 4)
#define PIT_8254_MCR_AC_LOHI            (3 << 4)

/* Select channel */
#define PIT_8254_MCR_SL_CH0             (0 << 6)
#define PIT_8254_MCR_SL_CH1             (1 << 6)
#define PIT_8254_MCR_SL_CH2             (2 << 6)
#define PIT_8254_MCR_RD_BACK            (3 << 6)

/* The oscillator used by the PIT chip runs at (roughly) 1.193182 MHz */
#define PIT_8254_OSC_FREQ               1193182

void timer_irq_handler
    (
    uint64_t stack_frame
    )
    {
    sched_tick((stack_frame_t *)stack_frame);
    }

void pit_timer_init
    (
    uint32_t frequency
    )
    {
    uint32_t divisor;

    /* Firstly, register our timer callback.*/
    irq_register(INTR_IRQ0, "timer", (addr_t)timer_irq_handler);

    /* 
     * The value we send to the PIT is the value to divide it's input clock
     * (1193180 Hz) by, to get our required frequency. Important to note is
     * that the divisor must be small enough to fit into 16-bits.
     */
     
    divisor = PIT_8254_OSC_FREQ / frequency;

    printk("pit_timer_init: frequency=%d, divisor = %d\n", 
           frequency, divisor);

    /* Send the command byte. */
    ioport_out8(PIT_8254_MCR, PIT_8254_MCR_BIN |
                              PIT_8254_MCR_OP_SQUARE |
                              PIT_8254_MCR_AC_LOHI |
                              PIT_8254_MCR_SL_CH0);

    /* Send the frequency divisor. */

    /*
     * Divisor has to be sent byte-wise, so split here 
     * into upper/lower bytes.
     */
    ioport_out8(PIT_8254_CHAN0, (uint8_t)(divisor & 0xFF));
    ioport_out8(PIT_8254_CHAN0, (uint8_t)((divisor >> 8) & 0xFF ));
    }

/* 
 * pit_timer_get_current - read the current count of PIT timer
 *
 * For the "lobyte/hibyte" access mode you need to send the latch command 
 * (described above) to avoid getting wrong results. If any other code 
 * could try set the PIT channel's reload value or read its current count
 * after you've sent the latch command but before you've read the highest
 * 8 bits, then you have to prevent it. Disabling interrupts works for 
 * single CPU computers. 
 */
uint16_t pit_timer_get_current(void)
    {
    ipl_t ipl = interrupts_disable();
    uint16_t cur;
    uint8_t  lo;
    uint8_t  hi;
    
    /* Send the command byte. */
    ioport_out8(PIT_8254_MCR, PIT_8254_MCR_BIN |
                              PIT_8254_MCR_OP_INTR |
                              PIT_8254_MCR_AC_LATCH |
                              PIT_8254_MCR_SL_CH0);

    /* Low byte of count */
    lo = ioport_in8(PIT_8254_CHAN0);

    /* High byte of count */
    hi = ioport_in8(PIT_8254_CHAN0);

    /* Form the current count */
    cur = (hi << 8) | lo;

    interrupts_restore(ipl);
    
    return cur;
    }

/* 
 * pit_timer_set_reload - setting the reload value of PIT timer
 *
 * For the "lobyte/hibyte" access mode you need to send the lowest 8 bits,
 * then the highest 8 bits. If any other code could try set the PIT channel's
 * reload value or read its current count after you've sent the lowest 8 
 * bits but before you've sent the highest 8 bits, then you have to prevent
 * it. Disabling interrupts works for single CPU computers.
 */
void pit_timer_set_reload
    (
    uint16_t reload
    )
    {
    uint8_t  lo;
    uint8_t  hi;
    ipl_t   ipl; 

    /* Low byte of count */
    lo = (uint8_t)(reload & 0xFF);

    /* High byte of count */
    hi = (uint8_t)((reload >> 8) & 0xFF);

    ipl = interrupts_disable();

    ioport_out8(PIT_8254_CHAN0, lo);
    ioport_out8(PIT_8254_CHAN0, hi);

    interrupts_restore(ipl);
    }

/* Calculate CPU frequency in HZ. */
uint64_t calculate_cpu_frequency(void)
    {
    uint16_t shi, slo, ehi, elo, ticks;
    uint64_t start, end, cycles;

    /* First set the PIT to rate generator mode. */
    ioport_out8(PIT_8254_MCR, PIT_8254_MCR_BIN |
                              PIT_8254_MCR_OP_RATE_GEN |
                              PIT_8254_MCR_AC_LOHI |
                              PIT_8254_MCR_SL_CH0);
    
    /* Send the frequency divisor. */
    ioport_out8(PIT_8254_CHAN0, 0xFF);
    ioport_out8(PIT_8254_CHAN0, 0xFF);

    /* Wait for the cycle to begin. */
    do
        {
        ioport_out8(PIT_8254_MCR, PIT_8254_MCR_BIN |
                                  PIT_8254_MCR_OP_INTR |
                                  PIT_8254_MCR_AC_LATCH |
                                  PIT_8254_MCR_SL_CH0);
        slo = ioport_in8(PIT_8254_CHAN0);
        shi = ioport_in8(PIT_8254_CHAN0);
        }
    while (shi != 0xFF);

    /* Get the start TSC value. */
    start = rdtsc();

    /* Wait for the high byte to drop to 128. */
    do
        {
        ioport_out8(PIT_8254_MCR, PIT_8254_MCR_BIN |
                                  PIT_8254_MCR_OP_INTR |
                                  PIT_8254_MCR_AC_LATCH |
                                  PIT_8254_MCR_SL_CH0);
        elo = ioport_in8(PIT_8254_CHAN0);
        ehi = ioport_in8(PIT_8254_CHAN0);
        }
    while (ehi > 0x80);

    /* Get the end TSC value. */
    end = rdtsc();

    /* Calculate the differences between the values. */
    cycles = end - start;
    ticks = ((ehi << 8) | elo) - ((shi << 8) | slo);

    /*
     * Calculate frequency. 
     *
     * Based on the simple theory that the two clocks were 
     * running through the same period of time, thus the 
     * following equation applies:
     * 
     * (1/cpu_freq) * cycles = (1/PIT_8254_OSC_FREQ) * ticks
     *
     * ==>
     *
     * cpu_freq = (cycles * PIT_8254_OSC_FREQ) / ticks;
     */
    return (cycles * PIT_8254_OSC_FREQ) / ticks;
    }

#define PM_TIMER_CALIBRATE_DELAY_COUNT 10000000

/* Calculate CPU frequency in HZ. */
uint64_t calculate_cpu_frequency2(void)
    {
    uint32_t start1 = 0, end1 = 0, ticks, us_elapsed;
    uint64_t start, end, cycles;
    volatile uint64_t count;

    printk("PM timer uses %s space %p and its width is %d bits\n",
           AcpiGbl_FADT.XPmTimerBlock.SpaceId ? "IOPORT" : "MEMORY", 
           AcpiGbl_FADT.XPmTimerBlock.Address,
           AcpiGbl_FADT.Flags & ACPI_FADT_32BIT_TIMER ? 32 : 24);
        
    /* Get the start TSC value. */
    start = rdtsc();
    
    if (AcpiGetTimer(&start1) != AE_OK)
        {
        printk("AcpiGetTimer fail 1\n");
        return 0;
        }

    for (count = 0; count < PM_TIMER_CALIBRATE_DELAY_COUNT; count++) 
        {
        count++;
        cpu_relax();
        count--;
        }
    
    /* Get the end TSC value. */
    end = rdtsc();

    if (AcpiGetTimer(&end1) != AE_OK)
        {
        printk("AcpiGetTimer fail 1\n");
        return 0;
        }
        
    /* Calculate the differences between the values. */
    cycles = end - start;
    ticks = end1 - start1;
    if (ticks == 0)
        {
        printk("AcpiGetTimer fail 3, start1 %d end1 %d\n", start1, end1);
        return 0;
        }
    
    AcpiGetTimerDuration(start1, end1, &us_elapsed);
    
    printk("AcpiGetTimer start1 %d end1 %d us_elapsed %d\n", start1, end1, us_elapsed);

    return (cycles * USECS_PER_SEC) / us_elapsed;
    }

/* Calculate BUS (LAPIC) frequency in HZ. */
uint64_t calculate_lapic_frequency2(void)
    {
    uint32_t start1 = 0, end1 = 0, ticks, us_elapsed;
    uint64_t end, lticks;
    uint32_t div;
    volatile uint64_t count;
    
    div = lapic_read(LAPIC_TDCR) & LAPIC_TDIV_MSK;
    switch (div)
        {
        case LAPIC_TDIV_1: div = 1; break;
        case LAPIC_TDIV_2: div = 2; break;
        case LAPIC_TDIV_4: div = 4; break;
        case LAPIC_TDIV_8: div = 8; break;
        case LAPIC_TDIV_16: div = 16; break;
        case LAPIC_TDIV_32: div = 32; break;
        case LAPIC_TDIV_64: div = 64; break;
        case LAPIC_TDIV_128: div = 128; break;
        default: div = 1; break;
        }

    /* Kick off the LAPIC timer. */
    lapic_write(LAPIC_TICR, 0xFFFFFFFF);

    if (AcpiGetTimer(&start1) != AE_OK)
        {
        printk("AcpiGetTimer fail 1\n");
        return 0;
        }
    
    for (count = 0; count < PM_TIMER_CALIBRATE_DELAY_COUNT; count++) 
        {
        count++;
        cpu_relax();
        count--;
        }

    /* Get the current timer value. */
    end = lapic_read(LAPIC_TCCR);
    
    if (AcpiGetTimer(&end1) != AE_OK)
        {
        printk("AcpiGetTimer fail 1\n");
        return 0;
        }

    /* Calculate the differences between the values. */
    lticks = 0xFFFFFFFF - end;

    ticks = end1 - start1;
    
    if (ticks == 0)
        {
        printk("AcpiGetTimer fail, start1 %d end1 %d\n", start1, end1);
        return 0;
        }
    
    AcpiGetTimerDuration(start1, end1, &us_elapsed);
    
    printk("AcpiGetTimer start1 %d end1 %d us_elapsed %d\n", start1, end1, us_elapsed);

    return (lticks * USECS_PER_SEC) / us_elapsed;
    }


/* Calculate BUS (LAPIC) frequency in HZ. */
uint64_t calculate_lapic_frequency(void)
    {
    uint16_t shi, slo, ehi, elo, pticks;
    uint64_t end, lticks;
    uint32_t div;
    
    div = lapic_read(LAPIC_TDCR) & LAPIC_TDIV_MSK;
    switch (div)
        {
        case LAPIC_TDIV_1: div = 1; break;
        case LAPIC_TDIV_2: div = 2; break;
        case LAPIC_TDIV_4: div = 4; break;
        case LAPIC_TDIV_8: div = 8; break;
        case LAPIC_TDIV_16: div = 16; break;
        case LAPIC_TDIV_32: div = 32; break;
        case LAPIC_TDIV_64: div = 64; break;
        case LAPIC_TDIV_128: div = 128; break;
        default: div = 1; break;
        }

    
    /* First set the PIT to rate generator mode. */
    ioport_out8(PIT_8254_MCR, PIT_8254_MCR_BIN |
                              PIT_8254_MCR_OP_RATE_GEN |
                              PIT_8254_MCR_AC_LOHI |
                              PIT_8254_MCR_SL_CH0);
    
    /* Send the frequency divisor. */
    ioport_out8(PIT_8254_CHAN0, 0xFF);
    ioport_out8(PIT_8254_CHAN0, 0xFF);

    /* Wait for the cycle to begin. */
    do
        {
        ioport_out8(PIT_8254_MCR, PIT_8254_MCR_BIN |
                                  PIT_8254_MCR_OP_INTR |
                                  PIT_8254_MCR_AC_LATCH |
                                  PIT_8254_MCR_SL_CH0);
        slo = ioport_in8(PIT_8254_CHAN0);
        shi = ioport_in8(PIT_8254_CHAN0);
        }
    while (shi != 0xFF);

    /* Kick off the LAPIC timer. */
    lapic_write(LAPIC_TICR, 0xFFFFFFFF);

    /* Wait for the high byte to drop to 128. */
    do
        {
        ioport_out8(PIT_8254_MCR, PIT_8254_MCR_BIN |
                                  PIT_8254_MCR_OP_INTR |
                                  PIT_8254_MCR_AC_LATCH |
                                  PIT_8254_MCR_SL_CH0);
        elo = ioport_in8(PIT_8254_CHAN0);
        ehi = ioport_in8(PIT_8254_CHAN0);
        }
    while (ehi > 0x80);

    /* Get the current timer value. */
    end = lapic_read(LAPIC_TCCR);

    /* Calculate the differences between the values. */
    lticks = 0xFFFFFFFF - end;
    pticks = ((ehi << 8) | elo) - ((shi << 8) | slo);

    /*
     * Calculate frequency. 
     *
     * Based on the simple theory that the two clocks were 
     * running through the same period of time, thus the 
     * following equation applies:
     * 
     * (1/lapic_freq_actual) * lticks = (1/PIT_8254_OSC_FREQ) * pticks;
     *
     * lapic_freq_actual = lapic_freq_hz/div;
     *
     * ==>
     *
     * lapic_freq_actual = (lticks * PIT_8254_OSC_FREQ) / pticks;
     *
     * lapic_freq_hz = lapic_freq_actual * div;
     */
    return (lticks * div * PIT_8254_OSC_FREQ) / pticks;
    }

int do_freq (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
    {
    ipl_t ipl = interrupts_disable();
    
    printk("CPU %lld HZ (PIT TIMER BASED)\n", calculate_cpu_frequency());
    printk("CPU %lld HZ (ACPI PM TIMER BASED)\n", calculate_cpu_frequency2());
    printk("BUS %lld HZ (PIT TIMER BASED)\n", calculate_lapic_frequency());
    printk("BUS %lld HZ (ACPI PM TIMER BASED)\n", calculate_lapic_frequency2());
    
    interrupts_restore(ipl);
    
    return 0;
    }

CELL_OS_CMD(
    freq,   1,        1,    do_freq,
    "show kernel thread list information",
    "[cpu...]\n"
    "    - list the kernel thread information on the specified cpus;\n"
    "if the cpu arguments are not specified, then list for all cpus."
    );

