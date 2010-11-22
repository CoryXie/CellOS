/* sched_thread_attr.c - scheduler thread attribute management */

#include <sys.h>
#include <arch.h>
#include <sched.h>
#include <pthread.h>
#include <os/sched_thread.h>

/*
 * pthread_attr_init - initialize the thread attributes object
 * pthread_attr_destroy - destroy the thread attributes object
 *
 * DESCRIPTION
 *
 * The pthread_attr_init() function shall initialize a thread attributes
 * object attr with the default value for all of the individual attributes
 * used by a given implementation.
 *
 * The pthread_attr_destroy() function shall destroy a thread attributes 
 * object. An implementation may cause pthread_attr_destroy() to set attr
 * to an implementation-defined invalid value. A destroyed attr attributes
 * object can be reinitialized using pthread_attr_init(); the results of
 * otherwise referencing the object after it has been destroyed are undefined.
 *
 * The resulting attributes object (possibly modified by setting individual
 * attribute values) when used by pthread_create() defines the attributes
 * of the thread created. A single attributes object can be used in multiple
 * simultaneous calls to pthread_create(). Results are undefined if 
 * pthread_attr_init() is called specifying an already initialized attr 
 * attributes object.
 *
 * The behavior is undefined if the value specified by the attr argument
 * to pthread_attr_destroy() does not refer to an initialized thread 
 * attributes object.
 *
 * RETURN VALUE
 *
 * Upon successful completion, pthread_attr_destroy() and pthread_attr_init()
 * shall return a value of 0; otherwise, an error number shall be returned 
 * to indicate the error.
 *
 * ERRORS
 *
 * The pthread_attr_init() function shall fail if:
 *
 * [ENOMEM]
 *      Insufficient memory exists to initialize the thread attributes object.
 *      These functions shall not return an error code of [EINTR].
 */
int pthread_attr_init
    (
    pthread_attr_t *attr
    )
    {
    sched_fifo_param_t * sched_param;
    pthread_attr_t attrP = kmalloc(sizeof(*attrP));

    if (!attrP)
        return ENOMEM;

    memset(attrP, 0, sizeof(*attrP));

    /* Set a default name */
    strncpy(attrP->name, "NoNameThread", NAME_MAX);
    
    /* 
     * Clearing the cpu set means the thread can only run on 
     * local CPU of the system.
     */
    CPU_ZERO(&attrP->cpu_set);

    attrP->stackaddr = NULL;

    attrP->stacksize = CONFIG_KSTACK_SIZE;

    /* Assume the user has to pthread_join() the thread after it terminates */
    attrP->detachstate = PTHREAD_CREATE_JOINABLE;

    /* The default signal is set so that it can continue executing */
    sigemptyset(&attrP->initial_signal);
    sigaddset(&attrP->initial_signal, SIGCONT);

    /* Construct the sched_policy and sched_param */
    
    attrP->policy = SCHED_RR;
    
    attrP->sched_param_size = sizeof(sched_fifo_param_t);

    sched_param = (sched_fifo_param_t *)attrP->sched_param_area;
    
    memset(sched_param, 0, sizeof(sched_fifo_param_t));
    
    sched_param->sched_priority = sched_get_priority_min(SCHED_FIFO);

    attrP->guardsize = PAGE_SIZE;

    attrP->inheritsched = PTHREAD_EXPLICIT_SCHED;

    attrP->magic = MAGIC_VALID;

    attrP->autorun = TRUE;

    attrP->intial_flags = THREAD_AUTO_RUN;

    *attr = attrP;
    
    return OK;
    }

/*
 * pthread_attr_destroy - destroy the thread attributes object
 *
 * SEE pthread_attr_init() for more infromation.
 */
int pthread_attr_destroy
    (
    pthread_attr_t *attr
    )
    {
    pthread_attr_t attrP = *attr;
    
    if (attrP->stackaddr)
        kfree(attrP->stackaddr);
    
    attrP->stackaddr = NULL;

    attrP->magic = MAGIC_INVALID;
    
    return OK;
    }

