/* search.h - search tables */

#ifndef _POSIX_SEARCH_H
#define _POSIX_SEARCH_H

#include <sys.h>


/* 
 * The <search.h> header shall define the ENTRY type 
 * for structure entry
 */
 
typedef struct entry 
    {
	char *keyr;
	void *data;
    } ENTRY;

/* 
 * Define ACTION and VISIT as enumeration data types through 
 * type definitions
 */
 
typedef enum 
    { 
	FIND, 
	ENTER 
    } ACTION;

typedef enum 
    {
	preorder,
	postorder,
	endorder,
	leaf
    } VISIT;

#ifdef __cplusplus
extern "C" {
#endif

int    hcreate(size_t);
void   hdestroy(void);
ENTRY *hsearch(ENTRY, ACTION);
void   insque(void *, void *);
void  *lfind(const void *, const void *, size_t *,
          size_t, int (*)(const void *, const void *));
void  *lsearch(const void *, void *, size_t *,
          size_t, int (*)(const void *, const void *));
void   remque(void *);
void  *tdelete(const void *restrict, void **restrict,
          int(*)(const void *, const void *));
void  *tfind(const void *, void *const *,
          int(*)(const void *, const void *));
void  *tsearch(const void *, void **,
          int(*)(const void *, const void *));
void   twalk(const void *,
          void (*)(const void *, VISIT, int ));

#ifdef __cplusplus
}
#endif

#endif	/* _POSIX_SEARCH_H */
