/* sched_thread_cancel.c - thread cancelation management */

/*
Thread Cancellation

    The thread cancellation mechanism allows a thread to terminate the 
    execution of any other thread in the process in a controlled manner. 
    The target thread (that is, the one that is being canceled) is allowed 
    to hold cancellation requests pending in a number of ways and to perform
    application-specific cleanup processing when the notice of cancellation
    is acted upon. 

    Cancellation is controlled by the cancellation control functions. Each 
    thread maintains its own cancelability state. Cancellation may only occur
    at cancellation points or when the thread is asynchronously cancelable.

    The thread cancellation mechanism described in this section depends upon
    programs having set deferred cancelability state, which is specified as 
    the default. Applications shall also carefully follow static lexical 
    scoping rules in their execution behavior. For example, use of setjmp( ),
    return, goto, and so on, to leave user-defined cancellation scopes 
    without doing the necessary scope pop operation results in undefined
    behavior.
    
    Use of asynchronous cancelability while holding resources which potentially
    need to be released may result in resource loss. Similarly, cancellation
    scopes may only be safely manipulated (pushed and popped) when the thread 
    is in the deferred or disabled cancelability states.

Cancelability States

    The cancelability state of a thread determines the action taken upon 
    receipt of a cancellation request. The thread may control cancellation 
    in a number of ways.
    
    Each thread maintains its own cancelability state, which may be encoded
    in two bits:

    1. Cancelability-Enable: When cancelability is PTHREAD_CANCEL_DISABLE 
    cancellation requests against the target thread are held pending. 
    By default, cancelability is set to PTHREAD_CANCEL_ENABLE.
    
    2. Cancelability Type: When cancelability is enabled and the cancelability
    type is PTHREAD_CANCEL_ASYNCHRONOUS, new or pending cancellation requests 
    may be acted upon at any time. When cancelability is enabled and the 
    cancelability type is PTHREAD_CANCEL_DEFERRED, cancellation requests are
    held pending until a cancellation point is reached. If cancelability is
    disabled, the setting of the cancelability type has no immediate effect
    as all cancellation requests are held pending; however, once cancelability
    is enabled again the new type is in effect. The cancelability type is
    PTHREAD_CANCEL_DEFERRED in all newly created threads including the thread
    in which main( ) was first invoked.

    An implementation shall not introduce cancellation points into any other
    functions specified in this volume of POSIX.1-2008.
    
    The side-effects of acting upon a cancellation request while suspended 
    during a call of a function are the same as the side-effects that may 
    be seen in a single-threaded program when a call to a function is 
    interrupted by a signal and the given function returns [EINTR]. Any such
    sideeffects occur before any cancellation cleanup handlers are called.
    Whenever a thread has cancelability enabled and a cancellation request 
    has been made with that thread as the target, and the thread then calls 
    any function that is a cancellation point (such as pthread_testcancel( ) 
    or read( )), the cancellation request shall be acted upon before the 
    function returns. If a thread has cancelability enabled and a cancellation
    request is made with the thread as a target while the thread is suspended
    at a cancellation point, the thread shall be awakened and the cancellation
    request shall be acted upon. It is unspecified whether the cancellation
    request is acted upon or whether the cancellation request remains pending 
    and the thread resumes normal execution if:
    
    - The thread is suspended at a cancellation point and the event for which 
      it is waiting occurs
    - A specified timeout expired  before the cancellation request is acted upon.

Thread Cancellation Cleanup Handlers

    Each thread maintains a list of cancellation cleanup handlers. The 
    programmer uses the pthread_cleanup_push( ) and pthread_cleanup_pop( ) 
    functions to place routines on and remove routines from this list.
    
    When a cancellation request is acted upon, or when a thread calls 
    pthread_exit( ), the thread first disables cancellation by setting 
    its cancelability state to PTHREAD_CANCEL_DISABLE and its cancelability 
    type to PTHREAD_CANCEL_DEFERRED. The cancelability state shall remain 
    set to PTHREAD_CANCEL_DISABLE until the thread has terminated. The behavior
    is undefined if a cancellation cleanup handler or thread-specific data 
    destructor routine changes the cancelability state to PTHREAD_CANCEL_ENABLE.
    
    The routines in the thread's list of cancellation cleanup handlers are 
    invoked one by one in LIFO sequence; that is, the last routine pushed 
    onto the list (Last In) is the first to be invoked (First Out). When the 
    cancellation cleanup handler for a scope is invoked, the storage for that
    scope remains valid. If the last cancellation cleanup handler returns,
    thread-specific data destructors (if any) associated with thread-specific
    data keys for which the thread has non-NULL values will be run, in 
    unspecified order, as described for pthread_key_create( ).
    
    After all cancellation cleanup handlers and thread-specific data destructors 
    have returned, thread execution is terminated. If the thread has terminated
    because of a call to pthread_exit( ), the value_ptr argument is made 
    available to any threads joining with the target. If the thread has 
    terminated by acting on a cancellation request, a status of PTHREAD_CANCELED 
    is made available to any threads joining with the target. The symbolic 
    constant PTHREAD_CANCELED expands to a constant expression of type (void *) 
    whose value matches no pointer to an object in memory nor the value NULL.
    
    A side-effect of acting upon a cancellation request while in a condition 
    variable wait is that the mutex is re-acquired before calling the first 
    cancellation cleanup handler. In addition, the thread is no longer 
    considered to be waiting for the condition and the thread shall not have 
    consumed any pending condition signals on the condition.
    
    A cancellation cleanup handler cannot exit via longjmp( ) or siglongjmp( ).

Async-Cancel Safety

    The pthread_cancel(), pthread_setcancelstate(), and pthread_setcanceltype()
    functions are defined to be async-cancel safe.
    
    No other functions in this volume of POSIX.1-2008 are required to be 
    async-cancel-safe.
*/

