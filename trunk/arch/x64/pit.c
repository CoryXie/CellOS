#include <sys.h>
#include <arch.h>
#include <os.h>

extern  void rtc_update(void);

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
    uint32_t divisor ;

    /* Firstly, register our timer callback.*/
    irq_register(INTR_IRQ0, "timer", (addr_t)timer_irq_handler);

    /* 
     * The value we send to the PIT is the value to divide it's input clock
     * (1193180 Hz) by, to get our required frequency. Important to note is
     * that the divisor must be small enough to fit into 16-bits.
     */
     
    divisor = 1193180 / frequency;

    printk("pit_timer_init: frequency=%d, divisor = %d\n", frequency, divisor);

    /* Send the command byte. */
    ioport_out8(0x43, 0x36);

    /* Send the frequency divisor. */

    /* Divisor has to be sent byte-wise, so split here into upper/lower bytes.*/
    ioport_out8(0x40, (uint8_t)(divisor & 0xFF));
    ioport_out8(0x40, (uint8_t)((divisor>>8) & 0xFF ));

    asm("sti");
    }

