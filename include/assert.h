/* assert.h - verify program assertion */

#ifndef _POSIX_ASSERT_H_
#define _POSIX_ASSERT_H_

/*
 * The assert() macro shall be redefined according to the current state 
 * of NDEBUG each time <assert.h> is included. 
 */
 
#undef assert

#ifndef NDEBUG

/* NDEBUG disables assert() functionality */

#ifdef __cplusplus
extern "C" {
#endif

extern void __assert_fail(const char *assertion, const char *file,
                unsigned int line, const char *function);

extern void __assert_perror_fail(int error, const char *file,
                unsigned int line, const char *function);

/*
 * The assert() macro shall be implemented as a macro, not as a function.
 * If the macro definition is suppressed in order to access an actual 
 * function, the behavior is undefined.
 */
 
#define assert(assertion) \
    ((assertion) ? (void)0 : __assert_fail(#assertion, __FILE__, __LINE__, __PRETTY_FUNCTION__))

#ifdef __cplusplus
}
#endif /* __cplusplus */

#else    /* NDEBUG */

/*
 * If NDEBUG is defined as a macro name before the inclusion 
 * of this header, the assert() macro shall be defined simply as:
 */
 
#define assert(ignore)((void) 0)

#endif /* NDEBUG */

#endif    /* _POSIX_ASSERT_H_ */
