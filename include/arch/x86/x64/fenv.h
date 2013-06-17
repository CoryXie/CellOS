/* fenv.h - floating-point environment for X64 */

#ifndef    _POSIX_ARCH_X64_FENV_H_
#define    _POSIX_ARCH_X64_FENV_H_

#include <stdint.h>
#include <sys/cdefs.h>
#include <sys.h>

/*
 * fenv_t
 *
 * Represents the entire floating-point environment.
 * The floating-point environment refers collectively to any
 * floating-point status flags and control modes supported
 * by the implementation.
 */

typedef struct fenv
    {
    struct
        {
        uint32_t    __control;
        uint32_t    __status;
        uint32_t    __tag;
        char        __other[16];
        } __x87;
    /*
     * The MXCSR register is a 32-bit register containing flags
     * for control and status information regarding SSE instructions.
     */

    uint32_t        __mxcsr;
    } fenv_t;

/*
 * fexcept_t
 *
 * Represents the floating-point status flags collectively,
 * including any status the implementation associates with the flags.
 * A floating-point status flag is a system variable whose value
 * is set (but never cleared) when a floating-point exception is
 * raised, which occurs as a side-effect of exceptional floating-point
 * arithmetic to provide auxiliary information. A floating-point
 * control mode is a system variable whose value may be set by
 * the user to affect the subsequent behavior of floating-point
 * arithmetic.
 */

typedef    uint16_t    fexcept_t;

/*
 * Exception flags
 *
 * The <fenv.h> header shall define each of the following macros
 * if and only if the implementation supports the floating-point
 * exception by means of the floating-point functions feclearexcept(),
 * fegetexceptflag(), feraiseexcept(), fesetexceptflag(), and
 * fetestexcept(). The defined macros shall expand to integer
 * constant expressions with values that are bitwise-distinct.
 *
 * FE_DIVBYZERO
 * FE_INEXACT
 * FE_INVALID
 * FE_OVERFLOW
 * FE_UNDERFLOW
 */

#define    FE_INVALID        0x01
#define    FE_DENORMAL        0x02
#define    FE_DIVBYZERO    0x04
#define    FE_OVERFLOW        0x08
#define    FE_UNDERFLOW    0x10
#define    FE_INEXACT        0x20

/*
 * The <fenv.h> header shall define the macro FE_ALL_EXCEPT as
 * the bitwise-inclusive OR of all floating-point exception
 * macros defined by the implementation, if any. If no such
 * macros are defined, then the macro FE_ALL_EXCEPT shall be
 * defined as zero.
 */

#define    FE_ALL_EXCEPT    (FE_DIVBYZERO | FE_DENORMAL | FE_INEXACT | \
             FE_INVALID | FE_OVERFLOW | FE_UNDERFLOW)

/*
 * Rounding modes
 *
 * The <fenv.h> header shall define each of the following macros
 * if and only if the implementation supports getting and setting
 * the represented rounding direction by means of the fegetround()
 * and fesetround() functions. The defined macros shall expand to
 * integer constant expressions whose values are distinct
 * non-negative values.
 *
 * FE_DOWNWARD
 * FE_TONEAREST
 * FE_TOWARDZERO
 * FE_UPWARD
 */

#define    FE_TONEAREST    0x0000
#define    FE_DOWNWARD        0x0400
#define    FE_UPWARD        0x0800
#define    FE_TOWARDZERO    0x0c00
#define    _ROUND_MASK        (FE_TONEAREST | FE_DOWNWARD | \
                        FE_UPWARD | FE_TOWARDZERO)

/*
 * As compared to the x87 control word, the SSE unit's control word
 * has the rounding control bits offset by 3 and the exception mask
 * bits offset by 7.
 */

#define    _SSE_ROUND_SHIFT    3
#define    _SSE_EMASK_SHIFT    7

__BEGIN_DECLS

