#ifndef _ARCH_X64_INTERRUPT_H
#define _ARCH_X64_INTERRUPT_H

#include <sys.h>

#define INTR_IRQ0 32 /* PIT */
#define INTR_IRQ1 33 /* i8042 Keyboard */
#define INTR_IRQ2 34
#define INTR_IRQ3 35
#define INTR_IRQ4 36
#define INTR_IRQ5 37
#define INTR_IRQ6 38
#define INTR_IRQ7 39
#define INTR_IRQ8 40
#define INTR_IRQ9 41
#define INTR_IRQ10 42
#define INTR_IRQ11 43
#define INTR_IRQ12 44
#define INTR_IRQ13 45
#define INTR_IRQ14 46
#define INTR_IRQ15 47

/* Local LAPIC interrupt vectors. */
#define INTR_LAPIC_TIMER        0xf0    /* Timer */
#define INTR_LAPIC_SPURIOUS     0xf1    /* Spurious */
#define INTR_LAPIC_IPI          0xf2     /* IPI message */
#define INTR_LAPIC_RESCHEDULE   0xf3     /* Reschedule */

/** Enable interrupts.
 * @return		Previous interrupt state. */
static inline bool intr_enable(void) {
	unative_t flags;

	__asm__ volatile("pushf; sti; pop %0" : "=r"(flags));
	return (flags & (1<<9)) ? true : false;
}

/** Disable interrupts.
 * @return		Previous interrupt state. */
static inline bool intr_disable(void) {
	unative_t flags;

	__asm__ volatile("pushf; cli; pop %0" : "=r"(flags));
	return (flags & (1<<9)) ? true : false;
}

/** Restore saved interrupt state.
 * @param state		State to restore. */
static inline void intr_restore(bool state) {
	if(state) {
		__asm__ volatile("sti");
	} else {
		__asm__ volatile("cli");
	}
}

/** Get interrupt state.
 * @return		Current interrupt state. */
static inline bool intr_state(void) {
	unative_t flags;

	__asm__ volatile("pushf; pop %0" : "=r"(flags));
	return (flags & (1<<9)) ? true : false;
}

int irq_register
    (
    uint16_t irq_no, 
    char * owner, 
    addr_t handler
    );

int irq_unregister
    (
    uint16_t irq_no
    );

void disable_pit_intr(void);
void enable_pit_intr(void);
void disable_keyboad_intr(void);
void enable_keyboad_intr(void);

#endif /* _ARCH_X64_INTERRUPT_H */
