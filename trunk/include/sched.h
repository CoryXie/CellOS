/* sched.h - execution scheduling */

#ifndef _POSIX_SCHED_H
#define _POSIX_SCHED_H

/* 
 * The <sched.h> header shall define the pid_t and time_t 
 * type as described in <sys.h>
 */
 
#include <sys.h>

/* 
 * The <sched.h> header shall define the timespec structure 
 * as described in <time.h> 
 */
 
#include <time.h>
 
#ifdef __cplusplus
extern "C" {
#endif

/*
 * Each process or thread is controlled by an associated scheduling
 * policy and priority. Associated with each policy is a priority 
 * range. Each policy definition specifies the minimum priority 
 * range for that policy. The priority ranges for each policy may 
 * overlap the priority ranges of other policies.
 *
 * Four scheduling policies are defined; others may be defined by
 * the implementation. The four standard policies are indicated by
 * the values of the following symbolic constants.
 */
 
#define SCHED_FIFO		1 /* First in-first out (FIFO) scheduling policy. */
#define SCHED_RR		2 /* Round robin scheduling policy. */
#define SCHED_SPORADIC	3 /* Sporadic server scheduling policy. */
#define SCHED_OTHER		4 /* Another scheduling policy. */

/*
 * The <sched.h> header shall define the sched_param structure,
 * which shall include the scheduling parameters required for 
 * implementation of each supported scheduling policy. 
 */
 
typedef struct sched_param 
    {
    /* Process or thread execution scheduling priority.*/
	int             sched_priority; 
    
    /* Low scheduling priority for sporadic server. */
    int             sched_ss_low_priority; 
    
    /* Replenishment period for sporadic server. */
    struct timespec sched_ss_repl_period;
    
    /* Initial budget for sporadic server. */
    struct timespec sched_ss_init_budget; 
    
    /*  Maximum pending replenishments for sporadic server.*/
    int             sched_ss_max_repl;     
    }sched_param_t;

int sched_get_priority_max(int policy);
int sched_get_priority_min(int policy);

int sched_getparam(pid_t pid, struct sched_param *param);
int sched_setparam(pid_t pid, const struct sched_param *param);

int sched_setscheduler(pid_t pid, int policy,
       const struct sched_param *param);
int sched_getscheduler(pid_t pid); 

int sched_rr_get_interval(pid_t pid, struct timespec *interval);

int    sched_yield(void);

#ifdef __cplusplus
}
#endif

#endif  /* _POSIX_SCHED_H */
