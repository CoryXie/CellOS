/* clock.c - clock managment */

static list_t os_clock_list;
static spinlock_t os_clock_list_lock;

#define OS_CLOCK_LIST_LOCK()    \
    spinlock_lock(&os_clock_list_lock)
#define OS_CLOCK_LIST_UNLOCK()  \
    spinlock_unlock(&os_clock_list_lock)

/* Add a clock to the global clock list */
status_t clock_add(struct os_event_clock *clk)
    {
    OS_CLOCK_LIST_LOCK();

    list_append(&os_clock_list, &clk->node);
    
    OS_CLOCK_LIST_UNLOCK();
    }

/* Remove a clock from the global clock list */
status_t clock_remove(struct os_event_clock *clk)
    {
    OS_CLOCK_LIST_LOCK();

    list_remove(&clk->node);
    
    OS_CLOCK_LIST_UNLOCK();
    }

/* Select a clock from the global clock list that meet the requirement */
struct os_event_clock * clock_select (abstime_t res)
    {
    struct os_event_clock * best;
    
    OS_CLOCK_LIST_LOCK();

    LIST_FOREACH(list, iter)
        {
        best = LIST_ENTRY(iter, struct os_event_clock, node);

        if (best && best->min_period_ns <= res)
            {
            break;
            }
        
        best = NULL;
        }
    
    OS_CLOCK_LIST_UNLOCK();

    return best;
    }

status_t clock_subsystem_init(void)
    {
    list_init(&os_clock_list);
    spinlock_init(&os_clock_list_lock);
    }
