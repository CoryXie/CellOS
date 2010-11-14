/* limits.h - implementation-defined constants */

/*
DESCRIPTION

    The <limits.h> header shall define macros and symbolic constants
    for various limits. Different categories of limits are described
    below, representing various limits on resources that the
    implementation imposes on applications. All macros and symbolic
    constants defined in this header shall be suitable for use in #if
    preprocessing directives.

    Implementations may choose any appropriate value for each limit,
    provided it is not more restrictive than the Minimum Acceptable
    Values listed below. Symbolic constant names beginning with
    _POSIX may be found in <unistd.h>.

    Applications should not assume any particular value for a limit.
    To achieve maximum portability, an application should not require
    more resource than the Minimum Acceptable Value quantity. However,
    an application wishing to avail itself of the full amount of a
    resource available on an implementation may make use of the value
    given in <limits.h> on that particular implementation, by using
    the macros and symbolic constants listed below. It should be noted,
    however, that many of the listed limits are not invariant, and at
    runtime, the value of the limit may differ from those given in
    this header, for the following reasons:

    - The limit is pathname-dependent.

    - The limit differs between the compile and runtime machines.

    For these reasons, an application may use the fpathconf(), pathconf(),
    and sysconf() functions to determine the actual value of a limit
    at runtime.

    The items in the list ending in _MIN give the most negative values
    that the mathematical types are guaranteed to be capable of
    representing. Numbers of a more negative value may be supported on
    some implementations, as indicated by the <limits.h> header on the
    implementation, but applications requiring such numbers are not
    guaranteed to be portable to all implementations. For positive
    constants ending in _MIN, this indicates the minimum acceptable
    value.


RATIONALE

    A request was made to reduce the value of {_POSIX_LINK_MAX} from
    the value of 8 specified for it in the POSIX.1-1990 standard to 2.
    The standard developers decided to deny this request for several
    reasons:

    - They wanted to avoid making any changes to the standard that
    could break conforming applications, and the requested change
    could have that effect.

    - The use of multiple hard links to a file cannot always be
    replaced with use of symbolic links. Symbolic links are semantically
    different from hard links in that they associate a pathname with
    another pathname rather than a pathname with a file. This has
    implications for access control, file permanence, and transparency.

    - The original standard developers had considered the issue of
    allowing for implementations that did not in general support
    hard links, and decided that this would reduce consensus on
    the standard.

    Systems that support historical versions of the development option
    of the ISO POSIX-2 standard retain the name {_POSIX2_RE_DUP_MAX}
    as an alias for {_POSIX_RE_DUP_MAX}.

    {PATH_MAX}
        IEEE PASC Interpretation 1003.1 #15 addressed the inconsistency
        in the standard with the definition of pathname and the
        description of {PATH_MAX}, allowing application developers to
        allocate either {PATH_MAX} or {PATH_MAX}+1 bytes. The
        inconsistency has been removed by correction to the {PATH_MAX}
        definition to include the null character. With this change,
        applications that previously allocated {PATH_MAX} bytes will
        continue to succeed.

    {SYMLINK_MAX}
        This symbol refers to space for data that is stored in the
        file system, as opposed to {PATH_MAX} which is the length
        of a name that can be passed to a function. In some existing
        implementations, the filenames pointed to by symbolic links
        are stored in the inodes of the links, so it is important
        that {SYMLINK_MAX} not be constrained to be as large as
        {PATH_MAX}.
*/

#ifndef _POSIX_LIMITS_H
#define _POSIX_LIMITS_H

#include <arch.h>

/*
 * Maximum Values
 *
 * The <limits.h> header shall define the following symbolic constants
 * with the values shown. These are the most restrictive values for
 * certain features on an implementation. A conforming implementation
 * shall provide values no larger than these values. A conforming
 * application must not require a smaller value for correct operation.
 */

/*
 * The resolution of the CLOCK_REALTIME clock, in nanoseconds.
 *
 * Note:
 *
 * If the Monotonic Clock option is supported, the resolution of
 *   the CLOCK_MONOTONIC clock, in nanoseconds, is represented by
 *   {_POSIX_CLOCKRES_MIN}.
 */
#define _POSIX_CLOCKRES_MIN             0x20000000

