/* ndbm.h - definitions for ndbm database operations */

#ifndef _POSIX_NDBM_H
#define _POSIX_NDBM_H

/* 
 * The <ndbm.h> header shall define the size_t type as 
 * described in <stddef.h>.
 */
 
#include <stddef.h>

/*
 * The <ndbm.h> header shall define the mode_t type through 
 * typedef, as described in <sys.h>.
 */
 
#include <sys.h>
 
#ifdef __cplusplus
extern "C" {
#endif

/* The <ndbm.h> header shall define the datum type as a structure */

struct datum
    {
    void   *dptr; /* A pointer to the application's data. */
    size_t  dsize;/* The size of the object pointed to by dptr. */
    }datum_t;

/* The <ndbm.h> header shall define the DBM type. */

typedef void * DBM;

/*
 * The <ndbm.h> header shall define the following symbolic
 * constants as possible values for the store_mode argument 
 * to dbm_store():
 */

#define DBM_INSERT  1   /* Insertion of new entries only. */
#define DBM_REPLACE 2   /* Allow replacing existing entries. */
    
int     dbm_clearerr(DBM *);
void    dbm_close(DBM *);
int     dbm_delete(DBM *, datum);
int     dbm_error(DBM *);
datum   dbm_fetch(DBM *, datum);
datum   dbm_firstkey(DBM *);
datum   dbm_nextkey(DBM *);
DBM    *dbm_open(const char *, int, mode_t);
int     dbm_store(DBM *, datum, datum, int);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _POSIX_NDBM_H */

