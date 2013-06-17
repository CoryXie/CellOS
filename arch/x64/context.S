.text

.global context_save
.global context_restore

#include <arch/x86/x64/context.h>

# ctx: address of the structure with saved context 
# pc: return address
.macro CONTEXT_SAVE_INTERNAL ctx:req pc:req
	movq \pc, OFFSET_PC(\ctx)
	movq %rsp, OFFSET_SP(\ctx)
	
	movq %rbx, OFFSET_RBX(\ctx)
	movq %rbp, OFFSET_RBP(\ctx)
	movq %r12, OFFSET_R12(\ctx)
	movq %r13, OFFSET_R13(\ctx)
	movq %r14, OFFSET_R14(\ctx)
	movq %r15, OFFSET_R15(\ctx)
.endm

# ctx: address of the structure with saved context 
.macro CONTEXT_RESTORE_INTERNAL ctx:req pc:req
	movq OFFSET_R15(\ctx), %r15
	movq OFFSET_R14(\ctx), %r14
	movq OFFSET_R13(\ctx), %r13
	movq OFFSET_R12(\ctx), %r12
	movq OFFSET_RBP(\ctx), %rbp
	movq OFFSET_RBX(\ctx), %rbx	
	
	movq OFFSET_SP(\ctx), %rsp   # ctx->sp -> %rsp
	
	movq OFFSET_PC(\ctx), \pc
.endm


## Save current CPU context
#
# Save CPU context to context_t variable
# pointed by the 1st argument. Returns 1 in RAX.
#
context_save:
	movq (%rsp), %rdx     # the caller's return %eip

	# In %rdi is passed 1st argument
	CONTEXT_SAVE_INTERNAL %rdi %rdx 
	
	xorq %rax,%rax		# context_save returns 1
	incq %rax
	ret


## Restore current CPU context
#
# Restore CPU context from context_t variable
# pointed by the 1st argument. Returns 0 in RAX.
#
context_restore:	

	CONTEXT_RESTORE_INTERNAL %rdi %rdx

	movq %rdx,(%rsp)

	xorq %rax,%rax		# context_restore returns 0
	ret