/*
  NAME
  
  pthread_attr_getdetachstate - get the detachstate attribute
  pthread_attr_setdetachstate - set the detachstate attribute
  
  SYNOPSIS
  
  #include <pthread.h>
  
  int pthread_attr_getdetachstate(const pthread_attr_t *attr,
         int *detachstate);
  int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate);
  
  DESCRIPTION
  
  The detachstate attribute controls whether the thread is created in a 
  detached state. If the thread is created detached, then use of the ID 
  of the newly created thread by the pthread_detach() or pthread_join() 
  function is an error.
  
  The pthread_attr_getdetachstate() and pthread_attr_setdetachstate() 
  functions, respectively, shall get and set the detachstate attribute 
  in the attr object.
  
  For pthread_attr_getdetachstate(), detachstate shall be set to either 
  PTHREAD_CREATE_DETACHED or PTHREAD_CREATE_JOINABLE.
  
  For pthread_attr_setdetachstate(), the application shall set detachstate 
  to either PTHREAD_CREATE_DETACHED or PTHREAD_CREATE_JOINABLE.
  
  A value of PTHREAD_CREATE_DETACHED shall cause all threads created with 
  attr to be in the detached state, whereas using a value of 
  PTHREAD_CREATE_JOINABLE shall cause all threads created with attr to be 
  in the joinable state. The default value of the detachstate attribute 
  shall be PTHREAD_CREATE_JOINABLE.
  
  The behavior is undefined if the value specified by the attr argument 
  to pthread_attr_getdetachstate() or pthread_attr_setdetachstate() does 
  not refer to an initialized thread attributes object.
  
  RETURN VALUE
  
  Upon successful completion, pthread_attr_getdetachstate() and 
  pthread_attr_setdetachstate() shall return a value of 0; otherwise, 
  an error number shall be returned to indicate the error.
  
  The pthread_attr_getdetachstate() function stores the value of the 
  detachstate attribute in detachstate if successful.
  
  ERRORS
  
  The pthread_attr_setdetachstate() function shall fail if:
  
  [EINVAL]
  
  The value of detachstate was not valid
  
  These functions shall not return an error code of [EINTR].
*/

int pthread_attr_getdetachstate
    (
    const pthread_attr_t *attr,
    int *detachstate
    )
    {
    pthread_attr_t attrP = *attr;
    
    if (attrP->magic != MAGIC_VALID)
        return EINVAL;

    if ((attrP->detachstate != PTHREAD_CREATE_DETACHED) &&
        (attrP->detachstate != PTHREAD_CREATE_JOINABLE))
        return EINVAL;
    
    *detachstate = attrP->detachstate;
    
    return OK;
    }

int pthread_attr_setdetachstate
    (
    pthread_attr_t *attr, 
    int detachstate
    )
    {
    pthread_attr_t attrP = *attr;
    
    if (attrP->magic != MAGIC_VALID)
        return EINVAL;

    if ((detachstate != PTHREAD_CREATE_DETACHED) &&
        (detachstate != PTHREAD_CREATE_JOINABLE))
        return EINVAL;
    
    attrP->detachstate = detachstate;
    
    return OK;
    }

/*
  NAME
  
  pthread_attr_getguardsize - get the thread guardsize attribute
  pthread_attr_setguardsize - set the thread guardsize attribute
  
  SYNOPSIS
  
  #include <pthread.h>
  
  int pthread_attr_getguardsize(const pthread_attr_t *restrict attr,
         size_t *restrict guardsize);
  int pthread_attr_setguardsize(pthread_attr_t *attr,
         size_t guardsize);
  
  DESCRIPTION
  
  The pthread_attr_getguardsize() function shall get the guardsize attribute
  in the attr object. This attribute shall be returned in the guardsize 
  parameter.
  
  The pthread_attr_setguardsize() function shall set the guardsize attribute 
  in the attr object. The new value of this attribute shall be obtained from 
  the guardsize parameter. If guardsize is zero, a guard area shall not be 
  provided for threads created with attr. If guardsize is greater than zero, 
  a guard area of at least size guardsize bytes shall be provided for each 
  thread created with attr.
  
  The guardsize attribute controls the size of the guard area for the created
  thread's stack. The guardsize attribute provides protection against overflow 
  of the stack pointer. If a thread's stack is created with guard protection, 
  the implementation allocates extra memory at the overflow end of the stack 
  as a buffer against stack overflow of the stack pointer. If an application 
  overflows into this buffer an error shall result (possibly in a SIGSEGV 
  signal being delivered to the thread).
  
  A conforming implementation may round up the value contained in guardsize to 
  a multiple of the configurable system variable {PAGE_SIZE} (see <sys/mman.h>).
  If an implementation rounds up the value of guardsize to a multiple of 
  {PAGE_SIZE}, a call to pthread_attr_getguardsize() specifying attr shall store 
  in the guardsize parameter the guard size specified by the previous 
  pthread_attr_setguardsize() function call.
  
  The default value of the guardsize attribute is implementation-defined.
  
  If the stackaddr attribute has been set (that is, the caller is allocating 
  and managing its own thread stacks), the guardsize attribute shall be 
  ignored and no protection shall be provided by the implementation. It is 
  the responsibility of the application to manage stack overflow along with 
  stack allocation and management in this case.
  
  The behavior is undefined if the value specified by the attr argument to 
  pthread_attr_getguardsize() or pthread_attr_setguardsize() does not refer 
  to an initialized thread attributes object.
  
  RETURN VALUE
  
  If successful, the pthread_attr_getguardsize() and pthread_attr_setguardsize()
  functions shall return zero; otherwise, an error number shall be returned to 
  indicate the error.
  
  ERRORS
  
  These functions shall fail if:
  
  [EINVAL]
  
  The parameter guardsize is invalid.
  
  These functions shall not return an error code of [EINTR].
*/

