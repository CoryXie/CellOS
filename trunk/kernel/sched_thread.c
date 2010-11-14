/* sched_thread.c - scheduler thread management */

#include <sys.h>
#include <arch.h>
#include <sched.h>
#include <pthread.h>
#include <os/sched_core.h>

static list_t sched_thread_zombie_list;
static spinlock_t sched_thread_zombie_list_lock;
static list_t sched_thread_suspended_list;
static spinlock_t sched_thread_suspended_list_lock;
static list_t sched_thread_global_list;
static spinlock_t sched_thread_global_list_lock;
static int sched_thread_concurrency = 0;

#define SCHED_THREAD_ZOMBIE_LOCK()    \
    spinlock_lock(&sched_thread_zombie_list_lock)
    
#define SCHED_THREAD_ZOMBIE_UNLOCK()  \
    spinlock_unlock(&sched_thread_zombie_list_lock)

#define SCHED_THREAD_SUSPENED_LOCK()  \
    spinlock_lock(&sched_thread_suspended_list_lock)
    
#define SCHED_THREAD_SUSPENED_UNLOCK()\
    spinlock_unlock(&sched_thread_suspended_list_lock)

#define SCHED_THREAD_ALL_LOCK()  \
    spinlock_lock(&sched_thread_global_list_lock)
    
#define SCHED_THREAD_ALL_UNLOCK()\
    spinlock_unlock(&sched_thread_global_list_lock)

status_t sched_thread_init(void)
    {
    list_init(&sched_thread_zombie_list);
    spinlock_init(&sched_thread_zombie_list_lock);
    list_init(&sched_thread_suspended_list);
    spinlock_init(&sched_thread_suspended_list_lock);
    list_init(&sched_thread_global_list);
    spinlock_init(&sched_thread_global_list_lock);
    return OK;
    }

status_t sched_thread_add_zombie
    (
    pthread_t thread
    )
    {
    SCHED_THREAD_ZOMBIE_LOCK();
    list_append(&sched_thread_zombie_list, &thread->inactive_node);
    SCHED_THREAD_ZOMBIE_UNLOCK();
    return OK;
    }

status_t sched_thread_remove_zombie
    (
    pthread_t thread
    )
    {
    SCHED_THREAD_ZOMBIE_LOCK();
    list_remove(&thread->inactive_node);
    SCHED_THREAD_ZOMBIE_UNLOCK();
    return OK;
    }

status_t sched_thread_add_suspended
    (
    pthread_t thread
    )
    {
    SCHED_THREAD_SUSPENED_LOCK();
    list_append(&sched_thread_suspended_list, &thread->inactive_node);
    SCHED_THREAD_SUSPENED_UNLOCK();
    return OK;
    }

status_t sched_thread_remove_suspended
    (
    pthread_t thread
    )
    {
    SCHED_THREAD_SUSPENED_LOCK();
    list_remove(&thread->inactive_node);
    SCHED_THREAD_SUSPENED_UNLOCK();
    return OK;
    }

status_t sched_thread_add_global
    (
    pthread_t thread
    )
    {
    SCHED_THREAD_ALL_LOCK();
    list_append(&sched_thread_global_list, &thread->global_list_node);
    SCHED_THREAD_ALL_UNLOCK();
    return OK;
    }

status_t sched_thread_remove_global
    (
    pthread_t thread
    )
    {
    SCHED_THREAD_ALL_LOCK();
    list_remove(&thread->global_list_node);
    SCHED_THREAD_ALL_UNLOCK();
    return OK;
    }

char *sched_thread_state_name(int state)
    {
    switch(state)
        {
        case STATE_READY:
            return "READY";
        case STATE_RUNNING:
            return "RUNNING";
        case STATE_SUSPENDED:
            return "SUSPENDED";
        case STATE_DELAY:
            return "DELAY";
        case STATE_PENDING:
            return "PENDING";
        case STATE_CANCEL_ARMED:
            return "CANCEL_ARMED";
        case STATE_CANCELING:
            return "CANCELING";
        case STATE_COMPLETED:
            return "COMPLETED";
        case STATE_TERMINATED:
            return "TERMINATED";
        default:
            return "UNKNOWN";
        }
    }

