/* signal.h - signals */

/*
APPLICATION USAGE

    On systems not supporting the XSI option, the si_pid and si_uid
    members of siginfo_t are only required to be valid when si_code
    is SI_USER or SI_QUEUE. On XSI-conforming systems, they are also
    valid for all si_code values less than or equal to 0; however,
    it is unspecified whether SI_USER and SI_QUEUE have values less
    than or equal to zero, and therefore XSI applications should 
    check whether si_code has the value SI_USER or SI_QUEUE or is 
    less than or equal to 0 to tell whether si_pid and si_uid are 
    valid.
*/
#ifndef _POSIX_SIGNAL_H
#define _POSIX_SIGNAL_H

/*
 * The <signal.h> header shall define the pthread_t,
 * size_t, pid_t, pthread_attr_t, and uid_t types 
 * as described in <sys/types.h>
 */
 
#include <sys/types.h>
#include <stdint.h>
#include <pthread.h>

/*
 * The <signal.h> header shall define the timespec structure 
 * as described in <time.h>.
 */

#include <time.h>

/*
 * sig_atomic_t
 *
 * Possibly volatile-qualified integer type of an object that
 * can be accessed as an atomic entity, even in the presence 
 * of asynchronous interrupts.
 */ 
 
typedef volatile long	sig_atomic_t;

/*
 * sigset_t
 *
 * Integer or structure type of an object used to represent 
 * sets of signals. 
 */
 
typedef ulong sigset_t;

/* The sigval union shall be defined */

union sigval 
    {
    int    sival_int;    /* Integer signal value. */
    void  *sival_ptr;    /* Pointer signal value. */
    };

/*
 * The <signal.h> header shall define the sigevent structure
 */

typedef struct sigevent
    {
    int              sigev_notify; /* Notification type. */
    int              sigev_signo;  /* Signal number. */
    union sigval     sigev_value;  /* Signal value. */
    void           (*sigev_notify_function)(union sigval);
                                   /* Notification function. */
    pthread_attr_t *sigev_notify_attributes;  
                                   /* Notification attributes. */

    }sigevent_t;

/* 
 * The <signal.h> header shall define the following symbolic 
 * constants for the values of sigev_notify:
 *
 * SIGEV_NONE
 *
 * No asynchronous notification is delivered when the event 
 * of interest occurs.
 *
 * SIGEV_SIGNAL
 *
 * A queued signal, with an application-defined value, is 
 * generated when the event of interest occurs.
 *
 * SIGEV_THREAD
 *
 * A notification function is called to perform notification.
 */
 
#define  SIGEV_NONE     0x00
#define  SIGEV_SIGNAL   0x01
#define  SIGEV_THREAD   0x02

/*
 * The <signal.h> header shall declare the SIGRTMIN and SIGRTMAX
 * macros, which shall expand to positive integer expressions 
 * with type int, but which need not be constant expressions.
 * These macros specify a range of signal numbers that are
 * reserved for application use and for which the realtime 
 * signal behavior specified in this volume of POSIX.1-2008 is
 * supported. The signal numbers in this range do not overlap 
 * any of the signals specified in the following table.
 *
 * The range SIGRTMIN through SIGRTMAX inclusive shall include 
 * at least {RTSIG_MAX} signal numbers.
 *
 * It is implementation-defined whether realtime signal behavior
 * is supported for other signals. 
 *
 * The real-time signals, ranging from SIGRTMIN to SIGRTMAX, are 
 * a set of signals that can be used for application-defined 
 * purposes. Because SIGRTMIN may have different values on different
 * Unix-like systems, applications should always refer to the 
 * signals in the form SIGRTMIN+n, where n is a constant integer 
 * expression. The real-time signals have a number of properties
 * that differentiate them from other signals and make them suitable 
 * for application-defined purposes:
 *
 * - Multiple instances of a real-time signal can be sent to a 
 *   process and all will be delivered.
 *
 * - Real-time signals can be accompanied by an integer or pointer
 *   value (see sigqueue).
 *
 * - Real-time signals are guaranteed to be delivered in the order 
 *   they were emitted.
 */

#define SIGRTMIN        (uint32_t)os_get_rt_sig_min()
#define SIGRTMAX        (uint32_t)os_get_rt_sig_max()

/* GNU-like signal handler typedef */

typedef void (*sighandler_t)(int); 

