/* iso646.h - alternative spellings */

/*
 * iso646.h is a standard C header file. It was added to the
 * ISO C standard in 1995 with Amendment 1.
 *
 * This header file provides macro substitutes for the operators
 * that use characters not available in the ISO-646 character set.
 * This helps avoid unreadable trigraphs and digraphs.
 */

#ifndef _POSIX_ISO646_H
#define _POSIX_ISO646_H

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(__cplusplus) || __cplusplus < 199711L

/*
 * The <iso646.h> header shall define the following eleven macros
 * (on the left) that expand to the corresponding tokens (on the right).
 */

#define and     &&
#define and_eq  &=
#define bitand  &
#define bitor   |
#define compl   ~
#define not     !
#define not_eq  !=
#define or      ||
#define or_eq   |=
#define xor     ^
#define xor_eq  ^=

#endif	/* !defined(__cplusplus) || __cplusplus < 199711 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _POSIX_ISO646_H */

