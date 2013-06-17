/* timer.c - timer management */

#include <sys.h>
#include <arch.h>
#include <time.h>
#include <os/timer.h>

struct timerchain_head global_interval_timerchain;

/*
  NAME
  
  timer_create - create a per-process timer
  
  SYNOPSIS
  
  #include <signal.h>
  #include <time.h>
  
  int timer_create(clockid_t clockid, struct sigevent *restrict evp,
         timer_t *restrict timerid); 
  
  DESCRIPTION
  
  The timer_create() function shall create a per-process timer using the 
  specified clock, clock_id, as the timing base. The timer_create() function 
  shall return, in the location referenced by timerid, a timer ID of type
  timer_t used to identify the timer in timer requests. This timer ID shall
  be unique within the calling process until the timer is deleted. The 
  particular clock, clock_id, is defined in <time.h>. The timer whose ID is 
  returned shall be in a disarmed state upon return from timer_create().
  
  The evp argument, if non-NULL, points to a sigevent structure. This structure,
  allocated by the application, defines the asynchronous notification to occur
  as specified in Signal Generation and Delivery when the timer expires. If 
  the evp argument is NULL, the effect is as if the evp argument pointed to a 
  sigevent structure with the sigev_notify member having the value SIGEV_SIGNAL, 
  the sigev_signo having a default signal number, and the sigev_value member 
  having the value of the timer ID.
  
  Each implementation shall define a set of clocks that can be used as timing 
  bases for per-process timers. All implementations shall support a clock_id 
  of CLOCK_REALTIME. If Monotonic Clock option is supported, implementations 
  shall support a clock_id of CLOCK_MONOTONIC. 
  
  Per-process timers shall not be inherited by a child process across a fork() 
  and shall be disarmed and deleted by an exec.
  
  - If _POSIX_CPUTIME is defined, implementations shall support clock_id 
  values representing the CPU-time clock of the calling process. 
  
  - If _POSIX_THREAD_CPUTIME is defined, implementations shall support
  clock_id values representing the CPU-time clock of the calling thread. 
  
  - It is implementation-defined whether a timer_create() function will succeed
  if the value defined by clock_id corresponds to the CPU-time clock of a 
  process or thread different from the process or thread invoking the function. 
  
  - If evp->sigev_sigev_notify is SIGEV_THREAD and sev->sigev_notify_attributes 
  is not NULL, if the attribute pointed to by sev->sigev_notify_attributes has
  a thread stack address specified by a call to pthread_attr_setstack(), the 
  results are unspecified if the signal is generated more than once. 
  
  RETURN VALUE
  
  If the call succeeds, timer_create() shall return zero and update the location
  referenced by timerid to a timer_t, which can be passed to the per-process 
  timer calls. If an error occurs, the function shall return a value of -1 and 
  set errno to indicate the error. The value of timerid is undefined if an
  error occurs.
  
  ERRORS
  
  The timer_create() function shall fail if:
  
  [EAGAIN]
  
  The system lacks sufficient signal queuing resources to honor the request.
  
  [EAGAIN]
  
  The calling process has already created all of the timers it is allowed by 
  this implementation.
  
  [EINVAL]
  
  The specified clock ID is not defined.
  
  [ENOTSUP]
  
  The implementation does not support the creation of a timer attached to the
  CPU-time clock that is specified by clock_id and associated with a process 
  or thread different from the process or thread invoking timer_create(). 
*/

int timer_create(clockid_t clockid, struct sigevent * evp, timer_t * timerid)
    {

    return OK;
    }

/*
  NAME
  
  timer_delete - delete a per-process timer
  
  SYNOPSIS
  
  #include <time.h>
  
  int timer_delete(timer_t timerid); 
  
  DESCRIPTION
  
  The timer_delete() function deletes the specified timer, timerid, 
  previously created by the timer_create() function. If the timer is 
  armed when timer_delete() is called, the behavior shall be as if the 
  timer is automatically disarmed before removal. The disposition of 
  pending signals for the deleted timer is unspecified.
  
  RETURN VALUE
  
  If successful, the timer_delete() function shall return a value of zero. 
  Otherwise, the function shall return a value of -1 and set errno to 
  indicate the error.
  
  ERRORS
  
  The timer_delete() function may fail if:
  
  [EINVAL]
  
  The timer ID specified by timerid is not a valid timer ID.
*/

