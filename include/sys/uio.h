/* sys/uio.h - definitions for vector I/O operations */

#ifndef _POSIX_SYS_UIO_H
#define _POSIX_SYS_UIO_H


#include <sys.h>


/* The <sys/uio.h> header shall define the iovec structure */
typedef struct iovec
    {
    void   *iov_base;  /* Base address of a memory region for input or output.*/
    size_t  iov_len;   /* The size of the memory pointed to by iov_base.*/

    } iovec;


#ifdef __cplusplus
extern "C" {
#endif

ssize_t readv(int fd, const struct iovec *vector, size_t count);
ssize_t readv_pos(int fd, off_t pos, const struct iovec *vec, size_t count);
ssize_t writev(int fd, const struct iovec *vector, size_t count);
ssize_t writev_pos(int fd, off_t pos, const struct iovec *vec, size_t count);

#ifdef __cplusplus
}
#endif

#endif /* _POSIX_SYS_UIO_H */