void sched_thread_show
    (
    pthread_t thread
    )
    {
    printk("NAME(%s)-ID(%d)-CPU(%d)-STATE(%s)-POLICY(%d)\n",
           thread->name,
           thread->id,
           thread->cpu_idx,
           sched_thread_state_name(thread->state),
           thread->sched_policy_id);
    }

void sched_thread_global_show(void)
    {
    pthread_t thread;
    
    LIST_FOREACH(&sched_thread_global_list, iter)
        {
        thread = LIST_ENTRY(iter, sched_thread_t, global_list_node);
        
        if (thread)
            {
            sched_thread_show(thread);
            }
        }    
    }

void sched_thread_common_entry
    (
    void *param
    )
    {
    /*
     * When context_restore() for this thread is called,
     * it restores starting here, this is the point for
     * newly created thread to start running!
     */
     
    kurrent->resume_cycle = rdtsc();

    SCHED_UNLOCK();
    
    /* Restore the saved interrupt flags */
    
    interrupts_restore(kurrent->saved_context.ipl);
    
    kurrent->entry((void *)(kurrent->param));  
    }

/*
 * NAME
 *
 * pthread_create - thread creation
 *
 * SYNOPSIS
 *
 * #include <pthread.h>
 *
 * int pthread_create(pthread_t *restrict thread,
 *       const pthread_attr_t *restrict attr,
 *       void *(*start_routine)(void*), void *restrict arg);
 *
 * DESCRIPTION
 *
 * The pthread_create() function is used to create a new thread,
 * with attributes specified by attr, within a process. If attr 
 * is NULL, the default attributes are used. If the attributes 
 * specified by attr are modified later, the thread's attributes 
 * are not affected. Upon successful completion, pthread_create()
 * stores the ID of the created thread in the location referenced
 * by thread.
 *
 * The thread is created executing start_routine with arg as its
 * sole argument. If the start_routine returns, the effect is as
 * if there was an implicit call to pthread_exit() using the 
 * return value of start_routine as the exit status. Note that 
 * the thread in which main() was originally invoked differs 
 * from this. When it returns from main(), the effect is as if 
 * there was an implicit call to exit() using the return value 
 * of main() as the exit status.
 *
 * The signal state of the new thread is initialised as follows:
 *
 * - The signal mask is inherited from the creating thread.
 * - The set of signals pending for the new thread is empty.
 *
 * [XSI]  The alternate stack shall not be inherited. 
 *
 * The floating-point environment shall be inherited from the 
 * creating thread.
 *
 * If pthread_create() fails, no new thread is created and the
 * contents of the location referenced by thread are undefined.
 *
 * [TCT]  If _POSIX_THREAD_CPUTIME is defined, the new thread 
 * shall have a CPU-time clock accessible, and the initial value
 * of this clock shall be set to zero. 
 *
 * The behavior is undefined if the value specified by the attr
 * argument to pthread_create() does not refer to an initialized
 * thread attributes object.
 *
 * RETURN VALUE
 *
 * If successful, the pthread_create() function shall return zero;
 * otherwise, an error number shall be returned to indicate the 
 * error.
 *
 * ERRORS
 *
 * The pthread_create() function shall fail if:
 *
 * [EAGAIN] 
 *      The system lacked the necessary resources to create another
 *      thread, or the system-imposed limit on the total number of 
 *      threads in a process {PTHREAD_THREADS_MAX} would be exceeded.
 *
 * [EPERM]
 *      The caller does not have appropriate privileges to set the 
 *      required scheduling parameters or scheduling policy.
 *
 * The pthread_create() function shall not return an error code of 
 * [EINTR].
 */
 
