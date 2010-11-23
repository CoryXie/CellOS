#include <sys.h>
#include <arch.h>
#include <os.h>

#undef LIST_TEST 
#undef PAGE_FAULT_TEST
#undef INT_CALL_TEST
#undef PAGE_ALLOC_TEST
#undef HEAP_ALLOC_TEST
#undef SPINLOCK_TEST
#undef QUEUE_TEST
#undef ATOMIC_TEST

#ifdef LIST_TEST

typedef struct test_list_node
    {
    list_t list;
    char * name;
    }test_list_node_t;

test_list_node_t nodes[6];

void my_list_apend
    (
    list_t *            list,
    test_list_node_t *    node,
    char *                name
    )
    {
    printk("appending %s\n", name);
    node->name = name;
    list_append(list,&node->list);
    }

void my_list_remove
    (
    test_list_node_t *node
    )
    {
    printk("removing %s\n", node->name);
    
    list_remove(&node->list);
    }

void list_test(void)
    {
    list_t list;
    test_list_node_t * entry;
    
    printk("UNIT TEST - list_test\n");
    
    printk("============== Begin ==============\n");
    
    printk("\n===== list_init =====\n");
    
    list_init(&list);
    
    printk("\n===== list_append =====\n");
    
    my_list_apend(&list, &nodes[0], "0bla");
    my_list_apend(&list, &nodes[1], "1hossa");
    my_list_apend(&list, &nodes[2], "2moep");
    my_list_apend(&list, &nodes[3], "3foo");
    my_list_apend(&list, &nodes[4], "4bar");
    my_list_apend(&list, &nodes[5], "5baz");
    
    
    printk("\n===== list_foreach =====\n");
    
    LIST_FOREACH(&list, node) 
        {
        entry = (test_list_node_t*)LIST_ENTRY(node, test_list_node_t, list);
        
        printk("data: %s\n", entry->name);        
        }
    
    printk("\n===== list_remove =====\n");
    
    my_list_remove(&nodes[0]);
    my_list_remove(&nodes[3]);
    my_list_remove(&nodes[5]);
    
    printk("\n===== list_foreach =====\n");
    
    LIST_FOREACH(&list, node) 
        {
        entry = (test_list_node_t*)LIST_ENTRY(node, test_list_node_t, list);
        
        printk("data: %s\n", entry->name);        
        }
    
    printk("==============  End  ==============\n");
    
    return 0;
    }
#endif /* LIST_TEST */

#ifdef QUEUE_TEST

typedef struct test_queue_node
    {
    qelement_t element;
    char * name;
    }test_queue_node_t;

test_queue_node_t queue_nodes[6];

void my_queue_apend
    (
    qhead_t *      head,
    test_queue_node_t *    node,
    char *                name
    )
    {
    printk("appending %s\n", name);
    node->name = name;
    enqueue(head, &node->element, 0);
    }

void queque_test (void)
    {
    qhead_t myQ;
    qelement_t *elem;
    test_queue_node_t *myNode;
    
    queue_init(&myQ,"myQ", 1);
        
    printk("Testing Queue %s empty %d\n", myQ.name, queue_empty(&myQ));

    printk("lock myQ #1\n");
    QUEUE_LOCK(&myQ);
    printk("unlock myQ #1\n");
    QUEUE_UNLOCK(&myQ);

    printk("lock myQ #2\n");
    QUEUE_LOCK(&myQ);
    printk("unlock myQ #2\n");
    QUEUE_UNLOCK(&myQ);

    printk("lock myQ #3\n");
    QUEUE_LOCK(&myQ);
    printk("unlock myQ #3\n");
    QUEUE_UNLOCK(&myQ);

    printk("lock myQ #4\n");

    QUEUE_LOCK(&myQ);

    my_queue_apend(&myQ, &queue_nodes[0], "0bla");
    my_queue_apend(&myQ, &queue_nodes[1], "1hossa");
    my_queue_apend(&myQ, &queue_nodes[2], "2moep");
    my_queue_apend(&myQ, &queue_nodes[3], "3foo");
    my_queue_apend(&myQ, &queue_nodes[4], "4bar");
    my_queue_apend(&myQ, &queue_nodes[5], "5baz");
    
    printk("unlock myQ #4\n");

    QUEUE_UNLOCK(&myQ);

    printk("Testing Queue %s empty %d, count %d\n",
            myQ.name, queue_empty_safe(&myQ), myQ.count);

    elem = dequeue(&myQ, 1);

    myNode = queue_entry(elem,test_queue_node_t,element);

    printk("Testing Queue %s empty %d, count %d, removed name %s\n", 
            myQ.name, queue_empty(&myQ), myQ.count, myNode->name);

    myNode = &queue_nodes[4];

    queue_remove(&myNode->element,1);

    printk("0 Testing Queue %s empty %d\n", myQ.name, queue_empty(&myQ));

    QUEUE_LOCK(&myQ);
    printk("1 Testing Queue %s empty %d\n", myQ.name, queue_empty(&myQ));
    
    QUEUE_UNLOCK(&myQ);
    printk("2 Testing Queue %s empty %d\n", myQ.name, queue_empty(&myQ));
    
    printk("Testing Queue %s empty %d, count %d, removed name %s\n", 
            myQ.name, queue_empty(&myQ), myQ.count, myNode->name);

    }
