/* unistd.h - standard symbolic constants and types */

/*
APPLICATION USAGE

    POSIX.1-2008 only describes the behavior of systems that claim
    conformance to it. However, application developers who want to
    write applications that adapt to other versions of this standard
    (or to systems that do not conform to any POSIX standard) may find
    it useful to code them so as to conditionally compile different
    code depending on the value of _POSIX_VERSION, for example:

    #if _POSIX_VERSION >= 200112L
    /@ Use the newer function that copes with large files. @/
    off_t pos=ftello(fp);
    #else
    /@ Either this is an old version of POSIX, or _POSIX_VERSION is
       not even defined, so use the traditional function. @/
    long pos=ftell(fp);
    #endif

    Earlier versions of POSIX.1-2008 and of the Single UNIX Specification
    can be identified by the following macros:

    POSIX.1-1988 standard
        _POSIX_VERSION==198808L
    POSIX.1-1990 standard
        _POSIX_VERSION==199009L
    ISO POSIX-1:1996 standard
        _POSIX_VERSION==199506L
    Single UNIX Specification, Version 1
        _XOPEN_UNIX and _XOPEN_VERSION==4
    Single UNIX Specification, Version 2
        _XOPEN_UNIX and _XOPEN_VERSION==500
    ISO POSIX-1:2001 and Single UNIX Specification, Version 3
        _POSIX_VERSION==200112L, plus (if the XSI option is supported)
        _XOPEN_UNIX and _XOPEN_VERSION==600

    POSIX.1-2008 does not make any attempt to define application binary
    interaction with the underlying operating system. However, application
    developers may find it useful to query _SC_VERSION at runtime via
    sysconf() to determine whether the current version of the operating
    system supports the necessary functionality as in the following
    program fragment:

    if (sysconf(_SC_VERSION) < 200809L) {
        fprintf(stderr, "POSIX.1-2008 system required, terminating \n");
        exit(1);
    }

    New applications should not use _XOPEN_SHM or _XOPEN_ENH_I18N.

RATIONALE

    As POSIX.1-2008 evolved, certain options became sufficiently
    standardized that it was concluded that simply requiring one of
    the option choices was simpler than retaining the option. However,
    for backwards-compatibility, the option flags (with required constant
    values) are retained.

    Version Test Macros

    The standard developers considered altering the definition of
    _POSIX_VERSION and removing _SC_VERSION from the specification
    of sysconf() since the utility to an application was deemed by
    some to be minimal, and since the implementation of the functionality
    is potentially problematic. However, they recognized that support
    for existing application binaries is a concern to manufacturers,
    application developers, and the users of implementations conforming
    to POSIX.1-2008.

    While the example using _SC_VERSION in the APPLICATION USAGE section
    does not provide the greatest degree of imaginable utility to the
    application developer or user, it is arguably better than a core
    file or some other equally obscure result. (It is also possible for
    implementations to encode and recognize application binaries compiled
    in various POSIX.1-conforming environments, and modify the semantics
    of the underlying system to conform to the expectations of the
    application.) For the reasons outlined in the preceding paragraphs
    and in the APPLICATION USAGE section, the standard developers elected
    to retain the _POSIX_VERSION and _SC_VERSION functionality.

    Compile-Time Symbolic Constants for System-Wide Options

    POSIX.1-2008 includes support in certain areas for the newly adopted
    policy governing options and stubs.

    This policy provides flexibility for implementations in how they
    support options. It also specifies how conforming applications can
    adapt to different implementations that support different sets of options.
    It allows the following:

       1. If an implementation has no interest in supporting an option,
          it does not have to provide anything associated with that option
          beyond the announcement that it does not support it.

       2. An implementation can support a partial or incompatible version
          of an option (as a non-standard extension) as long as it does not
          claim to support the option.
       3. An application can determine whether the option is supported.
          A strictly conforming application must check this announcement
          mechanism before first using anything associated with the option.

    There is an important implication of this policy. POSIX.1-2008 cannot
    dictate the behavior of interfaces associated with an option when the
    implementation does not claim to support the option. In particular,
    it cannot require that a function associated with an unsupported option
    will fail if it does not perform as specified. However, this policy does
    not prevent a standard from requiring certain functions to always be
    present, but that they shall always fail on some implementations. The
    setpgid() function in the POSIX.1-1990 standard, for example, is
    considered appropriate.

    The POSIX standards include various options, and the C-language binding
    support for an option implies that the implementation must supply data
    types and function interfaces. An application must be able to discover
    whether the implementation supports each option.

    Any application must consider the following three cases for each option:

       1. Option never supported.

          The implementation advertises at compile time that the option
          will never be supported. In this case, it is not necessary for
          the implementation to supply any of the data types or function
          interfaces that are provided only as part of the option. The
          implementation might provide data types and functions that are
          similar to those defined by POSIX.1-2008, but there is no guarantee
          for any particular behavior.

       2. Option always supported.

          The implementation advertises at compile time that the option
          will always be supported. In this case, all data types and function
          interfaces shall be available and shall operate as specified.

       3. Option might or might not be supported.

          Some implementations might not provide a mechanism to specify
          support of options at compile time. In addition, the
          implementation might be unable or unwilling to specify support
          or non-support at compile time. In either case, any application
          that might use the option at runtime must be able to compile and
          execute. The implementation must provide, at compile time, all
          data types and function interfaces that are necessary to allow
          this. In this situation, there must be a mechanism that allows
          the application to query, at runtime, whether the option is
          supported. If the application attempts to use the option when
          it is not supported, the result is unspecified unless explicitly
          specified otherwise in POSIX.1-2008.
*/
#ifndef _POSIX_UNISTD_H
#define _POSIX_UNISTD_H