int timer_delete(timer_t timerid)
    {
    return OK;
    }

/*
  NAME
  
  timer_getoverrun, timer_gettime, timer_settime - per-process timers
  
  SYNOPSIS
  
  #include <time.h>
  
  int timer_getoverrun(timer_t timerid);
  int timer_gettime(timer_t timerid, struct itimerspec *value);
  int timer_settime(timer_t timerid, int flags,
         const struct itimerspec *restrict value,
         struct itimerspec *restrict ovalue); 
  
  DESCRIPTION
  
  The timer_gettime() function shall store the amount of time until the 
  specified timer, timerid, expires and the reload value of the timer into
  the space pointed to by the value argument. The it_value member of this
  structure shall contain the amount of time before the timer expires, or 
  zero if the timer is disarmed. This value is returned as the interval 
  until timer expiration, even if the timer was armed with absolute time. 
  The it_interval member of value shall contain the reload value last set 
  by timer_settime().
  
  The timer_settime() function shall set the time until the next expiration 
  of the timer specified by timerid from the it_value member of the value 
  argument and arm the timer if the it_value member of value is non-zero. 
  If the specified timer was already armed when timer_settime() is called, 
  this call shall reset the time until next expiration to the value specified.
  If the it_value member of value is zero, the timer shall be disarmed. The
  effect of disarming or resetting a timer with pending expiration 
  notifications is unspecified.
  
  If the flag TIMER_ABSTIME is not set in the argument flags, timer_settime() 
  shall behave as if the time until next expiration is set to be equal to the 
  interval specified by the it_value member of value. That is, the timer
  shall expire in it_value nanoseconds from when the call is made. If the 
  flag TIMER_ABSTIME is set in the argument flags, timer_settime() shall 
  behave as if the time until next expiration is set to be equal to the 
  difference between the absolute time specified by the it_value member of 
  value and the current value of the clock associated with timerid. That is, 
  the timer shall expire when the clock reaches the value specified by the
  it_value member of value. If the specified time has already passed, the 
  function shall succeed and the expiration notification shall be made.
  
  The reload value of the timer shall be set to the value specified by the 
  it_interval member of value. When a timer is armed with a non-zero 
  it_interval, a periodic (or repetitive) timer is specified.
  
  Time values that are between two consecutive non-negative integer multiples
  of the resolution of the specified timer shall be rounded up to the larger
  multiple of the resolution. Quantization error shall not cause the timer 
  to expire earlier than the rounded time value.
  
  If the argument ovalue is not NULL, the timer_settime() function shall store,
  in the location referenced by ovalue, a value representing the previous 
  amount of time before the timer would have expired, or zero if the timer 
  was disarmed, together with the previous timer reload value. Timers shall 
  not expire before their scheduled time.
  
  Only a single signal shall be queued to the process for a given timer at 
  any point in time. When a timer for which a signal is still pending expires, 
  no signal shall be queued, and a timer overrun shall occur. When a timer 
  expiration signal is delivered to or accepted by a process, the 
  timer_getoverrun() function shall return the timer expiration overrun count
  for the specified timer. The overrun count returned contains the number of 
  extra timer expirations that occurred between the time the signal was 
  generated (queued) and when it was delivered or accepted, up to but not 
  including an implementation-defined maximum of {DELAYTIMER_MAX}. If the 
  number of such extra expirations is greater than or equal to {DELAYTIMER_MAX}, 
  then the overrun count shall be set to {DELAYTIMER_MAX}. The value returned 
  by timer_getoverrun() shall apply to the most recent expiration signal 
  delivery or acceptance for the timer. If no expiration signal has been 
  delivered for the timer, the return value of timer_getoverrun() is unspecified.
  
  RETURN VALUE
  
  If the timer_getoverrun() function succeeds, it shall return the timer 
  expiration overrun count as explained above.
  
  If the timer_gettime() or timer_settime() functions succeed, a value of 0
  shall be returned.
  
  If an error occurs for any of these functions, the value -1 shall be returned, 
  and errno set to indicate the error.
  
  ERRORS
  
  The timer_settime() function shall fail if:
  
  [EINVAL]
  
  A value structure specified a nanosecond value less than zero or greater than
  or equal to 1000 million, and the it_value member of that structure did not 
  specify zero seconds and nanoseconds.
  
  These functions may fail if:
  
  [EINVAL]
  
  The timerid argument does not correspond to an ID returned by timer_create() 
  but not yet deleted by timer_delete().
  
  The timer_settime() function may fail if:
  
  [EINVAL]
  
  The it_interval member of value is not zero and the timer was created with 
  notification by creation of a new thread ( sigev_sigev_notify was 
  SIGEV_THREAD) and a fixed stack address has been set in the thread attribute
  pointed to by sigev_notify_attributes.
*/

