OUTPUT_FORMAT(elf64-x86-64)
OUTPUT_ARCH(i386:x86-64)
ENTRY(_cell64_32_entry)
SECTIONS
	{

    KERNEL_VMALD = 0xFFFFFFFF80100000;    
    . = KERNEL_VMALD;

    .text : AT(ADDR(.text) - KERNEL_VMALD)
    	{
        _code = .;
        *(.text)
        *(.rodata*)
    	}

	.cell_os_cmd : AT(ADDR(.cell_os_cmd) - KERNEL_VMALD)
		{ 
		__cell_os_cmd_start = .;
		*(.cell_os_cmd) 
		__cell_os_cmd_end = .;
		}
	
   .data : AT(ADDR(.data) - KERNEL_VMALD)
   		{
        _data = .;
        *(.data)
        . = ALIGN(4096);
   		}

   _data_end = .;
   .ehframe : AT(ADDR(.ehframe) - KERNEL_VMALD)
   		{
       _ehframe = .;
       *(.ehframe)
        . = ALIGN(4096);
   		}

   .bss : AT(ADDR(.bss) - KERNEL_VMALD)
   		{
       _bss = .;
       *(.bss)
       . = ALIGN(4096);
   		}

   _end = .;

   /DISCARD/ :
   		{
        *(.comment)
   		}
	}

