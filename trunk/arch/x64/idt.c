#include <sys.h>
#include <arch.h>
#include <os.h>

#define     INTR_COUNT_MAX 256

#define     TRAP_GATE_FLAGS 0xEF
#define     INTR_GATE_FLAGS 0x8E

extern void _x64_isr0(void);
extern void _x64_isr1(void);
extern void _x64_isr2(void);
extern void _x64_isr3(void);
extern void _x64_isr4(void);
extern void _x64_isr5(void);
extern void _x64_isr6(void);
extern void _x64_isr7(void);
extern void _x64_isr8(void);
extern void _x64_isr9(void);
extern void _x64_isr10(void);
extern void _x64_isr11(void);
extern void _x64_isr12(void);
extern void _x64_isr13(void);
extern void _x64_isr14(void);
extern void _x64_isr15(void);
extern void _x64_isr16(void);
extern void _x64_isr17(void);
extern void _x64_isr18(void);
extern void _x64_isr19(void);
extern void _x64_isr20(void);
extern void _x64_isr21(void);
extern void _x64_isr22(void);
extern void _x64_isr23(void);
extern void _x64_isr24(void);
extern void _x64_isr25(void);
extern void _x64_isr26(void);
extern void _x64_isr27(void);
extern void _x64_isr28(void);
extern void _x64_isr29(void);
extern void _x64_isr30(void);
extern void _x64_isr31(void);
extern void _x64_isr32(void);
extern void _x64_isr33(void);
extern void _x64_isr34(void);
extern void _x64_isr35(void);
extern void _x64_isr36(void);
extern void _x64_isr37(void);
extern void _x64_isr38(void);
extern void _x64_isr39(void);
extern void _x64_isr40(void);
extern void _x64_isr41(void);
extern void _x64_isr42(void);
extern void _x64_isr43(void);
extern void _x64_isr44(void);
extern void _x64_isr45(void);
extern void _x64_isr46(void);
extern void _x64_isr47(void);
extern void _x64_isr128(void);
extern void _x64_isr240(void);
extern void _x64_isr241(void);
extern void _x64_isr242(void);
extern void _x64_isr243(void);

extern void _x64_isr_reserved(void);
void x64_idt_remap_pic(void);

uint8_t master_intr_mask = 0;
uint8_t slave_intr_mask = 0;

static x64_desc_ptr_64_t idtr;
static x64_int_descriptor_t idt[INTR_COUNT_MAX];

typedef struct irq_handler
    {
    char *owner;
    int (*handler)(addr_t);
    } irq_handler_t;
    
irq_handler_t irq_handlers [INTR_COUNT_MAX];
    
    
int irq_register
    (
    uint16_t irq_no, 
    char * owner, 
    addr_t handler
    )
    {

    if (irq_no > INTR_COUNT_MAX)
        return -1;

    if (irq_handlers[irq_no].handler != NULL)
        return -1;

    irq_handlers[irq_no].handler = (void*)handler;
    irq_handlers[irq_no].owner = (char*)owner;

    return 0;
    }

int irq_unregister
    (
    uint16_t irq_no
    )
    {

    if (irq_no > INTR_COUNT_MAX)
        return -1;

    if (irq_handlers[irq_no].handler == NULL)
        return 0;

    irq_handlers[irq_no].handler = NULL;
    irq_handlers[irq_no].owner = NULL;

    return 0;
    }    

void irq_init(void)
    {
    int i;

    for (i = 0; i < INTR_COUNT_MAX; i++)
        {
        irq_handlers[i].owner = NULL;
        irq_handlers[i].handler = NULL;
        }
    }

void x64_idt_reserved_exception
    (
    uint64_t stack_frame
    )
    {

    printk("x64_idt_reserved_exception() called\n");

    }