#include <stddef.h>
#include <sys.h>

/* Version Test Macros */

/*
 * _POSIX_VERSION
 *
 * Integer value indicating version of this standard (C-language binding)
 * to which the implementation conforms. For implementations conforming
 * to POSIX.1-2008, the value shall be 200809L.
 */

#define _POSIX_VERSION			(200809L)

/*
 * _POSIX2_VERSION
 *
 * Integer value indicating version of the Shell and Utilities volume
 * of POSIX.1 to which the implementation conforms. For implementations
 * conforming to POSIX.1-2008, the value shall be 200809L.
 */

#define _POSIX2_VERSION         (200809L)

/*
 * _XOPEN_VERSION
 *
 * Integer value indicating version of the X/Open Portability Guide to
 * which the implementation conforms. The value shall be 700.
 */

#define _XOPEN_VERSION         (700)

/* Constants for Options and Option Groups */

/*
 * The following symbolic constants, if defined in <unistd.h>, shall have
 * a value of -1, 0, or greater, unless otherwise specified below. The
 * values shall be suitable for use in #if preprocessing directives.
 *
 * If a symbolic constant is not defined or is defined with the value -1,
 * the option is not supported for compilation. If it is defined with a
 * value greater than zero, the option shall always be supported when
 * the application is executed. If it is defined with the value zero,
 * the option shall be supported for compilation and might or might not
 * be supported at runtime. See Options for further information about the
 * conformance requirements of these three categories of support.
 */

#define __OPTION_NOT_SUPPORTED          -1
#define __OPTION_MAYNOT_SUPPORTED       0
#define __OPTION_SUPPORTED              (200809L)


/*
 * _POSIX_ADVISORY_INFO
 *
 * The implementation supports the Advisory Information option.
 */
#define  _POSIX_ADVISORY_INFO       __OPTION_NOT_SUPPORTED

/*
 * _POSIX_ASYNCHRONOUS_IO
 *
 * The implementation supports asynchronous input and output.
 */

#define  _POSIX_ASYNCHRONOUS_IO       __OPTION_SUPPORTED

/*
 * _POSIX_BARRIERS
 *
 * The implementation supports barriers.
 */
#define  _POSIX_BARRIERS       __OPTION_SUPPORTED

/*
 * _POSIX_CHOWN_RESTRICTED
 *
 * The use of chown() and fchown() is restricted to a process with
 * appropriate privileges, and to changing the group ID of a file
 * only to the effective group ID of the process or to one of its
 * supplementary group IDs. This symbol shall be defined with a
 * value other than -1.
 */
#define  _POSIX_CHOWN_RESTRICTED       __OPTION_MAYNOT_SUPPORTED

/*
 * _POSIX_CLOCK_SELECTION
 *
 * The implementation supports clock selection.
 */
#define  _POSIX_CLOCK_SELECTION       __OPTION_SUPPORTED

/*
 * _POSIX_CPUTIME
 *
 * The implementation supports the Process CPU-Time Clocks option.
 */
#define  _POSIX_CPUTIME       __OPTION_SUPPORTED

/*
 * _POSIX_FSYNC
 *
 * The implementation supports the File Synchronization option.
 */
#define  _POSIX_FSYNC       __OPTION_SUPPORTED

/*
 * _POSIX_IPV6
 *
 * The implementation supports the IPv6 option.
 */
#define  _POSIX_IPV6       __OPTION_SUPPORTED

/*
 * _POSIX_JOB_CONTROL
 *
 * The implementation supports job control. This symbol shall always
 * be set to a value greater than zero.
 */
#define  _POSIX_JOB_CONTROL       __OPTION_SUPPORTED

/*
 * _POSIX_MAPPED_FILES
 *
 * The implementation supports memory mapped Files.
 */
#define  _POSIX_MAPPED_FILES       __OPTION_SUPPORTED

/*
 * _POSIX_MEMLOCK
 *
 * The implementation supports the Process Memory Locking option.
 */
#define  _POSIX_MEMLOCK       __OPTION_SUPPORTED

/*
 * _POSIX_MEMLOCK_RANGE
 *
 * The implementation supports the Range Memory Locking option.
 */
#define  _POSIX_MEMLOCK_RANGE       __OPTION_SUPPORTED


/*
 * _POSIX_MEMORY_PROTECTION
 *
 * The implementation supports memory protection.
 */
#define  _POSIX_MEMORY_PROTECTION       __OPTION_SUPPORTED

/*
 * _POSIX_MESSAGE_PASSING
 *
 * The implementation supports the Message Passing option.
 */
#define  _POSIX_MESSAGE_PASSING       __OPTION_SUPPORTED

/*
 * _POSIX_MONOTONIC_CLOCK
 *
 * The implementation supports the Monotonic Clock option.
 */
#define  _POSIX_MONOTONIC_CLOCK       __OPTION_SUPPORTED

/*
 * _POSIX_NO_TRUNC
 *
 * Pathname components longer than {NAME_MAX} generate an error.
 * This symbol shall be defined with a value other than -1.
 */
#define  _POSIX_NO_TRUNC       __OPTION_NOT_SUPPORTED

/*
 * _POSIX_PRIORITIZED_IO
 *
 * The implementation supports the Prioritized Input and Output option.
 */
#define  _POSIX_PRIORITIZED_IO       __OPTION_NOT_SUPPORTED

/*
 * _POSIX_PRIORITY_SCHEDULING
 *
 * The implementation supports the Process Scheduling option.
 */
#define  _POSIX_PRIORITY_SCHEDULING       __OPTION_SUPPORTED

/*
 * _POSIX_RAW_SOCKETS
 *
 * The implementation supports the Raw Sockets option.
 */