int pthread_create
    (
    pthread_t *thread,
    const pthread_attr_t *attr,
    void *(*start_routine)(void*), 
    void *arg
    )
    {
	void *                  stack_addr;
	size_t                  stack_size;
	struct sched_thread *   new_thread;
	pthread_attr_t          default_attr;
    sched_policy_t *        sched_policy;
    sched_runq_t *          sched_runq;
    pthread_attr_t          attrP;
    
	if (attr == NULL || *attr == NULL) 
        {
		if (pthread_attr_init(&default_attr) != OK)
            {
            return EAGAIN;
            }
        
		attrP = default_attr;
	    }
    else
        attrP = *attr;

    sched_policy = sched_policy_get_by_id(attrP->policy);
    if (!sched_policy)
        {
        printk("No scheduling policy %d registered!\n", 
            attrP->policy);
  		return EAGAIN;
        }

	stack_size = attrP->stacksize;
	stack_addr = attrP->stackaddr;

	new_thread = (struct sched_thread *) kmalloc(sizeof(struct sched_thread));
 	if (!new_thread) 
        {
        printk("No memory for a new thread structure\n");
  		return EAGAIN;
  	    }

    memset(new_thread, 0, sizeof(struct sched_thread));

    /* Copy the name */
    strncpy(new_thread->name, attrP->name, NAME_MAX);
    
    /* Set the scheduling policy */
    new_thread->sched_policy = sched_policy;
    
    /* 
     * If the user doesn't specify already allocated stack space,
     * then we need to allocate the stack on our own! 
     */
	if (!stack_addr) 
        {
        /*
         * If the specified stack size is not big enough,
         * use the big enough stack size! 
         */
        if (stack_size < CONFIG_KSTACK_SIZE)
            stack_size = CONFIG_KSTACK_SIZE;
        
		stack_addr = (char *) kmalloc(stack_size);
 		if (!stack_addr) 
            {
            printk("No memory for thread stack area\n");
			kfree(new_thread);
  			return EAGAIN;
  		    }
		new_thread->stack_base_free = stack_addr;
	    } 
    else 
	    {
		new_thread->stack_base_free = NULL;
	    }

	*thread = new_thread;

	new_thread->flags = 0;

    /* Default cancel state - enabled */
    new_thread->cancel_state = PTHREAD_CANCEL_ENABLE;

    /* Default cancel type - deferred */
    new_thread->cancel_type = PTHREAD_CANCEL_DEFERRED;
        
    /* 
     * When a thread is created detached (PTHREAD_CREATE_DETACHED),
     * its thread ID and other resources can be reused as soon as
     * the thread terminates.
     *
     * When a thread is created nondetached (PTHREAD_CREATE_JOINABLE),
     * it is assumed that you will be waiting for it. That is, it is 
     * assumed that you will be executing a pthread_join() on the 
     * thread. Whether a thread is created detached or nondetached, 
     * the process does not exit until all threads have exited.
     */
	if (attrP->detachstate == PTHREAD_CREATE_JOINABLE) 
        {
		atomic_set_bit(THREAD_JOINABLE, &new_thread->flags);
	    }

	new_thread->magic = MAGIC_VALID;
	new_thread->sig_pending = attrP->initial_signal;
	new_thread->sig_blocked = 0;

	new_thread->abort = NULL;

    /* Save the cpu set info */
    CPU_COPY(&attrP->cpu_set, &new_thread->cpu_set);
    
	new_thread->cleanup = 0;
	new_thread->resume_time = ABSTIME_INFINITY;

    /* Copy the maxium scheduler parameters */
	memcpy(new_thread->sched_param_area, 
           attrP->sched_param_area, SCHED_PARAM_AREA_SIZE);
    
	new_thread->stack_base = stack_addr;
	new_thread->stack_size = stack_size;
    new_thread->entry = start_routine;
    new_thread->param = arg;
    new_thread->sched_policy_id = attrP->policy;
        
	context_save(&new_thread->saved_context);
	context_update(&new_thread->saved_context, 
                sched_thread_common_entry, 
                new_thread->stack_base,
	            new_thread->stack_size);
    
	new_thread->saved_context.ipl = interrupts_read();

	new_thread->preemption_disabled = 0;
	new_thread->sched_cpu = current_cpus[this_cpu()];
	new_thread->tsk = NULL;
	new_thread->asp = NULL;
    
    new_thread->cpu_idx = this_cpu();
    
    sched_thread_arch_init(new_thread);

	new_thread->fpu_ready = FALSE;
	new_thread->use_fpu = attrP->usefpu;
    
    if (sched_policy->attach_cpu_group(&new_thread->cpu_set) != OK)
        {
        printk("Could not attached thread to the cpu group\n");
        return EPERM;
        }

    sched_runq = sched_policy->get_sys_runq();

    new_thread->sched_runq = sched_runq;

    printk("Created thread %s with context:\n", new_thread->name);
    
    sched_context_dump(&new_thread->saved_context);

    if (attrP->autorun)
        {
        new_thread->state = STATE_READY;
        
        sched_policy->thread_enqueue(sched_runq, new_thread, TRUE);
        }
    else
        {
        new_thread->state = STATE_SUSPENDED;

        sched_thread_add_suspended(new_thread);
        }

    sched_thread_add_global(new_thread);
    
	return OK;
    }