/*
 * Macros defining the standard signal handling behavior.
 *
 * The <signal.h> header shall define the following macros, 
 * which shall expand to constant expressions with distinct
 * values that have a type compatible with the second argument
 * to, and the return value of, the signal() function, and
 * whose values shall compare unequal to the address of any
 * declarable function.
 */
 
#define SIG_DFL		((sighandler_t)0)	/* Request for default signal handling.*/
#define SIG_IGN		((sighandler_t)1)	/* Request that signal be ignored. */
#define SIG_ERR		((sighandler_t)-1)	/* Return value from signal() in case of error. */
#define SIG_HOLD	((sighandler_t)3)	/* Request that signal be held. */

/* The <signal.h> header shall define the siginfo_t type as a structure */
typedef struct siginfo
    {
	int		si_signo;	/* Signal number */
	int		si_code;	/* Signal code */
	int		si_errno;	/* If non zero, an error number associated with this signal */
	pid_t	si_pid;		/* Sending process ID */
	uid_t	si_uid;		/* Real user ID of sending process */
	void	*si_addr;	/* Address of faulting instruction */
	int		si_status;	/* Exit value or signal */
	long	si_band;	/* Band event for SIGPOLL */
    } siginfo_t;

/*
 * The <signal.h> header shall declare the sigaction structure
 * which will be the structure used by sigaction().
 *
 * NOTE:
 *
 * The storage occupied by sa_handler and sa_sigaction may 
 * overlap, and a conforming application shall not use both 
 * simultaneously.
 */
 
struct sigaction 
    {
    /* 
     * Pointer to a signal-catching function or
     * one of the SIG_IGN or SIG_DFL. 
     */
	sighandler_t sa_handler;

    /* 
     * Set of signals to be blocked during execution 
     * of the signal handling function. 
     */
	sigset_t	sa_mask;
    
    /* Special flags. */
	int			sa_flags;

    /* Pointer to a signal-catching function. */
    void   (*sa_sigaction)(int, siginfo_t *, void *);
    };

struct sig_handler_node
    {
    list_t node;
    int sig;
    struct sigaction sa_action;
    };

struct	sched_thread;

void sched_thread_signal_process (struct sched_thread * thread);

/* Values for sa_flags */

#define SA_NOCLDSTOP	0x01 /* Do not generate SIGCHLD when children 
                              * stop or stopped children continue.*/
#define SA_NOCLDWAIT	0x02 /* Causes implementations not to create 
                              * zombie processes on child death.*/
#define SA_RESETHAND	0x04 /* Causes signal dispositions to be set 
                              * to SIG_DFL on entry to signal handlers.*/
#define SA_NODEFER		0x08 /* Causes signal not to be automatically 
                              * blocked on entry to signal handler.*/
#define SA_RESTART		0x10 /* Causes certain functions to become restartable. */
#define SA_ONSTACK		0x20 /* Causes signal delivery to occur on
                              * an alternate stack.*/
#define SA_SIGINFO		0x40 /* Causes extra information to be passed to 
                              * signal handlers at the time of receipt of a signal.*/
#define SA_NOMASK		SA_NODEFER
#define SA_STACK		SA_ONSTACK
#define SA_ONESHOT		SA_RESETHAND

/* Values for ss_flags */

#define SS_ONSTACK		0x1 /* Process is executing on an alternate signal stack. */
#define SS_DISABLE		0x2 /* Alternate signal stack is disabled.  */

#define MINSIGSTKSZ		4096 /* Minimum stack size for a signal handler. */
#define SIGSTKSZ		16384 /* Default size in bytes for the alternate signal stack.*/

/*
 * The <signal.h> header shall define the stack_t type 
 * as a structure for signals using an alternate stack
 */
 
typedef struct stack_t 
    {
	void	*ss_sp;     /* Stack base or pointer. */
	size_t	ss_size;    /* Stack size. */
	int		ss_flags;   /* Flags. */
    } stack_t;

typedef struct sigstack 
    {
	int 	ss_onstack;
	void	*ss_sp;
    } sigstack_t;

/*
 * Macros for the 'how' arg of sigprocmask() 
 *
 * The <signal.h> header shall define the following macros
 * which shall expand to integer constant expressions that 
 * need not be usable in #if preprocessing directives.
 *
 * SIG_BLOCK
 *
 * The resulting set is the union of the current set and 
 * the signal set pointed to by the argument set. 
 *
 * SIG_UNBLOCK
 *
 * The resulting set is the intersection of the current
 * set and the complement of the signal set pointed to by 
 * the argument set. 
 *
 * SIG_SETMASK
 *
 * The resulting set is the signal set pointed to by the
 * argument set. 
 */
 
