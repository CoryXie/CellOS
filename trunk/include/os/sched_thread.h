/* sched_thread.h - scheduler thread management */

#ifndef _OS_SCHED_THREAD_H
#define _OS_SCHED_THREAD_H

#include <sys.h>
#include <arch.h>
#include <sched.h>
#include <signal.h>
#include <pthread.h>
#include <os/queue.h>
#include <os/sched_core.h>

/* Thread States */
typedef enum sched_thread_state
    {
    STATE_READY =       (1 << 0),  /* Ready - on a runq */
    STATE_RUNNING =     (1 << 1),  /* Running - standalone */
    STATE_SUSPENDED =   (1 << 2),  /* Controled not running - on a suspend waitq */
    STATE_DELAY =       (1 << 3),  /* Give up CPU time - on a delayed waitq */
    STATE_PENDING =     (1 << 4),  /* Wait on event - on a pending waitq */
    STATE_COMPLETED =   (1 << 5),  /* Run to finish normally */
    STATE_TERMINATED =  (1 << 6)   /* Forced to die reluctantly */
    }SCHED_THREAD_STATE;

typedef enum sched_thread_flag
    {
    THREAD_AUTO_RUN =           (1 << 0), 
    THREAD_STANDALONE =         (1 << 1), 
    THREAD_JOINABLE =           (1 << 2), 
    THREAD_FINISHED =           (1 << 3), 
    THREAD_TIMER_ARMED =        (1 << 4), 
    THREAD_USE_FPU =            (1 << 5), 
    THREAD_FPU_READY =          (1 << 6), 
    THREAD_UNPREEMPTABL =       (1 << 7),
    THREAD_INTERRUPTIBLE =      (1 << 8),
    THREAD_CANCEL_PENDING =     (1 << 9),
    THREAD_WAIT_FOR_JOIN =      (1 << 10)
    }SCHED_THREAD_FLAG;

struct sched_thread;

typedef struct sched_thread_posix_extention
    {
	struct sched_thread *   joining_thread;
	void *                  retval;
	void *                  joined_thread_retval;
	pthread_spinlock_t      exit_lock;
    }sched_thread_posix_extention_t;

/* Default prameter area size */
#define SCHED_PARAM_AREA_SIZE   (256)

typedef struct sched_thread
    {
    /* Thread name */
    char            name[NAME_MAX]; 

    /* Magic for validity */
    int             magic; 
    
    /* Thread id which is an interger */
    id_t            id;  

    /* The CPU the thread is running on */
    id_t            cpu_idx;            

    /* The CPU SET that this thread can run on */
    cpu_set_t       cpu_set;
    
    /* The thread state */
    SCHED_THREAD_STATE   state;          

    /* Node to link to the global thread list */
    list_t          global_list_node;           

    /* The entry code when thread starts to run */
    void *         (*entry)(void *); 
    
    /* The entry parameter */
    void *          param;          

    /* The thread stack space base address */
    unsigned char * stack_base;   

    /* 
     * The thread stack space base address which  
     * needs to be free on thread exiting if non-NULL
     */
    unsigned char * stack_base_free;   

    /* The thread stack space size */
    size_t          stack_size;     

    /* The thread stack space top address */
    unsigned char * stack_top;     

    /* Pending resume it */
    unsigned int    resume_pending; 

    /* Thread flags */
    unsigned long   flags;    

    unsigned long   runcount;  
    
    /* Pending signals */
    sigset_t        sig_pending;

    /* Blocked signals */
    sigset_t        sig_blocked;

    /* Abort handler */
	void (*abort)(void *);

    /* Thread cleanup handler (user defind) */
    struct sched_thread_cleanup *cleanup;
    
    /* Thread cleanup handler (scheduler defind to free this thread) */
    struct sched_thread_cleanup free_me;

    /* Used for serialised access to public thread state */
    spinlock_t thread_lock;
    
    /* Used for async-cancel safety */
    pthread_mutex_t cancel_lock; 

    /* Thread cancel state */
    int             cancel_state;

    /* Thread cancel type */
    int             cancel_type;
    
    /* The thread running cycles accounting (in CPU HZ) */
    abstime_t        cycles;

    /* The thread resume cycle recorded at reschedule (in CPU HZ) */
    abstime_t        resume_cycle;

    /* The expected thread resume time (in scheduler HZ) */
    abstime_t        resume_time;

    /* Wait timeout (mutex, semaphore, msgQ, ..., one at a time) */
    int             timeout;

    /* List of all mutexes owned by thread */
    list_t          mutex_list; 

    /* Wait queue node */

    qelement_t      waitq_node;
    
    /* Run queue node */
    qelement_t      runq_node;  

    /* Inactive thread list node */
    list_t      inactive_node;  

    /* The CPU info */
    struct sched_cpu *  sched_cpu;  

    /* The CPU group info */
    struct sched_cpu_group *  sched_cpu_group;  

    /* The schduling policy info */
    struct sched_policy *  sched_policy;  

    /* Scheduling policy ID for this thread */
    int sched_policy_id;

    /* Scheduling runq the thread is on */
    sched_runq_t *          sched_runq;

    /* The address space */
    struct pmap *       asp;  
    
    /* The tsk we belong to */
    struct sched_task * tsk;  

    /* Preemption disable count */
    size_t  preemption_disabled;      

    /* Does this thread use FPU? */
    BOOL  use_fpu;

    /* Is the FPU ready for use for this thread? */
    BOOL  fpu_ready;
    
    /* Context save area when the thread is scheduled */
	sched_context_t saved_context;
    
	/* Timeout context save area when the thread sleeps */
	sched_context_t sleep_timeout_context;
    
	/* Interruption context save area when sleep is interrupted */
	sched_context_t sleep_interruption_context;
    
    /* Thread ARCH information */
    thread_arch_t   arch; 

    /* POSIX thead specific extenstion */
    sched_thread_posix_extention_t posix_extention;
    
    /* Scheduling parameter area */
    uint8_t   sched_param_area[SCHED_PARAM_AREA_SIZE]; 
    }sched_thread_t;

