/*
 * ksh.c - Simple Kernel Shell Command Parser
 *
 * The idea and some of the codes are stolen from u-boot common/command.c, 
 * but I modified it extensively so that it fits into my understanding.
 * It is important to note that although this code is derived from u-boot,
 * it has one important ehancment. 
 *
 * The original u-boot implementation uses the U_BOOT_CMD macro to put the 
 * command structures into one dedicated section called .u_boot_cmd, and 
 * the find_cmd_tbl routine is to scan through that section to match the 
 * command that the user inputs. However, there is an assumption that the
 * command structures in the .u_boot_cmd section are all contigious, so 
 * the code just increase each time sizeof(cmd_tbl_t) to go to the next 
 * command to match...But the command strcutres may not acctually contigious
 * at all, thus some times the match can not find the command correctly,
 * or even may crash...
 *
 * The enhanced version, as shown here, is to only keep the pointers to these
 * command structures in that dedicated section. Becasue pointers are always
 * of the same size (4 bytes in 32 bit systems, 8 bytes in 64 bit systems),
 * the section will be always natively contigious, thus scanning through the 
 * section can be considered safe. With this change, the command structures
 * themselves can just be located anywhere, in the normal data section, or
 * in another dedicated section...
 *
 * All thanks are given to u-boot contributors and they still hold the 
 * copyright to these stolen codes.
 */

#include <sys.h>
#include <arch.h>
#include <os.h>

