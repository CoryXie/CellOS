/* semaphore.c - XBD semaphore operations */

#include <sys.h>
#include <arch.h>
#include <os.h>
#include <semaphore.h>

static list_t           semaphore_list;
static spinlock_t       semaphore_lock;
static id_t             semaphore_id_next;

void semaphore_system_init(void)
    {
    list_init(&semaphore_list);
    spinlock_init(&semaphore_lock);
    semaphore_id_next = 0;
    }

/*
  NAME
  
  sem_init - initialize an unnamed semaphore
  SYNOPSIS
  
  #include <semaphore.h>
  
  int sem_init(sem_t *sem, int pshared, unsigned value);
  
  DESCRIPTION
  
  The sem_init() function shall initialize the unnamed semaphore referred to
  by sem. The value of the initialized semaphore shall be value. Following a 
  successful call to sem_init(), the semaphore may be used in subsequent calls 
  to sem_wait(), sem_timedwait(), sem_trywait(), sem_post(), and sem_destroy(). 
  This semaphore shall remain usable until the semaphore is destroyed.
  
  If the pshared argument has a non-zero value, then the semaphore is shared 
  between processes; in this case, any process that can access the semaphore 
  sem can use sem for performing sem_wait(), sem_timedwait(), sem_trywait(), 
  sem_post(), and sem_destroy() operations.
  
  Only sem itself may be used for performing synchronization. The result of 
  referring to copies of sem in calls to sem_wait(), sem_timedwait(), 
  sem_trywait(), sem_post(), and sem_destroy() is undefined.
  
  If the pshared argument is zero, then the semaphore is shared between threads
  of the process; any thread in this process can use sem for performing 
  sem_wait(), sem_timedwait(), sem_trywait(), sem_post(), and sem_destroy() 
  operations. The use of the semaphore by threads other than those created in 
  the same process is undefined.
  
  Attempting to initialize an already initialized semaphore results in 
  undefined behavior.
  
  RETURN VALUE
  
  Upon successful completion, the sem_init() function shall initialize the 
  semaphore in sem and return 0. Otherwise, it shall return -1 and set errno 
  to indicate the error.
  
  ERRORS
  
  The sem_init() function shall fail if:
  
  [EINVAL]
  
  The value argument exceeds {SEM_VALUE_MAX}.
  
  [ENOSPC]
  
  A resource required to initialize the semaphore has been exhausted, or the 
  limit on semaphores ( {SEM_NSEMS_MAX}) has been reached.
  
  [EPERM]
  
  The process lacks appropriate privileges to initialize the semaphore.
  
*/
int sem_init(sem_t *sem, int pshared, unsigned value)
    {
    if (!sem)
        {
        kurrent->err = EINVAL;
        
        return ERROR;
        }

    if (value > SEM_VALUE_MAX)
        {
        kurrent->err = EINVAL;
        
        return ERROR;
        }

    memset(sem, 0, sizeof(sem_t));
    
    spinlock_init(&sem->lock);

    queue_init(&sem->waitq, "SEMQUEUE", FALSE);

    sem->count = value;

    sprintf(sem->name, "NONAMESEM");
    
    spinlock_lock(&semaphore_lock);

    list_append(&semaphore_list,&sem->node);

    sem->id = semaphore_id_next++;
    
    spinlock_unlock(&semaphore_lock);

    sem->magic = MAGIC_VALID;

    return OK;
    }

/*
  NAME
  
  sem_destroy - destroy an unnamed semaphore
  
  SYNOPSIS
  
  #include <semaphore.h>
  
  int sem_destroy(sem_t *sem);
  
  DESCRIPTION
  
  The sem_destroy() function shall destroy the unnamed semaphore indicated
  by sem. Only a semaphore that was created using sem_init() may be destroyed 
  using sem_destroy(); the effect of calling sem_destroy() with a named 
  semaphore is undefined. The effect of subsequent use of the semaphore sem 
  is undefined until sem is reinitialized by another call to sem_init().
  
  It is safe to destroy an initialized semaphore upon which no threads are 
  currently blocked. The effect of destroying a semaphore upon which other 
  threads are currently blocked is undefined.
  
  RETURN VALUE
  
  Upon successful completion, a value of zero shall be returned. Otherwise, 
  a value of -1 shall be returned and errno set to indicate the error.
  
  ERRORS
  
  The sem_destroy() function may fail if:
  
  [EINVAL]
  
  The sem argument is not a valid semaphore.
  
  [EBUSY]
  
  There are currently processes blocked on the semaphore.
*/

