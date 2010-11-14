#ifndef _ARCH_X86_X64_SEGMENT_H
#define _ARCH_X86_X64_SEGMENT_H

void x64_gdt_init(void);
void x64_gdt_ap_init(void);
void x64_idt_init(void);
void x64_idt_ap_init(void);


#endif /* _X86_X64_SEGMENT_H */
