/* sys.h - data types */

#ifndef _POSIX_SYS_TYPES_H
#define _POSIX_SYS_TYPES_H

#include <config.h>
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <inttypes.h>
#include <ctype.h>
#include <errno.h>
#include <sys/cdefs.h>

/* BSD compatibility */
typedef unsigned long 		u_long;
typedef unsigned int 		u_int;
typedef unsigned short 		u_short;
typedef unsigned char 		u_char;

/* SysV compatibility */
typedef unsigned long 		ulong;
typedef unsigned short 		ushort;
typedef unsigned int 		uint;
typedef unsigned char		unchar;

/* 
 * id_t 
 *
 * Used as a general identifier; can be used to contain at least
 * a pid_t, uid_t, or gid_t.
 */

typedef long	id_t;   

typedef id_t    dev_t; /* Used for device IDs. */
typedef id_t    pid_t; /* Used for process IDs and process group IDs. */
typedef id_t    uid_t;  /* Used for user IDs. */
typedef id_t    gid_t;  /* Used for group IDs.*/

typedef id_t    trace_event_id_t;  /* Used to identify a trace event type.*/
typedef id_t    trace_event_set_t; /* Used to identify a trace event type set.*/
typedef id_t    trace_id_t;        /* Used to identify a trace stream.*/

typedef id_t    key_t;  /* Used for XSI interprocess communication.*/

typedef id_t    timer_t;/* Used for timer ID returned by timer_create().*/
                        /* Also used to identify a trace stream attributes object.*/
                        
/* Used for clock ID type in the clock and timer functions. */
typedef id_t   clockid_t; 

typedef int32_t	            cnt_t;

typedef int64_t 		    blkcnt_t;   /* Used for file block counts.*/
typedef int32_t	            blksize_t;  /* Used for block sizes.*/
typedef int64_t		        fsblkcnt_t; /* Used for file system block counts.*/
typedef int64_t		        fsfilcnt_t; /* Used for file system file counts.*/
typedef int64_t		        off_t; /* Used for file sizes.*/
typedef int64_t		        ino_t; /* Used for file serial numbers.*/


typedef uint32_t            mode_t; /* Used for some file attributes.*/
typedef uint32_t	        umode_t;/* Used for some file attributes.*/
typedef int32_t	            nlink_t;/* Used for link counts.*/

typedef off_t	            daddr_t; /* Used for disk address */	
typedef char*				caddr_t; /* Used for char pointer address */

typedef unsigned long		addr_t; /* Used for CPU and BUS address */

typedef int64_t             abstime_t;  /* Absolute time type (signed 64-bit) */
typedef int32_t             clock_t;    /* Used for system times in clock ticks or CLOCKS_PER_SEC;*/
typedef int32_t             time_t;     /* Used for time in seconds.*/
typedef int32_t             suseconds_t;/* Used for time in microseconds. */
typedef uint32_t            useconds_t; /* Used for time in microseconds. */

typedef unsigned long       unative_t;
typedef long                native_t;
typedef int                 status_t;
typedef int                 boolean_t;
typedef uint32_t            magic_t;
typedef long                policy_param_t;
typedef addr_t              cpu_addr_t;
typedef addr_t              bus_addr_t;
typedef unsigned long       ipl_t;
typedef unsigned long       ptr_t;
typedef unsigned long       irqstate_t;
typedef int32_t             wait_t;

#endif /* _POSIX_SYS_TYPES_H */