int timer_getoverrun(timer_t timerid)
    {
    return 0;
    }

int timer_gettime(timer_t timerid, struct itimerspec *value)
    {
    return OK;
    }

int timer_settime(timer_t timerid, int flags, const struct itimerspec * value,
       struct itimerspec * ovalue)
    {
    return OK;
    }

/*
  NAME
  
  getitimer, setitimer - get and set value of interval timer
  
  SYNOPSIS
  
  #include <sys/time.h>
  
  int getitimer(int which, struct itimerval *value);
  int setitimer(int which, const struct itimerval *restrict value,
         struct itimerval *restrict ovalue); 
  
  DESCRIPTION
  
  The getitimer() function shall store the current value of the timer 
  specified by which into the structure pointed to by value. The setitimer() 
  function shall set the timer specified by which to the value specified 
  in the structure pointed to by value, and if ovalue is not a null pointer, 
  store the previous value of the timer in the structure pointed to by ovalue.
  
  A timer value is defined by the itimerval structure, specified in 
  <sys/time.h>. If it_value is non-zero, it shall indicate the time to the 
  next timer expiration. If it_interval is non-zero, it shall specify a 
  value to be used in reloading it_value when the timer expires. Setting 
  it_value to 0 shall disable a timer, regardless of the value of it_interval.
  Setting it_interval to 0 shall disable a timer after its next expiration 
  (assuming it_value is non-zero).
  
  Implementations may place limitations on the granularity of timer values. 
  For each interval timer, if the requested timer value requires a finer 
  granularity than the implementation supports, the actual timer value shall
  be rounded up to the next supported value.
  
  An XSI-conforming implementation provides each process with at least three 
  interval timers, which are indicated by the which argument:
  
  ITIMER_PROF
  
  Decrements both in process virtual time and when the system is running on 
  behalf of the process. It is designed to be used by interpreters in 
  statistically profiling the execution of interpreted programs. Each time 
  the ITIMER_PROF timer expires, the SIGPROF signal is delivered.
  
  ITIMER_REAL
  
  Decrements in real time. A SIGALRM signal is delivered when this timer 
  expires.
  
  ITIMER_VIRTUAL
  
  Decrements in process virtual time. It runs only when the process is 
  executing. A SIGVTALRM signal is delivered when it expires.
  
  The interaction between setitimer() and alarm() or sleep() is unspecified.
  
  RETURN VALUE
  
  Upon successful completion, getitimer() or setitimer() shall return 0; 
  otherwise, -1 shall be returned and errno set to indicate the error.
  
  ERRORS
  
  The setitimer() function shall fail if:
  
  [EINVAL]
  
  The value argument is not in canonical form. (In canonical form, the number
  of microseconds is a non-negative integer less than 1000000 and the number
  of seconds is a non-negative integer.)
  
  The getitimer() and setitimer() functions may fail if:
  
  [EINVAL]
  
  The which argument is not recognized.
*/

