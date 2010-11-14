/* sched_mutex.c - scheduler mutex management */

#include <sys.h>
#include <arch.h>
#include <os.h>

/*
  NAME
  
  pthread_mutex_destroy, pthread_mutex_init - destroy and initialize a mutex
  
  SYNOPSIS
  
  #include <pthread.h>
  
  int pthread_mutex_destroy(pthread_mutex_t *mutex);
  int pthread_mutex_init(pthread_mutex_t *restrict mutex,
         const pthread_mutexattr_t *restrict attr);
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  
  DESCRIPTION
  
  The pthread_mutex_destroy() function shall destroy the mutex object 
  referenced by mutex; the mutex object becomes, in effect, uninitialized. 
  An implementation may cause pthread_mutex_destroy() to set the object 
  referenced by mutex to an invalid value.
  
  A destroyed mutex object can be reinitialized using pthread_mutex_init(); 
  the results of otherwise referencing the object after it has been destroyed
  are undefined.
  
  It shall be safe to destroy an initialized mutex that is unlocked. 
  Attempting to destroy a locked mutex or a mutex that is referenced 
  (for example, while being used in a pthread_cond_timedwait() or 
  pthread_cond_wait()) by another thread results in undefined behavior.
  
  The pthread_mutex_init() function shall initialize the mutex referenced 
  by mutex with attributes specified by attr. If attr is NULL, the default 
  mutex attributes are used; the effect shall be the same as passing the 
  address of a default mutex attributes object. Upon successful initialization, 
  the state of the mutex becomes initialized and unlocked.
  
  Only mutex itself may be used for performing synchronization. The result 
  of referring to copies of mutex in calls to pthread_mutex_lock(), 
  pthread_mutex_trylock(), pthread_mutex_unlock(), and pthread_mutex_destroy()
  is undefined.
  
  Attempting to initialize an already initialized mutex results in undefined 
  behavior.
  
  In cases where default mutex attributes are appropriate, the macro 
  PTHREAD_MUTEX_INITIALIZER can be used to initialize mutexes that are 
  statically allocated. The effect shall be equivalent to dynamic 
  initialization by a call to pthread_mutex_init() with parameter attr 
  specified as NULL, except that no error checks are performed.
  
  The behavior is undefined if the value specified by the mutex argument 
  to pthread_mutex_destroy() does not refer to an initialized mutex.
  
  The behavior is undefined if the value specified by the attr argument 
  to pthread_mutex_init() does not refer to an initialized mutex attributes
  object.
  
  RETURN VALUE
  
  If successful, the pthread_mutex_destroy() and pthread_mutex_init() 
  functions shall return zero; otherwise, an error number shall be returned 
  to indicate the error.
  
  ERRORS
  
  The pthread_mutex_init() function shall fail if:
  
  [EAGAIN]
  
  The system lacked the necessary resources (other than memory) to initialize
  another mutex.

  [ENOMEM]
  
  Insufficient memory exists to initialize the mutex.
  
  [EPERM]
  
  The caller does not have the privilege to perform the operation.
  
  The pthread_mutex_init() function may fail if:
  
  [EINVAL]
  
  The attributes object referenced by attr has the robust mutex attribute 
  set without the process-shared attribute being set.
  
  These functions shall not return an error code of [EINTR].
*/

int pthread_mutex_destroy(pthread_mutex_t *mutex)
    {
    pthread_mutex_t mutexP = *mutex;
    
    if (mutexP->magic != MAGIC_VALID)
        return EINVAL;
    
    mutexP->magic = MAGIC_INVALID;
    
    return OK;
    }

int pthread_mutex_init
    (
    pthread_mutex_t * mutex,
    const pthread_mutexattr_t * attr
    )
    {
    pthread_mutex_t mutexP;
    pthread_mutexattr_t attrP = *attr;
    
    if (attrP->magic != MAGIC_VALID)
        return EINVAL;

    mutexP = kmalloc(sizeof(*mutexP));

    if (!mutexP)
        return ENOMEM;

    memset(mutexP, 0, sizeof(*mutexP));
    
    atomic_set(&mutexP->counter, 0);
    
    queue_init(&mutexP->waitq, (char *)attrP->name, FALSE);
    
    spinlock_init(&mutexP->lock);
    
    mutexP->best_waiter = NULL;
    
    mutexP->owner = NULL;

    mutexP->magic = MAGIC_VALID;
    
    memcpy(&mutexP->attr, attrP, sizeof(*attrP));

    *mutex = mutexP;
    
    return OK;
    }