#include <sys.h>
#include <pthread.h>


/*
  NAME
  
  pthread_setcancelstate - set cancelability state
  pthread_setcanceltype - set cancelability type
  pthread_testcancel - create cancellation point in the calling thread
  
  SYNOPSIS
  
  #include <pthread.h>
  
  int pthread_setcancelstate(int state, int *oldstate);
  int pthread_setcanceltype(int type, int *oldtype);
  void pthread_testcancel(void);
  
  DESCRIPTION
  
  The pthread_setcancelstate() function shall atomically both set the 
  calling thread's cancelability state to the indicated state and return 
  the previous cancelability state at the location referenced by oldstate. 
  Legal values for state are PTHREAD_CANCEL_ENABLE and PTHREAD_CANCEL_DISABLE.
  
  The pthread_setcanceltype() function shall atomically both set the calling
  thread's cancelability type to the indicated type and return the previous 
  cancelability type at the location referenced by oldtype. Legal values for
  type are PTHREAD_CANCEL_DEFERRED and PTHREAD_CANCEL_ASYNCHRONOUS.
  
  The cancelability state and type of any newly created threads, including
  the thread in which main() was first invoked, shall be PTHREAD_CANCEL_ENABLE 
  and PTHREAD_CANCEL_DEFERRED respectively.
  
  The pthread_testcancel() function shall create a cancellation point in 
  the calling thread. The pthread_testcancel() function shall have no effect
  if cancelability is disabled.
  
  RETURN VALUE
  
  If successful, the pthread_setcancelstate() and pthread_setcanceltype() 
  functions shall return zero; otherwise, an error number shall be returned
  to indicate the error.
  
  ERRORS
  
  The pthread_setcancelstate() function may fail if:
  
  [EINVAL]
  
  The specified state is not PTHREAD_CANCEL_ENABLE or PTHREAD_CANCEL_DISABLE.
  The pthread_setcanceltype() function may fail if:
  
  [EINVAL]
  
  The specified type is not PTHREAD_CANCEL_DEFERRED or PTHREAD_CANCEL_ASYNCHRONOUS.
  These functions shall not return an error code of [EINTR].
*/

int pthread_setcancelstate(int state, int *oldstate)
    {
    return OK;
    }

int pthread_setcanceltype(int type, int *oldtype)
    {
    return OK;
    }

void pthread_testcancel(void)
    {
    if (atomic_test_and_clear_bit(THREAD_CANCEL_PENDING, &pthread_self()->flags)) 
        {
        pthread_exit(PTHREAD_CANCELED);
        }
    }

/*
  NAME
  
  pthread_cancel - cancel execution of a thread
  
  SYNOPSIS
  
  #include <pthread.h>
  
  int pthread_cancel(pthread_t thread);
  
  DESCRIPTION
  
  The pthread_cancel() function shall request that thread be canceled. 
  The target thread's cancelability state and type determines when the 
  cancellation takes effect. When the cancellation is acted on, the 
  cancellation cleanup handlers for thread shall be called. When the 
  last cancellation cleanup handler returns, the thread-specific data 
  destructor functions shall be called for thread. When the last 
  destructor function returns, thread shall be terminated.
  
  The cancellation processing in the target thread shall run asynchronously 
  with respect to the calling thread returning from pthread_cancel().
  
  RETURN VALUE
  
  If successful, the pthread_cancel() function shall return zero; 
  otherwise, an error number shall be returned to indicate the error.
  
  ERRORS
  
  The pthread_cancel() function shall not return an error code of [EINTR].
*/
int pthread_cancel
    (
    pthread_t thread
    )
    {

    return OK;
    }

