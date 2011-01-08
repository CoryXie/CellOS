/* clockeventer.c - clock eventer managment */

#include <sys.h>
#include <arch.h>
#include <os/list.h>

struct clockeventer * global_tick_eventer = NULL;

static list_t clockeventer_list;
static spinlock_t clockeventer_list_lock;

#define CLOCK_EVENTER_LIST_LOCK()    \
    spinlock_lock(&clockeventer_list_lock)
#define CLOCK_EVENTER_LIST_UNLOCK()  \
    spinlock_unlock(&clockeventer_list_lock)

void clockeventer_default_handler(struct clockeventer * eventer, void * arg)
    {
    printk("clockeventer_default_handler - name %s\n", 
        eventer->name);
    }

void clockeventer_tick_handler(struct clockeventer * eventer, void * arg)
    {
    /* 
     * Using PIT. At most 55ms interrupt interval makes it perfect for keeping
     * PM Timer timecounter "latest read" up to date and correct.
     */
    real_wall_time_regular_update();
    
    itimer_callback_handler();
    
    if (eventer->mode == CLOCK_EVENTER_MODE_ONESHOT)
        eventer->start(eventer, eventer->mode, eventer->expire);
    }

/* Add a clock eventer to the global clock list */
status_t clockeventer_add(struct clockeventer *eventer)
    {
    CLOCK_EVENTER_LIST_LOCK();

    list_append(&clockeventer_list, &eventer->node);
    
    eventer->handler = clockeventer_default_handler;
    
    CLOCK_EVENTER_LIST_UNLOCK();

    return OK;
    }

/* Remove a clock eventer from the global clock list */
status_t clockeventer_remove(struct clockeventer *eventer)
    {
    CLOCK_EVENTER_LIST_LOCK();

    list_remove(&eventer->node);
    
    CLOCK_EVENTER_LIST_UNLOCK();

    return OK;
    }

/* 
 * clockeventer_select - select a clock eventer from the global clock eventer 
 * list that meet the requirement.
 */
struct clockeventer * clockeventer_select (int check_mask, 
        int expect_mask, abstime_t res)
    {
    struct clockeventer * best;
    
    CLOCK_EVENTER_LIST_LOCK();

    LIST_FOREACH(&clockeventer_list, iter)
        {
        best = LIST_ENTRY(iter, struct clockeventer, node);

        if (best && 
            (best->used != TRUE) &&
            (best->precedence >= 0) &&
            ((best->flags & check_mask) == expect_mask) &&
            (best->min_period_ns <= res))
            {
            break;
            }
        
        best = NULL;
        }
    
    CLOCK_EVENTER_LIST_UNLOCK();

    return best;
    }

status_t clockeventer_setup(struct clockeventer * eventer,
        clockeventer_handler_t handler, void * arg)
    {
    if (handler == NULL)
        return EINVAL;

    if (eventer->used)
        return EBUSY;

    eventer->used = TRUE;

    eventer->handler = handler;

    eventer->arg = arg;

    return OK;
    }


/*
 * clockeventer_start - start the eventer to run with the expected behaviour 
 *
 * @param eventer - the clock eventer selected
 * @param mode - one-shot or peridic
 * @param expire - the time to trigger event in nanosecond (relative to now)
 */
status_t clockeventer_start(struct clockeventer * eventer,
        int mode, abstime_t expire)
    { 
    /* The clock eventer should have been selected to be used */
    if (eventer->used != TRUE)
        return ENXIO;
    
    /* Give it some time to expire */
    if (expire == 0)
        return EINVAL;
    
    /* If the eventer can not support periodic but user wants it, reject! */
    if (!(eventer->flags & CLOCK_EVENTER_FLAGS_PERIODIC) && 
        (mode == CLOCK_EVENTER_MODE_PERIODIC))
        return ENODEV;
    
    /* If the eventer can not support one-shot but user wants it, reject! */
    if (!(eventer->flags & CLOCK_EVENTER_FLAGS_ONESHOT) && 
        (mode == CLOCK_EVENTER_MODE_ONESHOT))
        return ENODEV;
    
    if (expire < eventer->min_period_ns)
        expire = eventer->min_period_ns;
    if (expire > eventer->max_period_ns)
        expire = eventer->max_period_ns;
        
    eventer->mode = mode;
    eventer->expire = expire;
    
    return eventer->start(eventer, mode, expire);
    }

status_t clockeventer_stop(struct clockeventer * eventer)
    {
    /* The clock eventer should have been selected to be used */
    if (eventer->used != TRUE)
        return ENXIO;
    
    if (eventer->stop == NULL)
        return ENOSYS;
    
    return eventer->stop(eventer);
    }

status_t clockeventer_subsystem_init(void)
    {
    list_init(&clockeventer_list);
    
    spinlock_init(&clockeventer_list_lock);

    return OK;
    }

void tick_eventer_init(void)
    {
    global_tick_eventer = clockeventer_select(CLOCK_EVENTER_FLAGS_PERIODIC, 
                                   CLOCK_EVENTER_FLAGS_PERIODIC, MSECS2NSECS(1));

    if (global_tick_eventer)
        {
        printk("global_tick_eventer name %s\n", global_tick_eventer->name);
        
        clockeventer_setup(global_tick_eventer, 
            clockeventer_tick_handler, NULL);
        
        clockeventer_start(global_tick_eventer, 
            CLOCK_EVENTER_MODE_PERIODIC, HZ2NSECS(CONFIG_HZ));
        }
    }
