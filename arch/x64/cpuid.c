#include <sys.h>
#include <arch.h>
#include <os.h>

struct cpuid_info_struct cpuid_features;

/* Intel Specific brand list */

char *Intel[] = 
    {
    "Brand ID Not Supported.", 
    "Intel(R) Celeron(R) processor", 
    "Intel(R) Pentium(R) III processor", 
    "Intel(R) Pentium(R) III Xeon(R) processor", 
    "Intel(R) Pentium(R) III processor", 
    "Reserved", 
    "Mobile Intel(R) Pentium(R) III processor-M", 
    "Mobile Intel(R) Celeron(R) processor", 
    "Intel(R) Pentium(R) 4 processor", 
    "Intel(R) Pentium(R) 4 processor", 
    "Intel(R) Celeron(R) processor", 
    "Intel(R) Xeon(R) Processor", 
    "Intel(R) Xeon(R) processor MP", 
    "Reserved", 
    "Mobile Intel(R) Pentium(R) 4 processor-M", 
    "Mobile Intel(R) Pentium(R) Celeron(R) processor", 
    "Reserved", 
    "Mobile Genuine Intel(R) processor", 
    "Intel(R) Celeron(R) M processor", 
    "Mobile Intel(R) Celeron(R) processor", 
    "Intel(R) Celeron(R) processor", 
    "Mobile Geniune Intel(R) processor", 
    "Intel(R) Pentium(R) M processor", 
    "Mobile Intel(R) Celeron(R) processor"
    };

/**
  * This table is for those brand strings that have two values depending on the processor
  * signature. It should have the same number of entries as the above table. 
  */
  
char *Intel_Other[] = 
    {
    "Reserved", 
    "Reserved", 
    "Reserved", 
    "Intel(R) Celeron(R) processor", 
    "Reserved", 
    "Reserved", 
    "Reserved", 
    "Reserved", 
    "Reserved", 
    "Reserved", 
    "Reserved", 
    "Intel(R) Xeon(R) processor MP", 
    "Reserved", 
    "Reserved", 
    "Intel(R) Xeon(R) processor", 
    "Reserved", 
    "Reserved", 
    "Reserved", 
    "Reserved", 
    "Reserved", 
    "Reserved", 
    "Reserved", 
    "Reserved", 
    "Reserved"
    };

/* Print Registers */

void printregs(cpuid_info_t *cpuid_info) 
    {
    int j;
    char string[17];
    
    string[16] = '\0';
    for (j = 0; j < 4; j++) 
        {
        string[j] = cpuid_info->eax >> (8 * j);
        string[j + 4] = cpuid_info->ebx >> (8 * j);
        string[j + 8] = cpuid_info->ecx >> (8 * j);
        string[j + 12] = cpuid_info->edx >> (8 * j);
        }
    printk("%s", string);
    }

/* Intel-specific information */
int do_intel(void) 
    {
    printk("Intel Specific Features:\n");
    
    cpuid_info_t cpuid_info;
    unsigned long eax, ebx, ecx, edx, max_eax, signature;
    int model, family, type, brand, stepping, reserved;
    int extended_family = -1;
    
    cpuid(CPUID_GETFEATURES, &cpuid_info);

    eax = cpuid_info.eax;
    ebx = cpuid_info.ebx;
    ecx = cpuid_info.ecx;
    edx = cpuid_info.edx;
    
    model = (eax >> 4) & 0xf;
    family = (eax >> 8) & 0xf;
    type = (eax >> 12) & 0x3;
    brand = ebx & 0xff;
    stepping = eax & 0xf;
    reserved = eax >> 14;
    signature = eax;

    cpuid_features.model = model;
    cpuid_features.family = family;
    cpuid_features.type = type;
    cpuid_features.brand = brand;    
    cpuid_features.step = stepping;
    cpuid_features.fill = reserved;
    cpuid_features.cpuid = signature;
        
    printk("Type %d - ", type);
    switch(type) 
        {
        case 0:
        printk("Original OEM");
        break;
        case 1:
        printk("Overdrive");
        break;
        case 2:
        printk("Dual-capable");
        break;
        case 3:
        printk("Reserved");
        break;
        }
    printk("\n");
    printk("Family %d - ", family);
    switch(family) 
        {
        case 3:
        printk("i386");
        break;
        case 4:
        printk("i486");
        break;
        case 5:
        printk("Pentium");
        break;
        case 6:
        printk("Pentium Pro");
        break;
        case 15:
        printk("Pentium 4");
        break;
        default:
        printk("Unknown CPU family");
        }
    printk("\n");
    
    if (family == 15) 
        {
        extended_family = (eax >> 20) & 0xff;
        printk("Extended family %d\n", extended_family);
        }
    
    printk("Model %d - ", model);
    switch(family) 
        {
        case 3:
        break;
        case 4:
        switch(model) 
            {
            case 0:
            case 1:
            printk("DX");
            break;
            case 2:
            printk("SX");
            break;
            case 3:
            printk("487/DX2");
            break;
            case 4:
            printk("SL");
            break;
            case 5:
            printk("SX2");
            break;
            case 7:
            printk("Write-back enhanced DX2");
            break;
            case 8:
            printk("DX4");
            break;
            default:
            printk("Unknown CPU model");
            }
        break;
        case 5:
        switch(model) 
            {
            case 1:
            printk("60/66");
            break;
            case 2:
            printk("75-200");
            break;
            case 3:
            printk("for 486 system");
            break;
            case 4:
            printk("MMX");
            break;
            default:
            printk("Unknown CPU model");
            }
        break;
        case 6:
        switch(model) 
            {
            case 1:
            printk("Pentium Pro");
            break;
            case 3:
            printk("Pentium II Model 3");
            break;
            case 5:
            printk("Pentium II Model 5/Xeon/Celeron");
            break;
            case 6:
            printk("Celeron");
            break;
            case 7:
            printk("Pentium III/Pentium III Xeon - external L2 cache");
            break;
            case 8:
            printk("Pentium III/Pentium III Xeon - internal L2 cache");
            break;
            default:
            printk("Unknown CPU model");
            }
        break;
        case 15:
        break;
        }
    printk("\n");
    
    cpuid(CPUID_INTELEXTENDED, &cpuid_info);
    max_eax = cpuid_info.eax;
    
    /* 
      * If the max extended eax value is high enough to support the processor brand 
      * string (values 0x80000002 to 0x80000004), then we'll use that information to 
      * return the brand information.  Otherwise, we'll refer back to the brand tables 
      * above for backwards compatibility with older processors.  According to the 
      * Sept. 2006 Intel Arch Software Developer's Guide, if extended eax values are 
      * supported, then all 3 values for the processor brand string are supported, but 
      * we'll test just to make sure and be safe. 
      */
      
    if (max_eax >= 0x80000004) /* CPUID_INTELBRANDSTRINGEND */
        {
        printk("Brand: ");
        if(max_eax >= 0x80000002) /* CPUID_INTELBRANDSTRING */
            {
            cpuid(CPUID_INTELBRANDSTRING, &cpuid_info);
            printregs(&cpuid_info);
            }
        if(max_eax >= 0x80000003) /* CPUID_INTELBRANDSTRINGMORE */
            {
            cpuid(CPUID_INTELBRANDSTRINGMORE, &cpuid_info);
            printregs(&cpuid_info);
            }
        if (max_eax >= 0x80000004) /* CPUID_INTELBRANDSTRINGEND */
            {
            cpuid(CPUID_INTELBRANDSTRINGEND, &cpuid_info);
            printregs(&cpuid_info);
            }
        printk("\n");
        } 
    else if(brand > 0) 
        {
        printk("Brand %d - ", brand);
        if (brand < 0x18) 
            {
            if(signature == 0x000006B1 || signature == 0x00000F13) 
                {
                printk("%s\n", Intel_Other[brand]);
                }
            else 
                {
                printk("%s\n", Intel[brand]);
                }
            } 
        else 
            {
            printk("Reserved\n");
            }
        }
    printk("Stepping: %d Reserved: %d\n", stepping, reserved);
    return 0;
    }