#define  _POSIX_RAW_SOCKETS       __OPTION_SUPPORTED

/*
 * _POSIX_READER_WRITER_LOCKS
 *
 * The implementation supports read-write locks.
 */
#define  _POSIX_READER_WRITER_LOCKS       __OPTION_SUPPORTED

/*
 * _POSIX_REALTIME_SIGNALS
 *
 *   The implementation supports realtime signals.
 */
#define  _POSIX_REALTIME_SIGNALS       __OPTION_SUPPORTED

/*
 * _POSIX_REGEXP
 *
 * The implementation supports the Regular Expression Handling option.
 */
#define  _POSIX_REGEXP       __OPTION_SUPPORTED

/*
 * _POSIX_SAVED_IDS
 *
 * Each process has a saved set-user-ID and a saved set-group-ID.
 * This symbol shall always be set to a value greater than zero.
 */
#define  _POSIX_SAVED_IDS       __OPTION_SUPPORTED

/*
 * _POSIX_SEMAPHORES
 *
 * The implementation supports semaphores.
 */
#define  _POSIX_SEMAPHORES       __OPTION_SUPPORTED

/*
 * _POSIX_SHARED_MEMORY_OBJECTS
 *
 * The implementation supports the Shared Memory Objects option.
 */
#define  _POSIX_SHARED_MEMORY_OBJECTS       __OPTION_SUPPORTED

/*
 * _POSIX_SHELL
 *
 * The implementation supports the POSIX shell. This symbol shall
 * always be set to a value greater than zero.
 */
#define  _POSIX_SHELL       __OPTION_SUPPORTED

/*
 * _POSIX_SPAWN
 *
 * The implementation supports the Spawn option.
 */
#define  _POSIX_SPAWN       __OPTION_SUPPORTED

/*
 * _POSIX_SPIN_LOCKS
 *
 * The implementation supports spin locks.
 */
#define  _POSIX_SPIN_LOCKS       __OPTION_SUPPORTED

/*
 * _POSIX_SPORADIC_SERVER
 *
 * The implementation supports the Process Sporadic Server option.
 */
#define  _POSIX_SPORADIC_SERVER       __OPTION_SUPPORTED

/*
 * _POSIX_SYNCHRONIZED_IO
 *
 * The implementation supports the Synchronized Input and Output option.
 */
#define  _POSIX_SYNCHRONIZED_IO       __OPTION_SUPPORTED

/*
 * _POSIX_THREAD_ATTR_STACKADDR
 *
 * The implementation supports the Thread Stack Address Attribute option.
 */
#define  _POSIX_THREAD_ATTR_STACKADDR       __OPTION_SUPPORTED

/*
 * _POSIX_THREAD_ATTR_STACKSIZE
 *
 * The implementation supports the Thread Stack Size Attribute option.
 */
#define  _POSIX_THREAD_ATTR_STACKSIZE       __OPTION_SUPPORTED

/*
 * _POSIX_THREAD_CPUTIME
 *
 * The implementation supports the Thread CPU-Time Clocks option.
 */
#define  _POSIX_THREAD_CPUTIME       __OPTION_SUPPORTED

/*
 * _POSIX_THREAD_PRIO_INHERIT
 *
 * The implementation supports the Non-Robust Mutex Priority Inheritance option.
 */
#define  _POSIX_THREAD_PRIO_INHERIT       __OPTION_SUPPORTED

/*
 * _POSIX_THREAD_PRIO_PROTECT
 *
 * The implementation supports the Non-Robust Mutex Priority Protection option.
 */
#define  _POSIX_THREAD_PRIO_PROTECT       __OPTION_SUPPORTED

/*
 * _POSIX_THREAD_PRIORITY_SCHEDULING
 *
 * The implementation supports the Thread Execution Scheduling option.
 */
#define  _POSIX_THREAD_PRIORITY_SCHEDULING       __OPTION_SUPPORTED

/*
 * _POSIX_THREAD_PROCESS_SHARED
 *
 * The implementation supports the Thread Process-Shared Synchronization option.
 */
#define  _POSIX_THREAD_PROCESS_SHARED       __OPTION_SUPPORTED

/*
 * _POSIX_THREAD_ROBUST_PRIO_INHERIT
 *
 * The implementation supports the Robust Mutex Priority Inheritance option.
 */
#define  _POSIX_THREAD_ROBUST_PRIO_INHERIT       __OPTION_SUPPORTED

/*
 * _POSIX_THREAD_ROBUST_PRIO_PROTECT
 *
 * The implementation supports the Robust Mutex Priority Protection option.
 */
#define  _POSIX_THREAD_ROBUST_PRIO_PROTECT       __OPTION_SUPPORTED

/*
 * _POSIX_THREAD_SAFE_FUNCTIONS
 *
 * The implementation supports thread-safe functions.
 */
#define  _POSIX_THREAD_SAFE_FUNCTIONS       __OPTION_SUPPORTED

/*
 * _POSIX_THREAD_SPORADIC_SERVER
 *
 * The implementation supports the Thread Sporadic Server option.
 */
#define  _POSIX_THREAD_SPORADIC_SERVER       __OPTION_SUPPORTED

/*
 * _POSIX_THREADS
 *
 * The implementation supports threads.
 */
#define  _POSIX_THREADS       __OPTION_SUPPORTED

/*
 * _POSIX_TIMEOUTS
 *
 * The implementation supports timeouts.
 */
#define  _POSIX_TIMEOUTS       __OPTION_SUPPORTED

/*
 * _POSIX_TIMERS
 *
 * The implementation supports timers.
 */
#define  _POSIX_TIMERS       __OPTION_SUPPORTED

/*
 * _POSIX_TRACE
 *
 * The implementation supports the Trace option.
 */
