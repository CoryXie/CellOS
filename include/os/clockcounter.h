/* timecounter.h - time counter management */

#include <sys.h>
#include <arch.h>

typedef uint64_t cycle_t;

typedef struct clockcounter
    {
    /* Time Counter node in the list of counters */
    list_t      node;
    
    /* The name of the time counter */
    
    const char * counter_name;

    /* Locate the time counter and activate it */
    
    status_t (* counter_enable)(void);

    /* Deactivate the time counter */
    
    status_t (* counter_disable)(void);

    /* Read the current time counter value (masked by the counter bits ) */
    
    cycle_t (* counter_read)(void);

    /* Caculate the time elapsed in nanoseconds */
    
    abstime_t (* counter_time_elapsed)(cycle_t t1, cycle_t t2);

    /* Valid bits for the time counter */
    
    cycle_t counter_bits;

    /* Last read value for the time counter */
    
    cycle_t counter_latest_read;

    /* Time counter frequency (changes this number of times per second) */
    
    uint64_t counter_frequency_hz;

    /* Resolution in nanosecond (uses this number of ns to change a value) */

    abstime_t counter_resolution_ns;

    /* Minium overflow fixup period in nanoseconds */

    abstime_t counter_fixup_period;
    }clockcounter_t;

void real_wall_time_init(void);

void real_wall_time_regular_update(void);

abstime_t get_now_nanosecond(void);

extern struct clockcounter clockcounter_pm_timer;
extern struct clockcounter * global_clockcounter;