/*
  NAME
  
  pthread_detach - detach a thread
  
  SYNOPSIS
  
  #include <pthread.h>
  
  int pthread_detach(pthread_t thread);
  
  DESCRIPTION
  
  The pthread_detach() function shall indicate to the implementation that 
  storage for the thread thread can be reclaimed when that thread terminates. 
  If thread has not terminated, pthread_detach() shall not cause it to 
  terminate.
  
  The behavior is undefined if the value specified by the thread argument to 
  pthread_detach() does not refer to a joinable thread.
  
  RETURN VALUE
  
  If the call succeeds, pthread_detach() shall return 0; otherwise, an error 
  number shall be returned to indicate the error.
  
  ERRORS
  
  The pthread_detach() function shall not return an error code of [EINTR].
*/

int pthread_detach
    (
    pthread_t thread
    )
    {
    if (thread->magic != MAGIC_VALID)
        return EINVAL;

    return OK;
    }
/*
  NAME
  
  pthread_equal - compare thread IDs
  
  SYNOPSIS
  
  #include <pthread.h>
  
  int pthread_equal(pthread_t t1, pthread_t t2);
  
  DESCRIPTION
  
  This function shall compare the thread IDs t1 and t2.
  
  RETURN VALUE
  
  The pthread_equal() function shall return a non-zero value if t1 and t2 
  are equal; otherwise, zero shall be returned.
  
  If either t1 or t2 are not valid thread IDs, the behavior is undefined.
  
  ERRORS
  
  No errors are defined.
  
  The pthread_equal() function shall not return an error code of [EINTR].
*/

int pthread_equal(pthread_t t1, pthread_t t2)
    {
    return (t1 == t2);
    }

