/* time.h - time types */

#ifndef _POSIX_TIME_H
#define _POSIX_TIME_H

/*
 * The <time.h> header shall define the clock_t, size_t, time_t, types
 * as described in <sys.h>.
 *
 * The <time.h> header shall define the clockid_t and timer_t types as
 * described in <sys.h>.
 *
 * The <time.h> header shall define the locale_t type as described in
 * <locale.h>.
 *
 * The <time.h> header shall define the pid_t type as described in
 * <sys.h>.
 *
 * The tag sigevent shall be declared as naming an incomplete structure
 * type, the contents of which are described in the <signal.h> header.
 */

#include <sys.h>
#include <locale.h>

struct sigevent;

/*
 * CLOCKS_PER_SEC
 *
 * A number used to convert the value returned by the clock() function
 * into seconds. The value shall be an expression with type clock_t.
 *
 * The value of CLOCKS_PER_SEC shall be 1 million on XSI-conformant systems.
 * However, it may be variable on other systems, and it should not be
 * that CLOCKS_PER_SEC is a compile-time constant.
 */
#define CLOCKS_PER_SEC	1000
#define CLK_TCK			CLOCKS_PER_SEC

/*
 * The <time.h> header shall define the following symbolic constants.
 * The values shall have a type that is assignment-compatible with clockid_t.
 */

/*
 * CLOCK_MONOTONIC
 *
 * The identifier for the system-wide monotonic clock, which is defined
 * as a clock measuring real time, whose value cannot be set via
 * clock_settime() and which cannot have negative clock jumps. The maximum
 * possible clock jump shall be implementation-defined.
 *
 *
 * CLOCK_PROCESS_CPUTIME_ID
 *
 * The identifier of the CPU-time clock associated with the process making
 * a clock() or timer*() function call.
 *
 * CLOCK_REALTIME
 *
 * The identifier of the system-wide clock measuring real time.
 *
 * CLOCK_THREAD_CPUTIME_ID
 *
 * The identifier of the CPU-time clock associated with the thread making a
 * clock() or timer*() function call.
 */

#define CLOCK_MONOTONIC             0x01
#define CLOCK_PROCESS_CPUTIME_ID    0x02
#define CLOCK_REALTIME              0x03
#define CLOCK_THREAD_CPUTIME_ID     0x04

/*
 * TIMER_ABSTIME
 *
 * Flag indicating time is absolute. For functions taking timer objects,
 * this refers to the clock associated with the timer.
 */

#define TIMER_ABSTIME               0x01

/*
 * The <time.h> header shall provide a declaration or definition for
 * getdate_err. The getdate_err symbol shall expand to an expression
 * of type int. It is unspecified whether getdate_err is a macro or
 * an identifier declared with external linkage, and whether or not
 * it is a modifiable lvalue. If a macro definition is suppressed in
 * order to access an actual object, or a progrm defines an identifier
 * with the name getdate_err, the behavior is undefined.
 */

extern int getdate_err;

/* Maximum length of a string returned by asctime(), and ctime() */

#define MAX_TIMESTR		70

/* The <time.h> header shall declare the timespec structure */
typedef struct timespec
    {
	time_t	tv_sec;		/* Seconds */
	long	tv_nsec;	/* Nanoseconds */
    }timespec_t;

/* The <time.h> header shall also declare the itimerspec structure */
typedef struct itimerspec
    {
	struct timespec it_interval; /* Timer period. */
	struct timespec it_value;    /* Timer expiration. */
    }itimerspec_t;

/*
 * The <time.h> header shall declare the tm structure.
 *
 * The value of tm_isdst shall be positive if Daylight Savings Time
 * is in effect, 0 if Daylight Savings Time is not in effect, and
 * negative if the information is not available.
 */
typedef struct tm
    {
    int    tm_sec;   /* Seconds [0,60]. */
    int    tm_min;   /* Minutes [0,59]. */
    int    tm_hour;  /* Hour [0,23]. */
    int    tm_mday;  /* Day of month [1,31]. */
    int    tm_mon;   /* Month of year [0,11]. */
    int    tm_year;  /* Years since 1900. */
    int    tm_wday;  /* Day of week [0,6] (Sunday =0). */
    int    tm_yday;  /* Day of year [0,365]. */
    int    tm_isdst; /* Daylight Savings flag. */
    int    tm_gmtoff;/* timezone offset to GMT */
	char * tm_zone;	 /* timezone name */
    }tm_t;


/* Special timezone support */
extern char *tzname[2];
extern int 	daylight;
extern long	timezone;


#ifdef __cplusplus
extern "C" {
#endif

char      *asctime(const struct tm *);
char      *asctime_r(const struct tm *restrict, char *restrict);
clock_t    clock(void);
int        clock_getcpuclockid(pid_t, clockid_t *);
int        clock_getres(clockid_t, struct timespec *);
int        clock_gettime(clockid_t, struct timespec *);
int        clock_nanosleep(clockid_t, int, const struct timespec *,
               struct timespec *);
int        clock_settime(clockid_t, const struct timespec *);
char      *ctime(const time_t *);
char      *ctime_r(const time_t *, char *);
double     difftime(time_t, time_t);
struct tm *getdate(const char *);
struct tm *gmtime(const time_t *);
struct tm *gmtime_r(const time_t *restrict, struct tm *restrict);
struct tm *localtime(const time_t *);
struct tm *localtime_r(const time_t *restrict, struct tm *restrict);
time_t     mktime(struct tm *);
int        nanosleep(const struct timespec *, struct timespec *);
size_t     strftime(char *restrict, size_t, const char *restrict,
           const struct tm *restrict);
size_t     strftime_l(char *restrict, size_t, const char *restrict,
               const struct tm *restrict, locale_t);
char      *strptime(const char *restrict, const char *restrict,
               struct tm *restrict);
time_t     time(time_t *);
int        timer_create(clockid_t, struct sigevent *restrict,
               timer_t *restrict);
int        timer_delete(timer_t);
int        timer_getoverrun(timer_t);
int        timer_gettime(timer_t, struct itimerspec *);
int        timer_settime(timer_t, int, const struct itimerspec *restrict,
               struct itimerspec *restrict);
void       tzset(void);

#ifdef __cplusplus
}
#endif
#endif	/* _POSIX_TIME_H */