#define SIG_BLOCK		1
#define SIG_UNBLOCK		2
#define SIG_SETMASK		3

/*
 * The <signal.h> header shall define the following macros that
 * are used to refer to the signals that occur in the system. 
 * Signals defined here begin with the letters SIG followed by 
 * an uppercase letter. The macros shall expand to positive 
 * integer constant expressions with type int and distinct 
 * values. The value 0 is reserved for use as the null signal 
 * (see kill()). Additional implementation-defined signals may 
 * occur in the system.
 *
 * The ISO C standard only requires the signal names SIGABRT, 
 * SIGFPE, SIGILL, SIGINT, SIGSEGV, and SIGTERM to be defined. 
 */
#define	SIGNULL		0	/* Null signal */ 
#define	SIGHUP		1	/* Hangup */
#define SIGINT		2	/* Terminal interrupt signal. */
#define SIGQUIT		3	/* Terminal quit signal.  */
#define SIGILL		4	/* Illegal instruction. */
#define SIGCHLD		5	/* Child process terminated, stopped, or continued. */
#define SIGABRT		6	/* Process abort signal. */
#define SIGPIPE		7	/* Write on a pipe with no one to read it. */
#define SIGFPE		8	/* Erroneous arithmetic operation. */
#define SIGKILL		9	/* Kill (cannot be caught or ignored). */
#define SIGSTOP		10	/* Stop executing (cannot be caught or ignored). */
#define SIGSEGV		11	/* Invalid memory reference. */
#define SIGCONT		12	/* Continue executing, if stopped. */
#define SIGTSTP		13	/* Terminal stop signal. */
#define SIGALRM		14	/* Alarm clock. */
#define SIGTERM		15	/* Termination signal. */
#define SIGTTIN		16	/* Background process attempting read. */
#define SIGTTOU		17	/* Background process attempting write. */
#define SIGUSR1		18	/* User-defined signal 1. */
#define SIGUSR2		19	/* User-defined signal 2. */
#define SIGWINCH	20	/* tty window size changed */
#define SIGKILLTHR	21	/* Specificly kill just the thread, not team */
#define SIGTRAP		22	/* Trace/breakpoint trap */
#define SIGPOLL		23	/* Pollable event. */
#define SIGPROF		24	/* Profiling timer expired */
#define SIGSYS		25	/* Bad system call */
#define SIGURG		26	/* High bandwidth data is available at socket */
#define SIGVTALRM	27	/* Virtual timer expired */
#define SIGXCPU		28	/* CPU time limit exceeded */
#define SIGXFSZ		29	/* File size limit exceeded */

#define SIGBUS		SIGSEGV /* Access to an undefined portion of a memory object. */

/*
 * Signal numbers 30-32 are currently free but may be used in future
 * releases.  Use them at your own peril (if you do use them, at least
 * be smart and use them backwards from signal 32).
 */
 
#define MAX_SIGNO		64	    /* the most signals that a single thread can reference */
#define __signal_max	29	    /* the largest signal number that is actually defined */
#define NSIG (__signal_max+1)   /* the number of defined signals */

/* The global table of text strings containing descriptions for each signal */
extern const char * const sys_siglist[NSIG];

/*
 * The <signal.h> header shall define the symbolic constants 
 * for use as values of si_code that are signal-specific or 
 * non-signal-specific reasons why the signal was generated.
 *
 * Implementations may support additional si_code values not 
 * included in this list, may generate values included in 
 * this list under circumstances other than those described 
 * in this list, and may contain extensions or limitations 
 * that prevent some values from being generated. 
 *
 * Implementations do not generate a different value from
 * the ones described in this list for circumstances described
 * in this list.
 *
 * In addition, the following signal-specific information shall 
 * be available.
 *
 * SIGILL, SIGFPE - void * si_addr - Address of faulting instruction.
 *
 * SIGSEGV, SIGBUS - void * si_addr - Address of faulting memory reference.
 *
 * SIGCHLD - pid_t si_pid - Child process ID.
 *         - int si_status - Exit value or signal.
 *         - uid_t si_uid - Real user ID of the process that sent the signal.
 * SIGPOLL - long si_band - Band event for POLL_IN, POLL_OUT, or POLL_MSG.
 *
 * For some implementations, the value of si_addr may be inaccurate.
 */