/* 
 * NAME
 *
 * pthread_exit - thread termination
 * 
 * SYNOPSIS
 *
 * #include <pthread.h>
 *
 * void pthread_exit(void *value_ptr);
 *
 * DESCRIPTION
 *
 * The pthread_exit() function shall terminate the calling thread and make 
 * the value value_ptr available to any successful join with the terminating 
 * thread. Any cancellation cleanup handlers that have been pushed and not
 * yet popped shall be popped in the reverse order that they were pushed
 * and then executed. After all cancellation cleanup handlers have been 
 * executed, if the thread has any thread-specific data, appropriate 
 * destructor functions shall be called in an unspecified order. Thread
 * termination does not release any application visible process resources,
 * including, but not limited to, mutexes and file descriptors, nor does 
 * it perform any process-level cleanup actions, including, but not limited
 * to, calling any atexit() routines that may exist.
 *
 * An implicit call to pthread_exit() is made when a thread other than the 
 * thread in which main() was first invoked returns from the start routine
 * that was used to create it. The function's return value shall serve as
 * the thread's exit status.
 *
 * The behavior of pthread_exit() is undefined if called from a cancellation
 * cleanup handler or destructor function that was invoked as a result of 
 * either an implicit or explicit call to pthread_exit().
 *
 * After a thread has terminated, the result of access to local (auto) 
 * variables of the thread is undefined. Thus, references to local variables
 * of the exiting thread should not be used for the pthread_exit() value_ptr 
 * parameter value.
 *
 * The process shall exit with an exit status of 0 after the last thread
 * has been terminated. The behavior shall be as if the implementation 
 * called exit() with a zero argument at thread termination time.
 *
 * RETURN VALUE
 *
 * The pthread_exit() function cannot return to its caller.
 *
 * ERRORS
 *
 * No errors are defined.
 */
void pthread_exit
    (
    void *retval
    )
    {
    ipl_t flags;
    pthread_t self = pthread_self();

    flags = interrupts_disable();

    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);

    sched_thread_do_cleanup(self, retval);

    atomic_set_bit (THREAD_FINISHED, &self->flags);
    
    sched_thread_delete(self);

    reschedule();
    
    /* Should never reach here... */
    interrupts_restore(flags);
    
    printk("pthread_exit() returned unexpectedly!\n");
    }
/*
  NAME
  
  pthread_getconcurrency - get the level of concurrency
  pthread_setconcurrency - set the level of concurrency

  SYNOPSIS
  
  #include <pthread.h>
  
  int pthread_getconcurrency(void);
  int pthread_setconcurrency(int new_level); 
  
  DESCRIPTION
  
  Unbound threads in a process may or may not be required to be simultaneously 
  active. By default, the threads implementation ensures that a sufficient 
  number of threads are active so that the process can continue to make 
  progress. While this conserves system resources, it may not produce the most 
  effective level of concurrency.
  
  The pthread_setconcurrency() function allows an application to inform the 
  threads implementation of its desired concurrency level, new_level. The 
  actual level of concurrency provided by the implementation as a result of 
  this function call is unspecified.
  
  If new_level is zero, it causes the implementation to maintain the concurrency 
  level at its discretion as if pthread_setconcurrency() had never been called.
  
  The pthread_getconcurrency() function shall return the value set by a previous 
  call to the pthread_setconcurrency() function. If the pthread_setconcurrency()
  function was not previously called, this function shall return zero to indicate 
  that the implementation is maintaining the concurrency level.
  
  A call to pthread_setconcurrency() shall inform the implementation of its 
  desired concurrency level. The implementation shall use this as a hint, not a 
  requirement.
  
  If an implementation does not support multiplexing of user threads on top of 
  several kernel-scheduled entities, the pthread_setconcurrency() and 
  pthread_getconcurrency() functions are provided for source code compatibility 
  but they shall have no effect when called. To maintain the function semantics, 
  the new_level parameter is saved when pthread_setconcurrency() is called so 
  that a subsequent call to pthread_getconcurrency() shall return the same value.
  
  RETURN VALUE
  
  If successful, the pthread_setconcurrency() function shall return zero; 
  otherwise, an error number shall be returned to indicate the error.
  
  The pthread_getconcurrency() function shall always return the concurrency 
  level set by a previous call to pthread_setconcurrency(). If the 
  pthread_setconcurrency() function has never been called, 
  pthread_getconcurrency() shall return zero.
  
  ERRORS
  
  The pthread_setconcurrency() function shall fail if:
  
  [EINVAL]
  
  The value specified by new_level is negative.
  
  [EAGAIN]
  
  The value specified by new_level would cause a system resource to be exceeded.
  
  The pthread_setconcurrency() function shall not return an error code of [EINTR].
*/

