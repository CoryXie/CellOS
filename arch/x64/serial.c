#include <sys.h>
#include <arch.h>
#include <os.h>

/* The 16550 UART registers */

#define UART_PORT           0x03F8    /* base port-address for the UART COM1 */
#define DIVISOR_LATCH_LOW   (UART_PORT + 0) /* Divisor Latch Register LOW 8 bit */
#define TX_DATA_REG         (UART_PORT + 0) /* Transmit Data Register  8 bit */
#define RX_DATA_REG         (UART_PORT + 0) /* Received Data Register 8 bit */
#define IER_REG             (UART_PORT + 1) /* Interrupt Enable Register 8 bit */
#define DIVISOR_LATCH_HIGH  (UART_PORT + 1) /* Divisor Latch Register HIGH 8 bit */
#define INT_ID_REG          (UART_PORT + 2) /* Interrupt Identification Register 8 bit */
#define FIFO_CONTROL        (UART_PORT + 2) /* FIFO Control Register 8 bit */
#define LINE_CONTROL        (UART_PORT + 3) /*  Line Control Register 8 bit */
#define MODEM_CONTROL       (UART_PORT + 4) /* Modem Control Register 8 bit */
#define LINE_STATUS         (UART_PORT + 5) /* Line Status Register 8 bit */
#define MODEM_STATUS        (UART_PORT + 6) /* Modem Status Register 8 bit */
#define SCRATCH_PAD         (UART_PORT + 7) /* Scratch Pad Register 8 bit */


static int has_seril_port = 0;

void serial_console_init(void)
    {
     uint8_t status = ioport_in8(UART_PORT + 6);

    /* Only add the serial device when it is present. */
    if((status & ((1<<4) | (1<<5))) && status != 0xFF) 
        {
        has_seril_port = 1;
    
        /* establish the UART's operational parameters */

        ioport_out8(IER_REG, 0x00);  /* Disable all interrupts */
        ioport_out8(LINE_CONTROL, 0x80);  /* Enable DLAB (set baud rate divisor) */
        ioport_out8(DIVISOR_LATCH_LOW, 0x01);  /* Set divisor to 1 (lo byte)   */
        ioport_out8(DIVISOR_LATCH_HIGH, 0x00); /* baud 115200    (hi byte) */
        ioport_out8(LINE_CONTROL, 0x03);  /* 8 bits, no parity, one stop bit */
        ioport_out8(FIFO_CONTROL, 0xC7);  /* Enable FIFO, clear them, with 14-byte threshold */
        ioport_out8(MODEM_CONTROL, 0x0B);  /* IRQs enabled, RTS/DSR set */
        }
    }
    
void serial_console_put_char(uint8_t ch)
    {
    if (has_seril_port)
        {
        if (ch == '\n') 
            {
            serial_console_put_char('\r');
            }
            
        /* Transmit Holding Register is Empty? */
        
        while ((ioport_in8(LINE_STATUS) & 0x20) == 0x00);

        /* Write byte to the Transmitter Data Register */
        
        ioport_out8(TX_DATA_REG, ch);
        }        
    }

uint8_t serial_console_get_char(void)
    {
    if (has_seril_port)
        {
        int    data;
        
        /* Received Data is Ready? is the RDR-bit zero? */
        
        while ((ioport_in8( LINE_STATUS ) & 0x01) == 0x00);

        /* Read byte from the Receiver Data Register */
        
        data = ioport_in8( RX_DATA_REG );

        return data;
        }

    return 0;
    }

    
