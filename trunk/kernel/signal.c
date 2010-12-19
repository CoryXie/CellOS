/* signal.c - signal management */

#include <sys.h>
#include <arch.h>
#include <signal.h>

/*
  NAME
  
  signal - signal management
  
  SYNOPSIS
  
  #include <signal.h>
  
  void (*signal(int sig, void (*func)(int)))(int);
  
  DESCRIPTION
  
  The functionality described on this reference page is aligned with
  the ISO C standard. Any conflict between the requirements described 
  here and the ISO C standard is unintentional. This volume of POSIX.1-2008 
  defers to the ISO C standard.
  
  Use of this function is unspecified in a multi-threaded process. 
  
  The signal() function chooses one of three ways in which receipt of the 
  signal number sig is to be subsequently handled. If the value of func is
  SIG_DFL, default handling for that signal shall occur. If the value of 
  func is SIG_IGN, the signal shall be ignored. Otherwise, the application
  shall ensure that func points to a function to be called when that signal 
  occurs. An invocation of such a function because of a signal, or 
  (recursively) of any further functions called by that invocation (other 
  than functions in the standard library), is called a "signal handler".
  
  When a signal occurs, and func points to a function, it is implementation-
  defined whether the equivalent of a:
  
  signal(sig, SIG_DFL);
  
  is executed or the implementation prevents some implementation-defined set 
  of signals (at least including sig) from occurring until the current signal 
  handling has completed. (If the value of sig is SIGILL, the implementation 
  may alternatively define that no action is taken.) Next the equivalent of:
  
  (*func)(sig);
  
  is executed. If and when the function returns, if the value of sig was SIGFPE, 
  SIGILL, or SIGSEGV or any other implementation-defined value corresponding to 
  a computational exception, the behavior is undefined. Otherwise, the program 
  shall resume execution at the point it was interrupted. If the signal occurs 
  as the result of calling the abort(), raise(), kill(), pthread_kill(), 
  or sigqueue()  function, the signal handler shall not call the raise() 
  function.
  
  If the signal occurs other than as the result of calling abort(), raise(), 
  kill(), pthread_kill(), or sigqueue(),   the behavior is undefined if the 
  signal handler refers to any object with static storage duration other than 
  by assigning a value to an object declared as volatile sig_atomic_t, or if 
  the signal handler calls any function in the standard library other than 
  one of the functions listed in Signal Concepts . Furthermore, if such a 
  call fails, the value of errno is unspecified.
  
  At program start-up, the equivalent of:
  
  signal(sig, SIG_IGN);
  
  is executed for some signals, and the equivalent of:
  
  signal(sig, SIG_DFL);
  
  is executed for all other signals [CX]   (see exec). 
  
  RETURN VALUE
  
  If the request can be honored, signal() shall return the value of func 
  for the most recent call to signal() for the specified signal sig. 
  Otherwise, SIG_ERR shall be returned and a positive value shall be 
  stored in errno.
  
  ERRORS
  
  The signal() function shall fail if:
  
  [EINVAL]
  
  The sig argument is not a valid signal number or an attempt is made to 
  catch a signal that cannot be caught or ignore a signal that cannot be
  ignored. 
  
  The signal() function may fail if:
  
  [EINVAL]
  
  An attempt was made to set the action to SIG_DFL for a signal that cannot 
  be caught or ignored (or both). 
*/

sighandler_t signal(int signum, sighandler_t handler)
    {
    sched_thread_t * curr = kurrent;
    struct sig_handler_node * signode = NULL;

    LIST_FOREACH(&curr->sig_handler_list, iter)
        {
        if (iter)
            {
            signode = LIST_ENTRY(iter, struct sig_handler_node, node);

            if (signode && signode->sig == signum)
                break;

            signode = NULL;
            }
        }

    if (signode == NULL)
        {
        signode = kmalloc(sizeof(struct sig_handler_node));
        if (signode == NULL)
            return SIG_ERR;
        
        memset(signode, 0, sizeof(struct sig_handler_node));
        }

    signode->sa_action.sa_handler = handler;
    signode->sig = signum;

    list_append(&curr->sig_handler_list, &signode->node);
    
    return handler;
    }

