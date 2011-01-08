/* sched_cpu.h - scheduler CPU management */

#ifndef _OS_SCHED_CPU_H
#define _OS_SCHED_CPU_H

#include <sys.h>
#include <arch.h>
#include <os/sched_core.h>

#define __CPU_SETSIZE 1024
#define __NCPUBITS     (8 * sizeof (__cpu_mask))

typedef unsigned long int __cpu_mask;

#define __CPUELT(cpu) ((cpu) / __NCPUBITS)
#define __CPUMASK(cpu) ((__cpu_mask) 1 << ((cpu) % __NCPUBITS))

/* 
 * Clearing the cpu set by CPU_ZERO() means the thread can only run 
 * on local CPU of the system.
 *
 * Setting the cpu set by CPU_ALL() means the thread can run on all  
 * CPUs of the system.
 *
 * Setting specific bits means the thread can run on these CPUs
 * as specified by these bits. 
 */

typedef struct cpu_set
    {
    __cpu_mask __bits[__CPU_SETSIZE / __NCPUBITS];
    } cpu_set_t;

#define __CPU_ZERO(cpusetp) \
    memset(cpusetp, 0, sizeof (cpu_set_t))
    
#define __CPU_ALL(cpusetp) \
    memset(cpusetp, 0xFF, sizeof (cpu_set_t))

#define __CPU_SET(cpu, cpusetp) \
    ((cpusetp)->__bits[__CPUELT (cpu)] |= __CPUMASK (cpu))

#define __CPU_CLR(cpu, cpusetp) \
    ((cpusetp)->__bits[__CPUELT (cpu)] &= ~(__CPUMASK (cpu)))

#define __CPU_ISSET(cpu, cpusetp) \
    (((cpusetp)->__bits[__CPUELT (cpu)] & __CPUMASK (cpu)) != 0)

/*
 * int CPU_SETSIZE
 *
 * The value of this macro is the maximum number of CPUs 
 * which can be handled with a cpu_set_t object.
 */
#define CPU_SETSIZE __CPU_SETSIZE 

/*
 * void CPU_SET (int cpu, cpu_set_t *set)
 *
 * This macro adds cpu to the CPU set set.
 *
 * The cpu parameter must not have side effects since 
 * it is evaluated more than once.
 */
#define CPU_SET(cpu, cpusetp)   __CPU_SET (cpu, cpusetp) 

/*
 * void CPU_CLR (int cpu, cpu_set_t *set)
 *
 * This macro removes cpu from the CPU set set.
 *
 * The cpu parameter must not have side effects since 
 * it is evaluated more than once.
 */
#define CPU_CLR(cpu, cpusetp)   __CPU_CLR (cpu, cpusetp) 

/*
 * int CPU_ISSET (int cpu, const cpu_set_t *set)
 *
 * This macro returns a nonzero value (true) if cpu is a 
 * member of the CPU set set, and zero (false) otherwise.
 *
 * The cpu parameter must not have side effects since 
 * it is evaluated more than once.
 */
#define CPU_ISSET(cpu, cpusetp) __CPU_ISSET (cpu, cpusetp)

/*
 * void CPU_ZERO (cpu_set_t *set)
 *
 * This macro initializes the CPU set to be the empty set.
 */
#define CPU_ZERO(cpusetp)       __CPU_ZERO (cpusetp) 

/*
 * void CPU_ALL (cpu_set_t *set)
 *
 * This macro initializes the CPU set to be the full set.
 */
#define CPU_ALL(cpusetp)       __CPU_ALL (cpusetp) 

/* 
 * void CPU_COPY (cpu_set_t *src, cpu_set_t *dst)
 *
 * Copy the source cpu_set_t content to destination cpu_set_t.
 */
#define CPU_COPY(src, dst)  (void)(*(dst) = *(src))

/* 
 * void CPU_CMP (int low, int high, cpu_set_t *cpusetp1, cpu_set_t *cpusetp2)
 *
 * Compare to see if the two set are the same.
 */
#define CPU_CMP(low, high, cpusetp1, cpusetp2)  \
    atomic_cmp_bit_range(low, high, (cpusetp1)->__bits, (cpusetp2)->__bits)

struct sched_thread;
struct sched_cpu_group;

typedef struct sched_cpu 
    {
    /* CPU index */
    id_t                    cpu_idx; 
    
    /* Node of the cpu_group list */
    list_t                  cpu_group_list;  

    /* Lock for this CPU */
	spinlock_t              lock;  

    /* Thread which owns the CPU */
	struct sched_thread *   prev_thread;   

    /* Thread which owns the CPU */
	struct sched_thread *   current;   

    /* Thread which owns the FPU */
	struct sched_thread *   fpuowner;

    /* Thread which runs when no other threads runs */
	struct sched_thread *   idle_thread;   

    /* Is the CPU idle */
    BOOL                    idle;
        
    /* Schedule clock ID */
    clockid_t               clock;  

    /* CPU saved context */
	sched_context_t saved_context;  

    /* CPU ARCH */
    sched_cpu_arch_t cpu_arch;
    }sched_cpu_t;

typedef struct sched_cpu_group
    {
    /* CPU SET */
    cpu_set_t   cpu_set;  

    /* Node of sched_cpu_group_list */
    list_t      cpu_group_node;
    
    /* List of sched_cpu in this group */
    list_t      cpu_list; 

    /* ID of this cpu group */
    id_t        cpu_group_id;
    }sched_cpu_group_t;

extern sched_cpu_t* current_cpus[];

void cpu_early_init(void); 

sched_cpu_group_t * sched_cpu_group_add 
    (
    cpu_set_t * cpu_set
    );

void sched_cpu_group_remove
    (
    cpu_set_t * cpu_set
    );

sched_cpu_group_t * sched_cpu_group_find 
    (
    cpu_set_t * cpu_set
    );

struct sched_thread * sched_cpu_group_find_best_thread 
    (
    sched_policy_t * policy,
    sched_cpu_t *    cpu,
    struct sched_thread * check
    );

#endif /* _OS_SCHED_CPU_H */