#define  _POSIX_TRACE       __OPTION_SUPPORTED

/*
 * _POSIX_TRACE_EVENT_FILTER
 *
 * The implementation supports the Trace Event Filter option.
 */
#define  _POSIX_TRACE_EVENT_FILTER       __OPTION_SUPPORTED

/*
 * _POSIX_TRACE_INHERIT
 *
 * The implementation supports the Trace Inherit option.
 */
#define  _POSIX_TRACE_INHERIT       __OPTION_SUPPORTED

/*
 * _POSIX_TRACE_LOG
 *
 * The implementation supports the Trace Log option.
 */
#define  _POSIX_TRACE_LOG       __OPTION_SUPPORTED

/*
 * _POSIX_TYPED_MEMORY_OBJECTS
 *
 * The implementation supports the Typed Memory Objects option.
 */
#define  _POSIX_TYPED_MEMORY_OBJECTS       __OPTION_SUPPORTED

/*
 * _POSIX_V6_ILP32_OFF32
 *
 * The implementation provides a C-language compilation environment
 * with 32-bit int, long, pointer, and off_t types.
 */
#define  _POSIX_V6_ILP32_OFF32       __OPTION_NOT_SUPPORTED

/*
 * _POSIX_V6_ILP32_OFFBIG
 *
 * The implementation provides a C-language compilation environment
 * with 32-bit int, long, and pointer types and an off_t type using
 * at least 64 bits.
 */
#define  _POSIX_V6_ILP32_OFFBIG       __OPTION_NOT_SUPPORTED

/*
 * _POSIX_V6_LP64_OFF64
 *
 * The implementation provides a C-language compilation environment
 * with 32-bit int and 64-bit long, pointer, and off_t types.
 */
#define  _POSIX_V6_LP64_OFF64         __OPTION_SUPPORTED

/*
 * _POSIX_V6_LPBIG_OFFBIG
 *
 * The implementation provides a C-language compilation environment
 * with an int type using at least 32 bits and long, pointer, and
 * off_t types using at least 64 bits.
 */
#define  _POSIX_V6_LPBIG_OFFBIG       __OPTION_SUPPORTED

/*
 * _POSIX_V7_ILP32_OFF32
 *
 * The implementation provides a C-language compilation environment
 * with 32-bit int, long, pointer, and off_t types.
 */
#define  _POSIX_V7_ILP32_OFF32       __OPTION_NOT_SUPPORTED

/*
 * _POSIX_V7_ILP32_OFFBIG
 *
 * The implementation provides a C-language compilation environment
 * with 32-bit int, long, and pointer types and an off_t type using
 * at least 64 bits.
 */
#define  _POSIX_V7_ILP32_OFFBIG       __OPTION_NOT_SUPPORTED

/*
 * _POSIX_V7_LP64_OFF64
 *
 * The implementation provides a C-language compilation environment
 * with 32-bit int and 64-bit long, pointer, and off_t types.
 */
#define  _POSIX_V7_LP64_OFF64       __OPTION_SUPPORTED

/*
 * _POSIX_V7_LPBIG_OFFBIG
 *
 * The implementation provides a C-language compilation environment
 * with an int type using at least 32 bits and long, pointer, and
 * off_t types using at least 64 bits.
 */
#define  _POSIX_V7_LPBIG_OFFBIG       __OPTION_SUPPORTED

/*
 * _POSIX2_C_BIND
 *
 * The implementation supports the C-Language Binding option.
 */
#define  _POSIX2_C_BIND       __OPTION_SUPPORTED

/*
 * _POSIX2_C_DEV
 *
 * The implementation supports the C-Language Development Utilities option.
 */
#define  _POSIX2_C_DEV       __OPTION_SUPPORTED

/*
 * _POSIX2_CHAR_TERM
 *
 * The implementation supports the Terminal Characteristics option.
 */
#define  _POSIX2_CHAR_TERM       __OPTION_SUPPORTED

/*
 * _POSIX2_FORT_DEV
 *
 * The implementation supports the FORTRAN Development Utilities option.
 */
#define  _POSIX2_FORT_DEV       __OPTION_NOT_SUPPORTED

/*
 * _POSIX2_FORT_RUN
 *
 * The implementation supports the FORTRAN Runtime Utilities option.
 */
#define  _POSIX2_FORT_RUN       __OPTION_NOT_SUPPORTED

/*
 * _POSIX2_LOCALEDEF
 *
 * The implementation supports the creation of locales by the localedef utility.
 */
#define  _POSIX2_LOCALEDEF       __OPTION_NOT_SUPPORTED

/*
 * _POSIX2_PBS
 *
 * The implementation supports the Batch Environment Services and Utilities option.
 */
#define  _POSIX2_PBS       __OPTION_SUPPORTED

/*
 * _POSIX2_PBS_ACCOUNTING
 *
 * The implementation supports the Batch Accounting option.
 */
#define  _POSIX2_PBS_ACCOUNTING       __OPTION_SUPPORTED

/*
 * _POSIX2_PBS_CHECKPOINT
 *
 * The implementation supports the Batch Checkpoint/Restart option.
 */
#define  _POSIX2_PBS_CHECKPOINT       __OPTION_SUPPORTED

/*
 * _POSIX2_PBS_LOCATE
 *
 * The implementation supports the Locate Batch Job Request option.
 */
#define  _POSIX2_PBS_LOCATE       __OPTION_SUPPORTED

/*
 * _POSIX2_PBS_MESSAGE
 *
 * The implementation supports the Batch Job Message Request option.
 */
#define  _POSIX2_PBS_MESSAGE       __OPTION_SUPPORTED

/*
 * _POSIX2_PBS_TRACK
 *
 * The implementation supports the Track Batch Job Request option.
 */