int sem_destroy(sem_t *sem)
    {
    if (!sem || sem->magic != MAGIC_VALID || LIST_EMPTY(&sem->node))
        {
        kurrent->err = EINVAL;
        
        return ERROR;
        }
    
    spinlock_lock(&sem->lock);

    if ((sem->count > 0) || (sem->best_waiter == NULL))
        {
        list_remove(&sem->node);

        sem->magic = MAGIC_INVALID;
        
        spinlock_unlock(&sem->lock);

        return OK;
        }
    
    kurrent->err = EBUSY;
    
    spinlock_unlock(&sem->lock);
    
    return ERROR;
    }

/*
  NAME
  
  sem_open - initialize and open a named semaphore
  SYNOPSIS
  
  #include <semaphore.h>
  
  sem_t *sem_open(const char *name, int oflag, ...);
  
  DESCRIPTION
  
  The sem_open() function shall establish a connection between a named 
  semaphore and a process. Following a call to sem_open() with semaphore 
  name name, the process may reference the semaphore associated with name 
  using the address returned from the call. This semaphore may be used in 
  subsequent calls to sem_wait(), sem_timedwait(), sem_trywait(), sem_post(),
  and sem_close(). The semaphore remains usable by this process until the 
  semaphore is closed by a successful call to sem_close(), _exit(), or one 
  of the exec functions.
  
  The oflag argument controls whether the semaphore is created or merely 
  accessed by the call to sem_open(). The following flag bits may be set 
  in oflag:
  
  O_CREAT
  
  This flag is used to create a semaphore if it does not already exist. If 
  O_CREAT is set and the semaphore already exists, then O_CREAT has no effect, 
  except as noted under O_EXCL. Otherwise, sem_open() creates a named semaphore.
  The O_CREAT flag requires a third and a fourth argument: mode, which is of 
  type mode_t, and value, which is of type unsigned. The semaphore is created 
  with an initial value of value. Valid initial values for semaphores are less
  than or equal to {SEM_VALUE_MAX}.
  
  The user ID of the semaphore shall be set to the effective user ID of the 
  process. The group ID of the semaphore shall be set to the effective group 
  ID of the process; however, if the name argument is visible in the file 
  system, the group ID may be set to the group ID of the containing directory. 
  The permission bits of the semaphore are set to the value of the mode 
  argument except those set in the file mode creation mask of the process. 
  When bits in mode other than the file permission bits are specified, the 
  effect is unspecified.
  
  After the semaphore named name has been created by sem_open() with the 
  O_CREAT flag, other processes can connect to the semaphore by calling 
  sem_open() with the same value of name.
  
  O_EXCL
  
  If O_EXCL and O_CREAT are set, sem_open() fails if the semaphore name 
  exists. The check for the existence of the semaphore and the creation of 
  the semaphore if it does not exist are atomic with respect to other 
  processes executing sem_open() with O_EXCL and O_CREAT set. If O_EXCL is
  set and O_CREAT is not set, the effect is undefined.
  
  If flags other than O_CREAT and O_EXCL are specified in the oflag parameter, 
  the effect is unspecified.
  
  The name argument points to a string naming a semaphore object. It is 
  unspecified whether the name appears in the file system and is visible to
  functions that take pathnames as arguments. The name argument conforms to 
  the construction rules for a pathname, except that the interpretation of
  <slash> characters other than the leading <slash> character in name is 
  implementation-defined, and that the length limits for the name argument 
  are implementation-defined and need not be the same as the pathname limits 
  {PATH_MAX} and {NAME_MAX}. If name begins with the <slash> character, then
  processes calling sem_open() with the same value of name shall refer to 
  the same semaphore object, as long as that name has not been removed. If 
  name does not begin with the <slash> character, the effect is 
  implementation-defined.
  
  If a process makes multiple successful calls to sem_open() with the same
  value for name, the same semaphore address shall be returned for each such 
  successful call, provided that there have been no calls to sem_unlink() 
  for this semaphore, and at least one previous successful sem_open() call 
  for this semaphore has not been matched with a sem_close() call.
  
  References to copies of the semaphore produce undefined results.
  
  RETURN VALUE
  
  Upon successful completion, the sem_open() function shall return the 
  address of the semaphore. Otherwise, it shall return a value of SEM_FAILED 
  and set errno to indicate the error. The symbol SEM_FAILED is defined in 
  the <semaphore.h> header. No successful return from sem_open() shall return
  the value SEM_FAILED.
  
  ERRORS
  
  If any of the following conditions occur, the sem_open() function shall 
  return SEM_FAILED and set errno to the corresponding value:
  
  [EACCES]
  
  The named semaphore exists and the permissions specified by oflag are denied, 
  or the named semaphore does not exist and permission to create the named 
  semaphore is denied.
  
  [EEXIST]
  
  O_CREAT and O_EXCL are set and the named semaphore already exists.
  
  [EINTR]
  
  The sem_open() operation was interrupted by a signal.
  
  [EINVAL]
  
  The sem_open() operation is not supported for the given name, or O_CREAT
  was specified in oflag and value was greater than {SEM_VALUE_MAX}.
  
  [EMFILE]
  
  Too many semaphore descriptors or file descriptors are currently in use 
  by this process.
  
  [ENFILE]
  
  Too many semaphores are currently open in the system.
  
  [ENOENT]
  
  O_CREAT is not set and the named semaphore does not exist.
  
  [ENOMEM]
  
  There is insufficient memory for the creation of the new named semaphore.
  
  [ENOSPC]
  
  There is insufficient space on a storage device for the creation of the 
  new named semaphore.
  
  If any of the following conditions occur, the sem_open() function may 
  return SEM_FAILED and set errno to the corresponding value:
  
  [ENAMETOOLONG]
  
  The length of the name argument exceeds {_POSIX_PATH_MAX} on systems that 
  do not support the XSI option or exceeds {_XOPEN_PATH_MAX} on XSI 
  systems, or has a pathname component that is longer than {_POSIX_NAME_MAX} 
  on systems that do not support the XSI option [XSI]   or longer than 
  {_XOPEN_NAME_MAX} on XSI systems. 
  
*/
sem_t *sem_open(const char *name, int oflag, ...)
    {
    return SEM_FAILED;
    }