/*
  NAME
  
  pthread_mutex_lock - lock a mutex
  pthread_mutex_trylock - try lock a mutex
  pthread_mutex_unlock - unlock a mutex
  
  SYNOPSIS
  
  #include <pthread.h>
  
  int pthread_mutex_lock(pthread_mutex_t *mutex);
  int pthread_mutex_trylock(pthread_mutex_t *mutex);
  int pthread_mutex_unlock(pthread_mutex_t *mutex);
  
  DESCRIPTION
  
  The mutex object referenced by mutex shall be locked by calling 
  pthread_mutex_lock(). If the mutex is already locked, the calling thread 
  shall block until the mutex becomes available. This operation shall return 
  with the mutex object referenced by mutex in the locked state with the
  calling thread as its owner.
  
  If the mutex type is PTHREAD_MUTEX_NORMAL, deadlock detection shall not be
  provided. Attempting to relock the mutex causes deadlock. If a thread 
  attempts to unlock a mutex that it has not locked or a mutex which is 
  unlocked, undefined behavior results.
  
  If the mutex type is PTHREAD_MUTEX_ERRORCHECK, then error checking shall
  be provided. If a thread attempts to relock a mutex that it has already
  locked, an error shall be returned. If a thread attempts to unlock a mutex
  that it has not locked or a mutex which is unlocked, an error shall be 
  returned.
  
  If the mutex type is PTHREAD_MUTEX_RECURSIVE, then the mutex shall maintain
  the concept of a lock count. When a thread successfully acquires a mutex 
  for the first time, the lock count shall be set to one. Every time a thread
  relocks this mutex, the lock count shall be incremented by one. Each time 
  the thread unlocks the mutex, the lock count shall be decremented by one.
  When the lock count reaches zero, the mutex shall become available for other
  threads to acquire. If a thread attempts to unlock a mutex that it has not 
  locked or a mutex which is unlocked, an error shall be returned.
  
  If the mutex type is PTHREAD_MUTEX_DEFAULT, attempting to recursively lock 
  the mutex results in undefined behavior. Attempting to unlock the mutex if
  it was not locked by the calling thread results in undefined behavior. 
  Attempting to unlock the mutex if it is not locked results in undefined 
  behavior.
  
  The pthread_mutex_trylock() function shall be equivalent to pthread_mutex_lock(), 
  except that if the mutex object referenced by mutex is currently locked 
  (by any thread, including the current thread), the call shall return 
  immediately. If the mutex type is PTHREAD_MUTEX_RECURSIVE and the mutex is 
  currently owned by the calling thread, the mutex lock count shall be 
  incremented by one and the pthread_mutex_trylock() function shall immediately
  return success.
  
  The pthread_mutex_unlock() function shall release the mutex object referenced
  by mutex. The manner in which a mutex is released is dependent upon the 
  mutex's type attribute. If there are threads blocked on the mutex object 
  referenced by mutex when pthread_mutex_unlock() is called, resulting in the 
  mutex becoming available, the scheduling policy shall determine which thread 
  shall acquire the mutex.
  
  (In the case of PTHREAD_MUTEX_RECURSIVE mutexes, the mutex shall become 
  available when the count reaches zero and the calling thread no longer has 
  any locks on this mutex.)
  
  If a signal is delivered to a thread waiting for a mutex, upon return from 
  the signal handler the thread shall resume waiting for the mutex as if it 
  was not interrupted.
  
  If mutex is a robust mutex and the process containing the owning thread 
  terminated while holding the mutex lock, a call to pthread_mutex_lock() 
  shall return the error value [EOWNERDEAD]. If mutex is a robust mutex and 
  the owning thread terminated while holding the mutex lock, a call to 
  pthread_mutex_lock() may return the error value [EOWNERDEAD] even if the 
  process in which the owning thread resides has not terminated. In these 
  cases, the mutex is locked by the thread but the state it protects is 
  marked as inconsistent. The application should ensure that the state is 
  made consistent for reuse and when that is complete call 
  pthread_mutex_consistent(). If the application is unable to recover the 
  state, it should unlock the mutex without a prior call to 
  pthread_mutex_consistent(), after which the mutex is marked permanently 
  unusable.
  
  If mutex does not refer to an initialized mutex object, the behavior of 
  pthread_mutex_lock(), pthread_mutex_trylock(), and pthread_mutex_unlock() 
  is undefined.
  
  RETURN VALUE
  
  If successful, the pthread_mutex_lock() and pthread_mutex_unlock() functions
  shall return zero; otherwise, an error number shall be returned to indicate 
  the error.
  
  The pthread_mutex_trylock() function shall return zero if a lock on the 
  mutex object referenced by mutex is acquired. Otherwise, an error number 
  is returned to indicate the error.
  
  ERRORS
  
  The pthread_mutex_lock() and pthread_mutex_trylock() functions shall fail if:
  
  [EAGAIN]
  
  The mutex could not be acquired because the maximum number of recursive 
  locks for mutex has been exceeded.

  [EINVAL]
  
  The mutex was created with the protocol attribute having the value 
  PTHREAD_PRIO_PROTECT and the calling thread's priority is higher than the 
  mutex's current priority ceiling. 

  [ENOTRECOVERABLE]
  
  The state protected by the mutex is not recoverable.
  
  [EOWNERDEAD]
  
  The mutex is a robust mutex and the process containing the previous owning 
  thread terminated while holding the mutex lock. The mutex lock shall be 
  acquired by the calling thread and it is up to the new owner to make the 
  state consistent.
  
  The pthread_mutex_lock() function shall fail if:
  
  [EDEADLK]
  
  The mutex type is PTHREAD_MUTEX_ERRORCHECK and the current thread already 
  owns the mutex.
  
  The pthread_mutex_trylock() function shall fail if:
  
  [EBUSY]
  
  The mutex could not be acquired because it was already locked.
  
  The pthread_mutex_unlock() function shall fail if:
  
  [EPERM]
  
  The mutex type is PTHREAD_MUTEX_ERRORCHECK or the mutex is a robust mutex, 
  and the current thread does not own the mutex.
  
  The pthread_mutex_lock() and pthread_mutex_trylock() functions may fail if:
  
  [EOWNERDEAD]
  
  The mutex is a robust mutex and the previous owning thread terminated while 
  holding the mutex lock. The mutex lock shall be acquired by the calling 
  thread and it is up to the new owner to make the state consistent.
  
  The pthread_mutex_lock() function may fail if:
  
  [EDEADLK]
  
  A deadlock condition was detected.
  
  These functions shall not return an error code of [EINTR].
*/
int pthread_mutex_lock
    (
    pthread_mutex_t *mutex
    )
    {
    pthread_t self_thread = pthread_self();
    sched_policy_t * self_policy = self_thread->sched_policy;
    pthread_mutex_t mutexP = *mutex;

    if (mutexP->magic != MAGIC_VALID)
        return EINVAL;
    
    if (mutexP->attr.protocol == PTHREAD_PRIO_PROTECT)
        {                 
        if (self_policy->id == SCHED_FIFO && 
            self_policy->get_priority(self_thread) > mutexP->attr.prioceiling)
            return EINVAL;
        }
    /* 
     * If the owned count is non-zero then the mutexP has been 
     * taken by a self_thread. 
     */
    if (atomic_cmpxchg(&mutexP->counter, 0, 1) != 0)
        {
        printk("Mutex %s already taken by thread %s on cpu %d\n",
            mutexP->attr.name, mutexP->owner->name, mutexP->owner->cpu_idx);
        
        /* Check if the caller self_thread is already the owner of the mutexP */
        if (self_thread == mutexP->owner)
            {
            /* 
             * We have already owned the mutexP, if we are recursive, 
             * increase the owned count and return to allow the lock!
             */
            if (mutexP->attr.type == PTHREAD_MUTEX_RECURSIVE)
                {
                printk("PTHREAD_MUTEX_RECURSIVE for cpu-%d self_thread %s\n",
                    this_cpu(), kurrent->name);

                if (atomic_read(&mutexP->counter) >= SCHED_MUTEX_MAX_RECURSIVES)
                    {
                    printk("Already exceeded max recusive count %d\n",
                        SCHED_MUTEX_MAX_RECURSIVES);

                    /* 
                     * The mutexP could not be acquired because the maximum 
                     * number of recursive locks for mutexP has been exceeded.
                     */
                    return EAGAIN;
                    }
                
                /* Increase the owned counter */
                atomic_inc(&mutexP->counter);
                
                return OK;
                }
            else
                {
                if (mutexP->attr.type == PTHREAD_MUTEX_ERRORCHECK)
                    {
                    /* 
                     * DEAD LOCK!
                     *
                     * The mutexP type is PTHREAD_MUTEX_ERRORCHECK 
                     * and the current self_thread already owns the mutexP.
                     */
                    return EDEADLK;
                    }
                
                panic("Thread %s already owns the mutexP!\n",
                      kurrent->name);

                return EINVAL;
                }
            }
        else /* (pthread_self() != mutexP->owner) */
            {
            lable_wokenup_and_try_again:
            /*
             * The mutexP owner is not us, so the calling thread will 
             * be put on to the mutexP thread waitq! Take the 
             * queue lock so that we can safely modify the queue!
             */

            spinlock_lock(&mutexP->lock);
            
            /*
             * See if we have a final chance to take the mutexP
             * when the owning released the mutexP during the time
             */
             
            if (atomic_cmpxchg(&mutexP->counter, 0, 1) != 0)
                {
                /* Upon here, there must be a mutexP owner */
                if (mutexP->owner == NULL)
                    {
                    spinlock_unlock(&mutexP->lock);
                    
                    return EINVAL;
                    }
                                
                /* Add the current thread to the waitq */  
                enqueue(&mutexP->waitq, &self_thread->waitq_node, FALSE);
                
                /* Set the current self_thread as pending on mutexP */
                self_thread->state = STATE_PENDING;
                
                /*
                 * Release the lock so other threads can be added
                 * onto the waiting list
                 */
                spinlock_unlock(&mutexP->lock);
                
                /* Setup how we can wake up the self_thread */
                if ((mutexP->attr.order == PTHREAD_ORDER_PRIO) ||
                    (mutexP->attr.protocol == PTHREAD_PRIO_INHERIT))
                    {
                    BOOL better;
                    
                    /* If there is no other waiting, caller is the best! */
                    if (mutexP->best_waiter == NULL)
                        {
                        mutexP->best_waiter = self_thread;
                        }
                    else
                        {
                        better = SCHED_THREAD_PRECEDENCE_COMPARE(self_thread, 
                                                        mutexP->best_waiter);
                        if (better)
                            mutexP->best_waiter = self_thread;
                        }
                    
                    /* 
                     * If the owner needs to inherit a better priority, 
                     * do it now, before the caller goes to PEND! 
                     */
                    if (mutexP->attr.protocol == PTHREAD_PRIO_INHERIT)
                        {
                        better = SCHED_THREAD_PRECEDENCE_COMPARE(mutexP->best_waiter,
                                                                 mutexP->owner);
                        if (better)
                            {
                            int better_prio;
                            sched_policy_t * better_policy;
                            
                            better_policy = 
                                mutexP->best_waiter->sched_policy;
                            better_prio = 
                                better_policy->get_priority(mutexP->best_waiter);
                            
                            /*
                             * We currently support only change priority
                             * in the same policy 
                             */
                            if (better_policy->id == mutexP->owner_policy->id)
                                mutexP->owner_policy->change_priority(mutexP->owner,
                                                              better_prio);
                            }
                        }
                    }
                                
                /* Reschedule for another thread to run on the CPU */
                reschedule();

                /* 
                 * Once we come here, someone has released the mutexP 
                 * and woken us up! Go and become the owner! However,
                 * we still need to increase the lock counter, so go
                 * back to try again, but this time we might need just
                 * one shot! The worst case is that the mutexP may be 
                 * locked by some new comer before we come here but 
                 * get preempted by the new comer! In this case we might
                 * go back to the waitq and life runs back again...
                 */
                goto lable_wokenup_and_try_again;
                }
            else
                {
                /* We have taken the chance to become the owner! */
                spinlock_unlock(&mutexP->lock);
                }
            }
        }
    
    /* Once here, we have been given the mutexP and become the owner */
    mutexP->owner = self_thread;
    mutexP->owner_policy = self_policy;
    mutexP->owner_priority = self_policy->get_priority(self_thread);
    
    /* The mutexP owner shall run at its highest possible priority */
    if ((mutexP->attr.protocol == PTHREAD_PRIO_PROTECT) &&
        (self_policy->id == SCHED_FIFO))
        {
        if (mutexP->owner_priority < mutexP->attr.prioceiling)
            self_policy->change_priority(self_thread, mutexP->attr.prioceiling);
        }
    
    return OK;
    }

