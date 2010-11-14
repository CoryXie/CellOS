/* sys/resource.h - definitions for XSI resource operations */

#ifndef _POSIX_SYS_RESOURCE_H
#define _POSIX_SYS_RESOURCE_H

#include <sys/cdefs.h>
#include <sys.h>
#include <sys/time.h>

/* getrlimit()/setrlimit() definitions */

typedef addr_t rlim_t; /* Unsigned integer type used for limit values. */

/* The <sys/resource.h> header shall define the rlimit structure */
typedef struct rlimit 
    {
	rlim_t	rlim_cur;		/* The current (soft) limit. */
	rlim_t	rlim_max;		/* The hard limit. */
    }rlimit_t;

/*
 * The <sys/resource.h> header shall define the following symbolic 
 * constants as possible values for the resource argument of 
 * getrlimit() and setrlimit()
 */
 
#define RLIMIT_CORE		0	/* Limit on size of core file. */
#define RLIMIT_CPU		1	/* Limit on CPU time per process. */
#define RLIMIT_DATA		2	/* Limit on data segment size. */
#define RLIMIT_FSIZE	3	/* Limit on file size. */
#define RLIMIT_NOFILE	4	/* Limit on number of open files. */
#define RLIMIT_STACK	5	/* Limit on stack size. */
#define RLIMIT_AS		6	/* Limit on address space size. */
#define RLIMIT_NOVMON	7	/* Number of open vnode monitors */
#define RLIM_NLIMITS	8	/* Number of resource limits */

/*
 * The <sys/resource.h> header shall define the following symbolic
 * constants, which shall have values suitable for use in #if 
 * preprocessing directives
 *
 * On implementations where all resource limits are representable 
 * in an object of type rlim_t, RLIM_SAVED_MAX and RLIM_SAVED_CUR 
 * need not be distinct from RLIM_INFINITY.
 */
 
#define RLIM_INFINITY	INTPTR_MAX    /* A value of rlim_t indicating no limit. */
#define RLIM_SAVED_MAX	RLIM_INFINITY /* A value of type rlim_t indicating an 
                                       * unrepresentable saved hard limit.*/
#define RLIM_SAVED_CUR	RLIM_INFINITY /* A value of type rlim_t indicating an
                                       * unrepresentable saved soft limit.*/

/* The <sys/resource.h> header shall define the rusage structure */

typedef struct rusage 
    {
	struct timeval ru_utime;	/* User time used */
	struct timeval ru_stime;	/* System time used */
    }rusage_t;

/* 
 * The <sys/resource.h> header shall define the following symbolic
 * constants as possible values of the who parameter of getrusage()
 */
 
#define RUSAGE_SELF     0   /* Returns information about the current process. */
#define RUSAGE_CHILDREN -1  /* Returns information about children of the current process.*/


/* 
 * The <sys/resource.h> header shall define the following 
 * symbolic constants as possible values of the which argument
 * of getpriority() and setpriority() 
 */

#define PRIO_PROCESS	0   /* Identifies the who argument as a process ID. */
#define PRIO_PGRP		1   /* Identifies the who argument as a process group ID. */
#define PRIO_USER		2   /* Identifies the who argument as a user ID. */

#ifdef __cplusplus
extern "C" {
#endif

int  getpriority(int, id_t);
int  getrlimit(int, struct rlimit *);
int  getrusage(int, struct rusage *);
int  setpriority(int, id_t, int);
int  setrlimit(int, const struct rlimit *);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	/* _POSIX_SYS_RESOURCE_H */
