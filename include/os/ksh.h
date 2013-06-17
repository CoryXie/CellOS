#ifndef _KSH_H
#define _KSH_H

/* The whole ideal here is from u-boot! */

typedef struct cmd_tbl_s 
    {
    char        *name;        /* Command Name            */
    int        maxargs;    /* maximum number of arguments    */
    int        repeatable;    /* autorepeat allowed?        */
                    /* Implementation function    */
    int        (*cmd)(struct cmd_tbl_s *, int, int, char *[]);
    char        *usage;        /* Usage message    (short)    */
    char        *help;        /* Usage message    (long)    */
#ifdef CONFIG_AUTO_COMPLETE
    /* do auto completion on the arguments */
    int        (*complete)(int argc, char *argv[], char last_char, int maxv, char *cmdv[]);
#endif
    }__attribute__((packed)) cmd_tbl_t;

extern cpu_addr_t __cell_os_cmd_start;
extern cpu_addr_t __cell_os_cmd_end;

int _do_help (cpu_addr_t *cmd_start, int cmd_items, cmd_tbl_t * cmdtp, 
              int flag, int argc, char *argv[]);
cmd_tbl_t *find_cmd(const char *cmd);
cmd_tbl_t *find_cmd_tbl (const char *cmd, cpu_addr_t *table, size_t items);

extern int cmd_usage(cmd_tbl_t *cmdtp);

/*
 * Command Flags:
 */
#define CMD_FLAG_REPEAT        0x0001        /* repeat last command        */

#define Struct_Section  __attribute__ ((unused,section (".cell_os_cmd")))

#define CELL_OS_CMD(name,maxargs,rep,cmd,usage, help)         \
        cmd_tbl_t __cell_os_cmd_##name =      \
                    {#name, maxargs, rep, cmd, usage, help}; \
        cpu_addr_t __cell_os_cmd_##name##_ptr  Struct_Section =  \
                    (cpu_addr_t)(&__cell_os_cmd_##name);

#endif /* _KSH_H */
