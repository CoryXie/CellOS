# Start an Application Processor. This must be placed on a 4KB boundary
# somewhere in the 1st MB of conventional memory (APBOOTSTRAP). The AP 
# starts in real-mode, with
#   CS selector set to the startup memory address/16;
#   CS base set to startup memory address;
#   CS limit set to 64KB;
#   CPL and IP set to 0.
#

.set PROT_MODE_CS, 0x8         /* kernel code segment selector */
.set PROT_MODE_DS, 0x10        /* kernel data segment selector */
.set CR0_PE,       0x1         /* protected mode enable flag */

.section .text

.code16

.globl ap_boot_16

ap_boot_16:

	cli                         /* Disable interrupts */
	cld                         /* String operations increment */

	/* Set up the important data segment registers (DS, ES, SS) */

	xorw    %ax,%ax             /* Segment number zero */
	movw    %ax,%ds             /* -> Data Segment */
	movw    %ax,%es             /* -> Extra Segment */
	movw    %ax,%ss             /* -> Stack Segment */

	/* Print a diagnostic message */
	
	movw	$0xb800, %ax	    /* 16 bit mode mem = (ES << 4)|offset */
	movw	%ax, %es	    /* ES = 0xb800 */
	movw	$(0x0600 + '$'), %es:(0x00) /* Video mem [0xb8000] = '$' */
	
	lgdt  	temp_gdt32_ptr

	/* Enable protected mode */
	
	movl   	%cr0, %eax
	orl    	$CR0_PE, %eax        
	movl   	%eax, %cr0

	/* Jump to enable protected mode */

	ljmp    $PROT_MODE_CS, $ap_boot_32

ap_boot_32:

.code32

    	/* Set some sane segment registers */

	movw	$PROT_MODE_DS, %ax	/* Our data segment selector */
	movw	%ax, %ds		/* -> DS: Data Segment */
	movw	%ax, %es		/* -> ES: Extra Segment */
	movw	%ax, %fs		/* -> FS */
	movw	%ax, %gs		/* -> GS */
	movw	%ax, %ss		/* -> SS: Stack Segment */
	
	/* Print a diagnostic message */
	
	/* 32 bit mem = GDT entry base + offset */
	
	movw	$(0x0600 + 'A'), (0xb8002) /* Video mem [0xb8002] = 'A' */

    	/* Stack goes here */
    
    	movl   $0xfe0, %esp    
                    
    	/* Enable PAE */
    
    	movl   %cr4, %eax
    	bts    $5, %eax 
    	mov   %eax, %cr4 
    
    	/* load our PML4, the kernel places a pointer a 0xff8 */
    
    	movl  (0xff8), %eax
    	movl   %eax, %cr3


    	/* enable long mode */
    
    	mov   $0xC0000080, %ecx
    	rdmsr
    	bts   $8, %eax
    	wrmsr

    	/* activate long mode by enabling paging */
    
    	mov   %cr0, %eax
    	bts   $31, %eax
    	mov   %eax, %cr0            

    	lgdt temp_gdt64_ptr

    	ljmp $PROT_MODE_CS, $ap_boot_64    

temp_gdt32:
	.quad 	0x0000000000000000	/* NULL */
	.quad 	0x00cf9f000000ffff	/* 32 bit 4GB code */
	.quad 	0x00cf93000000ffff	/* 32 bit 4GB data */

temp_gdt32_ptr:
    	.word	3*8 - 1        		/*  length - 1 */
    	.long	temp_gdt32     		/*  physical address */
	
temp_gdt64:
    	.quad 	0x0000000000000000	/* NULL */
    	.quad 	0x0020980000000000	/* 64 bit 4GB code */
    	.quad 	0x0000900000000000	/* 64 bit 4GB data */

temp_gdt64_ptr:
    	.word   3*8 - 1        		/*  length -1 */
    	.long   temp_gdt64     		/*  physical address */
       
.code64

ap_boot_64:
 
    	/* move stack high */
    
    	movq $0xffffffff80000000, %rax 
    	add %rax, %rbp 
    	add %rax, %rsp 
    
    	/* Kernel passes us entry point at 0xff0 */
    	    
    	movq (0xff0), %rcx
    	
    	/* enter kernel */
    	
    	jmp *%rcx /* Should never return! */
    	
   	/* 
   	 * "jmp %rcx" also works but has warning 
   	 * "Warning: indirect jmp without "*"" 
   	 */
   	
1: jmp 1b /* die */
    

