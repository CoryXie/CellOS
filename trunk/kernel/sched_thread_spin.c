/* sched_thread_spin.c - pthread spin lock managment */

#include <sys.h>
#include <arch.h>
#include <sched.h>
#include <pthread.h>
#include <os/sched_core.h>

/* 
 * NAME
 *
 * pthread_spin_init - initialize a spin lock object
 * pthread_spin_destroy - destroy a spin lock object
 *
 * SYNOPSIS
 *
 * #include <pthread.h>
 *
 * int pthread_spin_destroy(pthread_spinlock_t *lock);
 * int pthread_spin_init(pthread_spinlock_t *lock, int pshared);
 *
 * DESCRIPTION
 *
 * The pthread_spin_destroy() function shall destroy the spin lock 
 * referenced by lock and release any resources used by the lock. 
 * The effect of subsequent use of the lock is undefined until the
 * lock is reinitialized by another call to pthread_spin_init(). 
 * The results are undefined if pthread_spin_destroy() is called 
 * when a thread holds the lock, or if this function is called with
 * an uninitialized thread spin lock.
 *
 * The pthread_spin_init() function shall allocate any resources 
 * required to use the spin lock referenced by lock and initialize 
 * the lock to an unlocked state.
 *
 * [TSH]  If the Thread Process-Shared Synchronization option is 
 * supported and the value of pshared is PTHREAD_PROCESS_SHARED, 
 * the implementation shall permit the spin lock to be operated 
 * upon by any thread that has access to the memory where the spin
 * lock is allocated, even if it is allocated in memory that is 
 * shared by multiple processes.
 *
 * If the Thread Process-Shared Synchronization option is supported 
 * and the value of pshared is PTHREAD_PROCESS_PRIVATE,  or if the 
 * option is not supported, the spin lock shall only be operated 
 * upon by threads created within the same process as the thread 
 * that initialized the spin lock. If threads of differing processes
 * attempt to operate on such a spin lock, the behavior is undefined.
 *
 * The results are undefined if pthread_spin_init() is called specifying
 * an already initialized spin lock. The results are undefined if a 
 * spin lock is used without first being initialized.
 *
 * If the pthread_spin_init() function fails, the lock is not initialized 
 * and the contents of lock are undefined.
 *
 * Only the object referenced by lock may be used for performing 
 * synchronization.
 *
 * The result of referring to copies of that object in calls to 
 * pthread_spin_destroy(), pthread_spin_lock(), pthread_spin_trylock(), 
 * or pthread_spin_unlock() is undefined.
 *
 * RETURN VALUE
 *
 * Upon successful completion, these functions shall return zero; otherwise,
 * an error number shall be returned to indicate the error.
 *
 * ERRORS
 *
 * The pthread_spin_init() function shall fail if:
 *
 * [EAGAIN]
 *      The system lacks the necessary resources to initialize another 
 *      spin lock.
 *
 * [ENOMEM]
 *      Insufficient memory exists to initialize the lock.
 *
 * These functions shall not return an error code of [EINTR].
 */
int pthread_spin_destroy
    (
    pthread_spinlock_t *lock
    )
    {
	return OK;
    }

/* 
 * pthread_spin_init - initialize a spin lock object
 *
 * SEE pthread_spin_destroy() for more information.
 */
int pthread_spin_init
    (
    pthread_spinlock_t *lock, 
    int pshared
    )
    {
	spinlock_init(&lock->lock);
    lock->flags = 0;
	return OK;
    }

/* 
 * NAME
 *
 * pthread_spin_lock, pthread_spin_trylock - lock a spin lock object
 *
 * SYNOPSIS
 *
 * #include <pthread.h>
 *
 * int pthread_spin_lock(pthread_spinlock_t *lock);
 * int pthread_spin_trylock(pthread_spinlock_t *lock);
 *
 * DESCRIPTION
 *
 * The pthread_spin_lock() function shall lock the spin lock referenced
 * by lock. The calling thread shall acquire the lock if it is not held 
 * by another thread. Otherwise, the thread shall spin (that is, shall 
 * not return from the pthread_spin_lock() call) until the lock becomes
 * available. The results are undefined if the calling thread holds the 
 * lock at the time the call is made. The pthread_spin_trylock() function
 * shall lock the spin lock referenced by lock if it is not held by any
 * thread. Otherwise, the function shall fail.
 *
 * The results are undefined if any of these functions is called with an
 * uninitialized spin lock.
 *
 * RETURN VALUE
 *
 * Upon successful completion, these functions shall return zero; otherwise,
 * an error number shall be returned to indicate the error.
 *
 * ERRORS
 *
 * The pthread_spin_lock() function may fail if:
 *
 * [EDEADLK]
 *      A deadlock condition was detected.
 *
 * The pthread_spin_trylock() function shall fail if:
 *
 * [EBUSY]
 *      A thread currently holds the lock.
 *
 * These functions shall not return an error code of [EINTR].
 */
int pthread_spin_lock
    (
    pthread_spinlock_t *lock
    )
    {
	ipl_t flags;
    
	flags = interrupts_disable();
    
	spinlock_lock (&lock->lock);
    
	lock->flags = flags;
    
	return OK;
    }

/* 
 * pthread_spin_trylock - lock a spin lock object
 *
 * SEE pthread_spin_lock() for more infromation.
 */
int pthread_spin_trylock
    (
    pthread_spinlock_t *lock
    )
    {
	ipl_t flags;
    
	flags = interrupts_disable();
    
	if (spinlock_trylock(&lock->lock)) 
        {
		interrupts_restore(flags);
        
		return EBUSY;
	    } 
    else 
        {
		lock->flags = flags;
		return OK;
	    }
    }

/* 
 * NAME
 *
 * pthread_spin_unlock - unlock a spin lock object
 *
 * SYNOPSIS
 *
 * #include <pthread.h>
 *
 * int pthread_spin_unlock(pthread_spinlock_t *lock);
 *
 * DESCRIPTION
 *
 * The pthread_spin_unlock() function shall release the spin lock 
 * referenced by lock which was locked via the pthread_spin_lock() or
 * pthread_spin_trylock() functions.
 *
 * The results are undefined if the lock is not held by the calling thread.
 *
 * If there are threads spinning on the lock when pthread_spin_unlock() 
 * is called, the lock becomes available and an unspecified spinning thread 
 * shall acquire the lock.
 *
 * The results are undefined if this function is called with an uninitialized 
 * thread spin lock.
 *
 * RETURN VALUE
 *
 * Upon successful completion, the pthread_spin_unlock() function shall
 * return zero; otherwise, an error number shall be returned to indicate 
 * the error.
 *
 * ERRORS
 *
 * This function shall not return an error code of [EINTR].
 */
int pthread_spin_unlock
    (
    pthread_spinlock_t *lock
    )
    {
	ipl_t flags;
    
	flags = lock->flags;
    
	spinlock_unlock(&lock->lock);
    
	interrupts_restore(flags);
    
	return OK;
    }