int pthread_mutex_trylock
    (
    pthread_mutex_t *mutex
    )
    {
    pthread_t self_thread = pthread_self();
    sched_policy_t * self_policy = self_thread->sched_policy;
    pthread_mutex_t mutexP = *mutex;

    if (mutexP->magic != MAGIC_VALID)
        return EINVAL;

    if (mutexP->attr.protocol == PTHREAD_PRIO_PROTECT)
        {                 
        if (self_policy->id == SCHED_FIFO && 
            self_policy->get_priority(self_thread) > mutexP->attr.prioceiling)
            return EINVAL;
        }
    /* 
     * If the owned count is non-zero then the mutexP has been 
     * taken by a self_thread. 
     */
    if (atomic_cmpxchg(&mutexP->counter, 0, 1) != 0)
        {
        /* Check if the caller self_thread is already the owner of the mutexP */
        if (self_thread == mutexP->owner)
            {
            /* 
             * We have already owned the mutexP, if we are recursive, 
             * increase the owned count and return to allow the lock!
             */
            if (mutexP->attr.type == PTHREAD_MUTEX_RECURSIVE)
                {
                if (atomic_read(&mutexP->counter) >= SCHED_MUTEX_MAX_RECURSIVES)
                    {
                    printk("Already exceeded max recusive count %d\n",
                        SCHED_MUTEX_MAX_RECURSIVES);

                    /* 
                     * The mutexP could not be acquired because the maximum 
                     * number of recursive locks for mutexP has been exceeded.
                     */
                    return EAGAIN;
                    }
                
                /* Increase the owned counter */
                atomic_inc(&mutexP->counter);
                
                return OK;
                }
            else
                {
                if (mutexP->attr.type == PTHREAD_MUTEX_ERRORCHECK)
                    {
                    /* 
                     * DEAD LOCK!
                     *
                     * The mutexP type is PTHREAD_MUTEX_ERRORCHECK 
                     * and the current self_thread already owns the mutexP.
                     */
                    return EDEADLK;
                    }
                
                panic("Thread %s already owns the mutexP!\n",
                      kurrent->name);

                return EINVAL;
                }
            }
        else /* (pthread_self() != mutexP->owner) */
            {
            return EBUSY;
            }
        }
    
    /* Once here, we have been given the mutexP and become the owner */
    mutexP->owner = self_thread;
    mutexP->owner_policy = self_policy;
    mutexP->owner_priority = self_policy->get_priority(self_thread);
    
    /* The mutexP owner shall run at its highest possible priority */
    if ((mutexP->attr.protocol == PTHREAD_PRIO_PROTECT) &&
        (self_policy->id == SCHED_FIFO))
        {
        if (mutexP->owner_priority < mutexP->attr.prioceiling)
            self_policy->change_priority(self_thread, mutexP->attr.prioceiling);
        }
    
    return OK;
    }