/*
 * Minimum Values
 *
 * The <limits.h> header shall define the following symbolic constants
 * with the values shown. These are the most restrictive values for
 * certain features on an implementation conforming to this volume
 * of POSIX.1-2008. Related symbolic constants are defined elsewhere
 * in this volume of POSIX.1-2008 which reflect the actual implementation
 * and which need not be as restrictive. A conforming implementation
 * shall provide values at least this large. A strictly conforming
 * application must not require a larger value for correct operation.
 */

/*
 * The number of I/O operations that can be specified in a list I/O call.
 */
#define _POSIX_AIO_LISTIO_MAX               2

/*
 * The number of outstanding asynchronous I/O operations.
 */
#define _POSIX_AIO_MAX                      1

/*
 * Maximum length of argument to the exec functions including
 * environment data.
 */
#define _POSIX_ARG_MAX                      4096

/*
 * Maximum number of simultaneous processes per real user ID.
 */
#define _POSIX_CHILD_MAX                    25

/*
 * The number of timer expiration overruns.
 */
#define _POSIX_DELAYTIMER_MAX               32

/*
 * Maximum length of a host name (not including the terminating null)
 * as returned from the gethostname() function.
 */
#define _POSIX_HOST_NAME_MAX                255

/* Maximum number of links to a single file.*/
#define _POSIX_LINK_MAX                     8

/*
 * The size of the storage required for a login name, in bytes
 * (including the terminating null).
 */
#define _POSIX_LOGIN_NAME_MAX               9

/*
 * Maximum number of bytes in a terminal canonical input queue.
 */
#define _POSIX_MAX_CANON                    255

/*
 * Maximum number of bytes allowed in a terminal input queue.
 */
#define _POSIX_MAX_INPUT                    255

/*
 * The number of message queues that can be open for a single process.
 */
#define _POSIX_MQ_OPEN_MAX                  8

/*
 * The maximum number of message priorities supported by the implementation.
 */
#define _POSIX_MQ_PRIO_MAX                  32

/*
 * Maximum number of bytes in a filename (not including the terminating null).
 */
#define _POSIX_NAME_MAX                     32

/*
 * Maximum number of simultaneous supplementary group IDs per process.
 */
#define _POSIX_NGROUPS_MAX                  8

/*
 * Maximum number of files that one process can have open at any one time.
 */
#define _POSIX_OPEN_MAX                     20

/*
 * Minimum number the implementation will accept as the maximum number
 * of bytes in a pathname.
 */
#define _POSIX_PATH_MAX                     256

/*
 * Maximum number of bytes that is guaranteed to be atomic when
 * writing to a pipe.
 */
#define _POSIX_PIPE_BUF                     512

/*
 * The number of repeated occurrences of a BRE permitted by the
 * regexec() and regcomp() functions when using the interval
 * notation {\(m,n\}; see BREs Matching Multiple Characters .
 */
#define _POSIX_RE_DUP_MAX                   255

/*
 * The number of realtime signal numbers reserved for application use.
 */
#define _POSIX_RTSIG_MAX                    8

/*
 * The number of semaphores that a process may have.
 */
#define _POSIX_SEM_NSEMS_MAX                256

/*
 * The maximum value a semaphore may have.
 */
#define _POSIX_SEM_VALUE_MAX                32767

/*
 * The number of queued signals that a process may send and have
 * pending at the receiver(s) at any time.
 */
#define _POSIX_SIGQUEUE_MAX                 32

/*
 * The value that can be stored in an object of type ssize_t.
 */
#define _POSIX_SSIZE_MAX                    32767

/*
 * The number of replenishment operations that may be simultaneously
 * pending for a particular sporadic server reschedule.
 */
#define _POSIX_SS_REPL_MAX                  4

/*
 * The number of streams that one process can have open at one time.
 */
#define _POSIX_STREAM_MAX                   8

/*
 * The number of bytes in a symbolic link.
 */
#define _POSIX_SYMLINK_MAX                  255

/*
 * The number of symbolic links that can be traversed in the resolution
 * of a pathname in the absence of a loop.
 */
#define _POSIX_SYMLOOP_MAX                  8

/*
 * The number of attempts made to destroy a thread's thread-specific
 * data values on thread exit.
 */
#define _POSIX_THREAD_DESTRUCTOR_ITERATIONS 4

