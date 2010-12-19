/* clockeventer.c - clock eventer managment */

#include <sys.h>
#include <arch.h>
#include <os/list.h>

struct os_clock_eventer * global_tick_eventer = NULL;

static list_t os_clock_eventer_list;
static spinlock_t os_clock_eventer_list_lock;

#define OS_CLOCK_LIST_LOCK()    \
    spinlock_lock(&os_clock_eventer_list_lock)
#define OS_CLOCK_LIST_UNLOCK()  \
    spinlock_unlock(&os_clock_eventer_list_lock)

void clock_eventer_default_handler(struct os_clock_eventer * eventer, void * arg)
    {
    printk("clock_eventer_default_handler - name %s\n", 
        eventer->name);
    }

void clock_eventer_tick_handler(struct os_clock_eventer * eventer, void * arg)
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
status_t clock_eventer_add(struct os_clock_eventer *eventer)
    {
    OS_CLOCK_LIST_LOCK();

    list_append(&os_clock_eventer_list, &eventer->node);
    
    eventer->handler = clock_eventer_default_handler;
    
    OS_CLOCK_LIST_UNLOCK();

    return OK;
    }

/* Remove a clock eventer from the global clock list */
status_t clock_eventer_remove(struct os_clock_eventer *eventer)
    {
    OS_CLOCK_LIST_LOCK();

    list_remove(&eventer->node);
    
    OS_CLOCK_LIST_UNLOCK();

    return OK;
    }

/* 
 * clock_eventer_select - select a clock eventer from the global clock eventer 
 * list that meet the requirement.
 */
struct os_clock_eventer * clock_eventer_select (int check_mask, 
        int expect_mask, abstime_t res)
    {
    struct os_clock_eventer * best;
    
    OS_CLOCK_LIST_LOCK();

    LIST_FOREACH(&os_clock_eventer_list, iter)
        {
        best = LIST_ENTRY(iter, struct os_clock_eventer, node);

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
    
    OS_CLOCK_LIST_UNLOCK();

    return best;
    }

status_t clock_eventer_setup(struct os_clock_eventer * eventer,
        clock_eventer_handler_t handler, void * arg)
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
 * clock_eventer_start - start the eventer to run with the expected behaviour 
 *
 * @param eventer - the clock eventer selected
 * @param mode - one-shot or peridic
 * @param expire - the time to trigger event in nanosecond (relative to now)
 */
status_t clock_eventer_start(struct os_clock_eventer * eventer,
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

status_t clock_eventer_stop(struct os_clock_eventer * eventer)
    {
    /* The clock eventer should have been selected to be used */
    if (eventer->used != TRUE)
        return ENXIO;
    
    if (eventer->stop == NULL)
        return ENOSYS;
    
    return eventer->stop(eventer);
    }

status_t clock_eventer_subsystem_init(void)
    {
    list_init(&os_clock_eventer_list);
    
    spinlock_init(&os_clock_eventer_list_lock);

    return OK;
    }

void tick_eventer_init(void)
    {
    global_tick_eventer = clock_eventer_select(CLOCK_EVENTER_FLAGS_PERIODIC, 
                                   CLOCK_EVENTER_FLAGS_PERIODIC, MSECS2NSECS(1));

    if (global_tick_eventer)
        {
        printk("global_tick_eventer name %s\n", global_tick_eventer->name);
        
        clock_eventer_setup(global_tick_eventer, 
            clock_eventer_tick_handler, NULL);
        
        clock_eventer_start(global_tick_eventer, 
            CLOCK_EVENTER_MODE_PERIODIC, HZ2NSECS(CONFIG_HZ));
        }
    }
