/* setjmp.h - stack environment declarations */

#ifndef _POSIX_SETJMP_H
#define _POSIX_SETJMP_H

#include <signal.h>

#if defined(CONFIG_CPU_ARCH_X86)
#  include <arch/x86/setjmp.h>
#elif defined(CONFIG_CPU_ARCH_X64)
#  include <arch/x86/x64/setjmp.h>
#elif defined(CONFIG_CPU_ARCH_ARM)
#  include <arch/arm/setjmp.h>
#elif defined(CONFIG_CPU_ARCH_PPC)
#  include <arch/ppc/setjmp.h>
#else
#  error There is no ARCH setjmp.h for this architecture!
#endif

/* The <setjmp.h> header shall define the array types jmp_buf and sigjmp_buf. */

typedef struct __jmp_buf_tag 
    {
	__jmp_buf	regs;		/* saved registers, stack & program pointer */
	int			mask_was_saved;
	sigset_t	saved_mask;
    } jmp_buf[1];

typedef jmp_buf sigjmp_buf;

#ifdef __cplusplus
extern "C" {
#endif

int	_setjmp(jmp_buf jbuf);
int	setjmp(jmp_buf jbuf);
int	sigsetjmp(jmp_buf jbuf, int mask);

void	_longjmp(jmp_buf jbuf, int value);
void	longjmp(jmp_buf jbuf, int value);
void	siglongjmp(sigjmp_buf jbuf, int value);

#ifdef __cplusplus
}
#endif

#endif /* _POSIX_SETJMP_H */
