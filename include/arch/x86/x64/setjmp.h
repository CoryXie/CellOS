/* setjmp.h - stack environment declarations for X64 */

#ifndef _POSIX_ARCH_X64_SETJMP_H
#define _POSIX_ARCH_X64_SETJMP_H

/* 
 * TODO:
 *
 * A jmp_buf size of 12 might not be large enough. 
 * Increase to a large size if needed. 
 */
 
typedef int __jmp_buf[12];

#endif	/* _POSIX_ARCH_X64_SETJMP_H */