void dump_stack(struct stack_frame * stack)
    {
    printk("cpu-%d stack:\n"
           "stack->ss       %p\n"
           "stack->rsp      %p\n"
           "stack->rflags   %p\n"
           "stack->cs       %p\n"
           "stack->rip      %p\n"
           "stack->error    %p\n"
           "stack->int_no   %p\n"
           "stack->rax      %p\n"
           "stack->rcx      %p\n"
           "stack->rdx      %p\n"
           "stack->rdi      %p\n"
           "stack->rsi      %p\n"
           "stack->rbp      %p\n"
           "stack->r8       %p\n"
           "stack->r9       %p\n"
           "stack->r10      %p\n"
           "stack->r11      %p\n"
           ,
           this_cpu(),
           stack->ss,
           stack->rsp,
           stack->rflags,
           stack->cs,
           stack->rip,
           stack->error,
           stack->int_no,
           stack->rax,
           stack->rcx,
           stack->rdx,
           stack->rdi,
           stack->rsi,
           stack->rbp,
           stack->r8,
           stack->r9,
           stack->r10,
           stack->r11);
    }

void x64_exception_handler
    (
    uint64_t stack_frame
    )
    {
    struct stack_frame *frame;

    frame = (struct stack_frame *)stack_frame;

    if (frame->int_no >= INTR_COUNT_MAX)
        {
        printk("*** WRONG! Exception #%i occured, ip=%p rflags=%p\n",
            frame->int_no,
            frame->rip,
            frame->rflags);

        dump_stack(frame);
        return;
        }
       
    /* ACK these external interrupts */
    
    if ((frame->int_no >= INTR_IRQ0) && (frame->int_no <= INTR_IRQ15))
        {
        // Send an EOI (end of interrupt) signal to the PICs.
        // If this interrupt involved the slave.
        if (frame->int_no >= INTR_IRQ8)
            {
            // Send reset signal to slave.
            ioport_out8(0xA0, 0x20);
            }
        // Send reset signal to master. (As well as slave, if necessary).
        ioport_out8(0x20, 0x20);
        }
    
    if (irq_handlers[frame->int_no].handler == NULL)
        {
        printk("irq: unhandled IRQ on cpu-%d #%i, error %p\n",
            this_cpu(),frame->int_no, frame->error);
        
        x64_idt_reserved_exception(stack_frame);    

        dump_stack(frame);
        
        if (frame->int_no == 13)
            while(1);
        }
    else
        {
        irq_handlers[frame->int_no].handler(stack_frame);
        }    
    }

void x64_idt_ap_init(void)
    {
    asm  volatile ("lidt %0" :: "m"(idtr));
    }


void x64_idt_set_entry
    (
    uint16_t index,
    uint64_t handler,
    uint16_t sel,
    uint8_t flags
    )
    {

    idt[index].offset_0_15  = (handler & 0xFFFF);
    idt[index].offset_16_31 = handler >> 16;
    idt[index].offset_32_63 = handler >> 32;

    idt[index].type = flags & 0x1F;
    idt[index].dpl = (flags >> 5) & 0x3;
    idt[index].present = (flags >> 7) & 0x1;
    idt[index].selector = sel;
    idt[index].reserved0 = 0;
    idt[index].ist = 0;
    idt[index].reserved1 = 0;
    }