void itimer_callback_handler(void)
    {
    interval_timer_t * itimer;
    struct timeval now;
    abstime_t cmp;
    struct timerchain_node * timernode;

    timernode = timerchain_get_earliest(&global_interval_timerchain);

    if (!timernode)
        return;
       
    gettimeofday(&now, NULL);
    
    cmp = timeval_to_abstime(&now);
    
    if (cmp < timernode->expires)
        return;

    itimer = CONTAINER_OF(timernode, interval_timer_t, timer_node);

    if (itimer->enabled == TRUE)
        {
        itimer->handler(itimer->arg);
        
        timerchain_remove(&global_interval_timerchain, &itimer->timer_node);

        /*
         * If it_interval is non-zero, it shall specify a value to be 
         * used in reloading it_value when the timer expires. 
         *
         * Setting it_interval to 0 shall disable a timer after its 
         * next expiration (assuming it_value is non-zero).
         */
        if (timeval_nz(&itimer->timerval.it_interval))
            {            
            itimer->timerval.it_value = itimer->timerval.it_interval;
            
            gettimeofday(&now, NULL);
            timeval_add(&now, &itimer->timerval.it_value);
            cmp = timeval_to_abstime(&now);

            /* Setup expiry time */
            itimer->timer_node.expires = cmp;

            timerchain_add(&global_interval_timerchain, &itimer->timer_node);
            }
        else
            {
            itimer->enabled = FALSE;
            }
        }
    }

void itimer_expire_handler(void * arg)
    {
    interval_timer_t * itimer = (interval_timer_t *)arg;
    if (itimer->timer_id == ITIMER_REAL)
        pthread_kill(itimer->pid, SIGALRM);
    }

int getitimer(int which, struct itimerval *value)
    {
    /* Currently only supports ITIMER_REAL */
    if (which != ITIMER_REAL)
        return EINVAL;
    
    if (kurrent->itimer_REAL == NULL)
        return ENODEV;
    
    *value = kurrent->itimer_REAL->timerval;
    
    return OK;
    }

int setitimer(int which, const struct itimerval * value, 
              struct itimerval * ovalue)
    {
    int mode;
    interval_timer_t *itimer;
    
    /* Currently only supports ITIMER_REAL */
    if (which != ITIMER_REAL)
        return EINVAL;

    /* Make sure the value argument is in canonical form */
    if (!value || 
        value->it_interval.tv_usec >= USECS_PER_SEC ||
        value->it_interval.tv_sec < 0 ||
        value->it_value.tv_usec >= USECS_PER_SEC ||
        value->it_value.tv_sec < 0)
        return EINVAL;

    itimer = kurrent->itimer_REAL;

    if (itimer == NULL)
        {        
        itimer = kmalloc(sizeof(interval_timer_t));

        if (itimer == NULL)
            {
            printk("No memory for ITIMER_REAL for thread %s\n", kurrent->name);
            
            return ENOMEM;
            }
        
        kurrent->itimer_REAL = itimer;
        
        itimer->timer_id = ITIMER_REAL;
        itimer->handler = itimer_expire_handler;
        itimer->arg = itimer;
        
        timerchain_init_node(&itimer->timer_node);
        
        if (ovalue) 
            *ovalue = *value;
        }
    else
        {
        if (ovalue) 
            *ovalue = itimer->timerval;
        }
    
    itimer->timerval = *value;
    itimer->pid = kurrent;
    
    /* 
     * If it_value is non-zero, it shall indicate the time to the 
     * next timer expiration. 
     *
     * Setting it_value to 0 shall disable a timer, regardless of 
     * the value of it_interval.
     */  
    if (timeval_nz(&value->it_value))
        {
        struct timeval now;
        abstime_t cmp;

        gettimeofday(&now, NULL);
        timeval_add(&now, &itimer->timerval.it_value);
        cmp = timeval_to_abstime(&now);

        /* Setup expiry time */        
        itimer->timer_node.expires = cmp;
        
        itimer->enabled = TRUE;
        
        timerchain_add(&global_interval_timerchain, &itimer->timer_node);
        
        return OK;
        }
    else
        {
        /* Disable the timer */
        itimer->enabled = FALSE;
        
        timerchain_remove(&global_interval_timerchain, &itimer->timer_node);
        
        return OK;
        }
    }

