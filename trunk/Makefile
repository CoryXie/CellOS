# CELLOS64 top level make file
CELLOS = $(shell pwd)
ARCH = x64
DATE=$(shell date +%F-%H-%M-%S)

# Note about -mcmodel=mem_model
#
# Code models define constraints for symbolic values that allow the compiler to
# generate better code. Basically code models differ in addressing (absolute versus
# position independent), code size, data size and address range.
#
# mem_model Is the memory model to use. Possible values are:
#
#  	small    Tells the compiler to restrict code and data to the first 2GB
#			 of address space. All accesses of code and data can be done
#			 with Instruction Pointer (IP)-relative addressing.
#  	medium 	 Tells the compiler to restrict code to the first 2GB; it places
#			 no memory restriction on data. Accesses of code can be done
#			 with IP-relative addressing, but accesses of data must be
#			 done with absolute addressing.
#  	large 	 Places no memory restriction on code or data. All accesses of
#			 code and data must be done with absolute addressing.
#   kernel   Kernel code model. The kernel of an operating system is usually rather
#			 small but runs in the negative half of the address space. So we define
#			 all symbols to be in the range from 2^64 - 2^31 to 2^64 - 2^24 or from
#			 0xffffffff80000000 to 0xffffffffff000000.
#
# We choose kernel code model because we are using the negative half as the kernel
# symbol address range!

# Note about -ffreestanding
#
# This option ensures that compilation takes place in a freestanding
# environment. The compiler assumes that the standard library may not
# exist and program startup may not necessarily be at main. This
# environment meets the definition of a freestanding environment as
# described in the C and C++ standard.
#
# An example of an application requiring such an environment is an OS
# kernel. When you specify this option, the compiler will not assume
# the presence of compiler-specific libraries. It will only generate
# calls that appear in the source code.

# The following is to compile the X64 OS on a X64 Linux host
# in which the GCC is native 64 bit!

CC = gcc 
G++ = g++
LD  = ld
RM = rm -f

INCLUDEDIR = -Iinclude

CFLAGS = -ffreestanding -mcmodel=kernel -nostdlib -nostdinc -O0 -g -DKERNEL
CFLAGS += -Wall -fomit-frame-pointer -std=c99 -std=gnu99 -O $(INCLUDEDIR)

CPPFLAGS = -Wall -fomit-frame-pointer -O $(INCLUDEDIR)

LDFLAGS = -nostdlib -nostdinc -nodefaultlibs -Bstatic -Tcell64.ld 

ACPICA_DIR = $(CELLOS)/drivers/acpica
INCLUDEDIR += -I$(ACPICA_DIR)/include 
INCLUDEDIR += -I$(ACPICA_DIR)/compiler 

OBJS =  arch/x64/boot.o \
		arch/x64/gdt.o \
        arch/x64/idt.o \
		arch/x64/multiboot.o \
        arch/x64/paging.o \
        arch/x64/vga.o \
        arch/x64/serial.o \
        arch/x64/smp.o \
        arch/x64/acpi.o \
        arch/x64/utils.o \
        arch/x64/apic.o \
		arch/x64/isr.o\
		arch/x64/pit.o \
        arch/x64/cpuid.o \
        arch/x64/kbd.o \
        arch/x64/rtc.o \
		arch/x64/pci.o	\
		arch/x64/hpet.o	\
		arch/x64/pmc.o	\
		arch/x64/sched_arch.o \
		arch/x64/context.o \
		arch/x64/timecounter.o

OBJS += lib/string.o 	\
		lib/printk.o 	\
		lib/sprintf.o 	\
		lib/tlsf.o 		\
		lib/rbtree.o	\
		lib/radixtree.o
		
OBJS += kernel/page_alloc.o 

OBJS += kernel/sched_core.o 	\
		kernel/sched_cpu.o 		\
		kernel/sched_policy.o 	\
		kernel/sched_fifo.o 	\
		kernel/sched_rr.o 		\
		kernel/sched_runq.o		\
		kernel/sched_mutex.o	\
		kernel/sched_thread.o	\
		kernel/sched_thread_spin.o \
		kernel/sched_thread_attr.o \
		kernel/sched_thread_cancel.o \
		kernel/semaphore.o      \
	    kernel/clockeventer.o   \
	    kernel/timer.o          \
	    kernel/signal.o

		
OBJS += drivers/pci/pci_acpi.o

		
OBJS += tests/testing.o

OBJS += init/ksh.o init/main.o 

include $(CELLOS)/drivers/acpica/acpica.mk

DEPS = $(OBJS:.o=.dep)

-include $(OBJS:.o=.dep)

AP_BOOT = arch/x64/ap_boot

# The kernel filename
KERNELFN = kcell

# Link the kernel statically with fixed text+data address @1M
$(KERNELFN) : $(OBJS) ap_boot
	$(LD) $(LDFLAGS) -o $@ $(OBJS) -b binary ap_boot

# Compile the source files

%.o : %.c
	$(COMPILE.c) -MD -o $@ $<
	@cp $*.d $*.dep; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
		-e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.dep; \
	rm -f $*.d	
	
%.o : %.S
	$(COMPILE.S) -MD -D__ASM__ -c -o $@ $<
	@cp $*.d $*.dep; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
		-e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.dep; \
	rm -f $*.d	

ap_boot:
	gcc -o $(AP_BOOT).o -c $(AP_BOOT).S; \
	ld -N -e ap_boot_16 -Ttext=0x1000 -o $(AP_BOOT).elf $(AP_BOOT).o;\
	objcopy -S -O binary $(AP_BOOT).elf ap_boot;

#objdump -S $(AP_BOOT).elf  > $(AP_BOOT).asm
	
	
# Clean up the junk
clean:
	$(RM) $(OBJS) $(KERNELFN) ap_boot cdrom.iso \
	*~ arch/x64/*~ arch/x64/*elf init/*~ kernel/*~ lib/*~ \
	drivers/*~ grub/*~ include/*~ include/arch/x64/*~ $(OBJS) $(DEPS)

TMP = distroot

cdrom.iso: grub/stage2_eltorito grub/menu.lst $(KERNELFN) ap_boot
	echo $(OBJS)
	mkdir -p $(TMP)/boot/grub
	cp grub/stage2_eltorito $(TMP)/boot/grub/
	cp grub/menu.lst $(TMP)/boot/grub/
	cp $(KERNELFN) 	$(TMP)/
	mkisofs -J -r -b boot/grub/stage2_eltorito \
	-no-emul-boot -boot-load-size 4 -boot-info-table \
	-o $@ $(TMP)/

sim:
	qemu-system-x86_64 -cdrom cdrom.iso -smp 1 -S -s &

asm:
	objdump -D -S $(KERNELFN) > $(KERNELFN).asm

sym:
	nm -A -l -n  $(KERNELFN) > $(KERNELFN).txt
	
bak:
	7z a ../backup/cell64-$(DATE).7z ../cellos 

