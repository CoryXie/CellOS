/* fnmatch.h - filename-matching types */

#ifndef	_POSIX_FNMATCH_H
#define	_POSIX_FNMATCH_H


#define	FNM_NOMATCH		1       /* The string does not match
                                 * the specified pattern. */

#define	FNM_NOESCAPE	0x01	/* Disable backslash escaping. */
#define	FNM_PATHNAME	0x02	/* <slash> in string only matches
                                 * <slash> in pattern. */
#define	FNM_PERIOD		0x04	/* Leading <period> in string must be 
                                 * exactly matched by <period> in pattern. */

#define	FNM_LEADING_DIR	0x08	/* Ignore /<tail> after Imatch. */
#define	FNM_CASEFOLD	0x10	/* Case insensitive search. */
#define	FNM_IGNORECASE	FNM_CASEFOLD
#define	FNM_FILE_NAME	FNM_PATHNAME

#ifdef __cplusplus
extern "C" {
#endif

extern int	 fnmatch(const char *pattern, const char *string, int flags);

#ifdef __cplusplus
}
#endif

#endif	/* _POSIX_FNMATCH_H */
