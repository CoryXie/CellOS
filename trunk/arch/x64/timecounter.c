/* timecounter.c - time counter management */

#include <sys.h>
#include <arch.h>
#include <os.h>
#include <os/acpi.h>
#include <sys/time.h>

static list_t os_time_counter_list;
static spinlock_t os_time_counter_list_lock;

#define OS_TIME_COUNTER_LIST_LOCK()    \
    spinlock_lock(&os_time_counter_list_lock)
#define OS_TIME_COUNTER_LIST_UNLOCK()  \
    spinlock_unlock(&os_time_counter_list_lock)

struct os_time_counter * global_os_time_counter = NULL;
timespec_t real_wall_time;

/* Add a clock eventer to the global clock list */
status_t time_counter_add(struct os_time_counter *counter)
    {
    OS_TIME_COUNTER_LIST_LOCK();

    list_append(&os_time_counter_list, &counter->node);
    
    if (counter->counter_enable)
        counter->counter_enable();
    
    OS_TIME_COUNTER_LIST_UNLOCK();

    return OK;
    }

/* Remove a clock eventer from the global clock list */
status_t time_counter_remove(struct os_time_counter *counter)
    {
    OS_TIME_COUNTER_LIST_LOCK();

    list_remove(&counter->node);
    
    OS_TIME_COUNTER_LIST_UNLOCK();

    return OK;
    }

struct os_time_counter time_counter_pm_timer;

status_t pm_timer_enable(void)
    {
    printk("PM timer uses %s space %p and its width is %d bits\n",
           AcpiGbl_FADT.XPmTimerBlock.SpaceId ? "IOPORT" : "MEMORY", 
           AcpiGbl_FADT.XPmTimerBlock.Address,
           AcpiGbl_FADT.Flags & ACPI_FADT_32BIT_TIMER ? 32 : 24);

    time_counter_pm_timer.counter_bits = 
        AcpiGbl_FADT.Flags & ACPI_FADT_32BIT_TIMER ? 32 : 24;
    
    time_counter_pm_timer.counter_frequency_hz = PM_TIMER_FREQUENCY;

    time_counter_pm_timer.counter_resolution_ns =
        (NSECS_PER_SEC/PM_TIMER_FREQUENCY);
    /* 
     * PM Timer will overflow depending its bits:
     *
     * 2**24 Ticks / 3,600,000 Ticks/Sec = 4.66 sec
     * 2**32 Ticks / 3,600,000 Ticks/Sec = 1193 sec or 19.88 minutes
     *
     * We set to at least read it every 2 seconds.
     */
     
    time_counter_pm_timer.counter_fixup_period = 2 * NSECS_PER_SEC;
    
    return OK;
    }

status_t pm_timer_disable(void)
    {
    return OK;
    }

/* Read the current time counter value (masked by the counter bits ) */

cycle_t pm_timer_counter_read(void)
    {
    uint32_t now = 0;
    
    if (AcpiGetTimer(&now) != AE_OK)
        {
        printk("pm_timer_counter_read - AcpiGetTimer fail\n");
        
        return 0;
        }
    
    return (cycle_t)now;
    }

/* Caculate the time elapsed in nanoseconds */

abstime_t pm_timer_counter_time_elapsed(cycle_t t1, cycle_t t2)
    {
    uint32_t time_us = 0;
    
    if (AcpiGetTimerDuration((uint32_t)t1, (uint32_t)t2, &time_us) != AE_OK)
        {
        printk("pm_timer_counter_time_elapsed - AcpiGetTimerDuration fail\n");
               
        return 0;
        }

    return (abstime_t) (time_us * 1000);
    }

struct os_time_counter time_counter_pm_timer = 
    {
    .counter_name = "PMT",
    .counter_enable = pm_timer_enable,
    .counter_disable = pm_timer_disable,
    .counter_read = pm_timer_counter_read,
    .counter_time_elapsed = pm_timer_counter_time_elapsed,
    };