/*
 * The number of data keys per process.
 */
#define _POSIX_THREAD_KEYS_MAX              128

/*
 * The number of threads per process.
 */
#define _POSIX_THREAD_THREADS_MAX           64

/*
 * The per-process number of timers.
 */
#define _POSIX_TIMER_MAX                    32

/*
 * The length in bytes of a trace event name (not including the
 * terminating null).
 */
#define _POSIX_TRACE_EVENT_NAME_MAX         30

/*
 * The length in bytes of a trace generation version string or a
 * trace stream name (not including the terminating null).
 */
#define _POSIX_TRACE_NAME_MAX               8

/*
 * The number of trace streams that may simultaneously exist in the system.
 */
#define _POSIX_TRACE_SYS_MAX                8

/*
 * The number of user trace event type identifiers that may
 * simultaneously exist in a traced process, including the predefined
 * user trace event POSIX_TRACE_UNNAMED_USER_EVENT.
 */
#define _POSIX_TRACE_USER_EVENT_MAX         32

/*
 * The size of the storage required for a terminal device name,
 * in bytes (including the terminating null).
 */
#define _POSIX_TTY_NAME_MAX                 9

/*
 * Maximum number of bytes supported for the name of a timezone
 * (not of the TZ variable).
 *
 * Note:
 *    The length given by {_POSIX_TZNAME_MAX} does not include the
 * quoting characters mentioned in Other Environment Variables.
 */
#define _POSIX_TZNAME_MAX                   6

/*
 * Maximum obase values allowed by the bc utility.
 */
#define _POSIX2_BC_BASE_MAX                 99

/*
 * Maximum number of elements permitted in an array by the bc utility.
 */
#define _POSIX2_BC_DIM_MAX                  2048

/*
 * Maximum scale value allowed by the bc utility.
 */
#define _POSIX2_BC_SCALE_MAX                99

/*
 * Maximum length of a string constant accepted by the bc utility.
 */
#define _POSIX2_BC_STRING_MAX               1000

/*
 * Maximum number of bytes in a character class name.
 */
#define _POSIX2_CHARCLASS_NAME_MAX          14

/*
 * Maximum number of weights that can be assigned to an entry of
 * the LC_COLLATE order keyword in the locale definition file;
 * see Locale.
 */
#define _POSIX2_COLL_WEIGHTS_MAX            2

/*
 * Maximum number of expressions that can be nested within parentheses
 * by the expr utility.
 */
#define _POSIX2_EXPR_NEST_MAX               32

/*
 * Unless otherwise noted, the maximum length, in bytes, of a
 * utility's input line (either standard input or another file),
 * when the utility is described as processing text files. The
 * length includes room for the trailing <newline>.
 */
#define _POSIX2_LINE_MAX                    2048

/*
 * Maximum number of repeated occurrences of a regular expression
 * permitted when using the interval notation \{m,n\}; see Regular
 * Expressions .
 */
#define _POSIX2_RE_DUP_MAX                  255

/*
 * Maximum number of iovec structures that one process has available
 * for use with readv() or writev().
 */
#define _XOPEN_IOV_MAX                      16

/*
 * Maximum number of bytes in a filename (not including the
 * terminating null).
 */
#define _XOPEN_NAME_MAX                     255

/*
 * Minimum number the implementation will accept as the maximum
 * number of bytes in a pathname.
 */
#define _XOPEN_PATH_MAX                     1024

/*
 * Runtime Invariant Values (Possibly Indeterminate)
 *
 * A definition of one of the symbolic constants in the following list
 * shall be omitted from <limits.h> on specific implementations where
 * the corresponding value is equal to or greater than the stated
 * minimum, but is unspecified.
 *
 * This indetermination might depend on the amount of available memory
 * space on a specific instance of a specific implementation. The
 * actual value supported by a specific instance shall be provided by
 * the sysconf() function.
 */

/*
 * Maximum number of I/O operations in a single list I/O call
 * supported by the implementation.
 *
 * Minimum Acceptable Value: _POSIX_AIO_LISTIO_MAX
 */
#define AIO_LISTIO_MAX          _POSIX_AIO_LISTIO_MAX