int pthread_mutex_unlock
    (
    pthread_mutex_t *mutex
    )
    {
    pthread_mutex_t mutexP = *mutex;

    if (mutexP->magic != MAGIC_VALID)
        return EINVAL;
    
    spinlock_lock(&mutexP->lock);

	if (unlikely(atomic_read(&mutexP->counter) == 0) ||
        unlikely(mutexP->owner != pthread_self())) 
        {
        spinlock_unlock(&mutexP->lock);

        if ((mutexP->attr.type == PTHREAD_MUTEX_ERRORCHECK) ||
            (mutexP->attr.robust == PTHREAD_MUTEX_ROBUST))
            return EPERM;
        else
            {
            panic("The mutexP is not owned by the caller %s\n",
                  pthread_self()->name);
            return EINVAL;
            }
	    } 
        
    /* atomic_dec_and_test() returns TRUE when decreased to 0 */
    if ((atomic_dec_and_test(&mutexP->counter) == TRUE) &&
        (!queue_empty(&mutexP->waitq)))
        {
        pthread_t wake_thread = NULL;
        pthread_t next_thread = NULL;
        pthread_t better_thread = NULL;

        if (mutexP->attr.order == PTHREAD_ORDER_FIFO)
            {
            wake_thread = queue_entry(queue_first(&mutexP->waitq),
                                      sched_thread_t, waitq_node);
            queue_remove(&wake_thread->waitq_node, FALSE);
            }
        else if (mutexP->attr.order == PTHREAD_ORDER_PRIO)
            {
            if (mutexP->best_waiter)
                wake_thread = mutexP->best_waiter;
            else
                wake_thread = queue_entry(queue_first(&mutexP->waitq),
                                          sched_thread_t, waitq_node);
            
            queue_remove(&wake_thread->waitq_node, FALSE);
            
            QUEUE_ITERATE(&mutexP->waitq, iter)
                {
                next_thread = queue_entry(iter, sched_thread_t, waitq_node);

                if (next_thread)
                    {
                    if (!better_thread)
                        better_thread = next_thread;
                    else
                        {
                        BOOL better;
                        
                        better = SCHED_THREAD_PRECEDENCE_COMPARE(next_thread,
                                                                 better_thread);
                        if (better)
                            better_thread = next_thread;
                        }
                    }
                }
            
            /* Save the next better thread */
            mutexP->best_waiter = better_thread;
            }

        spinlock_unlock(&mutexP->lock);

        if (mutexP->owner_priority != 
            mutexP->owner_policy->get_priority(mutexP->owner))
            {
            mutexP->owner_policy->change_priority(mutexP->owner, 
                mutexP->owner_priority);
            }
        
        mutexP->owner = NULL;
        mutexP->owner_policy = NULL;
        mutexP->owner_priority = 0;
        
        if (wake_thread)
            {
            wake_thread->state = STATE_READY;
            
            wake_thread->sched_policy->thread_enqueue(wake_thread->sched_runq, 
                                                      wake_thread, TRUE);
            
            reschedule();
            }
        return OK;
        }
    else /* Owned count (recusive) not reach 0 yet! */
        {
        spinlock_unlock(&mutexP->lock);

        return OK;
        }
    }
