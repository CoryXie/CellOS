/* pmc.c - Performance Counter Managment */

#include <sys.h>
#include <arch.h>

#ifdef CONFIG_VMWARE_CLIENT

struct clockcounter clockcounter_pm_counter;

status_t pm_counter_enable(void)
    {
    clockcounter_pm_counter.counter_bits = 64;
    
    clockcounter_pm_counter.counter_frequency_hz = 1000000000;

    clockcounter_pm_counter.counter_resolution_ns = 1;
     
    clockcounter_pm_counter.counter_fixup_period = 2 * NSECS_PER_SEC;
    
    return OK;
    }

status_t pm_counter_disable(void)
    {
    return OK;
    }

/* Read the current time counter value (masked by the counter bits ) */

cycle_t pm_counter_counter_read(void)
    {    
    return (cycle_t)read_pmc(VMWARE_PMC_ELAPSED_REAL_TIME);
    }

/* Caculate the time elapsed in nanoseconds */

abstime_t pm_counter_counter_time_elapsed(cycle_t t1, cycle_t t2)
    {
    return (abstime_t) (t2 - t1);
    }

struct clockcounter clockcounter_pm_counter = 
    {
    .counter_name = "PMC",
    .counter_enable = pm_counter_enable,
    .counter_disable = pm_counter_disable,
    .counter_read = pm_counter_counter_read,
    .counter_time_elapsed = pm_counter_counter_time_elapsed,
    };
#endif /* CONFIG_VMWARE_CLIENT */

int do_pmc (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
    {
    printk("PMC_PerfCtr0 - %lld\n", read_pmc(PMC_PerfCtr0));
    printk("PMC_PerfCtr1 - %lld\n", read_pmc(PMC_PerfCtr1));
    printk("TSC - %lld\n", rdtsc());

#ifdef CONFIG_VMWARE_CLIENT
    printk("VMWARE_PMC_HOST_TSC - %lld\n", 
        read_pmc(VMWARE_PMC_HOST_TSC));
    printk("VMWARE_PMC_ELAPSED_REAL_TIME - %lld\n", 
        read_pmc(VMWARE_PMC_ELAPSED_REAL_TIME));
    printk("VMWARE_PMC_ELAPSED_APPARENT_TIME - %lld\n", 
        read_pmc(VMWARE_PMC_ELAPSED_APPARENT_TIME));
#endif
    return 0;
    }

CELL_OS_CMD(
    pmc,   1,        1,    do_pmc,
    "show current cpu PMC values",
    "show current cpu Performance Counter values\n"
    );

