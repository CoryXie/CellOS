#include <sys.h>
#include <arch.h>
#include <os.h>

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
    ioport_out8(0x43, 0x34);
    ioport_out8(0x40, 0xFF);
    ioport_out8(0x40, 0xFF);

    /* Wait for the cycle to begin. */
    do
        {
        ioport_out8(0x43, 0x00);
        slo = ioport_in8(0x40);
        shi = ioport_in8(0x40);
        }
    while (shi != 0xFF);

    /* Get the start TSC value. */
    start = rdtsc();

    /* Wait for the high byte to drop to 128. */
    do
        {
        ioport_out8(0x43, 0x00);
        elo = ioport_in8(0x40);
        ehi = ioport_in8(0x40);
        }
    while (ehi > 0x80);

    /* Get the end TSC value. */
    end = rdtsc();

    /* Calculate the differences between the values. */
    cycles = end - start;
    ticks = ((ehi << 8) | elo) - ((shi << 8) | slo);

    /* Calculate frequency. */
    return (cycles * PIT_8254_OSC_FREQ) / ticks;
    }

/* Calculate BUS (LAPIC) frequency in HZ. */
uint64_t calculate_lapic_frequency(void)
    {
    uint16_t shi, slo, ehi, elo, pticks;
    uint64_t end, lticks;

    /* First set the PIT to rate generator mode. */
    ioport_out8(0x43, 0x34);
    ioport_out8(0x40, 0xFF);
    ioport_out8(0x40, 0xFF);

    /* Wait for the cycle to begin. */
    do
        {
        ioport_out8(0x43, 0x00);
        slo = ioport_in8(0x40);
        shi = ioport_in8(0x40);
        }
    while (shi != 0xFF);

    /* Kick off the LAPIC timer. */
    lapic_write(LAPIC_TICR, 0xFFFFFFFF);

    /* Wait for the high byte to drop to 128. */
    do
        {
        ioport_out8(0x43, 0x00);
        elo = ioport_in8(0x40);
        ehi = ioport_in8(0x40);
        }
    while (ehi > 0x80);

    /* Get the current timer value. */
    end = lapic_read(LAPIC_TCCR);

    /* Calculate the differences between the values. */
    lticks = 0xFFFFFFFF - end;
    pticks = ((ehi << 8) | elo) - ((shi << 8) | slo);

    /* Calculate frequency. */
    return (lticks * 4 * PIT_8254_OSC_FREQ) / pticks;
    }


