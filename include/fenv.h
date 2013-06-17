/* fenv.h - floating-point environment */

/*
APPLICATION USAGE

    This header is designed to support the floating-point exception
    status flags and directed-rounding control modes required by the
    IEC 60559:1989 standard, and other similar floating-point state
    information. Also it is designed to facilitate code portability
    among all systems.

    Certain application programming conventions support the intended
    model of use for the floating-point environment:

    A function call does not alter its caller's floating-point
    control modes, clear its caller's floating-point status flags,
    nor depend on the state of its caller's floating-point status flags
    unless the function is so documented.

    A function call is assumed to require default floating-point
    control modes, unless its documentation promises otherwise.

    A function call is assumed to have the potential for raising
    floating-point exceptions, unless its documentation promises
    otherwise.

    With these conventions, an application can safely assume default
    floating-point control modes (or be unaware of them). The
    responsibilities associated with accessing the floating-point
    environment fall on the application that does so explicitly.

    Even though the rounding direction macros may expand to constants
    corresponding to the values of FLT_ROUNDS, they are not required
    to do so.

    For example:

    #include <fenv.h>
    void f(double x)
    {
        #pragma STDC FENV_ACCESS ON
        void g(double);
        void h(double);
        /@ ... @/
        g(x + 1);
        h(x + 1);
        /@ ... @/
    }

    If the function g() might depend on status flags set as a side-effect
    of the first x+1, or if the second x+1 might depend on control modes
    set as a side-effect of the call to function g(), then the application
    shall contain an appropriately placed invocation as follows:

    #pragma STDC FENV_ACCESS ON

RATIONALE

    The fexcept_t Type:

    fexcept_t does not have to be an integer type. Its values must be
    obtained by a call to fegetexceptflag(), and cannot be created by
    logical operations from the exception macros. An implementation
    might simply implement fexcept_t as an int and use the representations
    reflected by the exception macros, but is not required to; other
    representations might contain extra information about the exceptions.
    fexcept_t might be a struct with a member for each exception (that
    might hold the address of the first or last floating-point instruction
    that caused that exception). The ISO/IEC 9899:1999 standard makes
    no claims about the internals of an fexcept_t, and so the user cannot
    inspect it.

    Exception and Rounding Macros:

    Macros corresponding to unsupported modes and rounding directions
    are not defined by the implementation and must not be defined by
    the application. An application might use #ifdef to test for this.
*/

#ifndef _POSIX_FENV_H
#define _POSIX_FENV_H

#if defined(CONFIG_CPU_ARCH_X86)
#  include <arch/x86/fenv.h>
#elif defined(CONFIG_CPU_ARCH_X64)
#  include <arch/x86/x64/fenv.h>
#elif defined(CONFIG_CPU_ARCH_ARM)
#  include <arch/arm/fenv.h>
#elif defined(CONFIG_CPU_ARCH_PPC)
#  include <arch/ppc/fenv.h>
#else
#  error There is no fenv.h for this architecture!
#endif

#endif /* _POSIX_FENV_H */

