/* mqueue.h - message queues (REALTIME) */

#ifndef _POSIX_MQUEUE_H
#define _POSIX_MQUEUE_H

/* 
 * The <mqueue.h> header shall define the pthread_attr_t, 
 * size_t, and ssize_t types as described in <sys.h>
 */
 
#include <sys.h>

/* 
 * The <mqueue.h> header shall define the struct timespec
 * structure as described in <time.h>.
 */
 
#include <time.h>

/*
 * The tag sigevent shall be declared as naming an incomplete
 * structure type, the contents of which are described in the
 * <signal.h> header.
 */
 
#include <signal.h>

/* 
 * Inclusion of the <mqueue.h> header may make visible symbols 
 * defined in the headers <fcntl.h>, <signal.h>, and <time.h>.
 */
 
#include <fcntl.h>
 
#ifdef __cplusplus
extern "C" {
#endif

/*
 * Opaque message queue descriptor
 *
 * The <mqueue.h> header shall define the mqd_t type,
 * which is used for message queue descriptors. This 
 * is not an array type.
 */

typedef void    *mqd_t;    

/*
 * The <mqueue.h> header shall define the mq_attr structure, 
 * which is used in getting and setting the attributes of
 * a message queue. Attributes are initially set when the 
 * message queue is created. An mq_attr structure shall have
 * at least the following fields.
 */
 
struct mq_attr 
    {
    long    mq_flags;   /* Message queue flags */
    long    mq_maxmsg;  /* Maximum number of messages */
    long    mq_msgsize; /* Maximum message size */
    long    mq_curmsgs; /* Number of messages currently queued */
    int     mq_pad[12]; /* Padding */
    };

int      mq_close(mqd_t);
int      mq_getattr(mqd_t, struct mq_attr *);
int      mq_notify(mqd_t, const struct sigevent *);
mqd_t    mq_open(const char *, int, ...);
ssize_t  mq_receive(mqd_t, char *, size_t, unsigned *);
int      mq_send(mqd_t, const char *, size_t, unsigned);
int      mq_setattr(mqd_t, const struct mq_attr *restrict,
             struct mq_attr *restrict);
ssize_t  mq_timedreceive(mqd_t, char *restrict, size_t,
             unsigned *restrict, const struct timespec *restrict);
int      mq_timedsend(mqd_t, const char *, size_t, unsigned,
             const struct timespec *);
int      mq_unlink(const char *);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _POSIX_MQUEUE_H */