/* 
 * Values for `si_code'. 
 *
 * Positive values are reserved for kernel-generated signals.  
 */
enum
    {
    SI_ASYNCNL = -60,		/* Sent by asynch name lookup completion.  */
#define SI_ASYNCNL	SI_ASYNCNL
    SI_TKILL = -6,		    /* Sent by tkill.  */
#define SI_TKILL	SI_TKILL
    SI_SIGIO,			    /* Sent by queued SIGIO. */
#define SI_SIGIO	SI_SIGIO
    SI_ASYNCIO,			    /* Sent by AIO completion.  */
#define SI_ASYNCIO	SI_ASYNCIO
    SI_MESGQ,			    /* Sent by real time mesq state change.  */
#define SI_MESGQ	SI_MESGQ
    SI_TIMER,			    /* Sent by timer expiration.  */
#define SI_TIMER	SI_TIMER
    SI_QUEUE,			    /* Sent by sigqueue.  */
#define SI_QUEUE	SI_QUEUE
    SI_USER,			    /* Sent by kill, sigsend, raise.  */
#define SI_USER	SI_USER
    SI_KERNEL = 0x80		/* Send by kernel.  */
#define SI_KERNEL	SI_KERNEL
    };

/* `si_code' values for SIGILL signal.  */
enum
    {
    ILL_ILLOPC = 1,		/* Illegal opcode.  */
#define ILL_ILLOPC	ILL_ILLOPC
    ILL_ILLOPN,			/* Illegal operand.  */
#define ILL_ILLOPN	ILL_ILLOPN
    ILL_ILLADR,			/* Illegal addressing mode.  */
#define ILL_ILLADR	ILL_ILLADR
    ILL_ILLTRP,			/* Illegal trap. */
#define ILL_ILLTRP	ILL_ILLTRP
    ILL_PRVOPC,			/* Privileged opcode.  */
#define ILL_PRVOPC	ILL_PRVOPC
    ILL_PRVREG,			/* Privileged register.  */
#define ILL_PRVREG	ILL_PRVREG
    ILL_COPROC,			/* Coprocessor error.  */
#define ILL_COPROC	ILL_COPROC
    ILL_BADSTK			/* Internal stack error.  */
#define ILL_BADSTK	ILL_BADSTK
    };

/* `si_code' values for SIGFPE signal.  */
enum
    {
    FPE_INTDIV = 1,		/* Integer divide by zero.  */
#define FPE_INTDIV	FPE_INTDIV
    FPE_INTOVF,			/* Integer overflow.  */
#define FPE_INTOVF	FPE_INTOVF
    FPE_FLTDIV,			/* Floating point divide by zero.  */
#define FPE_FLTDIV	FPE_FLTDIV
    FPE_FLTOVF,			/* Floating point overflow.  */
#define FPE_FLTOVF	FPE_FLTOVF
    FPE_FLTUND,			/* Floating point underflow.  */
#define FPE_FLTUND	FPE_FLTUND
    FPE_FLTRES,			/* Floating point inexact result.  */
#define FPE_FLTRES	FPE_FLTRES
    FPE_FLTINV,			/* Floating point invalid operation.  */
#define FPE_FLTINV	FPE_FLTINV
    FPE_FLTSUB			/* Subscript out of range.  */
#define FPE_FLTSUB	FPE_FLTSUB
    };

/* `si_code' values for SIGSEGV signal.  */
enum
    {
    SEGV_MAPERR = 1,		/* Address not mapped to object.  */
#define SEGV_MAPERR	SEGV_MAPERR
    SEGV_ACCERR			/* Invalid permissions for mapped object.  */
#define SEGV_ACCERR	SEGV_ACCERR
    };

/* `si_code' values for SIGBUS signal.  */
enum
    {
    BUS_ADRALN = 1,		/* Invalid address alignment.  */
#define BUS_ADRALN	BUS_ADRALN
    BUS_ADRERR,			/* Non-existant physical address.  */
#define BUS_ADRERR	BUS_ADRERR
    BUS_OBJERR			/* Object specific hardware error.  */
#define BUS_OBJERR	BUS_OBJERR
    };

/* `si_code' values for SIGTRAP signal.  */
enum
    {
    TRAP_BRKPT = 1,		/* Process breakpoint.  */
#define TRAP_BRKPT	TRAP_BRKPT
    TRAP_TRACE			/* Process trace trap.  */
#define TRAP_TRACE	TRAP_TRACE
    };