int pthread_getconcurrency(void)
    {
    return sched_thread_concurrency;
    }

int pthread_setconcurrency(int new_level)
    {
    if (new_level < 0)
        return EINVAL;
    
    sched_thread_concurrency = new_level;
    
    return OK;
    }

/*
  NAME
  
  pthread_getcpuclockid - access a thread CPU-time clock (ADVANCED REALTIME)

  SYNOPSIS
  
  #include <pthread.h>
  #include <time.h>
  
  int pthread_getcpuclockid(pthread_t thread_id, clockid_t *clock_id); 
  
  DESCRIPTION
  
  The pthread_getcpuclockid() function shall return in clock_id the clock ID
  of the CPU-time clock of the thread specified by thread_id, if the thread 
  specified by thread_id exists.
  
  RETURN VALUE
  
  Upon successful completion, pthread_getcpuclockid() shall return zero; 
  otherwise, an error number shall be returned to indicate the error.
  
  ERRORS
  
  No errors are defined.
*/

int pthread_getcpuclockid
    (
    pthread_t thread_id, 
    clockid_t *clock_id
    )
    {
    if (thread_id->magic != MAGIC_VALID)
        return EINVAL;
    
    return OK;
    }

/*
NAME
      pthread_resume_np(), pthread_continue(), pthread_suspend() - resume
      execution of a thread, continue execution of a thread, and suspend
      execution of a thread

 SYNOPSIS
      #include <pthread.h>

      int pthread_continue(
	 pthread_t thread
      );

      int pthread_resume_np(
	 pthread_t thread,
	 int flags
      );

      int pthread_suspend(
	 pthread_t thread
      );

 PARAMETERS
	   thread    whose execution is to be suspended or resumed.

	   flags     Flags to be used by pthread_resume_np().  The valid
		     values are:

		     PTHREAD_COUNT_RESUME_NP
			  The target thread¡¯s suspension count is
			  decremented by one. If the target thread was
			  suspended and has a suspend count greater than
			  one, the thread will not resume execution.

		     PTHREAD_FORCE_RESUME_NP
			  The target thread¡¯s suspension count is set to
			  zero. The target will resume execution even if its
			  suspend count was greater than one.

 DESCRIPTION
      The pthread_suspend() function suspends execution of the target thread
      specified by thread.  The target thread may not be suspended
      immediately (at that exact instant).  On successful return from the
      pthread_suspend() function, thread is no longer executing. Once a
      thread has been suspended, subsequent calls to the pthread_suspend()
      function increment a per thread suspension count and return
      immediately.

      Calling pthread_suspend() with the calling thread specified in thread
      is allowed.  Note that in this case the calling thread shall be
      suspended during execution of the pthread_suspend() function call and
      shall only return after another thread has called the

      pthread_resume_np() or pthread_continue() function for thread.

      The pthread_continue() function resumes the execution of the target
      thread thread.  If thread was suspended by multiple calls to
      pthread_suspend(), only one call to pthread_continue() is required to
      resume the execution of thread.  Calling pthread_continue() for a
      target thread that is not suspended shall have no effect and return no
      errors.  Calling pthread_continue() is equivalent to calling
      pthread_resume_np() with the flags parameter specified as
      PTHREAD_FORCE_RESUME_NP.

      The pthread_resume_np() function resumes the execution of the target
      thread specified by thread.  If the flags argument is
      PTHREAD_COUNT_RESUME_NP, the target thread¡¯s suspension count is
      decremented by one. If the flags argument is PTHREAD_FORCE_RESUME_NP,
      the target thread¡¯s suspension count is set to zero. When the target
      thread¡¯s suspension count reaches zero, the target thread is allowed
      to continue execution. Calling pthread_resume_np() for a target thread
      that is not suspended shall have no effect and return no errors.

 WARNING
      This warning is applicable if any of the following conditions is true
      for a threaded application:

      a.  The PTHREAD_FORCE_SCOPE_SYSTEM or PTHREAD_COMPAT_MODE environment
	  variable is set to either 1 or on.

      b.  The application's binary is brought forward from a HP-UX 11i v1
	  release.

      In either of the above conditions, it is strongly recommended to set
      the environment variable PTHREAD_SUSPEND_SYNC to 1 if the application
      uses pthread_suspend() which is not POSIX compliant.  Failing to do
      so, can lead to undefined behavior.

 RETURN VALUE
      If successful, pthread_continue(), pthread_suspend() and
      pthread_resume_np() return zero. Otherwise, an error number shall be
      returned to indicate the error (the errno variable is not set).

 ERRORS
      If any of the following occur, the pthread_suspend() function returns
      the corresponding error number.

	   [ESRCH]	  The target thread thread is not in the current
			  process.

	   [EDEADLK]	  The target thread thread is the last running
			  thread in the process.  The operation would result
			  in deadlock for the process.


      If any of the following occur, the pthread_continue() and
      pthread_resume_np() functions return the corresponding error number.

	   [ESRCH]	  The target thread thread is not in the current
			  process.

	   [EINVAL]	  The value specified by flags is invalid.

 APPLICATION USAGE
      This functionality enables a process that is multithreaded to
      temporarily suspend all activity to a single thread of control.  When
      the process is single threaded, the address space is not changing, and
      a consistent view of the process can be gathered.	 One example of its
      use is for garbage collecting.  The garbage collector runs
      asynchronously within the process and assumes that the process is not
      changing while it is running.

      Suspending a thread may have adverse effects on an application. If a
      thread is suspended while it holds a critical resource, such as a
      mutex or a read-write lock, the application may stop or even deadlock
      until the thread is continued. While the thread is suspended, other
      threads which may contend for the same resource must block until the
      thread is continued. Depending on application behavior, this may even
      result in deadlock. Application programmers are advised to either a)
      only suspend threads which call async-signal safe functions or b)
      ensure that the suspending thread does not contend for the same
      resources that the suspended thread may have acquired.  Note: this
      includes resources that may be acquired by libraries.

      The pthread_suspend(), pthread_continue(), and pthread_resume_np()
      functions cannot reliably be used for thread synchronization.
      Synchronization primitives like mutexes, semaphores, read-write locks,
      and condition variables should be used instead.
*/
int pthread_suspend_np 
    (
    pthread_t thread
    )
    {
    if (thread == NULL)
        thread = kurrent;
    
    thread->state = STATE_SUSPENDED;  
    
    sched_thread_add_suspended(thread);
    
    reschedule();
    
    pthread_testcancel();

	return OK;
    }

