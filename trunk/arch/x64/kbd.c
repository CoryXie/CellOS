#include <sys.h>
#include <arch.h>
#include <os.h>

/* Keyboard code definitions */

#define    L_CTRL           0x1D        /* Left control key. */
#define    R_CTRL           0x1D        /* Right control key. */
#define    L_ALT            0x38        /* Left alt key. */
#define    R_ALT            0x38        /* Right alt key. */
#define    L_SHIFT          0x2A        /* Left shift key. */
#define    R_SHIFT          0x36        /* Right shift key. */

static pthread_t kbd_thread_ptr = NULL;

static char kbd_input_buffer[CONFIG_SYS_CBSIZE]; /* User input buffer. */

/* KBDUS means US Keyboard Layout. This is a scancode table
 * used to layout a standard US keyboard. I have left some
 * comments in to give you an idea of what key is what, even
 * though I set it's array index to 0. You can change that to
 * whatever you want using a macro, if you wish! 
 */
 
unsigned char kbd_US[128] =
    {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',    /* 9 */
   '9', '0', '-', '=', 
   '\b',            /* Backspace */
   '\t',            /* Tab */
   'q', 'w', 'e', 'r',    /* 19 */
   't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',        /* Enter key */
    0,            /* 29   - Control */
   'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',    /* 39 */
   '\'', '`',   0,        /* Left shift */
   '\\', 'z', 'x', 'c', 'v', 'b', 'n',            /* 49 */
   'm', ',', '.', '/',   0,                    /* Right shift */
   '*',
    0,    /* Alt */
   ' ',    /* Space bar */
    0,    /* Caps lock */
    0,    /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,    /* < ... F10 */
    0,    /* 69 - Num lock*/
    0,    /* Scroll Lock */
    0,    /* Home key */
    0,    /* Up Arrow */
    0,    /* Page Up */
   '-',
    0,    /* Left Arrow */
    0,
    0,    /* Right Arrow */
   '+',
    0,    /* 79 - End key*/
    0,    /* Down Arrow */
    0,    /* Page Down */
    0,    /* Insert Key */
    0,    /* Delete Key */
    0,   0,   0,
    0,    /* F11 Key */
    0,    /* F12 Key */
    0,    /* All other keys are undefined */
    };

unsigned char kbd_US_SHIFT[128] =
    {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*',    /* 9 */
   '(', ')', '_', '+', 
   '\b',            /* Backspace */
   '\t',            /* Tab */
   'Q', 'W', 'E', 'R',    /* 19 */
   'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 
   '\n',            /* Enter key */
    0,              /* 29   - Control */
   'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',    /* 39 */
   '\'', '`',   0,        /* Left shift */
   '\\', 'Z', 'X', 'C', 'V', 'B', 'N',              /* 49 */
   'M', '<', '>', '?',   0,                         /* Right shift */
   '*',
    0,    /* Alt */
   ' ',   /* Space bar */
    0,    /* Caps lock */
    0,    /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,    /* < ... F10 */
    0,    /* 69 - Num lock*/
    0,    /* Scroll Lock */
    0,    /* Home key */
    0,    /* Up Arrow */
    0,    /* Page Up */
   '-',
    0,    /* Left Arrow */
    0,
    0,    /* Right Arrow */
   '+',
    0,    /* 79 - End key*/
    0,    /* Down Arrow */
    0,    /* Page Down */
    0,    /* Insert Key */
    0,    /* Delete Key */
    0,   0,   0,
    0,    /* F11 Key */
    0,    /* F12 Key */
    0,    /* All other keys are undefined */
    };

/* Get a character from the keyboard.
 *
 * Waits for input to become available, takes it out of the i8042 buffer and
 * returns it.
 *
 * @return Character obtained from keyboard.
 */
 
unsigned char kbd_get_char(void) 
    {
    static int shift;
    static int ctrl;
    static int alt;
    unsigned char code, ret;

    while (1) 
        {
        /* Check for keyboard data. */
        
        code = ioport_in8(0x64);
        
        if (code & (1 << 0) && code & (1 << 5)) 
            {
            /* Mouse data, discard. */
            
            ioport_in8(0x60);
            
            code &= ~(1 << 0);
            }
        
        if ((code & (1 << 0)) == 0) 
            {
            
            sched_yield();
            
            continue;
            }

        code = ioport_in8(0x60);

        if (code & 0x80) 
            {
            code &= 0x7F;

            if (code == L_SHIFT || code == R_SHIFT) 
                {
                shift = 0;
                } 
            else if(code == L_CTRL || code == R_CTRL) 
                {
                ctrl = 0;
                } 
            else if(code == L_ALT || code == R_ALT) 
                {
                alt = 0;
                }
            
            sched_yield();
            
            continue;
            }

        if (code == L_SHIFT || code == R_SHIFT) 
            {
            shift = 1;
            } 
        else if (code == L_CTRL || code == R_CTRL) 
            {
            ctrl = 1;
            } 
        else if (code == L_ALT || code == R_ALT) 
            {
            alt = 1;
            } 
        else 
            {
            if (shift == 1) 
                {
                ret = kbd_US_SHIFT[code];
                } 
            else 
                {
                ret = kbd_US[code];
                }

            /* Little hack so that pressing Enter won't result in
             * an extra newline being sent. */
            if (ret == '\n') 
                {
                while ((ioport_in8 (0x64) & 1) == 0) {};
                
                ioport_in8(0x60);
                }

            if (ret != 0) 
                {
                return ret;
                }
            }
        
        sched_yield();
        }
    }

