/* poll.h - definitions for the poll() function */

#ifndef _POSIX_POLL_H
#define _POSIX_POLL_H

/* 
 * nfds_t
 *
 * An unsigned integer type used for the number of file descriptors.
 *
 * The implementation shall support one or more programming 
 * environments in which the width of nfds_t is no greater than
 * the width of type long. The names of these programming 
 * environments can be obtained using the confstr() function 
 * or the getconf utility.
 */
 
typedef unsigned long nfds_t;

/* The <poll.h> header shall define the pollfd structure */

struct pollfd 
    {
	int		fd;         /* The following descriptor being polled. */
	short	events;		/* The input event flags (see below). */
	short	revents;	/* The output event flags (see below). */
    };

/* 
 * The <poll.h> header shall define the following symbolic constants, 
 * zero or more of which may be OR'ed together to form the events or 
 * revents members in the pollfd structure 
 */
 
#define	POLLIN		0x0001		/* Data other than high-priority data
                                 * may be read without blocking. */
#define	POLLOUT		0x0002		/* Normal data may be written without 
                                 * blocking. */
#define	POLLRDNORM	POLLIN      /* Normal data may be read without 
                                 * blocking.*/
#define	POLLWRNORM	POLLOUT     /* Equivalent to POLLOUT. */
#define	POLLRDBAND	0x0008		/* Priority data may be read without
                                 * blocking. */
#define	POLLWRBAND	0x0010		/* Priority data may be written. */
#define	POLLPRI		0x0020		/* High priority data may be read 
                                 * without blocking. */

#define	POLLERR		0x0004		/* An error has occurred ( revents only). */
#define	POLLHUP		0x0080		/* Device has been disconnected ( revents only). */
#define	POLLNVAL	0x1000		/* Invalid fd member ( revents only). */

/* 
 * The significance and semantics of normal, priority, and high-priority 
 * data are file and device-specific.
 */
 
#ifdef __cplusplus
extern "C" {
#endif

int poll(struct pollfd *fds, nfds_t numfds, int timeout);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _POSIX_POLL_H */