int pthread_attr_getguardsize
    (
    const pthread_attr_t * attr,
    size_t * guardsize
    )
    {
    pthread_attr_t attrP = *attr;

    if (attrP->magic != MAGIC_VALID)
        return EINVAL;
    
    *guardsize = attrP->guardsize_raw;

    return OK;
    }

int pthread_attr_setguardsize
    (
    pthread_attr_t *attr,
    size_t guardsize
    )
    {
    pthread_attr_t attrP = *attr;

    if (attrP->magic != MAGIC_VALID)
        return EINVAL;

    attrP->guardsize_raw = guardsize;

    attrP->guardsize = ROUND_UP(guardsize, PAGE_SIZE);
    
    return OK;
    }

/*
  NAME
  
  pthread_attr_getinheritsched - get the inheritsched attribute (REALTIME)
  pthread_attr_setinheritsched - set the inheritsched attribute (REALTIME)

  SYNOPSIS
  
  #include <pthread.h>
  
  int pthread_attr_getinheritsched(const pthread_attr_t *restrict attr,
         int *restrict inheritsched);
  int pthread_attr_setinheritsched(pthread_attr_t *attr,
         int inheritsched); 
  
  DESCRIPTION
  
  The pthread_attr_getinheritsched() and pthread_attr_setinheritsched() 
  functions, respectively, shall get and set the inheritsched attribute
  in the attr argument.
  
  When the attributes objects are used by pthread_create(), the inheritsched 
  attribute determines how the other scheduling attributes of the created 
  thread shall be set.
  
  The supported values of inheritsched shall be:
  
  PTHREAD_INHERIT_SCHED
  
  Specifies that the thread scheduling attributes shall be inherited from the 
  creating thread, and the scheduling attributes in this attr argument shall 
  be ignored.

  PTHREAD_EXPLICIT_SCHED
  
  Specifies that the thread scheduling attributes shall be set to the 
  corresponding values from this attributes object.
  
  The symbols PTHREAD_INHERIT_SCHED and PTHREAD_EXPLICIT_SCHED are defined in 
  the <pthread.h> header.
  
  The following thread scheduling attributes defined by POSIX.1-2008 are 
  affected by the inheritsched attribute: 

  - scheduling policy (schedpolicy), 
  - scheduling parameters (schedparam),
  - and scheduling contention scope (contentionscope).
  
  The behavior is undefined if the value specified by the attr argument to 
  pthread_attr_getinheritsched() or pthread_attr_setinheritsched() does not 
  refer to an initialized thread attributes object.
  
  RETURN VALUE
  
  If successful, the pthread_attr_getinheritsched() and 
  pthread_attr_setinheritsched() functions shall return zero; otherwise, an 
  error number shall be returned to indicate the error.
  
  ERRORS
  
  The pthread_attr_setinheritsched() function may fail if:
  
  [EINVAL]
  
  The value of inheritsched is not valid.
  
  [ENOTSUP]
  
  An attempt was made to set the attribute to an unsupported value.
  
  These functions shall not return an error code of [EINTR].
 */

 int pthread_attr_getinheritsched
    (
    const pthread_attr_t * attr,
    int * inheritsched
    )
    {
    pthread_attr_t attrP = *attr;

    if (attrP->magic != MAGIC_VALID)
        return EINVAL;

    if ((attrP->inheritsched != PTHREAD_INHERIT_SCHED) &&
        (attrP->inheritsched != PTHREAD_EXPLICIT_SCHED))
        return EINVAL;

    *inheritsched = attrP->inheritsched;

    return OK;
    }

 int pthread_attr_setinheritsched
    (
    pthread_attr_t *attr,
    int inheritsched
    )
    {
    pthread_attr_t attrP = *attr;
     
    if (attrP->magic != MAGIC_VALID)
        return EINVAL;
     
    if ((inheritsched != PTHREAD_INHERIT_SCHED) &&
        (inheritsched != PTHREAD_EXPLICIT_SCHED))
        return EINVAL;
     
    attrP->inheritsched = inheritsched;
     
    return OK;
    }

