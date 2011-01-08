/* sched_arch.h - X86-64 ARCH specific sched management */

#ifndef _ARCH_X86_X64_SCHED_ARCH_H
#define _ARCH_X86_X64_SCHED_ARCH_H

#include <sys.h>
#include <arch.h>

typedef struct sched_cpu_arch
    {
    uint64_t apic_period_ns;
    uint64_t apic_scale_factor;
    x64_seg_descriptor_t gdt[GDT_SEL_ENTRIES];
    x64_tss_t            tss;
    }sched_cpu_arch_t;

#endif /* _ARCH_X86_X64_SCHED_ARCH_H */
