/* sys/time.h - time types */

#ifndef _POSIX_SYS_TIME_H
#define _POSIX_SYS_TIME_H


#include <sys.h>

/* The <sys/time.h> header shall define the timeval structure */
typedef struct timeval
    {
    time_t        tv_sec;        /* seconds */
    suseconds_t    tv_usec;    /* microseconds */
    }timeval_t;

#include <sys/select.h>
    /* circular dependency: fd_set needs to be defined and the
     * select prototype exported by this file, but <sys/select.h>
     * needs struct timeval.
     */

struct timezone
    {
    int tz_minuteswest;
    int tz_dsttime;
    };

struct itimerval
    {
    struct timeval it_interval;
    struct timeval it_value;
    };

/*
 * The <sys/time.h> header shall define the following symbolic
 * constants for the which argument of getitimer() and setitimer()
 */
#define ITIMER_REAL        1    /* Decrements in real time. */
#define ITIMER_VIRTUAL     2    /* Decrements in process virtual time. */
#define ITIMER_PROF        3    /* Decrements both in process virtual time
                             * and when the system is running on behalf
                             * of the process. */


#ifdef __cplusplus
extern "C" {
#endif

int   getitimer(int, struct itimerval *);
int   gettimeofday(struct timeval *restrict, void *restrict);
int   setitimer(int, const struct itimerval *restrict,
          struct itimerval *restrict);
int   select(int, fd_set *restrict, fd_set *restrict, fd_set *restrict,
          struct timeval *restrict);
int   utimes(const char *, const struct timeval [2]);

#ifdef __cplusplus
}
#endif

/* BSD compatible macros operating on timeval structs */
#define timeradd(a, b, res)                                \
    do {                                                \
        (res)->tv_sec = (a)->tv_sec + (b)->tv_sec;        \
        (res)->tv_usec = (a)->tv_usec + (b)->tv_usec;    \
        if ((res)->tv_usec >= 1000000) {                \
            (res)->tv_usec -= 1000000;                    \
            (res)->tv_sec++;                            \
        }                                                \
    } while (0)
#define timersub(a, b, res)                                \
    do {                                                \
        (res)->tv_sec = (a)->tv_sec - (b)->tv_sec;        \
        (res)->tv_usec = (a)->tv_usec - (b)->tv_usec;    \
        if ((res)->tv_usec < 0) {                        \
            (res)->tv_usec += 1000000;                    \
            (res)->tv_sec--;                            \
        }                                                \
    } while (0)
#define timerclear(a)    ((a)->tv_sec = (a)->tv_usec = 0)
#define timerisset(a)    ((a)->tv_sec != 0 || (a)->tv_usec != 0)
#define timercmp(a, b, cmp)    ((a)->tv_sec == (b)->tv_sec \
        ? (a)->tv_usec cmp (b)->tv_usec : (a)->tv_sec cmp (b)->tv_sec)

#endif    /* _POSIX_SYS_TIME_H */
