/* config.h - system wide configuration definitons */

#ifndef __INCLUDE_CONFIG_H
#define __INCLUDE_CONFIG_H

#define CONFIG_CELLOS_VERSION               "CELL OS for X64 0.0.01"

#define CONFIG_KSTACK_SIZE            4096

#define CONFIG_KHEAP_SIZE             (16 * 1024 * 1024)

#define CONFIG_HEAP_NUM_PAGES               (CONFIG_KHEAP_SIZE/PAGE_SIZE)

#define CONFIG_SMP                          1

#define CONFIG_NR_CPUS                      8

#define CONFIG_HZ                           100

#define CONFIG_SCHED_USE_APIC               1

#define CONFIG_SYS_CBSIZE                   1024

#define CONFIG_SYS_MAXARGS                  16

#define CONFIG_SYS_HELP_CMD_WIDTH           8

#define CONFIG_CPU_ARCH_X64                 1

#define CONFIG_LITTLE_ENDIAN                1

#define CONFIG_LP64                         1

#define CONFIG_ACPICA                       1

//#define CONFIG_VMWARE_CLIENT                0

#endif

