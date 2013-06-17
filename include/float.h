/* float.h - floating types */

#ifndef _POSIX_FLOAT_H
#define _POSIX_FLOAT_H

#if defined(CONFIG_CPU_ARCH_X86)
#  include <arch/x86/float.h>
#elif defined(CONFIG_CPU_ARCH_X64)
#  include <arch/x86/x64/float.h>
#elif defined(CONFIG_CPU_ARCH_ARM)
#  include <arch/arm/float.h>
#elif defined(CONFIG_CPU_ARCH_PPC)
#  include <arch/ppc/float.h>
#else
#  error There is no float.h for this architecture!
#endif

#endif /* _POSIX_FLOAT_H */