/* AMD-specific information */
int do_amd(void) 
    {
    printk("AMD Specific Features:\n");
    cpuid_info_t cpuid_info;
    uint32_t extended, eax, ebx, ecx, edx;
    uint32_t family, model, stepping, reserved;
    
    cpuid(CPUID_GETFEATURES, &cpuid_info);

    eax = cpuid_info.eax;
    ebx = cpuid_info.ebx;
    ecx = cpuid_info.ecx;
    edx = cpuid_info.edx;

    model = (eax >> 4) & 0xf;
    family = (eax >> 8) & 0xf;
    stepping = eax & 0xf;
    reserved = eax >> 12;
    printk("Family: %d Model: %d [", family, model);
    switch(family) 
        {
        case 4:
        printk("486 Model %d", model);
        break;
        case 5:
        switch(model) 
            {
            case 0:
            case 1:
            case 2:
            case 3:
            case 6:
            case 7:
            printk("K6 Model %d", model);
            break;
            case 8:
            printk("K6-2 Model 8");
            break;
            case 9:
            printk("K6-III Model 9");
            break;
            default:
            printk("K5/K6 Model %d", model);
            break;
            }
        break;
        case 6:
        switch(model) 
            {
            case 1:
            case 2:
            case 4:
            printk("Athlon Model %d", model);
            break;
            case 3:
            printk("Duron Model 3");
            break;
            case 6:
            printk("Athlon MP/Mobile Athlon Model 6");
            break;
            case 7:
            printk("Mobile Duron Model 7");
            break;
            default:
            printk("Duron/Athlon Model %d", model);
            break;
            }
        break;
        }
    printk("]\n");
    
    cpuid(CPUID_INTELEXTENDED,&cpuid_info);
    extended = cpuid_info.eax; 
    
    if (extended == 0) 
        {
        return 0;
        }
    
    if(extended >= 0x80000002) 
        {
        uint32_t j;
        printk("Detected Processor Name: ");
        for (j = 0x80000002; j <= 0x80000004; j++) 
            {
            cpuid(j, &cpuid_info);
            printregs(&cpuid_info);
            }
        printk("\n");
        }
    if(extended >= 0x80000007) 
        {
        cpuid(0x80000007, &cpuid_info);
        if(cpuid_info.edx & 1) 
            {
            printk("Temperature Sensing Diode Detected!\n");
            }
        }
    printk("Stepping: %d Reserved: %d\n", stepping, reserved);
    return 0;
}

/* Simply call this function detect_cpu(); */

int detect_cpu(void) 
    {
    cpuid_info_t cpuid_info;

    if (!has_cpuid())
        {
        printk("CPU does not support CPUID instrcution.\n");
        return -1;
        }
    
    cpuid(CPUID_GETVENDORSTRING, &cpuid_info);
    switch(cpuid_info.ebx) 
        {
        case 0x756e6547: /* Intel Magic Code */
        do_intel();
        break;
        case 0x68747541: /* AMD Magic Code */
        do_amd();
        break;
        default:
        printk("Unknown x86 CPU Detected\n");
        break;
        }
    return 0;
    }