/*
 * Maximum number of outstanding asynchronous I/O operations
 * supported by the implementation.
 *
 * Minimum Acceptable Value: _POSIX_AIO_MAX
 */
#define AIO_MAX                 _POSIX_AIO_MAX

/*
 * The maximum amount by which a process can decrease its
 * asynchronous I/O priority level from its own scheduling
 * priority.
 *
 * Minimum Acceptable Value: 0
 */
#define AIO_PRIO_DELTA_MAX      0

/*
 * Maximum length of argument to the exec functions including
 * environment data.
 *
 * Minimum Acceptable Value: _POSIX_ARG_MAX
 */
#define ARG_MAX                 _POSIX_ARG_MAX

/*
 * Maximum number of functions that may be registered with atexit().
 *
 * Minimum Acceptable Value: 32
 */
#define ATEXIT_MAX              2147483647

/*
 * Maximum number of simultaneous processes per real user ID.
 *
 * Minimum Acceptable Value: _POSIX_CHILD_MAX
 */
#define CHILD_MAX               _POSIX_CHILD_MAX

/*
 * Maximum number of timer expiration overruns.
 *
 * Minimum Acceptable Value: _POSIX_DELAYTIMER_MAX
 */
#define DELAYTIMER_MAX          _POSIX_DELAYTIMER_MAX

/*
 * Maximum length of a host name (not including the terminating null)
 * as returned from the gethostname() function.
 *
 * Minimum Acceptable Value: _POSIX_DELAYTIMER_MAX
 */
#define HOST_NAME_MAX           _POSIX_HOST_NAME_MAX

/*
 * Maximum number of iovec structures that one process has available
 * for use with readv() or writev().
 *
 * Minimum Acceptable Value: _XOPEN_IOV_MAX
 */
#define IOV_MAX                 _XOPEN_IOV_MAX

/*
 * Maximum length of a login name.
 *
 * Minimum Acceptable Value: _POSIX_LOGIN_NAME_MAX
 */
#define LOGIN_NAME_MAX          _POSIX_LOGIN_NAME_MAX

/*
 * The maximum number of open message queue descriptors a process may hold.
 *
 * Minimum Acceptable Value: _POSIX_MQ_OPEN_MAX
 */
#define MQ_OPEN_MAX             _POSIX_MQ_OPEN_MAX

/*
 * The maximum number of message priorities supported by the implementation.
 *
 * Minimum Acceptable Value: _POSIX_MQ_PRIO_MAX
 */
#define MQ_PRIO_MAX             _POSIX_MQ_PRIO_MAX

/*
 * A value one greater than the maximum value that the system may assign
 * to a newly-created file descriptor.
 *
 * Minimum Acceptable Value: _POSIX_OPEN_MAX
 */
#define OPEN_MAX                _POSIX_OPEN_MAX

/* PAGE_SIZE maybe defined by ARCH specific headers */

#ifndef PAGE_SIZE 

/*
 * Size in bytes of a page.
 *
 * Minimum Acceptable Value: 1
 */
#define PAGESIZE                (1 << PAGE_SHIFT)

/*
 * Equivalent to {PAGESIZE}. If either {PAGESIZE} or {PAGE_SIZE}
 * is defined, the other is defined with the same value.
 *
 * Minimum Acceptable Value: 1
 */
#define PAGE_SIZE               PAGESIZE

#endif /* PAGE_SIZE*/

/*
 * Maximum number of attempts made to destroy a thread's thread-specific
 * data values on thread exit.
 *
 * Minimum Acceptable Value: _POSIX_THREAD_DESTRUCTOR_ITERATIONS
 */
#define PTHREAD_DESTRUCTOR_ITERATIONS  _POSIX_THREAD_DESTRUCTOR_ITERATIONS

/*
 * Maximum number of data keys that can be created by a process.
 *
 * Minimum Acceptable Value: _POSIX_THREAD_KEYS_MAX
 */
#define PTHREAD_KEYS_MAX        _POSIX_THREAD_KEYS_MAX

/*
 * Minimum size in bytes of thread stack storage.
 *
 * Minimum Acceptable Value: 0
 */
#define PTHREAD_STACK_MIN       4096
#define PTHREAD_STACK_MAX       0x100000

/*
 * Maximum number of threads that can be created per process.
 *
 * Minimum Acceptable Value: _POSIX_THREAD_THREADS_MAX
 */