/*
 * pthread_attr_t
 *
 * Used to identify a thread attribute object.
 */
typedef struct sched_thread_attr 
    {
    /* Name */
    char        name[NAME_MAX];
    
    /* Magic number */
    int         magic;
    
    /* The CPU or CPU group for this thread */
	cpu_set_t   cpu_set;   

    /* Thread stack base address, if allocated by user */
	void *      stackaddr;

    /* Thread stack size */
	size_t		stacksize;

    /* Thread detach state */
	int32_t		detachstate;

    /* Initial singal state */
    sigset_t    initial_signal;

    /* Autorun state */
    BOOL        autorun;

    /* Scheduling policy */
    int         policy;

    /* Stack guard size */
    size_t      guardsize;
    size_t      guardsize_raw;

    /* 
     * Are scheduling attributes inherited from creating 
     * thread or use this attr specified? 
     */
    int         inheritsched;

    /* Scheduling contention scope */
    int         contentionscope;

    /* Bitmask flags */
    unsigned long   intial_flags;
    
    /* Scheduling parameter structure size */
    uint32_t    sched_param_size;
    
    /* Scheduling parameter structure area (need casting) */
    uint8_t     sched_param_area[SCHED_PARAM_AREA_SIZE];
    } sched_thread_attr_t;

void sched_thread_arch_init
    (
    sched_thread_t *t
    );

void sched_thread_do_cleanup
    (
    pthread_t thread, 
    void *retval
    );

int sched_thread_delete
    (
    pthread_t thread
    );

int pthread_attr_setname_np
    (
    pthread_attr_t *attr, 
    char *name
    );

int pthread_attr_setautorun_np
    (
    pthread_attr_t *attr, 
    int initial_state
    );

int pthread_attr_setflags_np
    (
    pthread_attr_t *attr, 
    unsigned long flags
    );

int pthread_attr_getflags_np
    (
    pthread_attr_t *attr, 
    unsigned long * flags
    );

status_t sched_thread_init(void);

status_t sched_thread_add_zombie
    (
    pthread_t thread
    );

status_t sched_thread_remove_zombie
    (
    pthread_t thread
    );

status_t sched_thread_add_suspended
    (
    pthread_t thread
    );

status_t sched_thread_remove_suspended
    (
    pthread_t thread
    );

char * sched_thread_state_name(int state);

/* Non-portable version interfaces are defined in private files */

/* Flags for pthread_resume_np() */
#define PTHREAD_COUNT_RESUME_NP 0x00
#define PTHREAD_FORCE_RESUME_NP 0x01

#endif /* _OS_SCHED_THREAD_H */

