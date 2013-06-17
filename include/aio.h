/* aio.h - asynchronous input and output */

/*
 *
 * Inclusion of the <aio.h> header may make visible symbols 
 * defined in the headers <fcntl.h>, <signal.h>, and <time.h>.
 */
 
#ifndef _POSIX_AIO_H_
#define _POSIX_AIO_H_

#include <sys.h>
#include <sys/cdefs.h>
#include <sys/signal.h>
#include <sys/aio.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct aiocb
    {
    int             aio_fildes;    /* File descriptor. */
    off_t           aio_offset;    /* File offset. */
    volatile void  *aio_buf;       /* Location of buffer. */ 
    size_t          aio_nbytes;    /* Length of transfer. */
    int             aio_reqprio;   /* Request priority offset. */
    struct sigevent aio_sigevent;  /* Signal number and value. */
    int             aio_lio_opcode;/* Operation to be performed.*/ 
    /* May add more ... */
    }aiocb_t;

int      aio_cancel(int, struct aiocb *);
int      aio_error(const struct aiocb *);
int      aio_fsync(int, struct aiocb *);
int      aio_read(struct aiocb *);
ssize_t  aio_return(struct aiocb *);
int      aio_suspend(const struct aiocb *const [], int,
             const struct timespec *);
int      aio_write(struct aiocb *);
int      lio_listio(int, struct aiocb *restrict const [restrict], int,
             struct sigevent *restrict);

/*
 * AIO_ALLDONE
 *
 * A return value indicating that none of the requested operations
 * could be canceled since they are already complete.
 *
 * AIO_CANCELED
 *
 * A return value indicating that all requested operations have been
 * canceled.
 * 
 * AIO_NOTCANCELED
 *
 * A return value indicating that some of the requested operations 
 * could not be canceled since they are in progress.
 */
 
#define AIO_ALLDONE         0x00
#define AIO_CANCELED        0x01
#define AIO_NOTCANCELED     0x02

/* 
 * LIO_NOP
 * 
 * A lio_listio() element operation option indicating that no transfer
 * is requested.
 * 
 * LIO_NOWAIT
 *
 * A lio_listio() synchronization operation indicating that the calling
 * thread is to continue execution while the lio_listio() operation is 
 * being performed, and no notification is given when the operation is 
 * complete.
 *
 * LIO_READ
 *
 * A lio_listio() element operation option requesting a read.
 *
 * LIO_WAIT
 *
 * A lio_listio() synchronization operation indicating that the calling
 * thread is to suspend until the lio_listio() operation is complete.
 *
 * LIO_WRITE
 *
 * A lio_listio() element operation option requesting a write.
 */

#define LIO_NOP     0x00
#define LIO_NOWAIT  0x00
#define LIO_READ    0x00
#define LIO_WAIT    0x00
#define LIO_WRITE   0x00

#ifdef __cplusplus
}
#endif
#endif /* _POSIX_AIO_H_ */
