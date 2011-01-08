/* fmtmsg.h - message display structures */

#ifndef _POSIX_XXX_H_
#define _POSIX_XXX_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Values to control 'fmtmsg' function.  */

enum
    {
    MM_HARD = 0x001,    /* Source of the condition is hardware.  */
    MM_SOFT = 0x002,    /* Source of the condition is software.  */
    MM_FIRM = 0x004,    /* Source of the condition is firmware.  */
    MM_APPL = 0x008,    /* Condition detected by application.  */
    MM_UTIL = 0x010,    /* Condition detected by utility.  */
    MM_OPSYS = 0x020,    /* Condition detected by operating system.  */
    MM_RECOVER = 0x040,    /* Recoverable error.  */
    MM_NRECOV = 0x080,    /* Non-recoverable error.  */
    MM_PRINT = 0x100,    /* Display message in standard error.  */
    MM_CONSOLE = 0x200    /* Display message on system console.  */
    };

/* Values to be for SEVERITY parameter of 'fmtmsg'. */

enum
    {
    MM_NOSEV = 0,   /* No severity level provided for the message.  */
    MM_HALT,        /* Error causing application to halt.  */
    MM_ERROR,        /* Application has encountered a non-fatal fault.  */
    MM_WARNING,        /* Application has detected unusual non-error condition.  */
    MM_INFO            /* Informative message.  */
    };

/* 
 * Macros which can be used as null values 
 * for the arguments of 'fmtmsg'. 
 */
 
#define MM_NULLLBL    ((char *) 0)
#define MM_NULLSEV    0
#define MM_NULLMC    ((long) 0L)
#define MM_NULLTXT    ((char *) 0)
#define MM_NULLACT    ((char *) 0)
#define MM_NULLTAG    ((char *) 0)


/* Possible return values of `fmtmsg'.  */

enum
    {
    MM_NOTOK = -1,  /* The function failed completely. */
    MM_OK = 0,      /* The function succeeded. */
    MM_NOMSG = 1,   /* The function was unable to generate a message on 
                     * standard error, but otherwise succeeded. */
    MM_NOCON = 2    /* The function was unable to generate a console
                     * message, but otherwise succeeded.*/
    };


/* 
 * Print message with given CLASSIFICATION, LABEL, SEVERITY, 
 * TEXT, ACTION and TAG to console or standard error.  
 */

extern int fmtmsg (long int __classification, __const char *__label,
           int __severity, __const char *__text,
           __const char *__action, __const char *__tag);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _POSIX_XXX_H_ */

