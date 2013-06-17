/* setjmp.h - stack environment declarations for ARM */

#ifndef _POSIX_ARCH_ARM_SETJMP_H
#define _POSIX_ARCH_ARM_SETJMP_H

#define _SETJMP_BUF_SZ (7+6+2+8*((96/8)/4))
typedef int __jmp_buf[_SETJMP_BUF_SZ];
#warning ARM: fix jmpbuf size

#endif	/* _POSIX_ARCH_ARM_SETJMP_H */