/*
  NAME
  
  pthread_attr_getschedparam - get the schedparam attribute
  pthread_attr_setschedparam - set the schedparam attribute

  SYNOPSIS
  
  #include <pthread.h>
  
  int pthread_attr_getschedparam(const pthread_attr_t *restrict attr,
         struct sched_param *restrict param);
  int pthread_attr_setschedparam(pthread_attr_t *restrict attr,
         const struct sched_param *restrict param);
  
  DESCRIPTION
  
  The pthread_attr_getschedparam() and pthread_attr_setschedparam() functions,
  respectively, shall get and set the scheduling parameter attributes in the 
  attr argument. The contents of the param structure are defined in the 
  <sched.h> header. For the SCHED_FIFO and SCHED_RR policies, the only required
  member of param is sched_priority.
  
  For the SCHED_SPORADIC policy, the required members of the param structure 
  are sched_priority, sched_ss_low_priority, sched_ss_repl_period, 
  sched_ss_init_budget, and sched_ss_max_repl. The specified sched_ss_repl_period 
  must be greater than or equal to the specified sched_ss_init_budget for the 
  function to succeed; if it is not, then the function shall fail. The value of 
  sched_ss_max_repl shall be within the inclusive range [1, {SS_REPL_MAX}] for 
  the function to succeed; if not, the function shall fail. It is unspecified 
  whether the sched_ss_repl_period and sched_ss_init_budget values are stored 
  as provided by this function or are rounded to align with the resolution of 
  the clock being used. 
  
  The behavior is undefined if the value specified by the attr argument to 
  pthread_attr_getschedparam() or pthread_attr_setschedparam() does not refer 
  to an initialized thread attributes object.
  
  RETURN VALUE
  
  If successful, the pthread_attr_getschedparam() and pthread_attr_setschedparam() 
  functions shall return zero; otherwise, an error number shall be returned to 
  indicate the error.
  
  ERRORS
  
  The pthread_attr_setschedparam() function may fail if:
  
  [EINVAL]
  
  The value of param is not valid.
  
  [ENOTSUP]
  
  An attempt was made to set the attribute to an unsupported value.
  
  These functions shall not return an error code of [EINTR].
*/

int pthread_attr_getschedparam
    (
    const pthread_attr_t * attr,
    struct sched_param * param
    )
    {
    pthread_attr_t attrP = *attr;

    if (attrP->policy == SCHED_FIFO)
        {
        sched_fifo_param_t * fifo = 
            (sched_fifo_param_t *)attrP->sched_param_area;

        param->sched_priority = fifo->sched_priority;
        }
    
    return OK;
    }

int pthread_attr_setschedparam
    (
    pthread_attr_t * attr,
    const struct sched_param * param
    )
    {
    pthread_attr_t attrP = *attr;
    
    if (attrP->policy == SCHED_FIFO)
        {
        sched_fifo_param_t * fifo = 
            (sched_fifo_param_t *)attrP->sched_param_area;

        fifo->sched_priority = param->sched_priority;
        }
    
    return OK;
    }