/* Handles the keyboard interrupt */

void keyboard_handler
    (
    uint64_t stack_frame
    )
    {
    unsigned char scancode;

    /* Read from the keyboard's data buffer */
    scancode = ioport_in8(0x60);

    /* 
     * If the top bit of the byte we read from the keyboard is
     * set, that means that a key has just been released 
     */
    if (scancode & 0x80)
        {
        /* You can use this one to see if the user released the
         *  shift, alt, or control keys... 
         */
        //printk("%c",kbd_US[scancode & 0x7F]);
        }
    else
        {
        /* Here, a key was just pressed. Please note that if you
         *  hold a key down, you will get repeated key press
         *  interrupts. 
         */

        /*  Just to show you how this works, we simply translate
         *  the keyboard scancode into an ASCII value, and then
         *  display it to the screen. You can get creative and
         *  use some flags to see if a shift is pressed and use a
         *  different layout, or you can add another 128 entries
         *  to the above layout to correspond to 'shift' being
         *  held. If shift is held using the larger lookup table,
         *  you would add 128 to the scancode when you look for it 
         */
        
        printk("%c",kbd_US[scancode]);
        }
    }


static char * kbd_read_line (int count) 
    {
    size_t len = 0, spaces = 0;
    unsigned char ch;

    printk("KSH:%03d> ", count);

    memset(kbd_input_buffer, 0, sizeof(kbd_input_buffer));

    while(1) 
        {
        ch = kbd_get_char();
        if (ch == '\n') 
            {
            printk("%c", ch);
            break;
            } 
        else if (ch == '\b') 
            {
            if (len > 0) 
                {
                if (kbd_input_buffer[len - 1] == ' ') 
                    {
                    spaces--;
                    }

                kbd_input_buffer[--len] = 0;
                printk("\b \b");
                }
            } 
        else if (len < (CONFIG_SYS_CBSIZE - 1)) 
            {
            if (ch == '\t') 
                {
                kbd_input_buffer[len] = 0;

                /* Handle TAB auto completion here! */
                
                }
            else 
                {
                printk("%c", ch);
                
                kbd_input_buffer[len++] = ch;
                
                if (ch == ' ') 
                    {
                    spaces++;
                    }
                }
            }
        sched_yield();
        }

    kbd_input_buffer[len] = 0;
    
    return kbd_input_buffer;
    }

void kbd_process_cmd(char * cmd_line)
    {
    int argc = 0;
    char *next, *cur;
    char *argv[CONFIG_SYS_MAXARGS];
    cmd_tbl_t * cmd_entry;
    
    /* Seperate all the arguments */
    
    next = cmd_line;
    
    while (next != NULL) 
        {
        cur = strsep(&next, " ");
        if (strlen(cur) == 0) 
            {
            continue;
            }

        argv[argc++] = cur;
        
        if (argc >= CONFIG_SYS_MAXARGS) 
            {
            printk("KSH: Error: Too many arguments, only %d allowed\n", 
                CONFIG_SYS_MAXARGS);
            
            return;
            }
        }
    
    argv[argc] = NULL;

    /* Search for the command and run it. */

    cmd_entry = find_cmd(argv[0]);

    if (cmd_entry == NULL)
        {
        printk("KSH: Unknown command '%s'\n", argv[0]);
        return;
        }

    /* found - check max args */
    
    if (argc > cmd_entry->maxargs) 
        {
        cmd_usage(cmd_entry);
        return;
        }
    
    /* OK - call function to do the command */
    
    if ((cmd_entry->cmd) (cmd_entry, 0, argc, argv) != 0) 
        {
        printk("KSH: command '%s' seems failed\n", argv[0]);
        }
    
    return;
    }

void * kbd_thread (void *param)
    {
    char * cmd_line;
    static uint32_t cmd_count = 0;

    vga_console_clear_screen();
    
    while(1)
        {
        cmd_line = kbd_read_line(cmd_count++);
        
        if (strlen(cmd_line) == 0)
            {
            printk("KSH: Please enter a command!\n");
            continue;
            }
        
        kbd_process_cmd(cmd_line);

        sched_yield();
        }
    
    return NULL;
    }

/* Installs the keyboard handler into IRQ1 */

void keyboard_init(void)
    {
    pthread_attr_t thread_attr;

    #ifdef USE_KBD_INTR
    irq_register(INTR_IRQ1, "keyboard", keyboard_handler);
    
    enable_keyboad_intr();
    #else

    /* We use polling mode */
    
    disable_keyboad_intr();
    
    pthread_attr_init(&thread_attr);

    pthread_attr_setname_np(&thread_attr, "KBD");
    pthread_attr_setschedpolicy(&thread_attr, SCHED_RR);
    pthread_attr_settimeslice_np(&thread_attr, 100);
    pthread_create(&kbd_thread_ptr,
                   &thread_attr,
                   kbd_thread,
                   NULL);
    #endif
    
    printk("KBD registered\r\n");
    }