#define PTHREAD_THREADS_MAX     _POSIX_THREAD_THREADS_MAX

/*
 * Maximum number of repeated occurrences of a BRE or ERE interval
 * expression; see BREs Matching Multiple Characters and EREs
 * Matching Multiple Characters .
 *
 * Minimum Acceptable Value: _POSIX2_RE_DUP_MAX
 */
#define RE_DUP_MAX              _POSIX2_RE_DUP_MAX

/*
 * Maximum number of realtime signals reserved for application
 * use in this implementation.
 *
 * Minimum Acceptable Value: _POSIX_RTSIG_MAX
 */
#define RTSIG_MAX               _POSIX_RTSIG_MAX

/*
 * Maximum number of semaphores that a process may have.
 *
 * Minimum Acceptable Value: _POSIX_SEM_NSEMS_MAX
 */
#define SEM_NSEMS_MAX           _POSIX_SEM_NSEMS_MAX

/*
 * The maximum value a semaphore may have.
 *
 * Minimum Acceptable Value: _POSIX_SEM_VALUE_MAX
 */
#define SEM_VALUE_MAX           _POSIX_SEM_VALUE_MAX

/*
 * Maximum number of queued signals that a process may send
 * and have pending at the receiver(s) at any time.
 *
 * Minimum Acceptable Value: _POSIX_SIGQUEUE_MAX
 */
#define SIGQUEUE_MAX            _POSIX_SIGQUEUE_MAX

/*
 * The maximum number of replenishment operations that may be
 * simultaneously pending for a particular sporadic server scheduler.
 *
 * Minimum Acceptable Value: _POSIX_SS_REPL_MAX
 */
#define SS_REPL_MAX             _POSIX_SS_REPL_MAX

/*
 * Maximum number of streams that one process can have open at one time.
 * If defined, it has the same value as {FOPEN_MAX} (see <stdio.h>).
 *
 * Minimum Acceptable Value: _POSIX_STREAM_MAX
 */
#define STREAM_MAX              _POSIX_STREAM_MAX

/*
 * Maximum number of symbolic links that can be reliably traversed in
 * the resolution of a pathname in the absence of a loop.
 *
 * Minimum Acceptable Value: _POSIX_STREAM_MAX
 */
#define SYMLOOP_MAX             _POSIX_SYMLOOP_MAX

/*
 * Maximum number of timers per process supported by the implementation.
 *
 * Minimum Acceptable Value: _POSIX_STREAM_MAX
 */
#define TIMER_MAX               _POSIX_TIMER_MAX

/*
 * Maximum length of the trace event name (not including the
 * terminating null).
 *
 * Minimum Acceptable Value: _POSIX_TRACE_EVENT_NAME_MAX
 */
#define TRACE_EVENT_NAME_MAX    _POSIX_TRACE_EVENT_NAME_MAX

/*
 * Maximum length of the trace generation version string or of
 * the trace stream name (not including the terminating null).
 *
 * Minimum Acceptable Value: _POSIX_TRACE_NAME_MAX
 */
#define TRACE_NAME_MAX          _POSIX_TRACE_NAME_MAX

/*
 * Maximum number of trace streams that may simultaneously exist
 * in the system.
 *
 * Minimum Acceptable Value: _POSIX_TRACE_NAME_MAX
 */
#define TRACE_SYS_MAX           _POSIX_TRACE_SYS_MAX

/*
 * Maximum number of user trace event type identifiers that may
 * simultaneously exist in a traced process, including the predefined
 * user trace event POSIX_TRACE_UNNAMED_USER_EVENT.
 *
 * Minimum Acceptable Value: _POSIX_TRACE_USER_EVENT_MAX
 */
#define TRACE_USER_EVENT_MAX    _POSIX_TRACE_USER_EVENT_MAX

/*
 * Maximum length of terminal device name.
 *
 * Minimum Acceptable Value: _POSIX_TTY_NAME_MAX
 */
#define TTY_NAME_MAX            _POSIX_TTY_NAME_MAX

/*
 * Maximum number of bytes supported for the name of a timezone
 * (not of the TZ variable).
 *
 * Note:
 *    The length given by {TZNAME_MAX} does not include the quoting
 * characters mentioned in Other Environment Variables .
 *
 * Minimum Acceptable Value: _POSIX_TZNAME_MAX
 */