void x64_idt_init(void)
    {
    int i;

    irq_init();
    
    /* setup our IDT pointer */

    idtr.limit = sizeof(x64_int_descriptor_t) * INTR_COUNT_MAX - 1;
    idtr.base = (uint64_t)&idt;

    memset(idt,0,sizeof(x64_int_descriptor_t) * INTR_COUNT_MAX);

    for(i = 0; i < INTR_COUNT_MAX ; i++)
        x64_idt_set_entry(i,(uint64_t)&_x64_isr_reserved,KERNEL_CS,INTR_GATE_FLAGS);


    x64_idt_set_entry(0,(uint64_t)&_x64_isr0,KERNEL_CS,TRAP_GATE_FLAGS);
    x64_idt_set_entry(1,(uint64_t)&_x64_isr1,KERNEL_CS,INTR_GATE_FLAGS);
    x64_idt_set_entry(2,(uint64_t)&_x64_isr2,KERNEL_CS,INTR_GATE_FLAGS);
    x64_idt_set_entry(3,(uint64_t)&_x64_isr3,KERNEL_CS,TRAP_GATE_FLAGS);
    x64_idt_set_entry(4,(uint64_t)&_x64_isr4,KERNEL_CS,INTR_GATE_FLAGS);
    x64_idt_set_entry(5,(uint64_t)&_x64_isr5,KERNEL_CS,INTR_GATE_FLAGS);
    x64_idt_set_entry(6,(uint64_t)&_x64_isr6,KERNEL_CS,TRAP_GATE_FLAGS);
    x64_idt_set_entry(7,(uint64_t)&_x64_isr7,KERNEL_CS,TRAP_GATE_FLAGS);
    x64_idt_set_entry(8,(uint64_t)&_x64_isr8,KERNEL_CS,INTR_GATE_FLAGS);
    x64_idt_set_entry(9,(uint64_t)&_x64_isr9,KERNEL_CS,INTR_GATE_FLAGS);
    x64_idt_set_entry(10,(uint64_t)&_x64_isr10,KERNEL_CS,INTR_GATE_FLAGS);
    x64_idt_set_entry(11,(uint64_t)&_x64_isr11,KERNEL_CS,INTR_GATE_FLAGS);
    x64_idt_set_entry(12,(uint64_t)&_x64_isr12,KERNEL_CS,INTR_GATE_FLAGS);
    x64_idt_set_entry(13,(uint64_t)&_x64_isr13,KERNEL_CS,TRAP_GATE_FLAGS);
    x64_idt_set_entry(14,(uint64_t)&_x64_isr14,KERNEL_CS,TRAP_GATE_FLAGS);
    x64_idt_set_entry(15,(uint64_t)&_x64_isr15,KERNEL_CS,INTR_GATE_FLAGS);
    x64_idt_set_entry(16,(uint64_t)&_x64_isr16,KERNEL_CS,TRAP_GATE_FLAGS);
    x64_idt_set_entry(17,(uint64_t)&_x64_isr17,KERNEL_CS,INTR_GATE_FLAGS);
    x64_idt_set_entry(18,(uint64_t)&_x64_isr18,KERNEL_CS,INTR_GATE_FLAGS);
    x64_idt_set_entry(19,(uint64_t)&_x64_isr19,KERNEL_CS,TRAP_GATE_FLAGS);
    x64_idt_set_entry(20,(uint64_t)&_x64_isr20,KERNEL_CS,INTR_GATE_FLAGS);
    x64_idt_set_entry(21,(uint64_t)&_x64_isr21,KERNEL_CS,INTR_GATE_FLAGS);
    x64_idt_set_entry(22,(uint64_t)&_x64_isr22,KERNEL_CS,INTR_GATE_FLAGS);
    x64_idt_set_entry(23,(uint64_t)&_x64_isr23,KERNEL_CS,INTR_GATE_FLAGS);
    x64_idt_set_entry(24,(uint64_t)&_x64_isr24,KERNEL_CS,INTR_GATE_FLAGS);
    x64_idt_set_entry(25,(uint64_t)&_x64_isr25,KERNEL_CS,INTR_GATE_FLAGS);
    x64_idt_set_entry(26,(uint64_t)&_x64_isr26,KERNEL_CS,INTR_GATE_FLAGS);
    x64_idt_set_entry(27,(uint64_t)&_x64_isr27,KERNEL_CS,INTR_GATE_FLAGS);
    x64_idt_set_entry(28,(uint64_t)&_x64_isr28,KERNEL_CS,INTR_GATE_FLAGS);
    x64_idt_set_entry(29,(uint64_t)&_x64_isr29,KERNEL_CS,INTR_GATE_FLAGS);
    x64_idt_set_entry(30,(uint64_t)&_x64_isr30,KERNEL_CS,INTR_GATE_FLAGS);
    x64_idt_set_entry(31,(uint64_t)&_x64_isr31,KERNEL_CS,INTR_GATE_FLAGS);
    x64_idt_set_entry(32,(uint64_t)&_x64_isr32,KERNEL_CS,INTR_GATE_FLAGS);
    x64_idt_set_entry(33,(uint64_t)&_x64_isr33,KERNEL_CS,INTR_GATE_FLAGS);
    x64_idt_set_entry(34,(uint64_t)&_x64_isr34,KERNEL_CS,INTR_GATE_FLAGS);
    x64_idt_set_entry(35,(uint64_t)&_x64_isr35,KERNEL_CS,INTR_GATE_FLAGS);
    x64_idt_set_entry(36,(uint64_t)&_x64_isr36,KERNEL_CS,INTR_GATE_FLAGS);
    x64_idt_set_entry(37,(uint64_t)&_x64_isr37,KERNEL_CS,INTR_GATE_FLAGS);
    x64_idt_set_entry(38,(uint64_t)&_x64_isr38,KERNEL_CS,INTR_GATE_FLAGS);
    x64_idt_set_entry(39,(uint64_t)&_x64_isr39,KERNEL_CS,INTR_GATE_FLAGS);
    x64_idt_set_entry(40,(uint64_t)&_x64_isr40,KERNEL_CS,INTR_GATE_FLAGS);
    x64_idt_set_entry(41,(uint64_t)&_x64_isr41,KERNEL_CS,INTR_GATE_FLAGS);
    x64_idt_set_entry(42,(uint64_t)&_x64_isr42,KERNEL_CS,INTR_GATE_FLAGS);
    x64_idt_set_entry(43,(uint64_t)&_x64_isr43,KERNEL_CS,INTR_GATE_FLAGS);
    x64_idt_set_entry(44,(uint64_t)&_x64_isr44,KERNEL_CS,INTR_GATE_FLAGS);
    x64_idt_set_entry(45,(uint64_t)&_x64_isr45,KERNEL_CS,INTR_GATE_FLAGS);
    x64_idt_set_entry(46,(uint64_t)&_x64_isr46,KERNEL_CS,INTR_GATE_FLAGS);
    x64_idt_set_entry(47,(uint64_t)&_x64_isr47,KERNEL_CS,INTR_GATE_FLAGS);
    x64_idt_set_entry(128,(uint64_t)&_x64_isr128,KERNEL_CS,INTR_GATE_FLAGS);

    x64_idt_set_entry(0xf0,(uint64_t)&_x64_isr240,KERNEL_CS,INTR_GATE_FLAGS);
    x64_idt_set_entry(0xf1,(uint64_t)&_x64_isr241,KERNEL_CS,INTR_GATE_FLAGS);
    x64_idt_set_entry(0xf2,(uint64_t)&_x64_isr242,KERNEL_CS,INTR_GATE_FLAGS);
    x64_idt_set_entry(0xf3,(uint64_t)&_x64_isr243,KERNEL_CS,INTR_GATE_FLAGS);


    x64_idt_remap_pic();

    /* load the IDT */

    asm  volatile ("lidt %0" :: "m"(idtr));
    }