struct os_time_counter * select_global_os_time_counter(void)
    {
    struct os_time_counter * best;
    if (global_os_time_counter && 
        global_os_time_counter->counter_disable)
        global_os_time_counter->counter_disable();

    OS_TIME_COUNTER_LIST_LOCK();

    global_os_time_counter = &time_counter_pm_timer;

    LIST_FOREACH(&os_time_counter_list, iter)
        {
        best = LIST_ENTRY(iter, struct os_time_counter, node);

        if (best && 
            (best->counter_resolution_ns < 
            global_os_time_counter->counter_resolution_ns))
            global_os_time_counter = best;
        }
    
    OS_TIME_COUNTER_LIST_UNLOCK();
    
    global_os_time_counter->counter_enable();
    
    return global_os_time_counter;
    }

void time_counter_subsystem_init(void)
    {
    list_init(&os_time_counter_list);
    spinlock_init(&os_time_counter_list_lock);
    
    time_counter_add(&time_counter_pm_timer);
    time_counter_add(&time_counter_pm_counter);
    select_global_os_time_counter();
    }

void real_wall_time_init(void)
    {
    real_wall_time.tv_sec = rtc_get_utc_time();
    real_wall_time.tv_nsec = 0;
    }

void real_wall_time_regular_update(void)
    {
    cycle_t last_read;
    abstime_t eplased;

    struct os_time_counter * timecounter = global_os_time_counter;

    if (timecounter == NULL)
        return;

    last_read = timecounter->counter_latest_read;

    timecounter->counter_latest_read = timecounter->counter_read();

    eplased = timecounter->counter_time_elapsed(last_read,
                                        timecounter->counter_latest_read);

    timespec_add_ns(&real_wall_time, eplased);

    }

/*
  NAME
  
  gettimeofday - get the date and time
  SYNOPSIS
  
  #include <sys/time.h>
  
  int gettimeofday(struct timeval *restrict tp, void *restrict tzp); 
  
  DESCRIPTION
  
  The gettimeofday() function shall obtain the current time, expressed as seconds and microseconds since the Epoch, and store it in the timeval structure pointed to by tp. The resolution of the system clock is unspecified.
  
  If tzp is not a null pointer, the behavior is unspecified.
  
  RETURN VALUE
  
  The gettimeofday() function shall return 0 and no value shall be reserved to indicate an error.
  
  ERRORS
  
  No errors are defined.

*/

int gettimeofday(struct timeval * tp, void * tzp)
    {
    cycle_t last;
    abstime_t eplased;
    
    struct os_time_counter * timecounter = global_os_time_counter;

    if (timecounter == NULL)
        return ENODEV;
    
    last = timecounter->counter_latest_read;

    timecounter->counter_latest_read = timecounter->counter_read();

    eplased = timecounter->counter_time_elapsed(last, 
                                timecounter->counter_latest_read);

    timespec_add_ns(&real_wall_time, eplased);

    tp->tv_sec = real_wall_time.tv_sec;
    tp->tv_usec = real_wall_time.tv_nsec / 1000;
    
    return OK;
    }

int getnstimeofday(struct timespec * tp, void * tzp)
    {
    cycle_t last;
    abstime_t eplased;
    
    struct os_time_counter * timecounter = global_os_time_counter;

    if (timecounter == NULL)
        return ENODEV;

    last = timecounter->counter_latest_read;

    timecounter->counter_latest_read = timecounter->counter_read();

    eplased = timecounter->counter_time_elapsed(last, 
                                timecounter->counter_latest_read);

    timespec_add_ns(&real_wall_time, eplased);

    tp->tv_sec = real_wall_time.tv_sec;
    tp->tv_nsec = real_wall_time.tv_nsec;
    
    return OK;
    }

abstime_t get_now_nanosecond(void)
    {
    struct timespec now;
    
    getnstimeofday(&now, NULL);

    return timespec_to_abstime(&now);
    }

int do_time (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
    {
    struct timeval timev;
    struct timespec times;
    
    ipl_t ipl = interrupts_disable();
    
    gettimeofday(&timev, NULL);
    getnstimeofday(&times, NULL);

    interrupts_restore(ipl);

    printk("Time in Micro Senconds (%lld sec: %ld usec)\n", 
        timev.tv_sec, timev.tv_usec);

    printk("Time in Nano Senconds  (%lld sec: %ld nsec)\n", 
        times.tv_sec, times.tv_nsec);
    
    return 0;
    }

CELL_OS_CMD(
    time,   1,        1,    do_time,
    "show current time",
    "show current time (in both nanosecond and microsecond resolution)\n"
    );