/*
  NAME
  
  pthread_mutex_timedlock - lock a mutex
  
  SYNOPSIS
  
  #include <pthread.h>
  #include <time.h>
  
  int pthread_mutex_timedlock(pthread_mutex_t *restrict mutex,
         const struct timespec *restrict abstime);
  
  DESCRIPTION
  
  The pthread_mutex_timedlock() function shall lock the mutex object 
  referenced by mutex. If the mutex is already locked, the calling thread 
  shall block until the mutex becomes available as in the pthread_mutex_lock() 
  function. If the mutex cannot be locked without waiting for another thread 
  to unlock the mutex, this wait shall be terminated when the specified 
  timeout expires.
  
  The timeout shall expire when the absolute time specified by abstime passes, 
  as measured by the clock on which timeouts are based (that is, when the
  value of that clock equals or exceeds abstime), or if the absolute time 
  specified by abstime has already been passed at the time of the call.
  
  The timeout shall be based on the CLOCK_REALTIME clock. The resolution of 
  the timeout shall be the resolution of the clock on which it is based. The 
  timespec data type is defined in the <time.h> header.
  
  Under no circumstance shall the function fail with a timeout if the mutex 
  can be locked immediately. The validity of the abstime parameter need not 
  be checked if the mutex can be locked immediately.
  
  [RPI|TPI]  As a consequence of the priority inheritance rules (for mutexes 
  initialized with the PRIO_INHERIT protocol), if a timed mutex wait is 
  terminated because its timeout expires, the priority of the owner of the 
  mutex shall be adjusted as necessary to reflect the fact that this thread 
  is no longer among the threads waiting for the mutex. 
  
  If mutex is a robust mutex and the process containing the owning thread 
  terminated while holding the mutex lock, a call to pthread_mutex_timedlock() 
  shall return the error value [EOWNERDEAD]. If mutex is a robust mutex and 
  the owning thread terminated while holding the mutex lock, a call to 
  pthread_mutex_timedlock() may return the error value [EOWNERDEAD] even if 
  the process in which the owning thread resides has not terminated. In these
  cases, the mutex is locked by the thread but the state it protects is marked 
  as inconsistent. The application should ensure that the state is made 
  consistent for reuse and when that is complete call pthread_mutex_consistent(). If the application is unable to recover the state, it should unlock the mutex without a prior call to pthread_mutex_consistent(), after which the mutex is marked permanently unusable.
  
  If mutex does not refer to an initialized mutex object, the behavior is 
  undefined.
  
  RETURN VALUE
  
  If successful, the pthread_mutex_timedlock() function shall return zero; 
  otherwise, an error number shall be returned to indicate the error.
  
  ERRORS
  
  The pthread_mutex_timedlock() function shall fail if:
  
  [EAGAIN]
  
  The mutex could not be acquired because the maximum number of recursive 
  locks for mutex has been exceeded.

  [EDEADLK]
  
  The mutex type is PTHREAD_MUTEX_ERRORCHECK and the current thread already 
  owns the mutex.

  [EINVAL]
  
  The mutex was created with the protocol attribute having the value 
  PTHREAD_PRIO_PROTECT and the calling thread's priority is higher than 
  the mutex' current priority ceiling.

  [EINVAL]
  
  The process or thread would have blocked, and the abstime parameter 
  specified a nanoseconds field value less than zero or greater than or 
  equal to 1000 million.

  [ENOTRECOVERABLE]
  
  The state protected by the mutex is not recoverable.
  
  [EOWNERDEAD]
  
  The mutex is a robust mutex and the process containing the previous owning
  thread terminated while holding the mutex lock. The mutex lock shall be 
  acquired by the calling thread and it is up to the new owner to make the 
  state consistent.

  [ETIMEDOUT]
  
  The mutex could not be locked before the specified timeout expired.
  
  The pthread_mutex_timedlock() function may fail if:
  
  [EDEADLK]
  
  A deadlock condition was detected.
  
  [EOWNERDEAD]
  
  The mutex is a robust mutex and the previous owning thread terminated while 
  holding the mutex lock. The mutex lock shall be acquired by the calling 
  thread and it is up to the new owner to make the state consistent.
  
  This function shall not return an error code of [EINTR].
*/

int pthread_mutex_timedlock
    (
    pthread_mutex_t * mutex,
    const struct timespec * abstime
    )
    {
    pthread_mutex_t mutexP = *mutex;

    if (mutexP->magic != MAGIC_VALID)
        return EINVAL;
    
    return OK;
    }

/*
  NAME
  
  pthread_mutexattr_init - initialize the mutex attributes object
  pthread_mutexattr_destroy - destroy the mutex attributes object

  SYNOPSIS
  
  #include <pthread.h>
  
  int pthread_mutexattr_destroy(pthread_mutexattr_t *attr);
  int pthread_mutexattr_init(pthread_mutexattr_t *attr);
  
  DESCRIPTION
  
  The pthread_mutexattr_destroy() function shall destroy a mutex attributes 
  object; the object becomes, in effect, uninitialized. An implementation 
  may cause pthread_mutexattr_destroy() to set the object referenced by attr 
  to an invalid value.
  
  A destroyed attr attributes object can be reinitialized using 
  pthread_mutexattr_init(); the results of otherwise referencing the object 
  after it has been destroyed are undefined.
  
  The pthread_mutexattr_init() function shall initialize a mutex attributes 
  object attr with the default value for all of the attributes defined by 
  the implementation.
  
  Results are undefined if pthread_mutexattr_init() is called specifying an 
  already initialized attr attributes object.
  
  After a mutex attributes object has been used to initialize one or more 
  mutexes, any function affecting the attributes object (including destruction)
  shall not affect any previously initialized mutexes.
  
  The behavior is undefined if the value specified by the attr argument to 
  pthread_mutexattr_destroy() does not refer to an initialized mutex 
  attributes object.
  
  RETURN VALUE
  
  Upon successful completion, pthread_mutexattr_destroy() and 
  pthread_mutexattr_init() shall return zero; otherwise, an error number 
  shall be returned to indicate the error.
  
  ERRORS
  
  The pthread_mutexattr_init() function shall fail if:
  
  [ENOMEM]
  
  Insufficient memory exists to initialize the mutex attributes object.
  
  These functions shall not return an error code of [EINTR].
 */
int pthread_mutexattr_destroy
    (
    pthread_mutexattr_t *attr
    )
    {
    pthread_mutexattr_t attrP = *attr;
    
    attrP->magic = MAGIC_INVALID;

    kfree(attrP);
    
    return OK;
    }

int pthread_mutexattr_init
    (
    pthread_mutexattr_t *attr
    )
    {
    pthread_mutexattr_t attrP = kmalloc(sizeof(*attrP));

    if (!attrP)
        return ENOMEM;
    
    memset(attrP, 0, sizeof(pthread_mutexattr_t));
    
    strncpy(attrP->name, "NoNameMutex", NAME_MAX);
    
    attrP->prioceiling = 0;
    attrP->order = PTHREAD_ORDER_FIFO;
    attrP->protocol = PTHREAD_PRIO_INHERIT;
    attrP->type = PTHREAD_MUTEX_RECURSIVE;
    attrP->pshared = PTHREAD_PROCESS_PRIVATE;
    attrP->robust = PTHREAD_MUTEX_STALLED;
    attrP->magic = MAGIC_VALID;
    
    *attr = attrP;

    return OK;
    }

