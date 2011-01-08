/* clockeventer.h - clock eventer managment */

#ifndef _CLOCK_EVENTER_EVENTER_H
#define _CLOCK_EVENTER_EVENTER_H

#include <sys.h>
#include <arch.h>

#ifdef __cplusplus
extern "C" {
#endif

struct clockeventer;

typedef void (*clockeventer_handler_t)(struct clockeventer * eventer, 
                                            void * arg);

/* Clock mode */
#define CLOCK_EVENTER_MODE_PERIODIC         0
#define CLOCK_EVENTER_MODE_ONESHOT          1

/* Clock Capability/Feature/Limitation flags */
#define CLOCK_EVENTER_FLAGS_PERIODIC        (1 << 0)
#define CLOCK_EVENTER_FLAGS_ONESHOT            (1 << 1)
#define CLOCK_EVENTER_FLAGS_PERCPU            (1 << 2)
#define CLOCK_EVENTER_FLAGS_C3STOP            (1 << 3)

typedef struct clockeventer 
    { 
    /* Colck Name */
    char * name;

    /* List of Clock in the system */
    list_t node;

    /* Is this eventer already used */
    BOOL used;
    
    /* Capability/Feature/Limitation flags */
    int flags;

    /* Current working mode */
    int mode;

    /* The higher value means its quality is better */
    int precedence;

    /* Base frequency in HZ */
    uint64_t base_frequency;

    /* The minimum possible period (ns) */
    abstime_t min_period_ns;

    /* The maximum possible period (ns) */
    abstime_t max_period_ns;

    /* The specified expiration time */
    abstime_t expire;

    /* The actual resolution in ns (interval between interrupts) */
    abstime_t resolution;
    
    /* The delta period (multiple of min_period_ns) */
    int delta_multi_num;

    /* 
     * Start to trigger at @expire@ nanosecond away! The implementation 
     * may round the trigger and period with most nearst values!
     */
    int (*start) (struct clockeventer * eventer, 
                  int mode, abstime_t expire);
    
    /* Stop the clock eventer to trigger */
    int (*stop) (struct clockeventer * eventer);

    /* Event handler */
    clockeventer_handler_t handler;

    /* Event handler argment */
    void * arg;
    }clockeventer_t;

status_t clockeventer_add(struct clockeventer *eventer);

status_t clockeventer_remove(struct clockeventer *eventer);

struct clockeventer * clockeventer_select (int check_mask, 
        int expect_mask, abstime_t res);

void clockeventer_tick_handler(struct clockeventer * eventer, 
        void * arg);

status_t clockeventer_setup(struct clockeventer * eventer,
        clockeventer_handler_t handler, void * arg);

status_t clockeventer_start(struct clockeventer * eventer,
        int mode, abstime_t expire);

status_t clockeventer_stop(struct clockeventer * eventer);

status_t clockeventer_subsystem_init(void);

extern struct clockeventer * global_tick_eventer;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _CLOCK_EVENTER_EVENTER_H */