/*
  NAME
  
  pthread_attr_getschedpolicy - get the schedpolicy attribute (REALTIME)
  pthread_attr_setschedpolicy - set the schedpolicy attribute (REALTIME)

  SYNOPSIS
  
  [TPS]  #include <pthread.h>
  
  int pthread_attr_getschedpolicy(const pthread_attr_t *restrict attr,
         int *restrict policy);
  int pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy); 
  
  DESCRIPTION
  
  The pthread_attr_getschedpolicy() and pthread_attr_setschedpolicy() 
  functions, respectively, shall get and set the schedpolicy attribute in 
  the attr argument.
  
  The supported values of policy shall include SCHED_FIFO, SCHED_RR, and 
  SCHED_OTHER, which are defined in the <sched.h> header. When threads 
  executing with the scheduling policy SCHED_FIFO, SCHED_RR, or SCHED_SPORADIC  
  are waiting on a mutex, they shall acquire the mutex in priority order when 
  the mutex is unlocked.
  
  The behavior is undefined if the value specified by the attr argument to 
  pthread_attr_getschedpolicy() or pthread_attr_setschedpolicy() does not 
  refer to an initialized thread attributes object.
  
  RETURN VALUE
  
  If successful, the pthread_attr_getschedpolicy() and 
  pthread_attr_setschedpolicy() functions shall return zero; otherwise, an 
  error number shall be returned to indicate the error.
  
  ERRORS
  
  The pthread_attr_setschedpolicy() function may fail if:
  
  [EINVAL]
  
  The value of policy is not valid.
  
  [ENOTSUP]
  
  An attempt was made to set the attribute to an unsupported value.
  
  These functions shall not return an error code of [EINTR].
*/
int pthread_attr_getschedpolicy
    (
    const pthread_attr_t *attr,
    int * policy
    )
    {
    pthread_attr_t attrP = *attr;
    
    if (attrP->magic != MAGIC_VALID)
        return EINVAL;
    
    if ((attrP->policy != SCHED_FIFO) &&
        (attrP->policy != SCHED_RR) &&
        (attrP->policy != SCHED_SPORADIC) &&
        (attrP->policy != SCHED_OTHER))
        return EINVAL;
    
    *policy = attrP->policy;

    return OK;
    }

int pthread_attr_setschedpolicy
    (
    pthread_attr_t *attr, 
    int policy
    )
    {
    pthread_attr_t attrP = *attr;

    if (attrP->magic != MAGIC_VALID)
        return EINVAL;

    /* If new schedule policy is added, extend this! */
    if ((policy != SCHED_FIFO) &&
        (policy != SCHED_RR) &&
        (policy != SCHED_SPORADIC) &&
        (policy != SCHED_OTHER))
        return EINVAL;
     
    attrP->policy = policy;
     
    return OK;
    }

/*
  NAME
  
  pthread_attr_getscope - get the contentionscope attribute (REALTIME)
  pthread_attr_setscope - set the contentionscope attribute (REALTIME)

  SYNOPSIS
  
  #include <pthread.h>
  
  int pthread_attr_getscope(const pthread_attr_t *restrict attr,
         int *restrict contentionscope);
  int pthread_attr_setscope(pthread_attr_t *attr, int contentionscope); 
  
  DESCRIPTION
  
  The pthread_attr_getscope() and pthread_attr_setscope() functions, 
  respectively, shall get and set the contentionscope attribute in the 
  attr object.
  
  The contentionscope attribute may have the values PTHREAD_SCOPE_SYSTEM, 
  signifying system scheduling contention scope, or PTHREAD_SCOPE_PROCESS, 
  signifying process scheduling contention scope. The symbols 
  PTHREAD_SCOPE_SYSTEM and PTHREAD_SCOPE_PROCESS are defined in the 
  <pthread.h> header.
  
  The behavior is undefined if the value specified by the attr argument to 
  pthread_attr_getscope() or pthread_attr_setscope() does not refer to an 
  initialized thread attributes object.
  
  RETURN VALUE
  
  If successful, the pthread_attr_getscope() and pthread_attr_setscope() 
  functions shall return zero; otherwise, an error number shall be returned 
  to indicate the error.
  
  ERRORS
  
  The pthread_attr_setscope() function may fail if:
  
  [EINVAL]
  
  The value of contentionscope is not valid.
  
  [ENOTSUP]
  
  An attempt was made to set the attribute to an unsupported value.
  
  These functions shall not return an error code of [EINTR].
*/

