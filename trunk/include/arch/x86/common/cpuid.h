/* cpuid.h - CPUID management */

#ifndef _ARCH_X86_COMMON_CPUID_H
#define _ARCH_X86_COMMON_CPUID_H

#include <sys.h>

int detect_cpu(void); 

struct cpuid_info_struct
    {
	char        type;
	char        model;
	char        family;
    char        brand;
	char        step;
	char        fill;
	uint32_t    cpuid;
	uint32_t    capability;
	char        vend_id[12];
	unsigned char cache_info[16];
	uint32_t    pwrcap;
	uint32_t    ext;
	uint32_t    feature_flag;
	uint32_t    dcache0_eax;
	uint32_t    dcache0_ebx;
	uint32_t    dcache0_ecx;
	uint32_t    dcache0_edx;
	uint32_t    dcache1_eax;
	uint32_t    dcache1_ebx;
	uint32_t    dcache1_ecx;
	uint32_t    dcache1_edx;	
	uint32_t    dcache2_eax;
	uint32_t    dcache2_ebx;
	uint32_t    dcache2_ecx;
	uint32_t    dcache2_edx;	
	uint32_t    dcache3_eax;
	uint32_t    dcache3_ebx;
	uint32_t    dcache3_ecx;
	uint32_t    dcache3_edx;
    };

extern struct cpuid_info_struct cpuid_features;

#endif /* _ARCH_X86_COMMON_CPUID_H */