/*
  NAME
  
  sigaction - examine and change a signal action
  
  SYNOPSIS
  
  #include <signal.h>
  
  int sigaction(int sig, const struct sigaction *restrict act,
         struct sigaction *restrict oact); 
  
  DESCRIPTION
  
  The sigaction() function allows the calling process to examine and/or 
  specify the action to be associated with a specific signal. The argument
  sig specifies the signal; acceptable values are defined in <signal.h>.
  
  The structure sigaction, used to describe an action to be taken, is defined
  in the <signal.h> header to include at least the following members:
  
  Member Type       Member Name         Description
  
  void(*) (int)     sa_handler          Pointer to a signal-catching function 
                                        or
                                        one of the macros SIG_IGN or SIG_DFL.
  
  sigset_t          sa_mask             Additional set of signals to be blocked 
                                        during execution of signal-catching 
                                        function.
  
  int               sa_flags            Special flags to affect behavior of signal.
  
  void(*) (int,
    siginfo_t *, 
    void *)         sa_sigaction        Pointer to a signal-catching function.
  
  The storage occupied by sa_handler and sa_sigaction may overlap, and a 
  conforming application shall not use both simultaneously.
  
  If the argument act is not a null pointer, it points to a structure 
  specifying the action to be associated with the specified signal. If the 
  argument oact is not a null pointer, the action previously associated with 
  the signal is stored in the location pointed to by the argument oact. If 
  the argument act is a null pointer, signal handling is unchanged; thus, 
  the call can be used to enquire about the current handling of a given signal. 
  The SIGKILL and SIGSTOP signals shall not be added to the signal mask using 
  this mechanism; this restriction shall be enforced by the system without 
  causing an error to be indicated.
  
  If the SA_SIGINFO flag (see below) is cleared in the sa_flags field of the 
  sigaction structure, the sa_handler field identifies the action to be 
  associated with the specified signal. If the SA_SIGINFO flag is set in the 
  sa_flags field, the sa_sigaction field specifies a signal-catching function.
  
  The sa_flags field can be used to modify the behavior of the specified 
  signal.
  
  The following flags, defined in the <signal.h> header, can be set in 
  sa_flags:
  
  SA_NOCLDSTOP
  
  Do not generate SIGCHLD when children stop [XSI]   or stopped children 
  continue. 
  
  If sig is SIGCHLD and the SA_NOCLDSTOP flag is not set in sa_flags, and
  the implementation supports the SIGCHLD signal, then a SIGCHLD signal 
  shall be generated for the calling process whenever any of its child 
  processes stop [XSI]   and a SIGCHLD signal may be generated for the 
  calling process whenever any of its stopped child processes are continued.   
  If sig is SIGCHLD and the SA_NOCLDSTOP flag is set in sa_flags, then the 
  implementation shall not generate a SIGCHLD signal in this way.
  
  SA_ONSTACK
  
  If set and an alternate signal stack has been declared with sigaltstack(), 
  the signal shall be delivered to the calling process on that stack. 
  Otherwise, the signal shall be delivered on the current stack. 
  
  SA_RESETHAND
  
  If set, the disposition of the signal shall be reset to SIG_DFL and the 
  SA_SIGINFO flag shall be cleared on entry to the signal handler.
  
  Note:
  
  SIGILL and SIGTRAP cannot be automatically reset when delivered; the system 
  silently enforces this restriction.
  
  Otherwise, the disposition of the signal shall not be modified on entry to 
  the signal handler.
  
  In addition, if this flag is set, sigaction() may behave as if the SA_NODEFER 
  flag were also set.
  
  SA_RESTART
  
  This flag affects the behavior of interruptible functions; that is, those 
  specified to fail with errno set to [EINTR]. If set, and a function 
  specified as interruptible is interrupted by this signal, the function shall
  restart and shall not fail with [EINTR] unless otherwise specified. If an 
  interruptible function which uses a timeout is restarted, the duration of 
  the timeout following the restart is set to an unspecified value that does 
  not exceed the original timeout value. If the flag is not set, interruptible 
  functions interrupted by this signal shall fail with errno set to [EINTR].
  
  SA_SIGINFO
  
  If cleared and the signal is caught, the signal-catching function shall be 
  entered as:
  
  void func(int signo);
  
  where signo is the only argument to the signal-catching function. In this 
  case, the application shall use the sa_handler member to describe the 
  signal-catching function and the application shall not modify the 
  sa_sigaction member.
  
  If SA_SIGINFO is set and the signal is caught, the signal-catching function
  shall be entered as:
  
  void func(int signo, siginfo_t *info, void *context);
  
  where two additional arguments are passed to the signal-catching function. 
  The second argument shall point to an object of type siginfo_t explaining 
  the reason why the signal was generated; the third argument can be cast to 
  a pointer to an object of type ucontext_t to refer to the receiving thread's 
  context that was interrupted when the signal was delivered. In this case, 
  the application shall use the sa_sigaction member to describe the 
  signal-catching function and the application shall not modify the sa_handler 
  member.
  
  The si_signo member contains the system-generated signal number.
  
  The si_errno member may contain implementation-defined additional 
  error information; if non-zero, it contains an error number identifying 
  the condition that caused the signal to be generated. 
  
  The si_code member contains a code identifying the cause of the signal, as
  described in Signal Actions .
  
  SA_NOCLDWAIT
  
  If set, and sig equals SIGCHLD, child processes of the calling processes 
  shall not be transformed into zombie processes when they terminate. If the 
  calling process subsequently waits for its children, and the process has 
  no unwaited-for children that were transformed into zombie processes, it 
  shall block until all of its children terminate, and wait(), waitid(), and
  waitpid() shall fail and set errno to [ECHILD]. Otherwise, terminating 
  child processes shall be transformed into zombie processes, unless SIGCHLD 
  is set to SIG_IGN.
  
  SA_NODEFER
  
  If set and sig is caught, sig shall not be added to the thread's signal mask
  on entry to the signal handler unless it is included in sa_mask. Otherwise, 
  sig shall always be added to the thread's signal mask on entry to the signal 
  handler.
  
  When a signal is caught by a signal-catching function installed by 
  sigaction(), a new signal mask is calculated and installed for the duration 
  of the signal-catching function (or until a call to either sigprocmask()
  or sigsuspend() is made). This mask is formed by taking the union of the 
  current signal mask and the value of the sa_mask for the signal being 
  delivered, and unless SA_NODEFER or SA_RESETHAND is set, then including 
  the signal being delivered. If and when the user's signal handler returns 
  normally, the original signal mask is restored.
  
  Once an action is installed for a specific signal, it shall remain installed 
  until another action is explicitly requested (by another call to sigaction()), 
  until the SA_RESETHAND flag causes resetting of the handler, or until one 
  of the exec functions is called.
  
  If the previous action for sig had been established by signal(), the values 
  of the fields returned in the structure pointed to by oact are unspecified, 
  and in particular oact-> sa_handler is not necessarily the same value passed 
  to signal(). However, if a pointer to the same structure or a copy thereof 
  is passed to a subsequent call to sigaction() via the act argument, handling 
  of the signal shall be as if the original call to signal() were repeated.
  
  If sigaction() fails, no new signal handler is installed.
  
  It is unspecified whether an attempt to set the action for a signal that 
  cannot be caught or ignored to SIG_DFL is ignored or causes an error to be
  returned with errno set to [EINVAL].
  
  If SA_SIGINFO is not set in sa_flags, then the disposition of subsequent 
  occurrences of sig when it is already pending is implementation-defined; 
  the signal-catching function shall be invoked with a single argument. If 
  SA_SIGINFO is set in sa_flags, then subsequent occurrences of sig generated 
  by sigqueue() or as a result of any signal-generating function that supports 
  the specification of an application-defined value (when sig is already 
  pending) shall be queued in FIFO order until delivered or accepted; the 
  signal-catching function shall be invoked with three arguments. The 
  application specified value is passed to the signal-catching function as 
  the si_value member of the siginfo_t structure.
  
  The result of the use of sigaction() and a sigwait() function concurrently 
  within a process on the same signal is unspecified.
  
  RETURN VALUE
  
  Upon successful completion, sigaction() shall return 0; otherwise, -1 shall 
  be returned, errno shall be set to indicate the error, and no new signal-
  catching function shall be installed.
  
  ERRORS
  
  The sigaction() function shall fail if:
  
  [EINVAL]
  The sig argument is not a valid signal number or an attempt is made to catch 
  a signal that cannot be caught or ignore a signal that cannot be ignored.
  
  [ENOTSUP]
  
  The SA_SIGINFO bit flag is set in the sa_flags field of the sigaction 
  structure.
  
  The sigaction() function may fail if:
  
  [EINVAL]
  
  An attempt was made to set the action to SIG_DFL for a signal that cannot 
  be caught or ignored (or both).
  
  In addition, the sigaction() function may fail if the SA_SIGINFO flag is 
  set in the sa_flags field of the sigaction structure for a signal not in
  the range SIGRTMIN to SIGRTMAX.
*/