/*
 * Default floating-point environment
 *
 * The <fenv.h> header shall define the following macro, which
 * represents the default floating-point environment (that is,
 * the one installed at program startup) and has type pointer
 * to const-qualified fenv_t. It can be used as an argument to
 * the functions within the <fenv.h> header that manage the
 * floating-point environment.
 *
 * FE_DFL_ENV
 */

extern const fenv_t    __fe_dfl_env;
#define    FE_DFL_ENV    (&__fe_dfl_env)

#define    __fldcw(__cw)        __asm __volatile("fldcw %0" : : "m" (__cw))
#define    __fldenv(__env)        __asm __volatile("fldenv %0" : : "m" (__env))
#define    __fldenvx(__env)    __asm __volatile("fldenv %0" : : "m" (__env)  \
                : "st", "st(1)", "st(2)", "st(3)", "st(4)",   \
                "st(5)", "st(6)", "st(7)")
#define    __fnclex()        __asm __volatile("fnclex")
#define    __fnstenv(__env)    __asm __volatile("fnstenv %0" : "=m" (*(__env)))
#define    __fnstcw(__cw)        __asm __volatile("fnstcw %0" : "=m" (*(__cw)))
#define    __fnstsw(__sw)        __asm __volatile("fnstsw %0" : "=am" (*(__sw)))
#define    __fwait()        __asm __volatile("fwait")
#define    __ldmxcsr(__csr)    __asm __volatile("ldmxcsr %0" : : "m" (__csr))
#define    __stmxcsr(__csr)    __asm __volatile("stmxcsr %0" : "=m" (*(__csr)))

/*
 * feclearexcept - clear floating-point exception
 *
 * The feclearexcept() function shall attempt to clear the supported
 * floating-point exceptions represented by excepts.
 *
 * RETURN:
 *
 * If the argument is zero or if all the specified exceptions
 * were successfully cleared, feclearexcept() shall return zero.
 * Otherwise, it shall return a non-zero value.
 */

static __inline int feclearexcept
    (
    int __excepts
    )
    {
    fenv_t __env;

    if (__excepts == FE_ALL_EXCEPT)
        {
        __fnclex();
        }
    else
        {
        __fnstenv(&__env.__x87);
        __env.__x87.__status &= ~__excepts;
        __fldenv(__env.__x87);
        }

    __stmxcsr(&__env.__mxcsr);
    __env.__mxcsr &= ~__excepts;
    __ldmxcsr(__env.__mxcsr);

    return (0);
    }
/*
 * fegetexceptflag - get and set floating-point status flags
 *
 * The fegetexceptflag() function shall attempt to store an
 * implementation-defined representation of the states of the
 * floating-point status flags indicated by the argument
 * excepts in the object pointed to by the argument flagp.
 *
 * RETURN:
 *
 * If the representation was successfully stored, fegetexceptflag()
 * shall return zero. Otherwise, it shall return a non-zero value.
 */

static __inline int fegetexceptflag
    (
    fexcept_t *__flagp,
    int __excepts
    )
    {
    int __mxcsr, __status;

    __stmxcsr(&__mxcsr);
    __fnstsw(&__status);
    *__flagp = (__mxcsr | __status) & __excepts;

    return (0);
    }

/*
 * fesetexceptflag - set floating-point status flags
 *
 * The fesetexceptflag() function shall attempt to set the
 * floating-point status flags indicated by the argument
 * excepts to the states stored in the object pointed to by flagp.
 * The value pointed to by flagp shall have been set by
 * a previous call to fegetexceptflag() whose second argument
 * represented at least those floating-point exceptions
 * represented by the argument excepts. This function does
 * not raise floating-point exceptions, but only sets the
 * state of the flags.
 *
 * RETURN:
 *
 * If the excepts argument is zero or if all the specified exceptions
 * were successfully set, fesetexceptflag() shall return zero.
 * Otherwise, it shall return a non-zero value.
 */

int fesetexceptflag
    (
    const fexcept_t *__flagp,
    int __excepts
    );