#define  _POSIX2_PBS_TRACK       __OPTION_SUPPORTED

/*
 * _POSIX2_SW_DEV
 *
 * The implementation supports the Software Development Utilities option.
 */
#define  _POSIX2_SW_DEV       __OPTION_SUPPORTED

/*
 * _POSIX2_UPE
 *
 * The implementation supports the User Portability Utilities option.
 */
#define  _POSIX2_UPE       __OPTION_SUPPORTED

/*
 * _XOPEN_CRYPT
 *
 * The implementation supports the X/Open Encryption Option Group.
 */
#define  _XOPEN_CRYPT       __OPTION_SUPPORTED

/*
 * _XOPEN_ENH_I18N
 *
 * The implementation supports the Issue 4, Version 2 Enhanced
 * Internationalization Option Group.
 */
#define  _XOPEN_ENH_I18N       __OPTION_SUPPORTED

/*
 * _XOPEN_REALTIME
 *
 * The implementation supports the X/Open Realtime Option Group.
 */
#define  _XOPEN_REALTIME       __OPTION_SUPPORTED

/*
 * _XOPEN_REALTIME_THREADS
 *
 * The implementation supports the X/Open Realtime Threads Option Group.
 */
#define  _XOPEN_REALTIME_THREADS       __OPTION_SUPPORTED

/*
 * _XOPEN_SHM
 *
 * The implementation supports the Issue 4, Version 2 Shared Memory Option Group.
 */
#define  _XOPEN_SHM       __OPTION_SUPPORTED

/*
 * _XOPEN_STREAMS
 *
 * The implementation supports the XSI STREAMS Option Group.
 */
#define  _XOPEN_STREAMS       __OPTION_SUPPORTED

/*
 * _XOPEN_UNIX
 *
 * The implementation supports the XSI option.
 */
#define  _XOPEN_UNIX       __OPTION_SUPPORTED

/*
 * _XOPEN_UUCP
 *
 * The implementation supports the UUCP Utilities option.
 */
#define  _XOPEN_UUCP       __OPTION_SUPPORTED

/*
 * Execution-Time Symbolic Constants
 *
 * If any of the following symbolic constants are not defined in
 * the <unistd.h> header, the value shall vary depending on the file
 * to which it is applied. If defined, they shall have values suitable
 * for use in #if preprocessing directives.
 *
 * If any of the following symbolic constants are defined to have
 * value -1 in the <unistd.h> header, the implementation shall not
 * provide the option on any file; if any are defined to have a
 * value other than -1 in the <unistd.h> header, the implementation
 * shall provide the option on all applicable files.
 *
 * All of the following values, whether defined as symbolic constants
 * in <unistd.h> or not, may be queried with respect to a specific
 * file using the pathconf() or fpathconf() functions
 */

/*
 * _POSIX_ASYNC_IO
 *
 * Asynchronous input or output operations may be performed for
 * the associated file.
 */
#define  _POSIX_ASYNC_IO       1

/*
 * _POSIX_PRIO_IO
 *
 * Prioritized input or output operations may be performed for
 * the associated file.
 */
#define  _POSIX_PRIO_IO        2

/*
 * _POSIX_SYNC_IO
 *
 * Synchronized input or output operations may be performed for
 * the associated file.
 */
#define  _POSIX_SYNC_IO        3

/*
 * If the following symbolic constants are defined in the <unistd.h>
 * header, they apply to files and all paths in all file systems on
 * the implementation
 */

/*
 * _POSIX_TIMESTAMP_RESOLUTION
 *
 * The resolution in nanoseconds for all file timestamps.
 */
#define  _POSIX_TIMESTAMP_RESOLUTION        1

/*
 * _POSIX2_SYMLINKS
 *
 * Symbolic links can be created.
 */
#define  _POSIX2_SYMLINKS        1

/* Constants for Functions */

/*
 * Access modes
 *
 * The constants F_OK, R_OK, W_OK, and X_OK and the expressions
 * R_OK|W_OK, R_OK|X_OK, and R_OK|W_OK|X_OK shall all have distinct values.
 */

#define R_OK	0x8   /* Test for read permission.*/
#define W_OK	0x4   /* Test for write permission.*/
#define X_OK	0x2   /* Test for execute (search) permission.*/
#define F_OK	0x1   /* Test for existence of file. */

/* Standard file descriptors */

#define STDIN_FILENO	0   /* File number of stdin; 0.*/
#define STDOUT_FILENO	1   /* File number of stdout; 1.*/
#define STDERR_FILENO	2   /* File number of stderr; 2.*/

/* Function arguments needed by lockf() */
#define F_ULOCK		0	/* Unlock locked sections */
#define F_LOCK		1	/* Lock a section for exclusive use */
#define F_TLOCK		2	/* Test and lock a section for exclusive use */
#define F_TEST		3	/* Test a section for locks by other processes */

/*
 * _POSIX_VDISABLE
 *
 * This symbol shall be defined to be the value of a character that
 * shall disable terminal special character handling as described
 * in Special Control Characters .
 */
#define _POSIX_VDISABLE			((unsigned char)-2)

/* pathconf() constants */

#define _PC_LINK_MAX                      0
#define _PC_MAX_CANON                     1
#define _PC_MAX_INPUT                     2
#define _PC_NAME_MAX                      3
#define _PC_PATH_MAX                      4
#define _PC_PIPE_BUF                      5
#define _PC_CHOWN_RESTRICTED              6
#define _PC_NO_TRUNC                      7
#define _PC_VDISABLE                      8
#define _PC_ASYNC_IO                      9
#define _PC_PRIO_IO                      10
#define _PC_SYNC_IO                      11
#define _PC_FILESIZEBITS                 12
#define _PC_2_SYMLINKS                   13
#define _PC_SYMLINK_MAX                  14
#define _PC_ALLOC_SIZE_MIN               15
#define _PC_REC_INCR_XFER_SIZE           16
#define _PC_REC_MAX_XFER_SIZE            17
#define _PC_REC_MIN_XFER_SIZE            18
#define _PC_REC_XFER_ALIGN               19
#define _PC_TIMESTAMP_RESOLUTION         20
#define _PC_POSIX_PERMISSIONS            90
#define _PC_POSIX_SECURITY               91