#define TZNAME_MAX              _POSIX_TZNAME_MAX

/*
 * Pathname Variable Values
 *
 * The values in the following list may be constants within an
 * implementation or may vary from one pathname to another. For example,
 * file systems or directories may have different characteristics.
 *
 * A definition of one of the symbolic constants in the following list
 * shall be omitted from the <limits.h> header on specific implementations
 * where the corresponding value is equal to or greater than the stated
 * minimum, but where the value can vary depending on the file to which
 * it is applied. The actual value supported for a specific pathname
 * shall be provided by the pathconf() function.
 */

/*
 * Minimum number of bits needed to represent, as a signed integer value,
 * the maximum size of a regular file allowed in the specified directory.
 *
 * Minimum Acceptable Value: 32
 */
#define FILESIZEBITS            32

/*
 * Maximum number of links to a single file.
 *
 * Minimum Acceptable Value: _POSIX_LINK_MAX
 */
#define LINK_MAX                _POSIX_LINK_MAX

/*
 * Maximum number of bytes in a terminal canonical input line.
 *
 * Minimum Acceptable Value: _POSIX_MAX_CANON
 */
#define MAX_CANON               _POSIX_MAX_CANON

/*
 * Minimum number of bytes for which space is available in a
 * terminal input queue; therefore, the maximum number of bytes
 * a conforming application may require to be typed as input
 * before reading them.
 *
 * Minimum Acceptable Value: _POSIX_MAX_INPUT
 */
#define MAX_INPUT               _POSIX_MAX_INPUT

/*
 * Minimum number of bytes for which space is available in a
 * terminal input queue; therefore, the maximum number of bytes
 * a conforming application may require to be typed as input
 * before reading them.
 *
 * Minimum Acceptable Value: _POSIX_NAME_MAX or _XOPEN_NAME_MAX
 */
#define NAME_MAX                _POSIX_NAME_MAX

/*
 * Maximum number of bytes the implementation will store as a pathname
 * in a user-supplied buffer of unspecified size, including the
 * terminating null character. Minimum number the implementation will
 * accept as the maximum number of bytes in a pathname.
 *
 * Minimum Acceptable Value: _POSIX_NAME_MAX or _XOPEN_NAME_MAX
 */
#define PATH_MAX                _POSIX_NAME_MAX

/*
 * Maximum number of bytes that is guaranteed to be atomic
 * when writing to a pipe.
 *
 * Minimum Acceptable Value: _POSIX_PIPE_BUF
 */
#define PIPE_BUF                _POSIX_NAME_MAX

/*
 * Minimum number of bytes of storage actually allocated for
 * any portion of a file.
 *
 * Minimum Acceptable Value: Not specified.
 */
#define POSIX_ALLOC_SIZE_MIN            4096

/*
 * Recommended increment for file transfer sizes between the
 * {POSIX_REC_MIN_XFER_SIZE} and {POSIX_REC_MAX_XFER_SIZE} values.
 *
 * Minimum Acceptable Value: Not specified.
 */
#define POSIX_REC_INCR_XFER_SIZE        4096

/*
 * Maximum recommended file transfer size.
 *
 * Minimum Acceptable Value: Not specified.
 */
#define POSIX_REC_MAX_XFER_SIZE         4096

/*
 * Minimum recommended file transfer size.
 *
 * Minimum Acceptable Value: Not specified.
 */
#define POSIX_REC_MIN_XFER_SIZE         4096

/*
 * Recommended file transfer buffer alignment.
 *
 * Minimum Acceptable Value: Not specified.
 */
#define POSIX_REC_XFER_ALIGN            4096

/*
 * Maximum number of bytes in a symbolic link.
 *
 * Minimum Acceptable Value: _POSIX_SYMLINK_MAX
 */
#define SYMLINK_MAX                 _POSIX_SYMLINK_MAX

/*
 * Runtime Increasable Values
 *
 * The magnitude limitations in the following list shall be fixed
 * by specific implementations. An application should assume
 * that the value of the symbolic constant defined by <limits.h>
 * in a specific implementation is the minimum that pertains
 * whenever the application is run under that implementation.
 * A specific instance of a specific implementation may increase
 * the value relative to that supplied by <limits.h> for that
 * implementation. The actual value supported by a specific
 * instance shall be provided by the sysconf() function.
 */