#endif 


void page_fault_test (void)
    {
    /* Please see vm.h to see what address ranges 
     * will trigger page fault! */

    uint64_t *p = (uint64_t*)0xFFFFFFFF80500000;
    uint64_t v;

    printk("Triggering page falut!\n");

    v = *p; /* A read is enough to fault! */

    printk("v = %p\n",v);
    }

void page_alloc_test (void)
    {
    uint64_t *p;
    uint64_t *pp;
    uint64_t *ppp;

    ppp = page_alloc();

    if (ppp)
        {
        printk("ppp=%p\n",ppp);
        memset(ppp, 0, PAGE_SIZE);
        page_free(ppp);
        }

    p = page_alloc_contig(5);
    if (p)
        {
        printk("page_alloc_contig p=%p\n",p);
        memset(p, 0, 5 * PAGE_SIZE);
        page_free(p);
        }

    pp = p;
    p = page_alloc();
    if (p)
        {
        printk("page_alloc p=%p\n",p);
        memset(p, 0, PAGE_SIZE);
        page_free(p);
        }
    printk("pp=%p p=%p\n",pp,p);
    }


void heap_alloc_test(void)
    {
    uint64_t *p;
    size_t i = 0 ;    
    size_t kmalloc_size = 0;
    
    printk("heap_alloc_test:start...\n");
    
    i = CONFIG_KHEAP_SIZE / 2;
    
    while (i < CONFIG_KHEAP_SIZE)
        {
        kmalloc_size = i;

        if ((i % 40960) == 0)
            printk("malloc(%d)\n", i);

        p = kmalloc((size_t)i);

        if (p)
            {
            /* memset(p,0,i); */
            kfree((void *)p);
            }
        else
            {
            printk("fail %d\n",i);
            break;
            }

        i = i + 1;
        }
    printk("heap_alloc_test:done!\n");
    }

void str_test(void)
    {
    char test[11] = "0123456789";
    
    if (strncmp(test, "01234", 5) == 0)
        printk("The first 5 characters are equal (1)!\n");
    if (strncmp(test, "01234!", 6) == 0)
        printk("The first 6 characters are equal (2)!\n");
    }

void int_call_handler(uint64_t stack_frame)
    {
    printk("breakpoint handler called.\n");
    }

void int_call_test(void)
    {
    irq_register(3, "breakpoint", (addr_t)int_call_handler);
    
    asm volatile ("int $3");
    }

void check_xcr(void)
    {
    uint64_t xcr0;

    if (!has_xcr_registers())
        {
        printk("NO XCR!\n");

        return;
        }
    printk("HAS XCR!\n");

    xcr0 = xgetbv(XCR_XFEATURE_ENABLED_MASK);

    if (xcr0 & XCR0_X87)
        printk("XCR0_X87 set!");

    if (xcr0 & XCR0_SSE)
        printk("XCR0_SSE set!");
    }