/* pthread_resume_np - resume a thread */
int pthread_resume_np
    (
    pthread_t thread,
    int flags
    )
    {
    if (thread == NULL || thread->state != STATE_SUSPENDED)
        return EINVAL;

    sched_thread_remove_suspended(thread);
    
    thread->state = STATE_READY;            
    
    thread->sched_policy->thread_enqueue(thread->sched_runq,
                                         thread, TRUE);
    reschedule();

	return OK;
    }

int pthread_continue
    (
    pthread_t thread
    )
    {
    /* 
     * Calling pthread_continue() is equivalent to calling
     * pthread_resume_np() with the flags parameter specified as
     * PTHREAD_FORCE_RESUME_NP.
     */
    pthread_resume_np(thread, PTHREAD_FORCE_RESUME_NP);

    return OK;
    }

void sched_thread_do_cleanup
    (
    pthread_t thread, 
    void *retval
    )
    {
	struct sched_thread_cleanup *thread_cleanup = thread->cleanup;
    sched_thread_posix_extention_t *posix_extention = &thread->posix_extention;
    
	while ((thread_cleanup = thread->cleanup) != NULL) 
        {
		thread->cleanup = thread_cleanup->next;
		thread_cleanup->routine(thread_cleanup->arg);
	    }

	pthread_spin_lock (&posix_extention->exit_lock);
    
	posix_extention->retval = retval;
    
	while (atomic_test_bit (THREAD_JOINABLE, &thread->flags) && 
           !posix_extention->joining_thread) 
        {
		atomic_set_bit (THREAD_WAIT_FOR_JOIN, &thread->flags);
        
        thread->state = STATE_SUSPENDED;
        
		pthread_spin_unlock (&posix_extention->exit_lock);
        
		reschedule();
        
		pthread_spin_lock (&posix_extention->exit_lock);
	    }

	pthread_spin_unlock (&posix_extention->exit_lock);
    }