/*
  NAME
  
  sem_close - close a named semaphore
  
  SYNOPSIS
  
  #include <semaphore.h>
  
  int sem_close(sem_t *sem);
  
  DESCRIPTION
  
  The sem_close() function shall indicate that the calling process is finished
  using the named semaphore indicated by sem. The effects of calling sem_close()
  for an unnamed semaphore (one created by sem_init()) are undefined. The 
  sem_close() function shall deallocate (that is, make available for reuse by 
  a subsequent sem_open() by this process) any system resources allocated by 
  the system for use by this process for this semaphore. The effect of 
  subsequent use of the semaphore indicated by sem by this process is undefined.
  If the semaphore has not been removed with a successful call to sem_unlink(), 
  then sem_close() has no effect on the state of the semaphore. If the 
  sem_unlink() function has been successfully invoked for name after the most 
  recent call to sem_open() with O_CREAT for this semaphore, then when all 
  processes that have opened the semaphore close it, the semaphore is no 
  longer accessible.
  
  RETURN VALUE
  
  Upon successful completion, a value of zero shall be returned. Otherwise,
  a value of -1 shall be returned and errno set to indicate the error.
  
  ERRORS
  
  The sem_close() function may fail if:
  
  [EINVAL]
  
  The sem argument is not a valid semaphore descriptor.
*/

int sem_close(sem_t *sem)
    {
    return OK;
    }

/*
  NAME
  
  sem_post - unlock a semaphore
  SYNOPSIS
  
  #include <semaphore.h>
  
  int sem_post(sem_t *sem);
  
  DESCRIPTION
  
  The sem_post() function shall unlock the semaphore referenced by sem by 
  performing a semaphore unlock operation on that semaphore.
  
  If the semaphore value resulting from this operation is positive, then no
  threads were blocked waiting for the semaphore to become unlocked; the 
  semaphore value is simply incremented.
  
  If the value of the semaphore resulting from this operation is zero, then 
  one of the threads blocked waiting for the semaphore shall be allowed to 
  return successfully from its call to sem_wait(). [PS]   If the Process 
  Scheduling option is supported, the thread to be unblocked shall be chosen 
  in a manner appropriate to the scheduling policies and parameters in effect 
  for the blocked threads. In the case of the schedulers SCHED_FIFO and 
  SCHED_RR, the highest priority waiting thread shall be unblocked, and if 
  there is more than one highest priority thread blocked waiting for the 
  semaphore, then the highest priority thread that has been waiting the 
  longest shall be unblocked. If the Process Scheduling option is not defined,
  the choice of a thread to unblock is unspecified. 
  
  If the Process Sporadic Server option is supported, and the scheduling
  policy is SCHED_SPORADIC, the semantics are as per SCHED_FIFO above. 
  
  The sem_post() function shall be async-signal-safe and may be invoked from
  a signal-catching function.
  
  RETURN VALUE
  
  If successful, the sem_post() function shall return zero; otherwise, the 
  function shall return -1 and set errno to indicate the error.
  
  ERRORS
  
  The sem_post() function may fail if:
  
  [EINVAL]
  
  The sem argument does not refer to a valid semaphore.
*/