void timerchain_subsystem_init(void)
    {
    timerchain_init_head(&global_interval_timerchain);
    }

int timerchain_compare(struct timerchain_node *t1, 
    struct timerchain_node *t2)
    {
    return (t2->expires > t1->expires) ? 1 : 
           ((t2->expires < t1->expires) ? -1 : 0);
    }

/**
 * timerchain_add - Adds timer to timerchain.
 *
 * @head: head of timerchain
 * @node: timer node to be added
 *
 * Adds the timer node to the timerchain, sorted by the node's expires value.
 */
void timerchain_add(struct timerchain_head *head, struct timerchain_node *node)
    {
    spinlock_lock(&head->lock);
    
    rbtree_insert_node(&head->rbtree, &node->rbnode);

    /* Save the next earliest entry */
    if (!head->earliest || node->expires < head->earliest->expires)
        head->earliest = node;
    
    spinlock_unlock(&head->lock);
    }

/**
 * timerchain_remove - Removes a timer from the timerchain.
 *
 * @head: head of timerchain
 * @node: timer node to be removed
 *
 * Removes the timer node from the timerchain.
 */
void timerchain_remove(struct timerchain_head *head, struct timerchain_node *node)
    {
    spinlock_lock(&head->lock);

    /* Update next earliest entry */
    if (head->earliest == node) 
        {
        struct rbnode *rbn = rbnode_successor(&node->rbnode);

        head->earliest = rbn ?
            RB_ENTRY(rbn, struct timerchain_node, rbnode) : NULL;
        }
    
    rbtree_remove_node(&head->rbtree, &node->rbnode);

    spinlock_unlock(&head->lock);
    }

void timerchain_show_node (struct timerchain_node *node)
    {
    printk("timer expires %lld\n", node->expires);
    }

void timerchain_show_all (struct timerchain_head *head)
    {
    spinlock_lock(&head->lock);
    rbtree_traverse(&head->rbtree, (rb_operation)timerchain_show_node);
    spinlock_unlock(&head->lock);
    }

int do_timerchain (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
    {
    struct timerchain_node timer[5];
    struct timerchain_head timerchain;
    struct timerchain_node * eariliest;
    
    printk("Showing global_interval_timerchain %d timers\n", 
        global_interval_timerchain.rbtree.size);

    timerchain_show_all(&global_interval_timerchain);
    
    timerchain_init_head(&timerchain);
    
    timer[0].expires = 1;
    timer[1].expires = 12;
    timer[2].expires = 23;
    timer[3].expires = 4;
    timer[4].expires = 4;

    timerchain_init_node(&timer[0]);
    timerchain_init_node(&timer[1]);
    timerchain_init_node(&timer[2]);
    timerchain_init_node(&timer[3]);
    timerchain_init_node(&timer[4]);

    eariliest = timerchain_get_earliest(&timerchain);
    
    printk("Showing %d timers, eariliest %lld\n", timerchain.rbtree.size, 
        eariliest ? eariliest->expires : -1);
    
    timerchain_add(&timerchain, &timer[0]);
    timerchain_add(&timerchain, &timer[1]);
    timerchain_add(&timerchain, &timer[2]);
    timerchain_add(&timerchain, &timer[3]);
    timerchain_add(&timerchain, &timer[4]);

    eariliest = timerchain_get_earliest(&timerchain);
    
    printk("Showing %d timers, eariliest %lld\n", timerchain.rbtree.size, 
        eariliest ? eariliest->expires : -1);
    
    timerchain_show_all(&timerchain);

    timerchain_remove(&timerchain, &timer[3]);
    timerchain_remove(&timerchain, &timer[3]);
    timerchain_remove(&timerchain, &timer[0]);
    
    eariliest = timerchain_get_earliest(&timerchain);
    
    printk("Showing %d timers, eariliest %lld\n", timerchain.rbtree.size, 
        eariliest ? eariliest->expires : -1);
    
    timerchain_show_all(&timerchain);
    
    return 0;
    }

CELL_OS_CMD(
    timerchain,   1,        1,    do_timerchain,
    "show current timer chain",
    "show current timer chain nodes\n"
    );
    

