#ifndef _ARCH_X86_COMMON_ASM_H
#define _ARCH_X86_COMMON_ASM_H

#include <config.h>

#ifdef __ASM__
# define __ASM_FORM(x)    x
#else
# define __ASM_FORM(x)    " " #x " "
#endif

#ifdef CONFIG_CPU_ARCH_X86
# define __ASM_SEL(a,b) __ASM_FORM(a)
#else /* if CONFIG_CPU_ARCH_X64 */
# define __ASM_SEL(a,b) __ASM_FORM(b)
#endif

#define __ASM_SIZE(inst)    __ASM_SEL(inst##l, inst##q)
#define __ASM_REG(reg)        __ASM_SEL(e##reg, r##reg)

#define _ASM_PTR    __ASM_SEL(.long, .quad)
#define _ASM_ALIGN    __ASM_SEL(.balign 4, .balign 8)

#define _ASM_MOV    __ASM_SIZE(mov)
#define _ASM_INC    __ASM_SIZE(inc)
#define _ASM_DEC    __ASM_SIZE(dec)
#define _ASM_ADD    __ASM_SIZE(add)
#define _ASM_SUB    __ASM_SIZE(sub)
#define _ASM_XADD    __ASM_SIZE(xadd)

#define _ASM_AX        __ASM_REG(ax)
#define _ASM_BX        __ASM_REG(bx)
#define _ASM_CX        __ASM_REG(cx)
#define _ASM_DX        __ASM_REG(dx)
#define _ASM_SP        __ASM_REG(sp)
#define _ASM_BP        __ASM_REG(bp)
#define _ASM_SI        __ASM_REG(si)
#define _ASM_DI        __ASM_REG(di)

#ifdef CONFIG_SMP
#define LOCK_PREFIX \
        ".section .smp_locks,\"a\"\n"    \
        _ASM_ALIGN "\n"            \
        _ASM_PTR "661f\n" /* address */    \
        ".previous\n"            \
        "661:\n\tlock; "

#else /* ! CONFIG_SMP */
#define LOCK_PREFIX ""
#endif

#endif /* _ASM_X86_ASM_H */