int sem_post(sem_t *sem)
    {
    pthread_t wake_thread = NULL;
    pthread_t next_thread = NULL;
    pthread_t better_thread = NULL;

    if (!sem || sem->magic != MAGIC_VALID || LIST_EMPTY(&sem->node))
        {
        kurrent->err = EINVAL;
        
        return ERROR;
        }

    spinlock_lock(&sem->lock);
    
    sem->count++;

    if (sem->count > 0)
        {
        spinlock_unlock(&sem->lock);
        
        return OK;
        }
        
    if (sem->best_waiter)
        {
        wake_thread = sem->best_waiter;

        queue_remove(&wake_thread->waitq_node, FALSE);
        
        QUEUE_ITERATE(&sem->waitq, iter)
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
        sem->best_waiter = better_thread;

#ifdef SEMAPHORE_DETAL
        printk("sem wakeup thread %s\n", wake_thread->name);
#endif
        
        wake_thread->state = STATE_READY;
        
        wake_thread->sched_policy->thread_enqueue(wake_thread->sched_runq, 
                                                  wake_thread, TRUE);
        
        reschedule();
        }
    
    spinlock_unlock(&sem->lock);

    return OK;
    }

/*
  NAME
  
  sem_timedwait - lock a semaphore
  
  SYNOPSIS
  
  #include <semaphore.h>
  #include <time.h>
  
  int sem_timedwait(sem_t *restrict sem,
         const struct timespec *restrict abstime);
  
  DESCRIPTION
  
  The sem_timedwait() function shall lock the semaphore referenced by sem as
  in the sem_wait() function. However, if the semaphore cannot be locked 
  without waiting for another process or thread to unlock the semaphore by 
  performing a sem_post() function, this wait shall be terminated when the 
  specified timeout expires.
  
  The timeout shall expire when the absolute time specified by abstime passes, 
  as measured by the clock on which timeouts are based (that is, when the 
  value of that clock equals or exceeds abstime), or if the absolute time 
  specified by abstime has already been passed at the time of the call.
  
  The timeout shall be based on the CLOCK_REALTIME clock. The resolution of 
  the timeout shall be the resolution of the clock on which it is based. The 
  timespec data type is defined as a structure in the <time.h> header.
  
  Under no circumstance shall the function fail with a timeout if the semaphore
  can be locked immediately. The validity of the abstime need not be checked 
  if the semaphore can be locked immediately.
  
  RETURN VALUE
  
  The sem_timedwait() function shall return zero if the calling process 
  successfully performed the semaphore lock operation on the semaphore 
  designated by sem. If the call was unsuccessful, the state of the semaphore
  shall be unchanged, and the function shall return a value of -1 and set 
  errno to indicate the error.
  
  ERRORS
  
  The sem_timedwait() function shall fail if:
  
  [EINVAL]
  
  The process or thread would have blocked, and the abstime parameter 
  specified a nanoseconds field value less than zero or greater than or 
  equal to 1000 million.
  
  [ETIMEDOUT]
  
  The semaphore could not be locked before the specified timeout expired.
  The sem_timedwait() function may fail if:
  
  [EDEADLK]
  
  A deadlock condition was detected.
  
  [EINTR]
  
  A signal interrupted this function.
  
  [EINVAL]
  
  The sem argument does not refer to a valid semaphore.
*/

int sem_timedwait(sem_t * sem, const struct timespec * abstime)
    {    
    return sem_wait(sem);
    }

/*
  NAME
  
  sem_trywait, sem_wait - lock a semaphore
  SYNOPSIS
  
  #include <semaphore.h>
  
  int sem_trywait(sem_t *sem);
  int sem_wait(sem_t *sem);
  
  DESCRIPTION
  
  The sem_trywait() function shall lock the semaphore referenced by sem only 
  if the semaphore is currently not locked; that is, if the semaphore value 
  is currently positive. Otherwise, it shall not lock the semaphore.
  
  The sem_wait() function shall lock the semaphore referenced by sem by 
  performing a semaphore lock operation on that semaphore. If the semaphore 
  value is currently zero, then the calling thread shall not return from 
  the call to sem_wait() until it either locks the semaphore or the call is 
  interrupted by a signal.
  
  Upon successful return, the state of the semaphore shall be locked and 
  shall remain locked until the sem_post() function is executed and returns 
  successfully.
  
  The sem_wait() function is interruptible by the delivery of a signal.
  
  RETURN VALUE
  
  The sem_trywait() and sem_wait() functions shall return zero if the calling 
  process successfully performed the semaphore lock operation on the semaphore 
  designated by sem. If the call was unsuccessful, the state of the semaphore
  shall be unchanged, and the function shall return a value of -1 and set 
  errno to indicate the error.
  
  ERRORS
  
  The sem_trywait() function shall fail if:
  
  [EAGAIN]
  
  The semaphore was already locked, so it cannot be immediately locked by 
  the sem_trywait() operation.
  
  The sem_trywait() and sem_wait() functions may fail if:
  
  [EDEADLK]
  
  A deadlock condition was detected.
  
  [EINTR]
  
  A signal interrupted this function.
  
  [EINVAL]
  
  The sem argument does not refer to a valid semaphore.
*/