/* `si_code' values for SIGCHLD signal.  */
enum
    {
    CLD_EXITED = 1,		/* Child has exited.  */
#define CLD_EXITED	CLD_EXITED
    CLD_KILLED,			/* Child was killed.  */
#define CLD_KILLED	CLD_KILLED
    CLD_DUMPED,			/* Child terminated abnormally.  */
#define CLD_DUMPED	CLD_DUMPED
    CLD_TRAPPED,		/* Traced child has trapped.  */
#define CLD_TRAPPED	CLD_TRAPPED
    CLD_STOPPED,		/* Child has stopped.  */
#define CLD_STOPPED	CLD_STOPPED
    CLD_CONTINUED		/* Stopped child has continued.  */
#define CLD_CONTINUED	CLD_CONTINUED
    };

/* `si_code' values for SIGPOLL signal.  */
enum
    {
    POLL_IN = 1,	    /* Data input available.  */
#define POLL_IN	POLL_IN
    POLL_OUT,			/* Output buffers available.  */
#define POLL_OUT	POLL_OUT
    POLL_MSG,			/* Input message available.   */
#define POLL_MSG	POLL_MSG
    POLL_ERR,			/* I/O error.  */
#define POLL_ERR	POLL_ERR
    POLL_PRI,			/* High priority input available.  */
#define POLL_PRI	POLL_PRI
    POLL_HUP			/* Device disconnected.  */
#define POLL_HUP	POLL_HUP
    };

#if defined(CONFIG_CPU_ARCH_X86)
#  include <arch/x86/signal.h>
#elif defined(CONFIG_CPU_ARCH_X64)
#  include <arch/x86/x64/signal.h>
#elif defined(CONFIG_CPU_ARCH_ARM)
#  include <arch/arm/signal.h>
#elif defined(CONFIG_CPU_ARCH_PPC)
#  include <arch/ppc/signal.h>
#else
#  error There is no ARCH signal.h for this architecture!
#endif


/* The <signal.h> header shall define the ucontext_t type as a structure */

typedef struct ucontext
    {
    /* Pointer to the context that is resumed when this context returns. */
    struct ucontext *uc_link;  
    
    /* The set of signals that are blocked when this context is active. */
    sigset_t    uc_sigmask; 
    
    /*  The stack used by this context. */
    stack_t     uc_stack;   

    /* A machine-specific representation of the saved context. */
    struct mcontext  uc_mcontext; 
    }ucontext_t;


#ifdef __cplusplus
extern "C" {
#endif

/* 
 * int    sigaddset(sigset_t *, int);
 */
#define sigaddset(set, sig) atomic_set_bit(sig, set)

/*
 * int    sigdelset(sigset_t *, int);
 */
#define sigdelset(set, sig) atomic_clear_bit(sig, set)

/*
 * int    sigismember(const sigset_t *, int);
 */
#define sigismember(set, sig) atomic_test_bit(sig, set)

/*
 * int    sigemptyset(sigset_t *);
 */
#define sigemptyset(set) do { *(set) = 0; } while (0)

/*
 * int    sigfillset(sigset_t *);
 */
#define sigfillset(set) do { *(set) = ~0; } while (0)

int    kill(pid_t, int);

int    killpg(pid_t, int);

void   psiginfo(const siginfo_t *, const char *);
void   psignal(int, const char *);
int    pthread_kill(pthread_t, int);
int    pthread_sigmask(int, const sigset_t *restrict,
           sigset_t *restrict);

int    raise(int);
int    sigaction(int, const struct sigaction *restrict,
           struct sigaction *restrict);

int    sigaltstack(const stack_t *restrict, stack_t *restrict);

int    sighold(int);
int    sigignore(int);
int    siginterrupt(int, int);

void (*signal(int, void (*)(int)))(int);
int    sigpause(int);

int    sigpending(sigset_t *);
int    sigprocmask(int, const sigset_t *restrict, sigset_t *restrict);
int    sigqueue(pid_t, int, const union sigval);

int    sigrelse(int);
void (*sigset(int, void (*)(int)))(int);

int    sigsuspend(const sigset_t *);
int    sigtimedwait(const sigset_t *restrict, siginfo_t *restrict,
           const struct timespec *restrict);
int    sigwait(const sigset_t *restrict, int *restrict);
int    sigwaitinfo(const sigset_t *restrict, siginfo_t *restrict);

#ifdef __cplusplus
}
#endif

#endif /* _POSIX_SIGNAL_H */