/* sysconf() constants */
#define _SC_ARG_MAX                       0
#define _SC_CHILD_MAX                     1
#define _SC_CLK_TCK                       2
#define _SC_NGROUPS_MAX                   3
#define _SC_OPEN_MAX                      4
#define _SC_JOB_CONTROL                   5
#define _SC_SAVED_IDS                     6
#define _SC_VERSION                       7
#define _SC_PAGESIZE                      8
#define _SC_PAGE_SIZE                     _SC_PAGESIZE
#define _SC_NPROCESSORS_CONF              9
#define _SC_NPROCESSORS_ONLN             10
#define _SC_PHYS_PAGES                   11
#define _SC_AVPHYS_PAGES                 12
#define _SC_MQ_OPEN_MAX                  13
#define _SC_MQ_PRIO_MAX                  14
#define _SC_RTSIG_MAX                    15
#define _SC_SEM_NSEMS_MAX                16
#define _SC_SEM_VALUE_MAX                17
#define _SC_SIGQUEUE_MAX                 18
#define _SC_TIMER_MAX                    19
#define _SC_TZNAME_MAX                   20
#define _SC_ASYNCHRONOUS_IO              21
#define _SC_FSYNC                        22
#define _SC_MAPPED_FILES                 23
#define _SC_MEMLOCK                      24
#define _SC_MEMLOCK_RANGE                25
#define _SC_MEMORY_PROTECTION            26
#define _SC_MESSAGE_PASSING              27
#define _SC_PRIORITIZED_IO               28
#define _SC_REALTIME_SIGNALS             29
#define _SC_SEMAPHORES                   30
#define _SC_SHARED_MEMORY_OBJECTS        31
#define _SC_SYNCHRONIZED_IO              32
#define _SC_TIMERS                       33
#define _SC_AIO_LISTIO_MAX               34
#define _SC_AIO_MAX                      35
#define _SC_AIO_PRIO_DELTA_MAX           36
#define _SC_DELAYTIMER_MAX               37
#define _SC_THREAD_KEYS_MAX              38
#define _SC_THREAD_STACK_MIN             39
#define _SC_THREAD_THREADS_MAX           40
#define _SC_TTY_NAME_MAX                 41
#define _SC_THREADS                      42
#define _SC_THREAD_ATTR_STACKADDR        43
#define _SC_THREAD_ATTR_STACKSIZE        44
#define _SC_THREAD_PRIORITY_SCHEDULING   45
#define _SC_THREAD_PRIO_INHERIT          46
#define _SC_THREAD_PRIO_PROTECT          47
#define _SC_THREAD_PRIO_CEILING          _SC_THREAD_PRIO_PROTECT
#define _SC_THREAD_PROCESS_SHARED        48
#define _SC_THREAD_SAFE_FUNCTIONS        49
#define _SC_GETGR_R_SIZE_MAX             50
#define _SC_GETPW_R_SIZE_MAX             51
#define _SC_LOGIN_NAME_MAX               52
#define _SC_THREAD_DESTRUCTOR_ITERATIONS 53
#define _SC_ADVISORY_INFO                54
#define _SC_ATEXIT_MAX                   55
#define _SC_BARRIERS                     56
#define _SC_BC_BASE_MAX                  57
#define _SC_BC_DIM_MAX                   58
#define _SC_BC_SCALE_MAX                 59
#define _SC_BC_STRING_MAX                60
#define _SC_CLOCK_SELECTION              61
#define _SC_COLL_WEIGHTS_MAX             62
#define _SC_CPUTIME                      63
#define _SC_EXPR_NEST_MAX                64
#define _SC_HOST_NAME_MAX                65
#define _SC_IOV_MAX                      66
#define _SC_IPV6                         67
#define _SC_LINE_MAX                     68
#define _SC_MONOTONIC_CLOCK              69
#define _SC_RAW_SOCKETS                  70
#define _SC_READER_WRITER_LOCKS          71
#define _SC_REGEXP                       72
#define _SC_RE_DUP_MAX                   73
#define _SC_SHELL                        74
#define _SC_SPAWN                        75
#define _SC_SPIN_LOCKS                   76
#define _SC_SPORADIC_SERVER              77
#define _SC_SS_REPL_MAX                  78
#define _SC_SYMLOOP_MAX                  79
#define _SC_THREAD_CPUTIME               80
#define _SC_THREAD_SPORADIC_SERVER       81
#define _SC_TIMEOUTS                     82
#define _SC_TRACE                        83
#define _SC_TRACE_EVENT_FILTER           84
#define _SC_TRACE_EVENT_NAME_MAX         85
#define _SC_TRACE_INHERIT                86
#define _SC_TRACE_LOG                    87
#define _SC_TRACE_NAME_MAX               88
#define _SC_TRACE_SYS_MAX                89
#define _SC_TRACE_USER_EVENT_MAX         90
#define _SC_TYPED_MEMORY_OBJECTS         91
#define _SC_V7_ILP32_OFF32               92
#define _SC_V6_ILP32_OFF32               _SC_V7_ILP32_OFF32
#define _SC_XBS5_ILP32_OFF32             _SC_V7_ILP32_OFF32
#define _SC_V7_ILP32_OFFBIG              93
#define _SC_V6_ILP32_OFFBIG              _SC_V7_ILP32_OFFBIG
#define _SC_XBS5_ILP32_OFFBIG            _SC_V7_ILP32_OFFBIG
#define _SC_V7_LP64_OFF64                94
#define _SC_V6_LP64_OFF64                _SC_V7_LP64_OFF64
#define _SC_XBS5_LP64_OFF64              _SC_V7_LP64_OFF64
#define _SC_V7_LPBIG_OFFBIG              95
#define _SC_V6_LPBIG_OFFBIG              _SC_V7_LPBIG_OFFBIG
#define _SC_XBS5_LPBIG_OFFBIG            _SC_V7_LPBIG_OFFBIG
#define _SC_XOPEN_CRYPT                  96
#define _SC_XOPEN_ENH_I18N               97
#define _SC_XOPEN_LEGACY                 98
#define _SC_XOPEN_REALTIME               99
#define _SC_STREAM_MAX                  100
#define _SC_PRIORITY_SCHEDULING         101
#define _SC_XOPEN_REALTIME_THREADS      102
#define _SC_XOPEN_SHM                   103
#define _SC_XOPEN_STREAMS               104
#define _SC_XOPEN_UNIX                  105
#define _SC_XOPEN_VERSION               106
#define _SC_2_CHAR_TERM                 107
#define _SC_2_C_BIND                    108
#define _SC_2_C_DEV                     109
#define _SC_2_FORT_DEV                  110
#define _SC_2_FORT_RUN                  111
#define _SC_2_LOCALEDEF                 112
#define _SC_2_PBS                       113
#define _SC_2_PBS_ACCOUNTING            114
#define _SC_2_PBS_CHECKPOINT            115
#define _SC_2_PBS_LOCATE                116
#define _SC_2_PBS_MESSAGE               117
#define _SC_2_PBS_TRACK                 118
#define _SC_2_SW_DEV                    119
#define _SC_2_UPE                       120
#define _SC_2_VERSION                   121
#define _SC_THREAD_ROBUST_PRIO_INHERIT  122
#define _SC_THREAD_ROBUST_PRIO_PROTECT  123
#define _SC_XOPEN_UUCP                  124

