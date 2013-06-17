
arch/x64/ap_boot.elf:     file format elf64-x86-64


Disassembly of section .text:

0000000000001000 <ap_boot_16>:
    1000:	fa                   	cli    
    1001:	fc                   	cld    
    1002:	31 c0                	xor    %eax,%eax
    1004:	8e d8                	mov    %eax,%ds
    1006:	8e c0                	mov    %eax,%es
    1008:	8e d0                	mov    %eax,%ss
    100a:	b8 00 b8 8e c0       	mov    $0xc08eb800,%eax
    100f:	26 c7 06 00 00 24 06 	movl   $0x6240000,%es:(%rsi)
    1016:	0f 01 16             	lgdt   (%rsi)
    1019:	95                   	xchg   %eax,%ebp
    101a:	10 0f                	adc    %cl,(%rdi)
    101c:	20 c0                	and    %al,%al
    101e:	66 83 c8 01          	or     $0x1,%ax
    1022:	0f 22 c0             	mov    %rax,%cr0
    1025:	ea                   	(bad)  
    1026:	2a 10                	sub    (%rax),%dl
    1028:	08 00                	or     %al,(%rax)

000000000000102a <ap_boot_32>:
    102a:	66 b8 10 00          	mov    $0x10,%ax
    102e:	8e d8                	mov    %eax,%ds
    1030:	8e c0                	mov    %eax,%es
    1032:	8e e0                	mov    %eax,%fs
    1034:	8e e8                	mov    %eax,%gs
    1036:	8e d0                	mov    %eax,%ss
    1038:	66 c7 05 02 80 0b 00 	movw   $0x641,0xb8002(%rip)        # b9043 <_end+0xb7f6b>
    103f:	41 06 
    1041:	bc e0 0f 00 00       	mov    $0xfe0,%esp
    1046:	0f 20 e0             	mov    %cr4,%rax
    1049:	0f ba e8 05          	bts    $0x5,%eax
    104d:	0f 22 e0             	mov    %rax,%cr4
    1050:	a1 f8 0f 00 00 0f 22 	mov    0xb9d8220f00000ff8,%eax
    1057:	d8 b9 
    1059:	80 00 00             	addb   $0x0,(%rax)
    105c:	c0 0f 32             	rorb   $0x32,(%rdi)
    105f:	0f ba e8 08          	bts    $0x8,%eax
    1063:	0f 30                	wrmsr  
    1065:	0f 20 c0             	mov    %cr0,%rax
    1068:	0f ba e8 1f          	bts    $0x1f,%eax
    106c:	0f 22 c0             	mov    %rax,%cr0
    106f:	0f 01 15 b3 10 00 00 	lgdt   0x10b3(%rip)        # 2129 <_end+0x1051>
    1076:	ea                   	(bad)  
    1077:	b9 10 00 00 08       	mov    $0x8000010,%ecx
	...

000000000000107d <temp_gdt32>:
	...
    1085:	ff                   	(bad)  
    1086:	ff 00                	incl   (%rax)
    1088:	00 00                	add    %al,(%rax)
    108a:	9f                   	lahf   
    108b:	cf                   	iret   
    108c:	00 ff                	add    %bh,%bh
    108e:	ff 00                	incl   (%rax)
    1090:	00 00                	add    %al,(%rax)
    1092:	93                   	xchg   %eax,%ebx
    1093:	cf                   	iret   
	...

0000000000001095 <temp_gdt32_ptr>:
    1095:	17                   	(bad)  
    1096:	00 7d 10             	add    %bh,0x10(%rbp)
	...

000000000000109b <temp_gdt64>:
	...
    10a7:	00 98 20 00 00 00    	add    %bl,0x20(%rax)
    10ad:	00 00                	add    %al,(%rax)
    10af:	00 90 00 00 17 00    	add    %dl,0x170000(%rax)

00000000000010b3 <temp_gdt64_ptr>:
    10b3:	17                   	(bad)  
    10b4:	00 9b 10 00 00 48    	add    %bl,0x48000010(%rbx)

00000000000010b9 <ap_boot_64>:
    10b9:	48 c7 c0 00 00 00 80 	mov    $0xffffffff80000000,%rax
    10c0:	48 01 c5             	add    %rax,%rbp
    10c3:	48 01 c4             	add    %rax,%rsp
    10c6:	48 8b 0c 25 f0 0f 00 	mov    0xff0,%rcx
    10cd:	00 
    10ce:	ff e1                	jmpq   *%rcx
    10d0:	eb fe                	jmp    10d0 <ap_boot_64+0x17>
