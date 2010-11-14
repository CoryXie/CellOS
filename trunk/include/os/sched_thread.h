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
    STATE_READY = 1,        /* Ready - on a runq */
    STATE_RUNNING = 2,      /* Running - standalone */
    STATE_SUSPENDED = 3,    /* Controled not running - on a suspend waitq */
    STATE_DELAY = 4,        /* Give up CPU time - on a delayed waitq */
    STATE_PENDING = 5,      /* Wait on event - on a pending waitq */
    STATE_CANCEL_ARMED = 6, /* Armed with cancel requests */
    STATE_CANCELING = 7,    /* Canceling */
    STATE_COMPLETED = 8,    /* Run to finish normally */
    STATE_TERMINATED = 9    /* Forced to die reluctantly */
    }SCHED_THREAD_STATE;

typedef enum sched_thread_flag
    {
    THREAD_CANCEL_PENDING = 0, 
    THREAD_CANCEL_TYPE = 1, 
    THREAD_JOINABLE = 2, 
    THREAD_FINISHED = 3, 
    THREAD_TIMER_ARMED = 4, 
    THREAD_WAIT_FOR_JOIN = 5, 
    THREAD_OK_TO_FINISH_JOIN = 6,
    THREAD_SIGNAL_INTERRUPTIBLE = 7
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

    /* Pending resume it */
    unsigned int    resume_pending; 

    /* Thread flags */
    unsigned long   flags;    

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
    pthread_mutex_t thread_lock;
    
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

    /* Is this thread using FPU */
	BOOL        usefpu;

    /* Thread detach state */
	int32_t		detachstate;

    /* Initial singal state */
    sigset_t    initial_signal;

    /* Initial singal state */
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