void check_ia32e(void)
    {
    uint64_t efer;
    /*
     * The LMA bit (IA32_EFER.LMA[bit 10]) determines whether the processor is
     * operating in IA-32e mode. When running in IA-32e mode, 64-bit or
     * compatibility sub-mode operation is determined by CS.L bit of the code
     * segment. The processor enters into IA-32e mode from protected mode by
     * enabling paging and setting the LME bit (IA32_EFER.LME[bit 8]).
     */

    efer = read_msr(MSR_EFER);

    printk("MSR_EFER =%p\n",efer);

    if (!(efer & EFER_LMA))
        {
        printk("CPU not in IA-32e mode\n");
        }
    else
        {
        printk("CPU in IA-32e mode\n");
        }
    }

void check_gdtr(void)
    {
    x64_desc_ptr_64_t gdtr = {0,0};

    store_gdtr(&gdtr);

    printk("gdtr limit %p, base %p\n",gdtr.limit, gdtr.base);

    }

#ifdef SPINLOCK_TEST

typedef struct my_strcut_with_spinlock
    {
    int dummy;
    spinlock_t lock;
    }my_strcut_with_spinlock_t;

my_strcut_with_spinlock_t my_spinlock;


void spin_lock_test(void)
    {
    
    printk("init spin lock\n");
    
    spinlock_init(&my_spinlock.lock);

    printk("lock spin lock 1st time\n");
    
    spinlock_lock(&my_spinlock.lock);

    printk("unlock spin lock 1st time\n");
    
    spinlock_unlock(&my_spinlock.lock);

    printk("lock spin lock 2nd time\n");

    spinlock_lock(&my_spinlock.lock);

    printk("unlock spin lock 2nd time\n");
    
    spinlock_unlock(&my_spinlock.lock);    
    }
#endif /* SPINLOCK_TEST */

#ifdef ATOMIC_TEST

void atomic_test(void)
    {
    uint8_t old8 = 0x12;
    uint16_t old16 = 0x1234;
    uint32_t old32 = 0x12345678;
    uint64_t old64 = 0x1234567890ABCDEF;
    uint64_t new = 0;
    
    printk("start xchg8 old %p,new %p\n", old8, new);
    new = xchg(&old8, new);
    printk("end xchg8 old %p,new %p\n", old8, new);

    printk("start xchg16 old %p,new %p\n", old16, new);
    new = xchg(&old16, new);
    printk("end xchg16 old %p,new %p\n", old16, new);

    printk("start xchg32 old %p,new %p\n", old32, new);
    new = xchg(&old32, new);
    printk("end xchg32 old %p,new %p\n", old32, new);

    printk("start xchg64 old %p,new %p\n", old64, new);
    new = xchg(&old64, new);
    printk("end xchg64 old %p,new %p\n", old64, new);

    old8 = 0x12;
    new = 0x12;
    
    printk("start cmpxchg8 old %p,new %p\n", old8, new);
    new = cmpxchg(&old8,0x12,new);
    printk("end cmpxchg8 old %p,new %p\n", old8, new);

    old8 = 0x12;
    new = 0x32;
    
    printk("start cmpxchg8 old %p,new %p\n", old8, new);
    new = cmpxchg(&old8,0x12,new);
    printk("end cmpxchg8 old %p,new %p ", old8, new);
    if (old8 != 0x32) 
        printk("NG\n");
    else
        printk("OK\n");
        
    old16 = 0x1234;
    new = 0xabcd;
    
    printk("start cmpxchg16 old %p,new %p\n", old16, new);
    new = cmpxchg(&old16,0x1234,new);
    printk("end cmpxchg16 old %p,new %p ", old16, new);
    if (old16 != 0xabcd) 
        printk("NG\n");
    else
        printk("OK\n");

    old16 = 0x1234;
    new = 0xabcd;
    
    printk("start cmpxchg32 old %p,new %p\n", old16, new);
    new = cmpxchg(&old16,0x1111,new);
    printk("end cmpxchg32 old %p,new %p ", old16, new);
    if (old16 != 0x1234) 
        printk("NG\n");
    else
        printk("OK\n");
    
    old32 = 0x12345678;
    new = 0xabcd;
    
    printk("start cmpxchg32 old %p,new %p\n", old32, new);
    new = cmpxchg(&old32,0x12345678,new);
    printk("end cmpxchg32 old %p,new %p ", old32, new);
    if (old32 != 0xabcd) 
        printk("NG\n");
    else
        printk("OK\n");

    old64 = 0x1234567890ABCDEF;
    new = 0xabcd;
    
    printk("start cmpxchg64 old %p,new %p\n", old64, new);
    new = cmpxchg(&old64,0x1234567890ABCDEF,new);
    printk("end cmpxchg64 old %p,new %p ", old64, new);
    if (old64 != 0xabcd) 
        printk("NG\n");
    else
        printk("OK\n");
    }

