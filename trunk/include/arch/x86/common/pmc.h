/* pmc.h - Performance Counter Managment */

#ifndef _ARCH_X86_COMMON_PMC_H
#define _ARCH_X86_COMMON_PMC_H

#include <config.h>
#include <sys.h>

#define PMC_PerfCtr0    0
#define PMC_PerfCtr1    1
#define PMC_PerfCtr2    2
#define PMC_PerfCtr3    3

/*
  VMWARE Pseudoperformance Counters
  
  For certain applications it can be useful to have direct access to real 
  time (as opposed to apparent time) within a virtual machine. For
  example, you might be writing performance-measuring software that is 
  aware it is running in a virtual machine and does not require its 
  fine-grained timer to stay in step with the number of interrupts delivered 
  on other timer devices.
  
  VMware virtual machines provide a set of pseudoperformance counters that 
  software running in the virtual machine can read with the rdpmc instruction
  to obtain fine-grained time. To enable this feature, use the following 
  configuration file setting:
  
  monitor_control.pseudo_perfctr = TRUE
  
  The following machine instructions then become available:
  
  Inst ruction          Time Value Retu rned
  
  rdpmc 0x10000         Physical host TSC
  rdpmc 0x10001         Elapsed real time in ns
  rdpmc 0x10002         Elapsed apparent time in ns
  
  Table 2. Instructions Available When Pseudoperformance Counters Are Enabled
  
  Although the rdpmc instruction normally is privileged unless the PCE flag 
  is set in the CR4 control register, a VMware virtual machine permits the 
  above pseudoperformance counters to be read from user space regardless of 
  the setting of the PCE flag.
  
  NOTE: The pseudoperformance counter feature uses a trap to catch a 
  privileged machine instruction issued by software running in the
  virtual machine and therefore has more overhead than reading a 
  performance counter or the TSC on physical hardware.
*/
#ifdef CONFIG_VMWARE_CLIENT
#define VMWARE_PMC_HOST_TSC                 0x10000 /* Physical host TSC */
#define VMWARE_PMC_ELAPSED_REAL_TIME        0x10001 /* Elapsed real time in ns */
#define VMWARE_PMC_ELAPSED_APPARENT_TIME    0x10001 /* Elapsed apparent time in ns */
#endif

extern struct clockcounter clockcounter_pm_counter;

static inline uint64_t read_pmc(uint32_t pmc)
    {
    uint32_t ax, dx;

    asm volatile (
        "rdpmc\n"
        : "=a" (ax), "=d" (dx)
        : "c" (pmc)
        );

    return ((uint64_t) dx << 32) | ax;
    }

#endif /* _ARCH_X86_COMMON_PMC_H */