/*
 * The <unistd.h> header shall define the following symbolic constants
 * for the confstr() function
 */
#define _CS_PATH                               0
#define _CS_POSIX_V7_ILP32_OFF32_CFLAGS        1
#define _CS_POSIX_V6_ILP32_OFF32_CFLAGS       _CS_POSIX_V7_ILP32_OFF32_CFLAGS
#define _CS_XBS5_ILP32_OFF32_CFLAGS           _CS_POSIX_V7_ILP32_OFF32_CFLAGS
#define _CS_POSIX_V7_ILP32_OFF32_LDFLAGS       2
#define _CS_POSIX_V6_ILP32_OFF32_LDFLAGS      _CS_POSIX_V7_ILP32_OFF32_LDFLAGS
#define _CS_XBS5_ILP32_OFF32_LDFLAGS          _CS_POSIX_V7_ILP32_OFF32_LDFLAGS
#define _CS_POSIX_V7_ILP32_OFF32_LIBS          3
#define _CS_POSIX_V6_ILP32_OFF32_LIBS         _CS_POSIX_V7_ILP32_OFF32_LIBS
#define _CS_XBS5_ILP32_OFF32_LIBS             _CS_POSIX_V7_ILP32_OFF32_LIBS
#define _CS_XBS5_ILP32_OFF32_LINTFLAGS         4
#define _CS_POSIX_V7_ILP32_OFFBIG_CFLAGS       5
#define _CS_POSIX_V6_ILP32_OFFBIG_CFLAGS      _CS_POSIX_V7_ILP32_OFFBIG_CFLAGS
#define _CS_XBS5_ILP32_OFFBIG_CFLAGS          _CS_POSIX_V7_ILP32_OFFBIG_CFLAGS
#define _CS_POSIX_V7_ILP32_OFFBIG_LDFLAGS      6
#define _CS_POSIX_V6_ILP32_OFFBIG_LDFLAGS     _CS_POSIX_V7_ILP32_OFFBIG_LDFLAGS
#define _CS_XBS5_ILP32_OFFBIG_LDFLAGS         _CS_POSIX_V7_ILP32_OFFBIG_LDFLAGS
#define _CS_POSIX_V7_ILP32_OFFBIG_LIBS         7
#define _CS_POSIX_V6_ILP32_OFFBIG_LIBS        _CS_POSIX_V7_ILP32_OFFBIG_LIBS
#define _CS_XBS5_ILP32_OFFBIG_LIBS            _CS_POSIX_V7_ILP32_OFFBIG_LIBS
#define _CS_XBS5_ILP32_OFFBIG_LINTFLAGS        8
#define _CS_POSIX_V7_LP64_OFF64_CFLAGS         9
#define _CS_POSIX_V6_LP64_OFF64_CFLAGS        _CS_POSIX_V7_LP64_OFF64_CFLAGS
#define _CS_XBS5_LP64_OFF64_CFLAGS            _CS_POSIX_V7_LP64_OFF64_CFLAGS
#define _CS_POSIX_V7_LP64_OFF64_LDFLAGS       10
#define _CS_POSIX_V6_LP64_OFF64_LDFLAGS       _CS_POSIX_V7_LP64_OFF64_LDFLAGS
#define _CS_XBS5_LP64_OFF64_LDFLAGS           _CS_POSIX_V7_LP64_OFF64_LDFLAGS
#define _CS_POSIX_V7_LP64_OFF64_LIBS          11
#define _CS_POSIX_V6_LP64_OFF64_LIBS          _CS_POSIX_V7_LP64_OFF64_LIBS
#define _CS_XBS5_LP64_OFF64_LIBS              _CS_POSIX_V7_LP64_OFF64_LIBS
#define _CS_XBS5_LP64_OFF64_LINTFLAGS         12
#define _CS_POSIX_V7_LPBIG_OFFBIG_CFLAGS      13
#define _CS_POSIX_V6_LPBIG_OFFBIG_CFLAGS      _CS_POSIX_V7_LPBIG_OFFBIG_CFLAGS
#define _CS_XBS5_LPBIG_OFFBIG_CFLAGS          _CS_POSIX_V7_LPBIG_OFFBIG_CFLAGS
#define _CS_POSIX_V7_LPBIG_OFFBIG_LDFLAGS     14
#define _CS_POSIX_V6_LPBIG_OFFBIG_LDFLAGS     _CS_POSIX_V7_LPBIG_OFFBIG_LDFLAGS
#define _CS_XBS5_LPBIG_OFFBIG_LDFLAGS         _CS_POSIX_V7_LPBIG_OFFBIG_LDFLAGS
#define _CS_POSIX_V7_LPBIG_OFFBIG_LIBS        15
#define _CS_POSIX_V6_LPBIG_OFFBIG_LIBS        _CS_POSIX_V7_LPBIG_OFFBIG_LIBS
#define _CS_XBS5_LPBIG_OFFBIG_LIBS            _CS_POSIX_V7_LPBIG_OFFBIG_LIBS
#define _CS_XBS5_LPBIG_OFFBIG_LINTFLAGS       16
#define _CS_POSIX_V7_WIDTH_RESTRICTED_ENVS    17
#define _CS_POSIX_V6_WIDTH_RESTRICTED_ENVS    _CS_POSIX_V7_WIDTH_RESTRICTED_ENVS
#define _CS_POSIX_V7_THREADS_CFLAGS           18
#define _CS_POSIX_V7_THREADS_LDFLAGS          19
#define _CS_V7_ENV                            20
#define _CS_V6_ENV                           _CS_V6_ENV