/*
 * Maximum obase values allowed by the bc utility.
 *
 * Minimum Acceptable Value: _POSIX2_BC_BASE_MAX
 */
#define BC_BASE_MAX                 _POSIX2_BC_BASE_MAX

/*
 * Maximum number of elements permitted in an array by the bc utility.
 *
 * Minimum Acceptable Value: _POSIX2_BC_DIM_MAX
 */
#define BC_DIM_MAX                  _POSIX2_BC_DIM_MAX

/*
 * Maximum scale value allowed by the bc utility.
 *
 * Minimum Acceptable Value: _POSIX2_BC_SCALE_MAX
 */
#define BC_SCALE_MAX                _POSIX2_BC_SCALE_MAX

/*
 * Maximum length of a string constant accepted by the bc utility.
 *
 * Minimum Acceptable Value: _POSIX2_BC_STRING_MAX
 */
#define BC_STRING_MAX               _POSIX2_BC_STRING_MAX

/*
 * Maximum number of bytes in a character class name.
 *
 * Minimum Acceptable Value: _POSIX2_CHARCLASS_NAME_MAX
 */
#define CHARCLASS_NAME_MAX          _POSIX2_CHARCLASS_NAME_MAX

/*
 * Maximum number of weights that can be assigned to an entry
 * of the LC_COLLATE order keyword in the locale definition file;
 * see Locale.
 *
 * Minimum Acceptable Value: _POSIX2_COLL_WEIGHTS_MAX
 */
#define COLL_WEIGHTS_MAX            _POSIX2_COLL_WEIGHTS_MAX

/*
 * Maximum number of expressions that can be nested within
 * parentheses by the expr utility.
 *
 * Minimum Acceptable Value: _POSIX2_EXPR_NEST_MAX
 */
#define EXPR_NEST_MAX               _POSIX2_EXPR_NEST_MAX

/*
 * Unless otherwise noted, the maximum length, in bytes, of a
 * utility's input line (either standard input or another file),
 * when the utility is described as processing text files. The
 * length includes room for the trailing <newline>.
 *
 * Minimum Acceptable Value: _POSIX2_LINE_MAX
 */
#define LINE_MAX                    _POSIX2_LINE_MAX

/*
 * Maximum number of simultaneous supplementary group IDs per process.
 *
 * Minimum Acceptable Value: _POSIX_NGROUPS_MAX
 */
#define NGROUPS_MAX                 _POSIX_NGROUPS_MAX

/*
 * Maximum number of repeated occurrences of a regular expression
 * permitted when using the interval notation \{m,n\}; see Regular
 * Expressions .
 *
 * Minimum Acceptable Value: _POSIX2_RE_DUP_MAX
 */
#define RE_DUP_MAX                  _POSIX2_RE_DUP_MAX

/*
 * Numerical Limits
 *
 * The <limits.h> header shall define the following macros and,
 * except for {CHAR_BIT}, {LONG_BIT}, {MB_LEN_MAX}, and {WORD_BIT},
 * they shall be replaced by expressions that have the same type
 * as would an expression that is an object of the corresponding
 * type converted according to the integer promotions.
 *
 * If the value of an object of type char is treated as a signed
 * integer when used in an expression, the value of {CHAR_MIN} is
 * the same as that of {SCHAR_MIN} and the value of {CHAR_MAX} is
 * the same as that of {SCHAR_MAX}. Otherwise, the value of {CHAR_MIN}
 * is 0 and the value of {CHAR_MAX} is the same as that of {UCHAR_MAX}.
 */

/*
 * Number of bits in a type char.
 */
#define CHAR_BIT                8

/*
 * Maximum value for an object of type char.
 *
 * Value: {UCHAR_MAX} or {SCHAR_MAX}
 */
#define CHAR_MAX                SCHAR_MAX

/*
 * Maximum value for an object of type char.
 *
 * Value: {SCHAR_MIN} or 0
 */
#define CHAR_MIN                SCHAR_MIN

/*
 * Maximum value for an object of type int.
 *
 * Minimum Acceptable Value: 2147483647
 */