/*
  NAME
  
  pthread_mutexattr_getprotocol - get protocol attribute of mutex (REALTIME)
  pthread_mutexattr_setprotocol - set protocol attribute of mutex (REALTIME)

  SYNOPSIS
  
  #include <pthread.h>
  
  int pthread_mutexattr_getprotocol(const pthread_mutexattr_t
         *restrict attr, int *restrict protocol);
  int pthread_mutexattr_setprotocol(pthread_mutexattr_t *attr,
         int protocol); 
  
  DESCRIPTION
  
  The pthread_mutexattr_getprotocol() and pthread_mutexattr_setprotocol()
  functions, respectively, shall get and set the protocol attribute of a 
  mutex attributes object pointed to by attr which was previously created 
  by the function pthread_mutexattr_init().
  
  The protocol attribute defines the protocol to be followed in utilizing 
  mutexes. The value of protocol may be one of:
  
  - PTHREAD_PRIO_INHERIT
  - PTHREAD_PRIO_NONE
  - PTHREAD_PRIO_PROTECT
  
  which are defined in the <pthread.h> header. The default value of the 
  attribute shall be PTHREAD_PRIO_NONE.
  
  When a thread owns a mutex with the PTHREAD_PRIO_NONE protocol attribute, 
  its priority and scheduling shall not be affected by its mutex ownership.
  
  When a thread is blocking higher priority threads because of owning one 
  or more robust/non-robust mutexes with the PTHREAD_PRIO_INHERIT protocol  
  attribute, it shall execute at the higher of its priority or the priority  
  of the highest priority thread waiting on any of the robust mutexes owned  
  by this thread and initialized with this protocol. 
    
  When a thread owns one or more robust/non-robust mutexes initialized with the 
  PTHREAD_PRIO_PROTECT protocol, it shall execute at the higher of its 
  priority or the highest of the priority ceilings of all the robust mutexes 
  owned by this thread and initialized with this attribute, regardless of
  whether other threads are blocked on any of these robust/non-robust mutexes
  or not. 
    
  While a thread is holding a mutex which has been initialized with the 
  PTHREAD_PRIO_INHERIT or PTHREAD_PRIO_PROTECT protocol attributes, it 
  shall not be subject to being moved to the tail of the scheduling 
  queue at its priority in the event that its original priority is changed, 
  such as by a call to sched_setparam(). Likewise, when a thread unlocks 
  a mutex that has been initialized with the PTHREAD_PRIO_INHERIT or 
  PTHREAD_PRIO_PROTECT protocol attributes, it shall not be subject to
  being moved to the tail of the scheduling queue at its priority in the 
  event that its original priority is changed.
  
  If a thread simultaneously owns several mutexes initialized with different 
  protocols, it shall execute at the highest of the priorities that it would 
  have obtained by each of these protocols.
  
  When a thread makes a call to pthread_mutex_lock(), the mutex was initialized
  with the protocol attribute having the value PTHREAD_PRIO_INHERIT, when 
  the calling thread is blocked because the mutex is owned by another thread, 
  that owner thread shall inherit the priority level of the calling thread as 
  long as it continues to own the mutex. The implementation shall update its 
  execution priority to the maximum of its assigned priority and all its 
  inherited priorities. Furthermore, if this owner thread itself becomes
  blocked on another mutex with the protocol attribute having the value 
  PTHREAD_PRIO_INHERIT, the same priority inheritance effect shall be 
  propagated to this other owner thread, in a recursive manner. 
  
  The behavior is undefined if the value specified by the attr argument to 
  pthread_mutexattr_getprotocol() or pthread_mutexattr_setprotocol() does 
  not refer to an initialized mutex attributes object.
  
  RETURN VALUE
  
  Upon successful completion, the pthread_mutexattr_getprotocol() and 
  pthread_mutexattr_setprotocol() functions shall return zero; otherwise, 
  an error number shall be returned to indicate the error.
  
  ERRORS
  
  The pthread_mutexattr_setprotocol() function shall fail if:
  
  [ENOTSUP]
  
  The value specified by protocol is an unsupported value.
  
  The pthread_mutexattr_getprotocol() and pthread_mutexattr_setprotocol() 
  functions may fail if:
  
  [EINVAL]
  
  The value specified by protocol is invalid.
  
  [EPERM]
  
  The caller does not have the privilege to perform the operation.
  
  These functions shall not return an error code of [EINTR].
*/
int pthread_mutexattr_getprotocol
    (
    const pthread_mutexattr_t * attr, 
    int * protocol
    )
    {
    pthread_mutexattr_t attrP = *attr;
    
    if (attrP->magic != MAGIC_VALID)
        return EINVAL;
    
    if ((attrP->protocol != PTHREAD_PRIO_NONE) && 
        (attrP->protocol != PTHREAD_PRIO_INHERIT) &&
        (attrP->protocol != PTHREAD_PRIO_PROTECT))
        return EINVAL;
        
    *protocol = attrP->protocol;

    return OK;
    }

int pthread_mutexattr_setprotocol
    (
    pthread_mutexattr_t *attr,
    int protocol
    )
    {
    pthread_mutexattr_t attrP = *attr;
    
    if (attrP->magic != MAGIC_VALID)
        return EINVAL;

    if ((protocol != PTHREAD_PRIO_NONE) && 
        (protocol != PTHREAD_PRIO_INHERIT) &&
        (protocol != PTHREAD_PRIO_PROTECT))
        return ENOTSUP;
    
    attrP->protocol = protocol;

    return OK;
    }

/*
  NAME
  
  pthread_mutexattr_getprioceiling - get prioceiling attribute of mutex (REALTIME)
  pthread_mutexattr_setprioceiling - set prioceiling attribute of mutex (REALTIME)

  SYNOPSIS
  
  [RPP|TPP]  #include <pthread.h>
  
  int pthread_mutexattr_getprioceiling(const pthread_mutexattr_t
         *restrict attr, int *restrict prioceiling);
  int pthread_mutexattr_setprioceiling(pthread_mutexattr_t *attr,
         int prioceiling); 
  
  DESCRIPTION
  
  The pthread_mutexattr_getprioceiling() and pthread_mutexattr_setprioceiling() 
  functions, respectively, shall get and set the priority ceiling attribute of 
  a mutex attributes object pointed to by attr which was previously created by 
  the function pthread_mutexattr_init().
  
  The prioceiling attribute contains the priority ceiling of initialized 
  mutexes. The values of prioceiling are within the maximum range of priorities
  defined by SCHED_FIFO.
  
  The prioceiling attribute defines the priority ceiling of initialized mutexes,
  which is the minimum priority level at which the critical section guarded by 
  the mutex is executed. In order to avoid priority inversion, the priority 
  ceiling of the mutex shall be set to a priority higher than or equal to the 
  highest priority of all the threads that may lock that mutex. The values of 
  prioceiling are within the maximum range of priorities defined under the 
  SCHED_FIFO scheduling policy.
  
  The behavior is undefined if the value specified by the attr argument to 
  pthread_mutexattr_getprioceiling() or pthread_mutexattr_setprioceiling() 
  does not refer to an initialized mutex attributes object.
  
  RETURN VALUE
  
  Upon successful completion, the pthread_mutexattr_getprioceiling() and 
  pthread_mutexattr_setprioceiling() functions shall return zero; otherwise, 
  an error number shall be returned to indicate the error.
  
  ERRORS
  
  These functions may fail if:
  
  [EINVAL]
  
  The value specified by prioceiling is invalid.
  
  [EPERM]
  
  The caller does not have the privilege to perform the operation.
  
  These functions shall not return an error code of [EINTR].
*/

