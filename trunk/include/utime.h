/* utime.h - access and modification times structure */

/*
APPLICATION USAGE

    The utime() function only allows setting file timestamps
    to the nearest second. Applications should use the utimensat()
    function instead. See <sys/stat.h>.
*/
#ifndef _POSIX_UTIME_H
#define _POSIX_UTIME_H

#include <sys.h>

#ifdef __cplusplus
extern "C" {
#endif

/* The <utime.h> header shall declare the utimbuf structure */

typedef struct utimbuf
    {
	time_t actime;		/* Mccess time */
	time_t modtime;		/* Modification time */
    }utimbuf_t;

int utime(const char *path, const struct utimbuf *buffer);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	/* _POSIX_UTIME_H */
