#ifndef    _ENDIAN_H
#define    _ENDIAN_H

#include <config.h>

#define    __LITTLE_ENDIAN    1234
#define    __BIG_ENDIAN    4321

#if defined(CONFIG_CPU_ARCH_X86) || defined(CONFIG_CPU_ARCH_X64)
#define __BYTE_ORDER        __LITTLE_ENDIAN
#define __FLOAT_WORD_ORDER    __BYTE_ORDER
#endif

#ifndef __BYTE_ORDER
#ifdef __BIG_ENDIAN
#define __BYTE_ORDER        __BIG_ENDIAN
#define __FLOAT_WORD_ORDER    __BYTE_ORDER
#else
#define __BYTE_ORDER        __LITTLE_ENDIAN
#define __FLOAT_WORD_ORDER    __BYTE_ORDER
#endif
#endif

#define LITTLE_ENDIAN       __LITTLE_ENDIAN
#define BIG_ENDIAN            __BIG_ENDIAN
#define BYTE_ORDER            __BYTE_ORDER

#if __BYTE_ORDER == __LITTLE_ENDIAN
#define __LONG_LONG_PAIR(HI, LO) LO, HI
#elif __BYTE_ORDER == __BIG_ENDIAN
#define __LONG_LONG_PAIR(HI, LO) HI, LO
#endif

#if defined(CONFIG_CPU_ARCH_X64)
#define __WORDSIZE 64
#define BITS_PER_LONG   64
#endif

#if defined(CONFIG_CPU_ARCH_X64)
#define __WORDSIZE_COMPAT32 1
#endif

#ifndef __WORDSIZE
#define __WORDSIZE      32
#define BITS_PER_LONG   32
#endif

#endif
