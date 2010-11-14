/* clock.c - clock managment */

static list_t os_clock_list;
static spinlock_t os_clock_list_lock;

#define OS_CLOCK_LIST_LOCK()    \
    spinlock_lock(&os_clock_list_lock)
#define OS_CLOCK_LIST_UNLOCK()  \
    spinlock_unlock(&os_clock_list_lock)

/* Add a clock to the global clock list */
status_t clock_add(struct os_clock *clk)
    {
    OS_CLOCK_LIST_LOCK();
    
    OS_CLOCK_LIST_UNLOCK();
    }

/* Remove a clock from the global clock list */
status_t clock_remove(struct os_clock *clk)
    {

    }

status_t clock_api_init(void)
    {
    list_init(&os_clock_list);
    spinlock_init(&os_clock_list_lock);
    }
