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

typedef struct os_clock 
    { 
    list_t node;
    
	int (*open) (struct os_clock *);
	int (*close) (struct os_clock *);
    
	int (*set_abstime)(struct os_clock *, abstime_t);
	int (*get_abstime)(struct os_clock *, abstime_t *);

	int (*set_timer_res)(struct os_clock *, abstime_t);
	int (*get_timer_res)(struct os_clock *, abstime_t *);

	int (*set_timer_mode)(struct os_clock *, int);
	int (*get_timer_mode)(struct os_clock *, int *);

    /* The clock device should maintain a list of handlers */
	int (*add_irq_handler)(clock_irq_handler_t);
    }os_clock_t;

status_t clock_add(struct os_clock *clk);
status_t clock_remove(struct os_clock *clk);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _OS_CLOCK_H */


