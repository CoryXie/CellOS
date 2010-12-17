/* clock.h - clock managment */

#ifndef _OS_CLOCK_H
#define _OS_CLOCK_H

#include <sys.h>
#include <arch.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (* clock_irq_handler_t)(stack_frame_t *r);

/* Clock mode */
#define CLOCK_MODE_UNKNOWN          0
#define CLOCK_MODE_ONESHOT          1
#define CLOCK_MODE_PERIODIC         2

/* Clock capability flags */
#define CLOCK_FLAGS_PERIODIC	    1
#define CLOCK_FLAGS_ONESHOT	        2
#define CLOCK_FLAGS_PERCPU		    4
#define CLOCK_FLAGS_C3STOP		    8
#define CLOCK_FLAGS_POW2DIV	        16

typedef struct os_event_clock 
    { 
    /* Colck Name */
    char * name;

    /* List of Clock in the system */
    list_t node;

    /* Capability/Feature/Limitation flags */
    int flags;

    /* Current working mode */
    int mode;

    /* The higher value means its quality is better in two clocks */
    int precedence;

    /* Base frequency in HZ */
    uint64_t base_freq;

    /* The minimum possible period (ns) */
    abstime_t min_period_ns;

    /* The maximum possible period (ns) */
    abstime_t max_period_ns;

    /* 
     * Start to trigger at 'first' and repeat in 'period' ns;
     * If 'period' is 0 then it is oneshot! The implementation 
     * may round the trigger and period with most nearst values!
     */
	int (*start) (struct os_event_clock *, 
                  struct timespec *, abstime_t);
    
    /* Stop the clock to trigger */
	int (*stop) (struct os_event_clock *);

    /* Get offset from the last trigger in ns */
	int (*get_offset)(struct os_event_clock *, abstime_t *);

    /* Get the actual resolution in ns */
	int (*get_resolution)(struct os_event_clock *, abstime_t *);

	int (*event_handler)(struct os_event_clock *, void *);
    }os_event_clock_t;

status_t clock_add(struct os_event_clock *clk);
status_t clock_remove(struct os_event_clock *clk);
struct os_event_clock * clock_select(abstime_t res);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _OS_CLOCK_H */


