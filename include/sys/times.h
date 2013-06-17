/* sys/times.h - file access and modification times structure */

#ifndef _POSIX_SYS_TIMES_H
#define _POSIX_SYS_TIMES_H

#include <time.h>

/* The <sys/times.h> header shall define the tms structure */
typedef struct tms
    {
    clock_t  tms_utime;  /* User CPU time. */
    clock_t  tms_stime;  /* System CPU time. */
    clock_t  tms_cutime; /* User CPU time of terminated child processes. */
    clock_t  tms_cstime; /* System CPU time of terminated child processes. */
    }tms_t;


#ifdef __cplusplus
extern "C" {
#endif

clock_t times(struct tms *tms);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	/* _POSIX_SYS_TIMES_H */
