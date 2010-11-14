/* tar.h - extended tar definitions */

#ifndef _POSIX_TAR_H
#define _POSIX_XXX_H


#ifdef __cplusplus
extern "C" {
#endif

/* General definitions */

#define TMAGIC  "ustar" /* ustar plus null byte. */
#define TMAGLEN 6       /* Length of the above. */
#define TVERSION "00"   /* 00 without a null byte.*/
#define TVERSLEN 2      /* Length of the above. */

/* Typeflag field definitions */

#define REGTYPE     '0'      /* Regular file. */
#define AREGTYPE    '\0'     /* Regular file. */
#define LNKTYPE     '1'      /* Link. */
#define SYMTYPE     '2'      /* Symbolic link. */
#define CHRTYPE     '3'      /* Character special. */
#define BLKTYPE     '4'      /* Block special */
#define DIRTYPE     '5'      /* Directory */
#define FIFOTYPE    '6'      /* FIFO special */
#define CONTTYPE    '7'      /* Reserved */

/* Mode field bit definitions (octal) */

#define TSUID 	    04000 	/* Set user ID on execution.*/
#define TSGID 	    02000 	/* Set group ID on execution.*/
#define TSVTX 	    01000 	/* Reserved.*/
#define TUREAD 	    00400 	/* Read by owner.*/
#define TUWRITE 	00200 	/* Write by owner.*/
#define TUEXEC 	    00100 	/* Execute or search by owner.*/
#define TGREAD 	    00040 	/* Read by group.*/
#define TGWRITE 	00020 	/* Write by group.*/
#define TGEXEC 	    00010 	/* Execute or search by group.*/
#define TOREAD 	    00004 	/* Read by others.*/
#define TOWRITE 	00002 	/* Write by others.*/
#define TOEXEC 	    00001 	/* Execute or search by other.*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _POSIX_TAR_H */