/* lseek() constants */
#ifndef SEEK_SET
#	define SEEK_SET 0
#endif
#ifndef SEEK_CUR
#	define SEEK_CUR 1
#endif
#ifndef SEEK_END
#	define SEEK_END 2
#endif

#ifdef __cplusplus
extern "C" {
#endif

int          access(const char *, int);
unsigned     alarm(unsigned);
int          chdir(const char *);
int          chown(const char *, uid_t, gid_t);
int          close(int);
size_t       confstr(int, char *, size_t);
char        *crypt(const char *, const char *);
char        *ctermid(char *);
int          dup(int);
int          dup2(int, int);
void         _exit(int);
void         encrypt(char [64], int);
int          execl(const char *, const char *, ...);
int          execle(const char *, const char *, ...);
int          execlp(const char *, const char *, ...);
int          execv(const char *, char *const []);
int          execve(const char *, char *const [], char *const []);
int          execvp(const char *, char *const []);
int          faccessat(int, const char *, int, int);
int          fchdir(int);
int          fchown(int, uid_t, gid_t);
int          fchownat(int, const char *, uid_t, gid_t, int);
int          fdatasync(int);
int          fexecve(int, char *const [], char *const []);
pid_t        fork(void);
long         fpathconf(int, int);
int          fsync(int);
int          ftruncate(int, off_t);
char        *getcwd(char *, size_t);
gid_t        getegid(void);
uid_t        geteuid(void);
gid_t        getgid(void);
int          getgroups(int, gid_t []);
long         gethostid(void);
int          gethostname(char *, size_t);
char        *getlogin(void);
int          getlogin_r(char *, size_t);
int          getopt(int, char * const [], const char *);
pid_t        getpgid(pid_t);
pid_t        getpgrp(void);
pid_t        getpid(void);
pid_t        getppid(void);
pid_t        getsid(pid_t);
uid_t        getuid(void);
int          isatty(int);
int          lchown(const char *, uid_t, gid_t);
int          link(const char *, const char *);
int          linkat(int, const char *, int, const char *, int);
int          lockf(int, int, off_t);
off_t        lseek(int, off_t, int);
int          nice(int);
long         pathconf(const char *, int);
int          pause(void);
int          pipe(int [2]);
ssize_t      pread(int, void *, size_t, off_t);
ssize_t      pwrite(int, const void *, size_t, off_t);
ssize_t      read(int, void *, size_t);
ssize_t      readlink(const char *restrict, char *restrict, size_t);
ssize_t      readlinkat(int, const char *restrict, char *restrict, size_t);
int          rmdir(const char *);
int          setegid(gid_t);
int          seteuid(uid_t);
int          setgid(gid_t);
int          setpgid(pid_t, pid_t);
pid_t        setpgrp(void);
int          setregid(gid_t, gid_t);
int          setreuid(uid_t, uid_t);
pid_t        setsid(void);
int          setuid(uid_t);
unsigned     sleep(unsigned);
void         swab(const void *restrict, void *restrict, ssize_t);
int          symlink(const char *, const char *);
int          symlinkat(const char *, int, const char *);
void         sync(void);
long         sysconf(int);
pid_t        tcgetpgrp(int);
int          tcsetpgrp(int, pid_t);
int          truncate(const char *, off_t);
char        *ttyname(int);
int          ttyname_r(int, char *, size_t);
int          unlink(const char *);
int          unlinkat(int, const char *, int);
ssize_t      write(int, const void *, size_t);

/* getopt() related external variables */
extern char *optarg;
extern int optind, opterr, optopt;

#ifdef __cplusplus
}
#endif

#endif  /* _POSIX_UNISTD_H */
