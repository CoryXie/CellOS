#include <sys.h>
#include <arch.h>
#include <os.h>

uint8_t *videoMemory = (uint8_t *)VGA_TEXT_MODE_KERN_BASE_ADDR;
uint16_t curLine,curColumn = 0;

static inline void vga_console_set_cursor(uint8_t col, uint8_t row)
    {
    int vidoff;

    vidoff = row * (80*2) + (col*2);
    vidoff >>= 0x1;

    ioport_out8(0x3d4, 0x0f);
    ioport_out8(0x3d5, vidoff & 0x0ff);
    ioport_out8(0x3d4, 0x0e);
    ioport_out8(0x3d5, vidoff >> 8);
    }

extern void serial_console_put_char(uint8_t ch);
    
void vga_console_put_char(uint8_t c, uint8_t col)
    {
    uint16_t offset,x,y;
    uint8_t *pos;

    serial_console_put_char(c);
    
    col = 0x7;

    pos = (uint8_t*)videoMemory;

    x = curColumn * 2;
    y = curLine * (VGA_TEXT_MODE_COLUMNS * 2);
    offset = x+y;

    switch(c)
        {
        case '\n' : 
            {
            curLine++;
            curColumn = 0;
            break;
            }
        case '\t' : 
            {
            curColumn = (curColumn + 8) & ~(8-1);
            break;
            }
        case '\r' : 
            {
            curColumn = 0;
            break;
            }
        case '\b' : 
            {
            if (curColumn > 0)
                curColumn --;
            else
                {
                curColumn = VGA_TEXT_MODE_COLUMNS;
                
                if (curLine > 0)
                    curLine --;
                else
                    {
                    curColumn = 0;
                    curLine = 0;
                    }
                }
            break;
            }
        default:
            {
            pos[offset] = c;
            pos[offset+1] = col;
            curColumn++;
            break;
            }
        }
    
    if (curColumn > VGA_TEXT_MODE_COLUMNS)
        {
        curColumn = 0;
        curLine++;
        }

    if (curLine >= VGA_TEXT_MODE_LINES)
        {
        pos = (uint8_t*) ((uint64_t)videoMemory + (VGA_TEXT_MODE_COLUMNS * 2));
        memcpy(videoMemory, pos, VGA_TEXT_MODE_LINES * VGA_TEXT_MODE_COLUMNS * 2);
        curLine = VGA_TEXT_MODE_LINES - 1;
        }
    
    vga_console_set_cursor(curColumn,curLine);
    }

void vga_console_put_string
    (
    uint8_t *s
    )
    {
    uint8_t c;

    while ((c = *s++) != '\0')
        vga_console_put_char(c, 0x3);

    }

extern void serial_console_init(void);

void vga_console_clear_screen(void)
    {
    uint8_t *p = (uint8_t*)videoMemory;
    
    int i;

    /* Clear the screen */

    for (i = 0 ;i < (25 * 80); i++)
        {
        p[0] = 0x20;
        p[1] = 0x7;
        p++; p++;
        }
    
    vga_console_set_cursor(0, 0);
    }

void vga_console_init(void)
    {

    vga_console_clear_screen();
   
    serial_console_init();    
    }
