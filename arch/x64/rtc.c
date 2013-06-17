/*
  RTC
  
  The Real-Time Clock was added in XT 286 machines. It also includes a
  counter running at 32 kHz but does not provide for direct access. Instead,
  the chip uses the count to keep track of the time of day and also generate
  periodic interrupts. Its only use nowadays seems to be storing the local
  time in battery-buffered CMOS, which is then read during boot time and
  maintained by the OS.
*/
#include <sys.h>
#include <arch.h>
#include <os.h>
#include <time.h>

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

/*
 * The 2 ports used for the RTC and CMOS is 0x70 and 0x71.
 * Port 0x70 is used to specify an index. Port 0x71 is used 
 * to read or write to/from that byte of CMOS configuration space.
 */
#define RTC_INDEX 0x70
#define RTC_DATA  0x71

#define MINUTE 60
#define HOUR (60*MINUTE)
#define DAY (24*HOUR)
#define YEAR (365*DAY)

static int month[12] = 
    {                
    0,
    DAY*(31),
    DAY*(31+28),
    DAY*(31+28+31),
    DAY*(31+28+31+30),
    DAY*(31+28+31+30+31),
    DAY*(31+28+31+30+31+30),
    DAY*(31+28+31+30+31+30+31),
    DAY*(31+28+31+30+31+30+31+31),
    DAY*(31+28+31+30+31+30+31+31+30),
    DAY*(31+28+31+30+31+30+31+31+30+31),
    DAY*(31+28+31+30+31+30+31+31+30+31+30)
    };

long _mktime(int _sec,int _min,int _hour,int _day,int _mon,int _year)
    {
    long res;
    int year;
    res = YEAR*(_year-1970);
    for (year = 1970;year<_year;year++)
       if ( (year%4==0) && (year%100!=0) || year%400==0)
    res += DAY;
    res += month[_mon-1];
    res += (_day-1)*DAY;
    res += _hour*HOUR;
    res += _min*MINUTE;
    res += _sec;

    return res;
    }

int get_cmos_time(unsigned char addr)   
    { 
    char buf[2];
    memset(buf,0,sizeof(buf));

    ioport_out8(RTC_INDEX, addr); 
    sprintf(buf,"%x", ioport_in8(RTC_DATA));
    
    return atoi(buf); 
    }


long rtc_get_utc_time(void)
    {
    long unix_time;
    int sec,min,hour,day,mon,year;
    unsigned char sec_addr=0x00,min_addr=0x02,hou_addr=0x04,day_addr=0x07,mon_addr=0x08,year_addr=0x09;
    char buf[12];
    memset(buf,0,sizeof(buf));
    sec=get_cmos_time(sec_addr);
    min=get_cmos_time(min_addr);
    hour=get_cmos_time(hou_addr);
    day=get_cmos_time(day_addr);
    mon=get_cmos_time(mon_addr);
    year=get_cmos_time(year_addr);
    
    if ((year += 1900) < 1970)
       year += 100;
       
    printk("Current CMOS Date (UTC): %d-%d-%d %d:%d:%d\n",
        year, mon,day, hour, min, sec);
    
    unix_time=_mktime(sec,min,hour,day,mon,year);
    
    printk("Unix Time (CMOS Seconds):%ld\n", unix_time);

    return unix_time;
    }

int do_utctime (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
    {        
    rtc_get_utc_time();
    return 0;
    }

CELL_OS_CMD(
    utc,   1,        1,    do_utctime,
    "show current UTC time",
    "show current UTC time (UTC time and Unix Time)\n"
    );

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
    pthread_attr_settimeslice_np(&thread_attr, 10);
    pthread_create(&rtc_thread_ptr,
                   &thread_attr,
                   rtc_thread,
                   NULL);
    
    printk("RTC registered\r\n");
    }

