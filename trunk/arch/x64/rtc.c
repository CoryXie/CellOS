#include <sys.h>
#include <arch.h>
#include <os.h>

uint32_t rtc_seconds = 0;
uint32_t rtc_minutes = 0;
uint32_t rtc_hours = 0;

void rtc_read_time(void)
    {
    uint32_t tmp;
  #if 0  
    do {                                       // Loop until UIP is set
        ioport_out8(0x70, 0x0A);               // Select status register
    } while (!(ioport_in8(0x71) & 0x80));     // Check UIP bit set?
    
    do {                                       // Loop until UIP is not set
        ioport_out8(0x70, 0x0A);               // Select status register
    } while (ioport_in8(0x71) & 0x80);         // Check UIP bit clear?
  #endif  
    ioport_out8(0x70, 0x00);                           // Select rtc_seconds register
    tmp = ioport_in8(0x71);                            // Read rtc_seconds
    rtc_seconds = ((tmp >> 4) & 0xF) * 10 + (tmp & 0xF);
    ioport_out8(0x70, 0x02);                           // Select rtc_minutes regitser
    tmp = ioport_in8(0x71);                            // Read rtc_minutes
    rtc_minutes = ((tmp >> 4) & 0xF) * 10 + (tmp & 0xF);
    ioport_out8(0x70, 0x04);                           // Select rtc_hours register
    tmp = ioport_in8(0x71);                            // Read rtc_hours
    rtc_hours = ((tmp >> 4) & 0xF) * 10 + (tmp & 0xF);
    }

void rtc_update_time(void)
    {
    rtc_seconds++;
    if (rtc_seconds == 60) 
        {
        rtc_seconds = 0;
        rtc_minutes++;
        if (rtc_minutes == 60) 
            {
            rtc_minutes = 0;
            rtc_hours++;
            if (rtc_hours == 24)
                rtc_hours = 0;
            }
        }

    }

void rtc_update_screen_time(void)
    {
    unsigned char *vidmem = (unsigned char *)VGA_TEXT_MODE_KERN_BASE_ADDR;
    char buf[15], buf2[15];
    uint32_t len, start;
    int i;

    buf[0] = '\0';
    if (rtc_hours < 10)
            strcat(buf, "0");
    strcat(buf, tostring(buf2, rtc_hours));
    strcat(buf, ":");
    if (rtc_minutes < 10)
            strcat(buf, "0");
    strcat(buf, tostring(buf2, rtc_minutes));
    strcat(buf, ":");
    if (rtc_seconds < 10)
            strcat(buf, "0");
    strcat(buf, tostring(buf2, rtc_seconds));

    len = strlen(buf);
    start = 80 - len;
    for (i = 0; i < len; i++) 
        {
        vidmem[(start + i) * 2] = buf[i];
        vidmem[(start + i) * 2 + 1] = 0x1E;
        }
    }

void rtc_update(void)
    {
    rtc_read_time();
    rtc_update_time();
    rtc_update_screen_time();
    }


void * rtc_thread (void *param)
    {
    
    while (TRUE)
        {
        rtc_update();
        sched_yield();
        }
    
    return NULL;
    }

/* Installs the keyboard handler into IRQ1 */
pthread_t rtc_thread_ptr;
void rtc_init(void)
    {
    pthread_attr_t thread_attr;
    
    pthread_attr_init(&thread_attr);

    pthread_attr_setname_np(&thread_attr, "RTC");
    pthread_attr_setschedpolicy(&thread_attr, SCHED_RR);
    pthread_attr_settimeslice_np(&thread_attr, 100);
    pthread_create(&rtc_thread_ptr,
                   &thread_attr,
                   rtc_thread,
                   NULL);
    
    printk("RTC registered\r\n");
    }

