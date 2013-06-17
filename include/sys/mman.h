/* sys/mman.h - memory management declarations */

#ifndef _POSIX_SYS_MMAN_H
#define _POSIX_SYS_MMAN_H

#include <sys/cdefs.h>
#include <sys.h>

/*
 * The <sys/mman.h> header shall define the following symbolic
 * constants for use as protection options 
 */

#define PROT_READ		0x01    /* Page can be read. */
#define PROT_WRITE		0x02    /* Page can be written. */
#define PROT_EXEC		0x04    /* Page can be executed. */
#define PROT_NONE		0x00    /* Page cannot be accessed. */

/* 
 * The <sys/mman.h> header shall define the following symbolic
 * constants for use as flag options 
 */
 
#define MAP_SHARED		0x01    /* Changes are seen by others */
#define MAP_PRIVATE		0x02    /* Changes are only seen by caller */
#define MAP_FIXED		0x04    /* Require mapping to specified addr */
#define MAP_ANONYMOUS	0x08    /* No underlying object */
#define MAP_ANON		MAP_ANONYMOUS

/* 
 * The <sys/mman.h> header shall define the following symbolic
 * constants for the msync() function 
 */
#define MS_ASYNC		0x01    /* Perform asynchronous writes. */
#define MS_SYNC			0x02    /* Invalidate mappings. */
#define MS_INVALIDATE	0x04    /* Perform synchronous writes. */

/* 
 * The <sys/mman.h> header shall define the following symbolic 
 * constants for the mlockall() function
 */

#define MCL_CURRENT 0x01 /* Lock currently mapped pages. */
#define MCL_FUTURE  0x02 /* Lock pages that become mapped. */

/*
 * The <sys/mman.h> header shall define the symbolic constant 
 * MAP_FAILED which shall have type void * and shall be used 
 * to indicate a failure from the mmap() function
 */
 
#define MAP_FAILED		((void*)-1)

/* 
 * If the Advisory Information option is supported, the 
 * <sys/mman.h> header shall define symbolic constants for 
 * the advice argument to the posix_madvise() function
 * as follows. 
 */

/*
 * The application has no advice to give on its behavior with
 * respect to the specified range. It is the default characteristic
 * if no advice is given for a range of memory.
 */ 
#define POSIX_MADV_NORMAL		1

/* 
 * The application expects to access the specified range 
 * sequentially from lower addresses to higher addresses.
 */
#define POSIX_MADV_SEQUENTIAL	2

/* 
 * The application expects to access the specified range 
 * in a random order.
 */
#define POSIX_MADV_RANDOM		3

/*
 * The application expects to access the specified range
 * in the near future.
 */
#define POSIX_MADV_WILLNEED		4

/* 
 * The application expects that it will not access the specified 
 * range in the near future.
 */
#define POSIX_MADV_DONTNEED		5 

/*
 * The <sys/mman.h> header shall define the following symbolic 
 * constants for use as flags for the posix_typed_mem_open() function
 */

#define POSIX_TYPED_MEM_ALLOCATE        1 /* Allocate on mmap(). */
#define POSIX_TYPED_MEM_ALLOCATE_CONTIG 2 /* Allocate contiguously on mmap().*/
#define POSIX_TYPED_MEM_MAP_ALLOCATABLE 3 /* Map on mmap(), without affecting allocatability.*/

/* The <sys/mman.h> header shall define the posix_typed_mem_info structure */

typedef struct posix_typed_mem_info
    {
    size_t  posix_tmi_length;  /* Maximum length which may be allocated 
                                * from a typed memory object. */
    }posix_typed_mem_info_t;


#ifdef __cplusplus
extern "C" {
#endif

int    mlock(const void *, size_t);

int    mlockall(int);

void  *mmap(void *, size_t, int, int, int, off_t);
int    mprotect(void *, size_t, int);
int    msync(void *, size_t, int);

int    munlock(const void *, size_t);

int    munlockall(void);

int    munmap(void *, size_t);
int    posix_madvise(void *, size_t, int);

int    posix_mem_offset(const void *restrict, size_t, off_t *restrict,
           size_t *restrict, int *restrict);
int    posix_typed_mem_get_info(int, struct posix_typed_mem_info *);
int    posix_typed_mem_open(const char *, int, int);

int    shm_open(const char *, int, mode_t);
int    shm_unlink(const char *);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	/* _POSIX_SYS_MMAN_H */