int do_version (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
    {
    printk ("CELL OS VERSION : %s\n", CONFIG_CELLOS_VERSION);
    return 0;
    }

CELL_OS_CMD(
    version,    1,        1,    do_version,
    "print monitor version",
    ""
    );

int do_echo (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
    {
    int i, putnl = 1;

    for (i = 1; i < argc; i++) {
        char *p = argv[i], c;

        if (i > 1)
            putchar(' ');
        while ((c = *p++) != '\0') {
            if (c == '\\' && *p == 'c') {
                putnl = 0;
                p++;
            } else {
                putchar(c);
            }
        }
    }

    if (putnl)
        putchar('\n');
    return 0;
    }

CELL_OS_CMD(
    echo,    CONFIG_SYS_MAXARGS,    1,    do_echo,
    "echo args to console",
    "[args..]\n"
    "    - echo args to console; \\c suppresses newline"
    );


int _do_help (cpu_addr_t *cmd_start, int cmd_items, cmd_tbl_t * cmdtp, int
          flag, int argc, char *argv[])
    {
    int i;
    int rcode = 0;

    if (argc == 1)  /* show list of commands */
        {    
        cmd_tbl_t **cmd_array = (cmd_tbl_t **)cmd_start;
        int i, j, swaps;

        /* Sort command list (trivial bubble sort) */
        for (i = cmd_items - 1; i > 0; --i) {
            swaps = 0;
            for (j = 0; j < i; ++j) {
                if (strcmp (cmd_array[j]->name,
                        cmd_array[j + 1]->name) > 0) {
                    cmd_tbl_t *tmp;
                    tmp = cmd_array[j];
                    cmd_array[j] = cmd_array[j + 1];
                    cmd_array[j + 1] = tmp;
                    ++swaps;
                }
            }
            if (!swaps)
                break;
        }

        /* print short help (usage) */
        for (i = 0; i < cmd_items; i++) {
            const char *usage = cmd_array[i]->usage;

            /* TODO: allow user abort - ctrlc() and return !*/

            if (usage == NULL)
                continue;
            printk("%-*s- %s\n", CONFIG_SYS_HELP_CMD_WIDTH,
                   cmd_array[i]->name, usage);
        }
        return 0;
        }
    /*
     * command help (long version)
     */
    for (i = 1; i < argc; ++i) {
        if ((cmdtp = find_cmd_tbl (argv[i], cmd_start, cmd_items )) != NULL) {
            rcode |= cmd_usage(cmdtp);
        } else {
            printk ("Unknown command '%s' - try 'help'"
                " without arguments for list of all"
                " known commands\n\n", argv[i]
                    );
            rcode = 1;
        }
    }
    return rcode;
    }

int do_help (cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
    {
    int items = ((cpu_addr_t)&__cell_os_cmd_end - 
                 (cpu_addr_t)&__cell_os_cmd_start) / sizeof(cpu_addr_t);
    return _do_help((cpu_addr_t *)&__cell_os_cmd_start,
                    items,
                    cmdtp, flag, argc, argv);
    }


CELL_OS_CMD(
    help,    CONFIG_SYS_MAXARGS,    1,    do_help,
    "print online help",
    "[command ...]\n"
    "    - show help information (for 'command')\n"
    "'help' prints online help for the monitor commands.\n\n"
    "Without arguments, it prints a short usage message for all commands.\n\n"
    "To get detailed help information for specific commands you can type\n"
    "'help' with one or more command names as arguments."
    );

/* This does not use the CELL_OS_CMD macro as ? can't be used in symbol names */

cmd_tbl_t __cell_os_cmd_question_mark = 
    {
    "?",    CONFIG_SYS_MAXARGS,    1,    do_help,
    "alias for 'help'",
    ""
    };

cpu_addr_t __cell_os_cmd_question_mark_ptr Struct_Section 
    = (cpu_addr_t)(&__cell_os_cmd_question_mark);

/***************************************************************************
 * find command table entry for a command
 */
cmd_tbl_t *find_cmd_tbl (const char *cmd, cpu_addr_t *table, size_t items)
    {
    cmd_tbl_t *cmdtp;
    cmd_tbl_t *cmdtp_temp = (cmd_tbl_t *)table[0];    /* Init value */
    const char *p;
    int len;
    int n_found = 0;
    int i = 0;
    /*
     * Some commands allow length modifiers (like "cp.b");
     * compare command name only until first dot.
     */
     
    len = ((p = strchr(cmd, '.')) == NULL) ? strlen (cmd) : (p - cmd);

    for (i = 0; i < items; i++) 
        {  
        cmdtp = (cmd_tbl_t *)table[i];
                
        if (strncmp (cmd, cmdtp->name, len) == 0) 
            {
            if (len == strlen (cmdtp->name))
                return cmdtp;    /* full match */

            cmdtp_temp = cmdtp;    /* abbreviated command ? */
            
            n_found++;
            }
        }
         
    if (n_found == 1) 
        {            /* exactly one match */
        return cmdtp_temp;
        }

    return NULL;    /* not found or ambiguous command */
    }

cmd_tbl_t *find_cmd (const char *cmd)
    {
    int items = ((cpu_addr_t)&__cell_os_cmd_end - 
                 (cpu_addr_t)&__cell_os_cmd_start) / sizeof(cpu_addr_t);
    
    return find_cmd_tbl(cmd, &__cell_os_cmd_start, items);
    }

int cmd_usage(cmd_tbl_t *cmdtp)
    {
    printk("%s - %s\n\n", cmdtp->name, cmdtp->usage);
    printk("Usage:\n%s ", cmdtp->name);

    if (!cmdtp->help) {
        printk ("- No additional help available.\n");
        return 1;
    }

    printk (cmdtp->help);
    putchar ('\n');

    return 0;
    }

#ifdef CONFIG_AUTO_COMPLETE

int var_complete(int argc, char *argv[], char last_char, int maxv, char *cmdv[])
{
    static char tmp_buf[512];
    int space;

    space = last_char == '\0' || last_char == ' ' || last_char == '\t';

    if (space && argc == 1)
        return env_complete("", maxv, cmdv, sizeof(tmp_buf), tmp_buf);

    if (!space && argc == 2)
        return env_complete(argv[1], maxv, cmdv, sizeof(tmp_buf), tmp_buf);

    return 0;
}

static void install_auto_complete_handler(const char *cmd,
        int (*complete)(int argc, char *argv[], char last_char, int maxv, char *cmdv[]))
{
    cmd_tbl_t *cmdtp;

    cmdtp = find_cmd(cmd);
    if (cmdtp == NULL)
        return;

    cmdtp->complete = complete;
}

void install_auto_complete(void)
{
    install_auto_complete_handler("printenv", var_complete);
    install_auto_complete_handler("setenv", var_complete);
#if defined(CONFIG_CMD_RUN)
    install_auto_complete_handler("run", var_complete);
#endif
}

/*************************************************************************************/

static int complete_cmdv(int argc, char *argv[], char last_char, int maxv, char *cmdv[])
{
    cmd_tbl_t *cmdtp;
    const char *p;
    int len, clen;
    int n_found = 0;
    const char *cmd;

    /* sanity? */
    if (maxv < 2)
        return -2;

    cmdv[0] = NULL;

    if (argc == 0) {
        /* output full list of commands */
        for (cmdtp = &__cell_os_cmd_start; cmdtp != &__cell_os_cmd_end; cmdtp++) {
            if (n_found >= maxv - 2) {
                cmdv[n_found++] = "...";
                break;
            }
            cmdv[n_found++] = cmdtp->name;
        }
        cmdv[n_found] = NULL;
        return n_found;
    }

    /* more than one arg or one but the start of the next */
    if (argc > 1 || (last_char == '\0' || last_char == ' ' || last_char == '\t')) {
        cmdtp = find_cmd(argv[0]);
        if (cmdtp == NULL || cmdtp->complete == NULL) {
            cmdv[0] = NULL;
            return 0;
        }
        return (*cmdtp->complete)(argc, argv, last_char, maxv, cmdv);
    }

    cmd = argv[0];
    /*
     * Some commands allow length modifiers (like "cp.b");
     * compare command name only until first dot.
     */
    p = strchr(cmd, '.');
    if (p == NULL)
        len = strlen(cmd);
    else
        len = p - cmd;

    /* return the partial matches */
    for (cmdtp = &__cell_os_cmd_start; cmdtp != &__cell_os_cmd_end; cmdtp++) {

        clen = strlen(cmdtp->name);
        if (clen < len)
            continue;

        if (memcmp(cmd, cmdtp->name, len) != 0)
            continue;

        /* too many! */
        if (n_found >= maxv - 2) {
            cmdv[n_found++] = "...";
            break;
        }

        cmdv[n_found++] = cmdtp->name;
    }

    cmdv[n_found] = NULL;
    return n_found;
}

static int make_argv(char *s, int argvsz, char *argv[])
{
    int argc = 0;

    /* split into argv */
    while (argc < argvsz - 1) {

        /* skip any white space */
        while ((*s == ' ') || (*s == '\t'))
            ++s;

        if (*s == '\0')    /* end of s, no more args    */
            break;

        argv[argc++] = s;    /* begin of argument string    */

        /* find end of string */
        while (*s && (*s != ' ') && (*s != '\t'))
            ++s;

        if (*s == '\0')        /* end of s, no more args    */
            break;

        *s++ = '\0';        /* terminate current arg     */
    }
    argv[argc] = NULL;

    return argc;
}

static void print_argv(const char *banner, const char *leader, const char *sep, int linemax, char *argv[])
{
    int ll = leader != NULL ? strlen(leader) : 0;
    int sl = sep != NULL ? strlen(sep) : 0;
    int len, i;

    if (banner) {
        printk("\n");
        printk(banner);
    }

    i = linemax;    /* force leader and newline */
    while (*argv != NULL) {
        len = strlen(*argv) + sl;
        if (i + len >= linemax) {
            printk("\n");
            if (leader)
                printk(leader);
            i = ll - sl;
        } else if (sep)
            printk(sep);
        printk(*argv++);
        i += len;
    }
    printk("\n");
}

static int find_common_prefix(char *argv[])
{
    int i, len;
    char *anchor, *s, *t;

    if (*argv == NULL)
        return 0;

    /* begin with max */
    anchor = *argv++;
    len = strlen(anchor);
    while ((t = *argv++) != NULL) {
        s = anchor;
        for (i = 0; i < len; i++, t++, s++) {
            if (*t != *s)
                break;
        }
        len = s - anchor;
    }
    return len;
}

static char tmp_buf[CONFIG_SYS_CBSIZE];    /* copy of console I/O buffer    */

int cmd_auto_complete(const char *const prompt, char *buf, int *np, int *colp)
{
    int n = *np, col = *colp;
    char *argv[CONFIG_SYS_MAXARGS + 1];        /* NULL terminated    */
    char *cmdv[20];
    char *s, *t;
    const char *sep;
    int i, j, k, len, seplen, argc;
    int cnt;
    char last_char;

    if (strcmp(prompt, CONFIG_SYS_PROMPT) != 0)
        return 0;    /* not in normal console */

    cnt = strlen(buf);
    if (cnt >= 1)
        last_char = buf[cnt - 1];
    else
        last_char = '\0';

    /* copy to secondary buffer which will be affected */
    strcpy(tmp_buf, buf);

    /* separate into argv */
    argc = make_argv(tmp_buf, sizeof(argv)/sizeof(argv[0]), argv);

    /* do the completion and return the possible completions */
    i = complete_cmdv(argc, argv, last_char, sizeof(cmdv)/sizeof(cmdv[0]), cmdv);

    /* no match; bell and out */
    if (i == 0) {
        if (argc > 1)    /* allow tab for non command */
            return 0;
        putchar('\a');
        return 1;
    }

    s = NULL;
    len = 0;
    sep = NULL;
    seplen = 0;
    if (i == 1) { /* one match; perfect */
        k = strlen(argv[argc - 1]);
        s = cmdv[0] + k;
        len = strlen(s);
        sep = " ";
        seplen = 1;
    } else if (i > 1 && (j = find_common_prefix(cmdv)) != 0) {    /* more */
        k = strlen(argv[argc - 1]);
        j -= k;
        if (j > 0) {
            s = cmdv[0] + k;
            len = j;
        }
    }

    if (s != NULL) {
        k = len + seplen;
        /* make sure it fits */
        if (n + k >= CONFIG_SYS_CBSIZE - 2) {
            putchar('\a');
            return 1;
        }

        t = buf + cnt;
        for (i = 0; i < len; i++)
            *t++ = *s++;
        if (sep != NULL)
            for (i = 0; i < seplen; i++)
                *t++ = sep[i];
        *t = '\0';
        n += k;
        col += k;
        printk(t - k);
        if (sep == NULL)
            putchar('\a');
        *np = n;
        *colp = col;
    } else {
        print_argv(NULL, "  ", " ", 78, cmdv);

        printk(prompt);
        printk(buf);
    }
    return 1;
}

#endif