int pthread_mutexattr_getprioceiling
    (
    const pthread_mutexattr_t * attr, 
    int * prioceiling
    )
    {
    sched_policy_t * policy = sched_policy_get_by_id(SCHED_FIFO);
    pthread_mutexattr_t attrP = *attr;
    
    if (attrP->magic != MAGIC_VALID)
        return EINVAL;

    if (policy == NULL)
        return EINVAL;
    
    *prioceiling = attrP->prioceiling;
    
    return OK;
    }

int pthread_mutexattr_setprioceiling
    (
    pthread_mutexattr_t *attr,
    int prioceiling
    )
    {
    sched_policy_t * policy = sched_policy_get_by_id(SCHED_FIFO);
    pthread_mutexattr_t attrP = *attr;
    
    if (attrP->magic != MAGIC_VALID)
        return EINVAL;

    if (policy == NULL)
        return EINVAL;

    if (prioceiling < policy->priority_min || 
        prioceiling > policy->priority_max)
        return EINVAL;
    
    attrP->prioceiling = prioceiling;

    return OK;
    }

/*
  NAME
  
  pthread_mutexattr_getpshared - get the process-shared attribute
  pthread_mutexattr_setpshared - set the process-shared attribute

  SYNOPSIS
  
  #include <pthread.h>
  
  int pthread_mutexattr_getpshared(const pthread_mutexattr_t
         *restrict attr, int *restrict pshared);
  int pthread_mutexattr_setpshared(pthread_mutexattr_t *attr,
         int pshared); 
 
  DESCRIPTION
  
  The pthread_mutexattr_getpshared() function shall obtain the value of the 
  process-shared attribute from the attributes object referenced by attr.
  
  The pthread_mutexattr_setpshared() function shall set the process-shared 
  attribute in an initialized attributes object referenced by attr.
  
  The process-shared attribute is set to PTHREAD_PROCESS_SHARED to permit a 
  mutex to be operated upon by any thread that has access to the memory where 
  the mutex is allocated, even if the mutex is allocated in memory that is 
  shared by multiple processes. If the process-shared attribute is 
  PTHREAD_PROCESS_PRIVATE, the mutex shall only be operated upon by threads 
  created within the same process as the thread that initialized the mutex; 
  if threads of differing processes attempt to operate on such a mutex, the 
  behavior is undefined. The default value of the attribute shall be 
  PTHREAD_PROCESS_PRIVATE.
  
  The behavior is undefined if the value specified by the attr argument to 
  pthread_mutexattr_getpshared() or pthread_mutexattr_setpshared() does not 
  refer to an initialized mutex attributes object.
  
  RETURN VALUE
  
  Upon successful completion, pthread_mutexattr_setpshared() shall return zero;
  otherwise, an error number shall be returned to indicate the error.
  
  Upon successful completion, pthread_mutexattr_getpshared() shall return zero 
  and store the value of the process-shared attribute of attr into the object 
  referenced by the pshared parameter. Otherwise, an error number shall be 
  returned to indicate the error.
  
  ERRORS
  
  The pthread_mutexattr_setpshared() function may fail if:
  
  [EINVAL]
  
  The new value specified for the attribute is outside the range of legal 
  values for that attribute.
  
  These functions shall not return an error code of [EINTR].
*/
int pthread_mutexattr_getpshared
    (
    const pthread_mutexattr_t * attr, 
    int * pshared
    )
    {
    pthread_mutexattr_t attrP = *attr;
    
    if (attrP->magic != MAGIC_VALID)
        return EINVAL;

    *pshared = attrP->pshared;
    
    return OK;
    }

int pthread_mutexattr_setpshared
    (
    pthread_mutexattr_t *attr,
    int pshared
    )
    {
    pthread_mutexattr_t attrP = *attr;
    
    if (attrP->magic != MAGIC_VALID)
        return EINVAL;

    if ((pshared != PTHREAD_PROCESS_PRIVATE) &&
        (pshared != PTHREAD_PROCESS_SHARED))
        return EINVAL;
    
    attrP->pshared = pshared;
    
    return OK;
    }