#define X64_MASTER_PIC      0x20
#define X64_MASTER_IMR      0x21
#define X64_SLAVE_PIC       0xA0
#define X64_SLAVE_IMR       0xA1

#define X64_PIC_ICW1        0x11
#define X64_PIC_ICW4        0x01
#define X64_PIC_MASTER_VEC  0x20
#define X64_PIC_SLAVE_VEC   0x28

/*
We actually have two PICS. a master PIC that contains the IRQ0 to IRQ7 and a slave 
PIC that contains IRQ8 to IRQ15. the slave PIC is connected to the master PIC via IRQ2.

--- master PIC ---
IRQ    VECTOR    System Resource
NMI               I/O channel check
0    0x08    Reserved, interval timer
1    0x09    Reserved, keyboard buffer full
2    0x0a    Reserved, cascade interrupt from slave PIC
3    0x0b    COM2 (Note)
4    0x0c    COM1 (Note)
5    0x0d    LPT2 (Plug and Play option) / Audio / User available
6    0x0e    Diskette drive
7    0x0f           LPT1 (Note)
--- slave pic ---
8    0x70    Real-time clock
9    0x71    Reserved for ICH system management bus
10    0x72    User available
11    0x73    User available
12    0x74    Onboard mouse port (if present, else user available)
13    0x75    Reserved, math coprocessor
14    0x76    Primary IDE (if present, else user available)
15    0x77    Secondary IDE (if present, else user available)

The table above shows the IRQ lines, each with it's designated vector (in real mode). 
but that raises an issue, because we are in protected mode now and those vectors 
are reserved for exceptions in protected mode. in the interrupt descriptor table we 
made, vector 0x08 is the vector for the double fault exception (#DF). when the timer 
raise an interrupt, the handler for the double fault exception will get executed and 
(clearly) no one would want that. We quote the intel software developer manual.

Vectors in the range 0 through 31 are reserved by the IA-32 architecture for 
architecture-defined exceptions and interrupts. Not all of the vectors in this range
have a currently defined function. The unassigned vectors in this range are reserved. 
Do not use the reserved vectors.

The vectors in the range 32 to 255 are designated as user-defined interrupts and 
are not reserved by the IA-32 architecture. These interrupts are generally assigned 
to external I/O devices.

I/O map in the motherboard is that the PIC resides as address 0x20 - 0x21 and 
0xa0 - 0xa1. Ports 0x20 and 0xa0 are command ports of the master and slave pic 
respectively. and 0x21 and 0xa1 are their data ports.

Steps on how to remap the master and slave PICs, making them point to vectors 
0x20 and 0xA0 respectively are:

- send ICW1 to PIC1 (20h) and PIC2 (A0h)
- send ICW2 to 21h for the first PIC and 0A1h for the second PIC
- send ICW3 to 21h for the first PIC and 0A1h for the second PIC
- finally, send ICW4 to 21h for the first PIC and 0A1h for the second PIC
*/
void x64_idt_remap_pic(void)
    {
    /* ICW1 to ports 0x20 and 0xa0  */

    ioport_out8(X64_MASTER_PIC, X64_PIC_ICW1);
    ioport_out8(X64_SLAVE_PIC, X64_PIC_ICW1);
    
    /* ICW2. remap PICs     */

    ioport_out8(X64_MASTER_PIC+0x1, X64_PIC_MASTER_VEC);
    ioport_out8(X64_SLAVE_PIC+0x1, X64_PIC_SLAVE_VEC);

    /* ICW3. master-slave relationship and vice versa */

    ioport_out8(X64_MASTER_PIC+0x1, 1 << 2); /* irq2 in master PIC is connected to a slave */
    ioport_out8(X64_SLAVE_PIC+0x1, 0x2); /* slave 2 */

    /* ICW4 */

    ioport_out8(X64_MASTER_PIC+0x1, X64_PIC_ICW4);
    ioport_out8(X64_SLAVE_PIC+0x1, X64_PIC_ICW4);

    /* OCW1 - unmask all the 16 IRQs */

    ioport_out8(X64_MASTER_IMR, 0);
    ioport_out8(X64_SLAVE_IMR, 0);

    slave_intr_mask = 0;
    master_intr_mask = 0;
    }

void disable_pit_intr(void)
    {
    master_intr_mask |= (1 << 0);
    ioport_out8(X64_MASTER_IMR, master_intr_mask);
    }

void enable_pit_intr(void)
    {
    master_intr_mask &= ~(1 << 0);
    ioport_out8(X64_MASTER_IMR, master_intr_mask);
    }

void disable_keyboad_intr(void)
    {
    master_intr_mask |= (1 << 1);
    ioport_out8(X64_MASTER_IMR, master_intr_mask);
    }

void enable_keyboad_intr(void)
    {
    master_intr_mask |= (1 << 1);
    ioport_out8(X64_MASTER_IMR, master_intr_mask);
    }