int sigaction(int sig, const struct sigaction * act,
       struct sigaction * oact)
    {
    sched_thread_t * curr = kurrent;
    struct sig_handler_node * signode = NULL;

    LIST_FOREACH(&curr->sig_handler_list, iter)
        {
        if (iter)
            {
            signode = LIST_ENTRY(iter, struct sig_handler_node, node);

            if (signode && signode->sig == sig)
                break;

            signode = NULL;
            }
        }

    if (signode == NULL)
        {
        signode = kmalloc(sizeof(struct sig_handler_node));
        if (signode == NULL)
            {
            curr->err = ENOMEM;
            
            return ERROR;
            }
        
        memset(signode, 0, sizeof(struct sig_handler_node));
        }

    if (oact) 
        *oact = signode->sa_action;
    
    signode->sa_action = *act;
    signode->sig = sig;

    list_append(&curr->sig_handler_list, &signode->node);
    
    return OK;
    }

/* Called under thread lock */
void sched_thread_signal_process (struct sched_thread * thread)
    {
    struct sig_handler_node * signode = NULL;

    if (LIST_EMPTY(&thread->sig_handler_list))
        {
        return;
        }
        
    LIST_FOREACH(&thread->sig_handler_list, iter)
        {
        if (iter == NULL)
            continue;
                
        signode = LIST_ENTRY(iter, struct sig_handler_node, node);

        if (signode == NULL)
            continue;
                
        if (atomic_test_and_clear_bit(signode->sig, &thread->sig_pending) &&
            (signode->sa_action.sa_handler != NULL))
            {            
            thread->sig_blocked = signode->sa_action.sa_mask;
            signode->sa_action.sa_handler(signode->sig);
            thread->sig_blocked = 0;
            }
        }
    }
