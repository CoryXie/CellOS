/* barrier.h - memory barrier managment */

#ifndef _ARCH_X86_X64_BARRIER_H
#define _ARCH_X86_X64_BARRIER_H

/*
 * The code you write is not necessarily executed in the order in which 
 * the instructions appear in the source.
 *
 * Optimizing compilers sometimes eliminate or reorder read and write 
 * instructions if the optimizations do not break the logic of the 
 * routine being compiled. In addition, certain hardware architectures 
 * sometimes reorder read and write instructions to improve performance. 
 * Furthermore, on multiprocessor architectures, the sequence in which
 * read and write operations are executed can appear different from the
 * perspective of different processors.
 *
 * Most of the time, reordering by the compiler or the hardware is 
 * completely invisible and has no effect on results other than generating
 * them more efficiently. However, in a few situations, you must prevent 
 * or control reordering. The volatile keyword in C and the synchronization
 * mechanisms can ensure program order of execution in nearly all situations. 
 * In some rare instances, the executable code must contain memory barriers
 * to prevent hardware reordering.
 *
 * Usage Case 1:
 *
 * 1    int a = 5, b = 6;
 * 2    memory_barrier();
 * 3    a = b;
 *
 * The memory_barrier() above causes GCC not to use the value in register used 
 * for variable b to assign to variable a, instead, it causes an explict memory
 * read from the memory address for variable b, and assign the memory read 
 * value to variable a.
 *
 * Usage Case 2:
 * 
 * Assume we are inserting a new element into a list:
 * 
 * 1    new->next = i->next;
 * 2    write_barrier();
 * 3    i->next = new;
 * 
 * And at the same time another processor is iterating that list. The effect
 * might be that the another processor may see the new element, or not. The 
 * two memory writes in the above code should be strictly in sequence. In week
 * memory order architectures, the two memory writes might be reodered, thus
 * it would be danger (consider if "i->next" is set to "new" before it is 
 * assigned to "new->next"...).
 *
 * However, the lucky case is that until now there is no X86 CPU using
 * weak memory ordering for memory writes. So for now write_barrier() is
 * effectively a no-op, that is, are using
 *
 * #define write_barrier()  asm volatile ("" ::: "memory");
 *
 * as the actual code for write_barrier(). The above code is only to cause
 * the GCC not to do any optimization on the output code sequency, the CPU
 * itself will keep it as strong ordered.
 *
 * Usage Case 3:
 *
 * 1    COMPILER_ENTER_BARRIER();
 * 2    a = 1;
 * 3    a = 2;
 * 4    b = 2;
 * 5    COMPILER_LEAVE_BARRIER();
 *
 * If without the COMPILER_ENTER_BARRIER()/COMPILER_LEAVE_BARRIER() pair, the 
 * compiler could actually generate code like this:
 * 6    b = 2;
 * 7    a = 2;
 *
 * That is, it might optimize out the line 2 and reorder line 3 and line 4.
 */

/*
 * Compiler memory barrier
 *
 * These barriers prevent a compiler from reordering instructions, they do 
 * not prevent reordering by CPU.
 */

#define COMPILER_ENTER_BARRIER()  asm volatile ("" ::: "memory")
#define COMPILER_LEAVE_BARRIER()  asm volatile ("" ::: "memory")

/* 
 * cpuid_serialization - serialize instruction execution by using CPUID instruction
 *
 * The CPUID instruction can be executed at any privilege level to serialize
 * instruction execution. Serializing instruction execution guarantees that 
 * any modifications to flags, registers, and memory for previous instructions 
 * are completed before the next instruction is fetched and executed.
 */
static inline void cpuid_serialization(void)
    {
    asm volatile (
        "xorl %%eax, %%eax\n"
        "cpuid\n"
        ::: "eax", "ebx", "ecx", "edx", "memory"
        );
    }

#if defined(CONFIG_FENCES_P4)
    #define memory_barrier()  asm volatile ("mfence\n" ::: "memory")
    #define read_barrier()    asm volatile ("lfence\n" ::: "memory")
    #ifdef CONFIG_WEAK_MEMORY
        #define write_barrier()  asm volatile ("sfence\n" ::: "memory")
    #else
        #define write_barrier()  asm volatile ("" ::: "memory");
    #endif
#elif defined(CONFIG_FENCES_P3)
    #define memory_barrier()  cpuid_serialization()
    #define read_barrier()    cpuid_serialization()
    #ifdef CONFIG_WEAK_MEMORY
        #define write_barrier()  asm volatile ("sfence\n" ::: "memory")
    #else
        #define write_barrier()  asm volatile ("" ::: "memory");
    #endif
#else
    #define memory_barrier()  cpuid_serialization()
    #define read_barrier()    cpuid_serialization()
    #ifdef CONFIG_WEAK_MEMORY
        #define write_barrier()  cpuid_serialization()
    #else
        #define write_barrier()  asm volatile ("" ::: "memory");
    #endif
#endif


/*
 * On X86, the hardware takes care about instruction and data cache coherence,
 * even on SMP systems. We issue a write barrier to be sure that writes
 * queueing in the store buffer drain to the memory (even though it would be
 * sufficient for them to drain to the D-cache).
 */

#define smp_coherence(a)           write_barrier()
#define smp_coherence_block(a, l)  write_barrier()

#endif /* _ARCH_X86_X64_BARRIER_H */