int sem_trywait(sem_t *sem)
    {
    if (!sem || sem->magic != MAGIC_VALID || LIST_EMPTY(&sem->node))
        {
        kurrent->err = EINVAL;
        
        return ERROR;
        }
    
    spinlock_lock(&sem->lock);

    if (sem->count > 0)
        {
        sem->count--;

        spinlock_unlock(&sem->lock);

        return OK;
        }

    spinlock_unlock(&sem->lock);

    kurrent->err = EAGAIN;

    return ERROR;
    }

int sem_wait(sem_t *sem)
    {
    BOOL better;

    if (!sem || sem->magic != MAGIC_VALID || LIST_EMPTY(&sem->node))
        {
        kurrent->err = EINVAL;
        
        return ERROR;
        }
    
    spinlock_lock(&sem->lock);

    if (sem->count > 0)
        {
        sem->count--;

        spinlock_unlock(&sem->lock);

        return OK;
        }

#ifdef SEMAPHORE_DETAL
    printk("sem %s pend thread %s\n", sem->name, kurrent->name);
#endif

    /* Add the current thread to the waitq */  
    enqueue(&sem->waitq, &kurrent->waitq_node, FALSE);
    
    /* If there is no other waiting, caller is the best! */
    if (sem->best_waiter == NULL)
        {
        sem->best_waiter = kurrent;
        }
    else
        {
        better = SCHED_THREAD_PRECEDENCE_COMPARE(kurrent, 
                                        sem->best_waiter);
        if (better)
            sem->best_waiter = kurrent;
        }

    /* Set the current self_thread as pending on mutexP */
    kurrent->state = STATE_PENDING;

    spinlock_unlock(&sem->lock);

    reschedule();
    
    return OK;
    }

/*
  NAME
  
  sem_unlink - remove a named semaphore
  
  SYNOPSIS
  
  #include <semaphore.h>
  
  int sem_unlink(const char *name);
  
  DESCRIPTION
  
  The sem_unlink() function shall remove the semaphore named by the string 
  name. If the semaphore named by name is currently referenced by other 
  processes, then sem_unlink() shall have no effect on the state of the 
  semaphore. If one or more processes have the semaphore open when sem_unlink() 
  is called, destruction of the semaphore is postponed until all references 
  to the semaphore have been destroyed by calls to sem_close(), _exit(), or 
  exec. Calls to sem_open() to recreate or reconnect to the semaphore refer 
  to a new semaphore after sem_unlink() is called. The sem_unlink() call 
  shall not block until all references have been destroyed; it shall return 
  immediately.
  
  RETURN VALUE
  
  Upon successful completion, the sem_unlink() function shall return a value 
  of 0. Otherwise, the semaphore shall not be changed and the function shall 
  return a value of -1 and set errno to indicate the error.
  
  ERRORS
  
  The sem_unlink() function shall fail if:
  
  [EACCES]
  
  Permission is denied to unlink the named semaphore.
  
  [ENOENT]
  
  The named semaphore does not exist.
  
  The sem_unlink() function may fail if:
  
  [ENAMETOOLONG]
  
  The length of the name argument exceeds {_POSIX_PATH_MAX} on systems that
  do not support the XSI option [XSI]   or exceeds {_XOPEN_PATH_MAX} on XSI 
  systems,   or has a pathname component that is longer than {_POSIX_NAME_MAX} 
  on systems that do not support the XSI option [XSI]   or longer than
  {_XOPEN_NAME_MAX} on XSI systems.   A call to sem_unlink() with a name 
  argument that contains the same semaphore name as was previously used in 
  a successful sem_open() call shall not give an [ENAMETOOLONG] erro
*/

int sem_unlink(const char *name)
    {
    return OK;
    }