int pthread_attr_getscope
    (
    const pthread_attr_t * attr,
    int * contentionscope
    )
    {
    pthread_attr_t attrP = *attr;

    if (attrP->magic != MAGIC_VALID)
        return EINVAL;

    if ((attrP->contentionscope != PTHREAD_SCOPE_SYSTEM) &&
        (attrP->contentionscope != PTHREAD_SCOPE_PROCESS))
         return EINVAL;

    *contentionscope = attrP->contentionscope;

    return OK;
    }

int pthread_attr_setscope
    (
    pthread_attr_t *attr, 
    int contentionscope
    )
    {
    pthread_attr_t attrP = *attr;

    if (attrP->magic != MAGIC_VALID)
         return EINVAL;
     
    if ((contentionscope != PTHREAD_SCOPE_SYSTEM) &&
        (contentionscope != PTHREAD_SCOPE_PROCESS))
         return EINVAL;
     
    attrP->contentionscope = contentionscope;
     
    return OK;
    }

/*
  NAME
  
  pthread_attr_getstack - get stack attributes
  pthread_attr_setstack - set stack attributes

  SYNOPSIS
  
  #include <pthread.h>
  
  int pthread_attr_getstack(const pthread_attr_t *restrict attr,
         void **restrict stackaddr, size_t *restrict stacksize);
  int pthread_attr_setstack(pthread_attr_t *attr, void *stackaddr,
         size_t stacksize); 
  
  DESCRIPTION
  
  The pthread_attr_getstack() and pthread_attr_setstack() functions, 
  respectively, shall get and set the thread creation stack attributes 
  stackaddr and stacksize in the attr object.
  
  The stack attributes specify the area of storage to be used for the created 
  thread's stack. The base (lowest addressable byte) of the storage shall be 
  stackaddr, and the size of the storage shall be stacksize bytes. The 
  stacksize shall be at least {PTHREAD_STACK_MIN}. The pthread_attr_setstack() 
  function may fail with [EINVAL] if stackaddr does not meet 
  implementation-defined alignment requirements. All pages within the stack 
  described by stackaddr and stacksize shall be both readable and writable by 
  the thread.
  
  If the pthread_attr_getstack() function is called before the stackaddr 
  attribute has been set, the behavior is unspecified.
  
  The behavior is undefined if the value specified by the attr argument to 
  pthread_attr_getstack() or pthread_attr_setstack() does not refer to an 
  initialized thread attributes object.
  
  RETURN VALUE
  
  Upon successful completion, these functions shall return a value of 0; 
  otherwise, an error number shall be returned to indicate the error.
  
  The pthread_attr_getstack() function shall store the stack attribute values
  in stackaddr and stacksize if successful.
  
  ERRORS
  
  The pthread_attr_setstack() function shall fail if:
  
  [EINVAL]
  
  The value of stacksize is less than {PTHREAD_STACK_MIN} or exceeds an 
  implementation-defined limit.
  
  The pthread_attr_setstack() function may fail if:
  
  [EINVAL]
  
  The value of stackaddr does not have proper alignment to be used as a stack,
  or (( char *) stackaddr + stacksize) lacks proper alignment.
  
  [EACCES]
  
  The stack page(s) described by stackaddr and stacksize are not both readable
  and writable by the thread.
  
  These functions shall not return an error code of [EINTR].
*/

int pthread_attr_getstack
    (
    const pthread_attr_t * attr,
    void ** stackaddr, 
    size_t * stacksize
    )
    { 
    pthread_attr_t attrP = *attr;

    if (attrP->magic != MAGIC_VALID)
        return EINVAL;

    *stackaddr = attrP->stackaddr;
    *stacksize = attrP->stacksize;

    return OK;
    }

int pthread_attr_setstack
    (
    pthread_attr_t *attr,
    void *stackaddr,
    size_t stacksize
    )
    {   
    addr_t stacktop;
    pthread_attr_t attrP = *attr;

    if (attrP->magic != MAGIC_VALID)
        return EINVAL;

    if ((stacksize < PTHREAD_STACK_MIN) || (stacksize > PTHREAD_STACK_MAX)) 
        {
        return EINVAL;
        }

    stacktop = (addr_t)(( char *) stackaddr + stacksize);

    if (ALIGN_DOWN(stacktop, ARCH_STACK_ALIGN) != stacktop)
        return EINVAL;

    attrP->stackaddr = stackaddr;
    
    attrP->stacksize = stacksize;

    return OK;
    }