/*
 * feraiseexcept - raise floating-point exception
 *
 * The feraiseexcept() function shall attempt to raise the supported
 * floating-point exceptions represented by the argument excepts.
 * The order in which these floating-point exceptions are raised
 * is unspecified. Whether the feraiseexcept() function additionally
 * raises the inexact floating-point exception whenever it raises
 * the overflow or underflow floating-point exception is
 * implementation-defined.
 *
 * RETURN:
 *
 * If the argument is zero or if all the specified exceptions were
 * successfully raised, feraiseexcept() shall return zero. Otherwise,
 * it shall return a non-zero value.
 */

int feraiseexcept(int __excepts);

static __inline int fetestexcept(int __excepts)
    {
    int __mxcsr, __status;

    __stmxcsr(&__mxcsr);
    __fnstsw(&__status);

    return ((__status | __mxcsr) & __excepts);
    }

/*
 * fegetround - get current rounding direction
 *
 * The fegetround() function shall get the current rounding direction.
 *
 * RETURN:
 *
 * The fegetround() function shall return the value of the rounding
 * direction macro representing the current rounding direction or a
 * negative value if there is no such rounding direction macro or
 * the current rounding direction is not determinable.
 */

static __inline int fegetround (void)
    {
    int __control;

    /*
     * We assume that the x87 and the SSE unit agree on the
     * rounding mode.  Reading the control word on the x87 turns
     * out to be about 5 times faster than reading it on the SSE
     * unit on an Opteron 244.
     */

    __fnstcw(&__control);

    return (__control & _ROUND_MASK);
    }

/*
 * fesetround - set current rounding direction
 *
 * The fesetround() function shall establish the rounding direction
 * represented by its argument round. If the argument is not equal
 * to the value of a rounding direction macro, the rounding direction
 * is not changed.
 *
 * RETURN:
 *
 * The fesetround() function shall return a zero value if and only
 * if the requested rounding direction was established.
 */

static __inline int fesetround
    (
    int __round
    )
    {
    int __mxcsr, __control;

    if (__round & ~_ROUND_MASK)
        return (-ENOTSUP);

    __fnstcw(&__control);
    __control &= ~_ROUND_MASK;
    __control |= __round;
    __fldcw(__control);

    __stmxcsr(&__mxcsr);
    __mxcsr &= ~(_ROUND_MASK << _SSE_ROUND_SHIFT);
    __mxcsr |= __round << _SSE_ROUND_SHIFT;
    __ldmxcsr(__mxcsr);

    return (0);
    }

int fegetenv(fenv_t *__envp);
int feholdexcept(fenv_t *__envp);

static __inline int fesetenv
    (
    const fenv_t *__envp
    )
    {

    /*
     * Using fldenvx() instead of fldenv() tells the compiler that this
     * instruction clobbers the x87 register stack.  This happens because
     * we restore the tag word from the saved environment.  Normally, this
     * would happen anyway and we wouldn't care, because the ABI allows
     * function calls to clobber the x87 regs.  However, fesetenv() is
     * inlined, so we need to be more careful.
     */

    __fldenvx(__envp->__x87);
    __ldmxcsr(__envp->__mxcsr);

    return (0);
    }

/*
 * feupdateenv - update floating-point environment
 *
 * The feupdateenv() function shall attempt to save the currently
 * raised floating-point exceptions in its automatic storage,
 * attempt to install the floating-point environment represented
 * by the object pointed to by envp, and then attempt to raise
 * the saved floating-point exceptions. The argument envp shall
 * point to an object set by a call to feholdexcept() or fegetenv(),
 * or equal a floating-point environment macro.
 *
 * RETURN:
 *
 * The feupdateenv() function shall return a zero value if and
 * only if all the required actions were successfully carried out.
 */

int feupdateenv(const fenv_t *__envp);

__END_DECLS

#endif    /* _POSIX_ARCH_X64_FENV_H_ */