#endif /* ATOMIC_TEST*/

#undef KMUTEX_TEST
#undef KMUTEX_TEST_DETAIL

/* When show details, yield more to see the progresss */
#ifdef KMUTEX_TEST_DETAIL
#define KMUTEX_TEST_YIELD_TIMES 40000 
#else
#define KMUTEX_TEST_YIELD_TIMES 500
#endif

#ifdef KMUTEX_TEST
pthread_mutex_t test_mutex;
int mutex_test_variable = 0;
#endif

void * test_thread1(void *param)
    {
    uint8_t *vidmem = (uint8_t *)VGA_TEXT_MODE_KERN_BASE_ADDR;
    uint64_t *params = (uint64_t *)param;
    uint64_t count = 0;
    uint64_t cpu;
    uint64_t count2 = 0;
    
    if (params == NULL)
        {
        printk("testing thread %s parameter is NULL\n",kurrent->name);

        return NULL;
        }
    
    cpu = params[1];

    while(1)
        {
        vidmem[70 + cpu * 2] = '0' + count++;

#ifdef KMUTEX_TEST

#ifdef KMUTEX_TEST_DETAIL
        printk("cpu%d- thread %s locking mutex, mutex_test_variable = %d\n",
            this_cpu(), kurrent->name, mutex_test_variable);
#endif

        pthread_mutex_lock(&test_mutex);

#ifdef KMUTEX_TEST_DETAIL
        printk("cpu%d- thread %s locked mutex, mutex_test_variable = %d\n",
            this_cpu(), kurrent->name, mutex_test_variable);
#endif        
        mutex_test_variable++;
        
        count2 = 0;
        
        while (count2++ < KMUTEX_TEST_YIELD_TIMES)
            sched_yield();
        
#ifdef KMUTEX_TEST_DETAIL
        printk("cpu%d- thread %s release mutex, mutex_test_variable = %d\n", 
            this_cpu(), kurrent->name, mutex_test_variable);
#endif
        pthread_mutex_unlock(&test_mutex);

#ifdef KMUTEX_TEST_DETAIL
        printk("cpu%d- thread %s released mutex, mutex_test_variable = %d\n", 
            this_cpu(), kurrent->name, mutex_test_variable);
#endif

#endif
        if (count > 9)
            count = 5;

        sched_yield();
        }

    return NULL;
    }


void * test_thread2(void *param)
    {
    uint8_t *vidmem = (uint8_t *)VGA_TEXT_MODE_KERN_BASE_ADDR;
    uint64_t *params = (uint64_t *)param;
    uint64_t count = 0;
    uint64_t cpu;
    uint64_t count2 = 0;
    
    if (params == NULL)
        {
        printk("testing thread %s parameter is NULL\n", kurrent->name);

        return NULL;
        }
    
    cpu = params[1];
    
    while(1)
        {
        vidmem[76 + 2 + cpu * 2] = '0' + count++;

#ifdef KMUTEX_TEST

#ifdef KMUTEX_TEST_DETAIL
        printk("cpu%d- thread %s locking mutex, mutex_test_variable = %d\n",
            this_cpu(), kurrent->name, mutex_test_variable);
#endif

        pthread_mutex_lock(&test_mutex);

#ifdef KMUTEX_TEST_DETAIL        
        printk("cpu%d- thread %s locked mutex, mutex_test_variable = %d\n",
            this_cpu(), kurrent->name, mutex_test_variable);
#endif

        count2 = 0;
        
        while (count2++ < KMUTEX_TEST_YIELD_TIMES)
            sched_yield();

#ifdef KMUTEX_TEST_DETAIL        
        printk("cpu%d- thread %s release mutex, mutex_test_variable = %d\n", 
            this_cpu(), kurrent->name, mutex_test_variable);
#endif

        pthread_mutex_unlock(&test_mutex);

#ifdef KMUTEX_TEST_DETAIL
        printk("cpu%d- thread %s release mutex, mutex_test_variable = %d\n", 
            this_cpu(), kurrent->name, mutex_test_variable);
#endif

#endif
        if (count > 5)
            count = 0;

        sched_yield();
        }

    return NULL;
    }