/*
  NAME
  
  pthread_mutexattr_getrobust - get the mutex robust attribute
  pthread_mutexattr_setrobust - set the mutex robust attribute

  SYNOPSIS
  
  #include <pthread.h>
  
  int pthread_mutexattr_getrobust(const pthread_mutexattr_t *restrict
         attr, int *restrict robust);
  int pthread_mutexattr_setrobust(pthread_mutexattr_t *attr,
         int robust);
  
  DESCRIPTION
  
  The pthread_mutexattr_getrobust() and pthread_mutexattr_setrobust() 
  functions, respectively, shall get and set the mutex robust attribute. 
  This attribute is set in the robust parameter. Valid values for robust 
  include:
  
  PTHREAD_MUTEX_STALLED
  
  No special actions are taken if the owner of the mutex is terminated 
  while holding the mutex lock. This can lead to deadlocks if no other 
  thread can unlock the mutex.
  
  This is the default value.
  
  PTHREAD_MUTEX_ROBUST
  
  If the process containing the owning thread of a robust mutex terminates 
  while holding the mutex lock, the next thread that acquires the mutex 
  shall be notified about the termination by the return value [EOWNERDEAD] 
  from the locking function. If the owning thread of a robust mutex terminates 
  while holding the mutex lock, the next thread that acquires the mutex may 
  be notified about the termination by the return value [EOWNERDEAD]. The 
  notified thread can then attempt to mark the state protected by the mutex 
  as consistent again by a call to pthread_mutex_consistent(). After a 
  subsequent successful call to pthread_mutex_unlock(), the mutex lock shall 
  be released and can be used normally by other threads. If the mutex is 
  unlocked without a call to pthread_mutex_consistent(), it shall be in a 
  permanently unusable state and all attempts to lock the mutex shall fail 
  with the error [ENOTRECOVERABLE]. The only permissible operation on such 
  a mutex is pthread_mutex_destroy().
  
  The behavior is undefined if the value specified by the attr argument to 
  pthread_mutexattr_getrobust() or pthread_mutexattr_setrobust() does not 
  refer to an initialized mutex attributes object.
  
  RETURN VALUE
  
  Upon successful completion, the pthread_mutexattr_getrobust() function 
  shall return zero and store the value of the robust attribute of attr 
  into the object referenced by the robust parameter. Otherwise, an error 
  value shall be returned to indicate the error. If successful, the 
  pthread_mutexattr_setrobust() function shall return zero; otherwise, an 
  error number shall be returned to indicate the error.
  
  ERRORS
  
  The pthread_mutexattr_setrobust() function shall fail if:
  
  [EINVAL]
  
  The value of robust is invalid.
  
  These functions shall not return an error code of [EINTR].
*/
int pthread_mutexattr_getrobust
    (
    const pthread_mutexattr_t * attr, 
    int * robust
    )
    {
    pthread_mutexattr_t attrP = *attr;
    
    if (attrP->magic != MAGIC_VALID)
        return EINVAL;

    *robust = attrP->robust;

    return OK;
    }

int pthread_mutexattr_setrobust
    (
    pthread_mutexattr_t *attr,
    int robust
    )
    {
    pthread_mutexattr_t attrP = *attr;
    
    if (attrP->magic != MAGIC_VALID)
        return EINVAL;

    if ((robust != PTHREAD_MUTEX_STALLED) &&
        (robust != PTHREAD_MUTEX_ROBUST))
        return EINVAL;
    
    attrP->robust = robust;
    
    return OK;
    }
/*
  NAME
  
  pthread_mutexattr_gettype - get the mutex type attribute
  pthread_mutexattr_settype - set the mutex type attribute

  SYNOPSIS
  
  #include <pthread.h>
  
  int pthread_mutexattr_gettype(const pthread_mutexattr_t *restrict attr,
         int *restrict type);
  int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type);
  
  DESCRIPTION
  
  The pthread_mutexattr_gettype() and pthread_mutexattr_settype() functions, 
  respectively, shall get and set the mutex type attribute. This attribute 
  is set in the type parameter to these functions. The default value of the 
  type attribute is PTHREAD_MUTEX_DEFAULT.
  
  The type of mutex is contained in the type attribute of the mutex attributes.
  Valid mutex types include:
  
  PTHREAD_MUTEX_NORMAL
  
  This type of mutex does not detect deadlock. A thread attempting to relock 
  this mutex without first unlocking it shall deadlock. Attempting to unlock 
  a mutex locked by a different thread results in undefined behavior. 

  Attempting to unlock an unlocked mutex results in undefined behavior.

  PTHREAD_MUTEX_ERRORCHECK
  
  This type of mutex provides error checking. A thread attempting to relock 
  this mutex without first unlocking it shall return with an error. A thread 
  attempting to unlock a mutex which another thread has locked shall return 
  with an error. A thread attempting to unlock an unlocked mutex shall return 
  with an error.

  PTHREAD_MUTEX_RECURSIVE
  
  A thread attempting to relock this mutex without first unlocking it shall 
  succeed in locking the mutex. The relocking deadlock which can occur with 
  mutexes of type PTHREAD_MUTEX_NORMAL cannot occur with this type of mutex. 
  Multiple locks of this mutex shall require the same number of unlocks to 
  release the mutex before another thread can acquire the mutex. A thread 
  attempting to unlock a mutex which another thread has locked shall return 
  with an error. A thread attempting to unlock an unlocked mutex shall return 
  with an error.
  
  PTHREAD_MUTEX_DEFAULT
  
  Attempting to recursively lock a mutex of this type results in undefined 
  behavior. Attempting to unlock a mutex of this type which was not locked 
  by the calling thread results in undefined behavior. Attempting to unlock 
  a mutex of this type which is not locked results in undefined behavior. 
  An implementation may map this mutex to one of the other mutex types.
  
  The behavior is undefined if the value specified by the attr argument to 
  pthread_mutexattr_gettype() or pthread_mutexattr_settype() does not refer 
  to an initialized mutex attributes object.
  
  RETURN VALUE
  
  Upon successful completion, the pthread_mutexattr_gettype() function shall 
  return zero and store the value of the type attribute of attr into the 
  object referenced by the type parameter. Otherwise, an error shall be 
  returned to indicate the error.
  
  If successful, the pthread_mutexattr_settype() function shall return zero; 
  otherwise, an error number shall be returned to indicate the error.
  
  ERRORS
  
  The pthread_mutexattr_settype() function shall fail if:
  
  [EINVAL]
  
  The value type is invalid.
  
  These functions shall not return an error code of [EINTR].
*/
int pthread_mutexattr_gettype
    (
    const pthread_mutexattr_t * attr,
    int * type
    )
    {
    pthread_mutexattr_t attrP = *attr;
    
    if (attrP->magic != MAGIC_VALID)
        return EINVAL;

    *type = attrP->type;
    
    return OK;
    }

int pthread_mutexattr_settype
    (
    pthread_mutexattr_t *attr, 
    int type
    )
    {
    pthread_mutexattr_t attrP = *attr;
    
    if (attrP->magic != MAGIC_VALID)
        return EINVAL;

    if ((type != PTHREAD_MUTEX_DEFAULT) &&
        (type != PTHREAD_MUTEX_NORMAL) &&
        (type != PTHREAD_MUTEX_ERRORCHECK) &&
        (type != PTHREAD_MUTEX_RECURSIVE) &&
        (type != PTHREAD_MUTEX_ADAPTIVE))
        return EINVAL;
    
    attrP->type = type;
    
    return OK;
    }