#define INT_MAX                 2147483647

/*
 * Minimum value for an object of type int.
 *
 * Minimum Acceptable Value: -2147483647
 */
#define INT_MIN                 -2147483647

/*
 * Maximum value for an object of type long long.
 *
 * Minimum Acceptable Value: +9223372036854775807
 */
#define LLONG_MAX               9223372036854775807LL

/*
 * Minimum value for an object of type long long.
 *
 * Maximum Acceptable Value: -9223372036854775807
 */
#define LLONG_MIN               -9223372036854775807LL

/*
 * Number of bits in an object of type long.
 *
 * Minimum Acceptable Value: 32
 */

#if (__WORDSIZE == 64)
#define LONG_BIT                64
#else
#define LONG_BIT                32
#endif

/*
 * Maximum value for an object of type long.
 *
 * Minimum Acceptable Value: +2147483647
 */
#if (__WORDSIZE == 64)
#define LONG_MAX                9223372036854775807L
#else
#define LONG_MAX                2147483647L
#endif

/*
 * Minimum value for an object of type long.
 *
 * Maximum Acceptable Value: -2147483647
 */
#if (__WORDSIZE == 64)
#define LONG_MIN                -9223372036854775807L
#else
#define LONG_MIN                -2147483647L
#endif

/*
 * Maximum number of bytes in a character, for any supported locale.
 *
 * Minimum Acceptable Value: 1
 */
#define MB_LEN_MAX              16

/*
 * Maximum value for an object of type signed char.
 *
 * Value: +127
 */
#define SCHAR_MAX               127

/*
 * Minimum value for an object of type signed char.
 *
 * Value: -128
 */
#define SCHAR_MIN               -128

/*
 * Maximum value for an object of type short.
 *
 * Value: +32767
 */
#define SHRT_MAX                32767

/*
 * Maximum value for an object of type short.
 *
 * Value: -32767
 */
#define SHRT_MIN                -32767

/*
 * Maximum value for an object of type ssize_t.
 *
 * Minimum Acceptable Value: {_POSIX_SSIZE_MAX}
 */
#define SSIZE_MAX               _POSIX_SSIZE_MAX

/*
 * Maximum value for an object of type unsigned char.
 *
 * Value: 255
 */
#define UCHAR_MAX               255

/*
 * Maximum value for an object of type unsigned.
 *
 * Minimum Acceptable Value: 4294967295
 */
#define UINT_MAX                4294967295

/*
 * Maximum value for an object of type unsigned long long.
 *
 * Minimum Acceptable Value: 18446744073709551615
 */
#define ULLONG_MAX             18446744073709551615ULL

/*
 * Maximum value for an object of type unsigned long.
 *
 * Minimum Acceptable Value: 4294967295
 */
#if (__WORDSIZE == 64)
#define ULONG_MAX               18446744073709551615UL
#else
#define ULONG_MAX               4294967295UL
#endif
/*
 * Maximum value for an object of type unsigned short.
 *
 * Minimum Acceptable Value: 65535
 */
#define USHRT_MAX               65535

/*
 * Number of bits in an object of type int.
 *
 * Minimum Acceptable Value: 32
 */
#define WORD_BIT                32

/*
 * Other Invariant Values
 *
 * The <limits.h> header shall define the following symbolic constants
 */

/*
 * Maximum value of n in conversion specifications using the "%n$"
 * sequence in calls to the printf() and scanf() families of functions.
 *
 * Minimum Acceptable Value: 9
 */
#define NL_ARGMAX               4096

/*
 * Maximum number of bytes in a LANG name.
 *
 * Minimum Acceptable Value: 14
 */
#define NL_LANGMAX              2048

/*
 * Maximum message number.
 *
 * Minimum Acceptable Value: 32767
 */
#define NL_MSGMAX               INT_MAX

/*
 * Maximum set number.
 *
 * Minimum Acceptable Value: 255
 */
#define NL_SETMAX               INT_MAX

/*
 * Maximum number of bytes in a message string.
 *
 * Minimum Acceptable Value: 255
 */
#define NL_TEXTMAX              INT_MAX

/*
 * Default process priority.
 *
 * Minimum Acceptable Value: 20
 */
#define NZERO                   20

#endif /* _POSIX_LIMITS_H */