/* sched_thread_free_memory - thread-specific data destructor function */
static void sched_thread_free_memory
    (
    void *p
    )
    {
	struct sched_thread *thread = (struct sched_thread *) p;

    if (!thread)
        return;
    
	if (thread->stack_base_free) 
        {
		kfree (thread->stack_base_free);
	    }

    sched_thread_remove_global(thread);
    
	kfree (thread);
    }

int sched_thread_delete
    (
    pthread_t thread
    )
    {
	status_t ret;
	ipl_t interrupt_state;
    
	interrupt_state = interrupts_disable();

    /* Construct a final cleanup facility */
	memset(&thread->free_me, 0, sizeof(struct sched_thread_cleanup));
	thread->free_me.arg = thread;
	thread->free_me.routine = sched_thread_free_memory;
	thread->cleanup = (struct sched_thread_cleanup *) &thread->free_me;

    /* The thread is now considered invalid */
	thread->magic = MAGIC_INVALID;

    ret = thread->sched_policy->detach_cpu_group(&thread->cpu_set);
        
	if (unlikely(ret != OK)) 
        {
		interrupts_restore(interrupt_state);
		return ret;
	    }
    
    /* Put the thread into zombie list */
    sched_thread_add_zombie(thread);
	
	pthread_suspend_np(pthread_self());

	interrupts_restore(interrupt_state);
    
	return OK;
    }

/*
  NAME
  
  pthread_kill - send a signal to a thread
  
  SYNOPSIS
  
  #include <signal.h>
  
  int pthread_kill(pthread_t thread, int sig); 
  
  DESCRIPTION
  
  The pthread_kill() function shall request that a signal be delivered 
  to the specified thread.
  
  As in kill(), if sig is zero, error checking shall be performed but 
  no signal shall actually be sent.
  
  RETURN VALUE
  
  Upon successful completion, the function shall return a value of zero.
  Otherwise, the function shall return an error number. If the pthread_kill() 
  function fails, no signal shall be sent.
  
  ERRORS
  
  The pthread_kill() function shall fail if:
  
  [EINVAL]      The value of the sig argument is an invalid or unsupported
                signal number.
  
  The pthread_kill() function shall not return an error code of [EINTR].
*/

int pthread_kill
    (
    pthread_t thread, 
    int sig
    )
    {
    if (!thread || thread->magic != MAGIC_VALID)
        return EINVAL;
    
    if (sig < 0 || sig >= MAX_SIGNO)
        return EINVAL;

    /* The value 0 is reserved for use as the null signal */
    if (unlikely(sig == SIGNULL)) 
        {
        return OK;
        }
    
    /* Set the singal bit */
    atomic_set_bit(sig, &thread->sig_pending);

    /* 
     * If the target thread has better precedence than current 
     * thread, reschedule so that that thread could receive this
     * signal.
     */
    if (SCHED_THREAD_PRECEDENCE_COMPARE(thread, pthread_self()))
        reschedule();
    
    return OK;
    }
