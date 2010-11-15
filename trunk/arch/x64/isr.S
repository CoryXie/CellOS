.align 16,0x90

.extern x64_exception_handler

/*
 * The processor will sometimes need to signal your kernel. Something major 
 * may have happened, such as a divide-by-zero, or a page fault. To do this, 
 * it uses the first 32 interrupts. It is therefore doubly important that all 
 * of these handlers are mapped and non-NULL - else the CPU will triple-fault  
 * and reset (bochs will panic with an 'unhandled exception' error).
 *
 * The special, CPU-dedicated interrupts are shown below.
 *
 * 0 - Division by zero exception
 * 1 - Debug exception
 * 2 - Non maskable interrupt
 * 3 - Breakpoint exception
 * 4 - 'Into detected overflow'
 * 5 - Out of bounds exception
 * 6 - Invalid opcode exception
 * 7 - No coprocessor exception
 * 8 - Double fault (pushes an error code)
 * 9 - Coprocessor segment overrun
 * 10 - Bad TSS (pushes an error code)
 * 11 - Segment not present (pushes an error code)
 * 12 - Stack fault (pushes an error code)
 * 13 - General protection fault (pushes an error code)
 * 14 - Page fault (pushes an error code)
 * 15 - Unknown interrupt exception
 * 16 - Coprocessor fault
 * 17 - Alignment check exception
 * 18 - Machine check exception
 * 19-31 - Reserved
 */
 
/* 
 * Only interrupts 8, 10-14 inclusive push error codes onto the stack. 
 * The rest require dummy error codes.
 */

#define IREGISTER_SPACE	0x58

#define IOFFSET_RAX	0x0
#define IOFFSET_RCX	0x8
#define IOFFSET_RDX	0x10
#define IOFFSET_RSI	0x18
#define IOFFSET_RDI	0x20
#define IOFFSET_R8	0x28
#define IOFFSET_R9	0x30
#define IOFFSET_R10	0x38
#define IOFFSET_R11	0x40
#define IOFFSET_RBP	0x48
#define IOFFSET_INTNO	0x50

 /*
  * This macro creates a stub for an ISR which does NOT pass it's own 
  * error code (adds a dummy errcode byte)
  */
  
.macro ISR_NOERRCODE isr_num

.align 128

.globl _x64_isr\isr_num

_x64_isr\isr_num:
	pushq	$0 /* a dummy errno */
	subq $(IREGISTER_SPACE), %rsp
	movq $(\isr_num), IOFFSET_INTNO(%rsp)
    	jmp _x64_isr_stub
.endm

 /*
  * This macro creates a stub for an ISR which passes it's own 
  * error code
  */
  
.macro ISR_ERRCODE isr_num
.align 128
.globl _x64_isr\isr_num

_x64_isr\isr_num:
	subq $IREGISTER_SPACE, %rsp
	movq $(\isr_num), IOFFSET_INTNO(%rsp)
    	jmp _x64_isr_stub
.endm

.macro EXC_PROLOGUE
	movq %rax, IOFFSET_RAX(%rsp)
	movq %rcx, IOFFSET_RCX(%rsp)
	movq %rdx, IOFFSET_RDX(%rsp)
	movq %rsi, IOFFSET_RSI(%rsp)
	movq %rdi, IOFFSET_RDI(%rsp)
	movq %r8,  IOFFSET_R8(%rsp)
	movq %r9,  IOFFSET_R9(%rsp)
	movq %r10, IOFFSET_R10(%rsp)
	movq %r11, IOFFSET_R11(%rsp)
	movq %rbp, IOFFSET_RBP(%rsp)
.endm

.macro EXC_EPILOGUE
	movq IOFFSET_RAX(%rsp), %rax
	movq IOFFSET_RCX(%rsp), %rcx
	movq IOFFSET_RDX(%rsp), %rdx
	movq IOFFSET_RSI(%rsp), %rsi
	movq IOFFSET_RDI(%rsp), %rdi
	movq IOFFSET_R8(%rsp),  %r8
	movq IOFFSET_R9(%rsp),  %r9
	movq IOFFSET_R10(%rsp), %r10
	movq IOFFSET_R11(%rsp), %r11
	movq IOFFSET_RBP(%rsp), %rbp
.endm

ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE   8
ISR_NOERRCODE 9
ISR_ERRCODE   10
ISR_ERRCODE   11
ISR_ERRCODE   12
ISR_ERRCODE   13
ISR_ERRCODE   14
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_NOERRCODE 17
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_NOERRCODE 30
ISR_NOERRCODE 31
ISR_NOERRCODE 32 /* IRQ 0 */
ISR_NOERRCODE 33 /* IRQ 1 */
ISR_NOERRCODE 34 /* IRQ 2 */
ISR_NOERRCODE 35 /* IRQ 3 */
ISR_NOERRCODE 36 /* IRQ 4 */
ISR_NOERRCODE 37 /* IRQ 5 */
ISR_NOERRCODE 38 /* IRQ 6 */
ISR_NOERRCODE 39 /* IRQ 7 */
ISR_NOERRCODE 40 /* IRQ 8 */
ISR_NOERRCODE 41 /* IRQ 9 */
ISR_NOERRCODE 42 /* IRQ 10 */
ISR_NOERRCODE 43 /* IRQ 11 */
ISR_NOERRCODE 44 /* IRQ 12 */
ISR_NOERRCODE 45 /* IRQ 13 */
ISR_NOERRCODE 46 /* IRQ 14 */
ISR_NOERRCODE 47 /* IRQ 15 */
ISR_NOERRCODE 128 /* INT 0x80 */
ISR_NOERRCODE 240 /* INT LAPIC_VECT_TIMER */
ISR_NOERRCODE 241 /* INT LAPIC_VECT_SPURIOUS */
ISR_NOERRCODE 242 /* INT LAPIC_VECT_IPI */
ISR_NOERRCODE 243 /* INT LAPIC_VECT_RESCHEDULE */

_x64_isr_stub:

    	EXC_PROLOGUE
    
	cld

	# Stop stack traces here
	xorq %rbp, %rbp

	# %rdi - pointer to stack frame as 1st parameter    
	movq %rsp, %rdi   	

    	call x64_exception_handler

    	EXC_EPILOGUE

	# $8 = Skip error word
	addq $(IREGISTER_SPACE+8), %rsp
	iretq
	
.globl _x64_isr_reserved

_x64_isr_reserved:    
	pushq	$0 /* a dummy errno */
	subq $(IREGISTER_SPACE), %rsp
	movq $48, IOFFSET_INTNO(%rsp)
    	jmp _x64_isr_stub

.globl switch_context

switch_context:
	pushq	%rbp
	pushq	%rdi
	pushq	%rsi
	pushq	%rcx
	pushq	%r8
	pushq	%r9
	pushq	%r10
	pushq	%r11
	movq	%rsp,(%rsi)             /* *save_sp = rsp */
	movq	(%rdi),%rsp             /* rsp = *new_sp */
	popq	%r11
	popq	%r10
	popq	%r9
	popq	%r8
	popq	%rcx
	popq	%rsi
	popq	%rdi
	popq	%rbp
	ret    
