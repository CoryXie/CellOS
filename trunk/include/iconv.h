/* iconv.h - codeset conversion facility */

#ifndef _POSIX_ICONV_H
#define _POSIX_ICONV_H

#include <sys/cdefs.h>
#include <sys.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Identifier for conversion method from one codeset to another. */

typedef unsigned int iconv_t;

/*
 * Allocate descriptor for code conversion from codeset FROMCODE to
 * codeset TOCODE.
 */

iconv_t iconv_open (const char *tocode, const char *fromcode);

/* Free resources allocated for descriptor CD for code conversion. */

int iconv_close (iconv_t cd);

/*
 * Convert at most *INBYTESLEFT bytes from *INBUF according to the
 * code conversion algorithm specified by CD and place up to
 * *OUTBYTESLEFT bytes in buffer at *OUTBUF.
 */

extern size_t iconv (iconv_t cd,
                     char** inbuf,
        		     size_t* inbytesleft,
        		     char** outbuf,
        		     size_t* outbytesleft);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _POSIX_ICONV_H */