/*
 * NAME
 *
 * pthread_attr_getstacksize - get the stacksize attribute
 * pthread_attr_setstacksize - set the stacksize attribute
 *
 * SYNOPSIS
 *
 * #include <pthread.h>
 *
 * int pthread_attr_getstacksize(const pthread_attr_t *restrict attr,
 *       size_t *restrict stacksize);
 * int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize); 
 *
 * DESCRIPTION
 *
 * The pthread_attr_getstacksize() and pthread_attr_setstacksize() functions,
 * respectively, shall get and set the thread creation stacksize attribute 
 * in the attr object.
 *
 * The stacksize attribute shall define the minimum stack size (in bytes) 
 * allocated for the created threads stack.
 *
 * The behavior is undefined if the value specified by the attr argument
 * to pthread_attr_getstacksize() or pthread_attr_setstacksize() does not
 * refer to an initialized thread attributes object.
 *
 * RETURN VALUE
 *
 * Upon successful completion, pthread_attr_getstacksize() and
 * pthread_attr_setstacksize() shall return a value of 0; otherwise,
 * an error number shall be returned to indicate the error.
 *
 * The pthread_attr_getstacksize() function stores the stacksize attribute
 * value in stacksize if successful.
 *
 * ERRORS
 *
 * The pthread_attr_setstacksize() function shall fail if:
 *
 * [EINVAL]
 *      The value of stacksize is less than {PTHREAD_STACK_MIN} or exceeds
 *      a system-imposed limit.
 *
 *  These functions shall not return an error code of [EINTR].
 */

int pthread_attr_getstacksize
    (
    const pthread_attr_t * attr,
    size_t * stacksize
    )
    {
    pthread_attr_t attrP = *attr;

    if (attrP->magic != MAGIC_VALID)
        return EINVAL;
    
    *stacksize = attrP->stacksize;
    
    return OK;
    }

/*
 * pthread_attr_setstacksize - set the stacksize attribute
 *
 * SEE pthread_attr_getstacksize() for more infromation.
 */
int pthread_attr_setstacksize
    (
    pthread_attr_t *attr, 
    size_t stacksize
    )
    {
    pthread_attr_t attrP = *attr;

    if (attrP->magic != MAGIC_VALID)
        return EINVAL;
    
    if ((stacksize < PTHREAD_STACK_MIN) || (stacksize > PTHREAD_STACK_MAX)) 
        {
        return EINVAL;
        }

    attrP->stacksize = stacksize;
    
    return OK;
    }

int pthread_attr_setname_np
    (
    pthread_attr_t *attr, 
    char *name
    )
    {
    pthread_attr_t attrP = *attr;

    if (attrP->magic != MAGIC_VALID)
        return EINVAL;
    
    if (!name) 
        {
        return EINVAL;
        }
    
    strncpy(attrP->name, name, NAME_MAX);
    
    return OK;
    }

int pthread_attr_setautorun_np
    (
    pthread_attr_t *attr, 
    int initial_state
    )
    {
    pthread_attr_t attrP = *attr;

    if (attrP->magic != MAGIC_VALID)
        return EINVAL;
        
    attrP->autorun = initial_state;
    
    return OK;
    }

int pthread_attr_setflags_np
    (
    pthread_attr_t *attr, 
    unsigned long flags
    )
    {
    pthread_attr_t attrP = *attr;

    if (attrP->magic != MAGIC_VALID)
        return EINVAL;
        
    attrP->intial_flags = flags;
    
    return OK;
    }

int pthread_attr_getflags_np
    (
    pthread_attr_t *attr, 
    unsigned long * flags
    )
    {
    pthread_attr_t attrP = *attr;

    if (attrP->magic != MAGIC_VALID)
        return EINVAL;
        
    *flags = attrP->intial_flags;
    
    return OK;
    }

int pthread_attr_gettimeslice_np
    (
    const pthread_attr_t * attr,
    int * timeslice
    )
    {
    pthread_attr_t attrP = *attr;

    if (attrP->magic != MAGIC_VALID)
        return EINVAL;
    
    *timeslice = attrP->sched_time_slice;
    
    return OK;
    }

int pthread_attr_settimeslice_np
    (
    pthread_attr_t *attr, 
    int timeslice
    )
    {
    pthread_attr_t attrP = *attr;

    if (attrP->magic != MAGIC_VALID)
        return EINVAL;
    
    attrP->sched_time_slice = timeslice;
    
    return OK;
    }

