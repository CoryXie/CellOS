/* stdbool.h - boolean type and values */

#ifndef _POSIX_STDBOOL_H
#define _POSIX_STDBOOL_H

#ifdef __cplusplus
extern "C" {
#endif

#define bool                            BOOL
#define false                           0
#define true                            1
#define __bool_true_false_are_defined   1

/* 
 * An application may undefine and then possibly redefine 
 * the macros bool, true, and false. 
 */
 
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _POSIX_STDBOOL_H */

