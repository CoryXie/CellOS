#ifndef _ARCH_X86_COMMON_VGA_H
#define _ARCH_X86_COMMON_VGA_H

#define VGA_TEXT_MODE_PHYS_BASE_ADDR 0xB8000
#define VGA_TEXT_MODE_KERN_BASE_ADDR PA2KA(0xB8000)

#define VGA_TEXT_MODE_LINES 25
#define VGA_TEXT_MODE_COLUMNS 80

void vga_console_init(void);
void vga_console_clear_screen(void);
void vga_console_put_char(uint8_t c, uint8_t col);

#endif /* _ARCH_X86_COMMON_VGA_H */