void * test_thread3(void *param)
    {
    uint8_t *vidmem = (uint8_t *)VGA_TEXT_MODE_KERN_BASE_ADDR;
    uint64_t *params = (uint64_t *)param;
    uint64_t count = 0;
    uint64_t cpu;
    uint64_t count2 = 0;
    
    if (params == NULL)
        {
        printk("testing thread %s parameter is NULL\n",kurrent->name);

        return NULL;
        }
    
    cpu = params[1];
    
    while(1)
        {
        vidmem[64 + cpu * 2] = 'a' + count++;

        if (count >= 26)
            count = 0;

        sched_yield();
        }

    return NULL;
    }

void thread_create_test(void)
    {
    uint64_t * params1 = kmalloc(16);
    uint64_t * params2 = kmalloc(16);
    uint64_t * params3 = kmalloc(16);
    pthread_attr_t thread_attr;
    char name[NAME_MAX];
    pthread_t task1;
    pthread_t task2;
    pthread_t task3;

    
#ifdef KMUTEX_TEST 
      {
      pthread_mutexattr_t attr;
      pthread_mutexattr_init(&attr);
      pthread_mutexattr_setname_np(&attr, "test_mutex");
      pthread_mutex_init(&test_mutex, &attr);
      }
#endif

    if (params1 == NULL || params2 == NULL)
        return;
    
    params1[0] = 0x1234567890ABCDEF;
    params1[1] = this_cpu();

    pthread_attr_init(&thread_attr);

    snprintf(name, NAME_MAX, "cpu%d-task1", this_cpu());
    
    pthread_attr_setname_np(&thread_attr, name);
    pthread_create(&task1,
                   &thread_attr,
                   test_thread1,
                   params1);

    params2[0] = 0x55aa55aa55aa55aa;
    params2[1] = this_cpu();
    
    snprintf(name, NAME_MAX, "cpu%d-task2", this_cpu());

    pthread_attr_setname_np(&thread_attr, name);
    pthread_attr_setschedpolicy(&thread_attr, SCHED_RR);
    pthread_attr_settimeslice_np(&thread_attr, 10);

    pthread_create(&task2,
                   &thread_attr,
                   test_thread2,
                   params2);

    params2[0] = 0x55aa55aa55bb55bb;
    params2[1] = this_cpu();
    
    snprintf(name, NAME_MAX, "cpu%d-task3", this_cpu());

    pthread_attr_setname_np(&thread_attr, name);
    pthread_attr_setschedpolicy(&thread_attr, SCHED_RR);
    pthread_attr_settimeslice_np(&thread_attr, 10);

    pthread_create(&task3,
                   &thread_attr,
                   test_thread3,
                   params2);
    }

void unit_testing(void)
    {
    str_test();
    
#ifdef ATOMIC_TEST
    atomic_test();
#endif /* ATOMIC_TEST */

#ifdef SPINLOCK_TEST
    spin_lock_test();
#endif /* SPINLOCK_TEST */

#ifdef QUEUE_TEST
    queque_test();
#endif

#ifdef LIST_TEST
    list_test();
#endif /* LIST_TEST */
   
#ifdef PAGE_FAULT_TEST
    page_fault_test();
#endif
    
#ifdef INT_CALL_TEST
    int_call_test();
#endif
    
#ifdef PAGE_ALLOC_TEST
    page_alloc_test();
#endif
    
#ifdef HEAP_ALLOC_TEST
    heap_alloc_test();
#endif

#ifdef TASK_CREATE_TEST
    thread_create_test();
#endif

#ifdef APIC_TEST
    {
    cpu_addr_t lapic_base;

    x64_debug();

    check_xcr();

    check_ia32e();

    check_gdtr();

    lapic_base = read_msr(MSR_IA32_APICBASE);

    printk("BSP lapic_base %p, id %d, RIP=%p\n",
        lapic_base,
        lapic_id(),get_ip());
    }
#endif

    }
