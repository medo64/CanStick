#include <p18cxxx.h>
#include <stdbool.h>

#include "config.h"
#include "io.h"
#include "uart.h"


#define BUFFER_MAX   64
unsigned char Buffer[BUFFER_MAX];
unsigned char BufferCount = 0;
bool Echo = false;

void processUart(void);


void main(void) {    
    init();
    io_init();
    uart_init(115200);

    
    while (true) {

        processUart();

        //uart_writeByte(uart_readByte());
    }
}


void processUart() {
    ClrWdt();
    while (uart_canRead()) {
        io_led_active();
        unsigned char data = uart_readByte();

        if (Echo) { uart_writeByte(data); }

        if (((data == '\r') || (data == '\n')) && (BufferCount > 0)) {

            switch (Buffer[0]) {

                case 'O': { //Echo ON
                    if (BufferCount == 1) {
                        Echo = true;
                    } else {
                        uart_writeByte('!');
                    }
                } break;

                case 'o': { //Echo OFF
                    if (BufferCount == 1) {
                        Echo = false;
                    } else {
                        uart_writeByte('!');
                    }
                } break;

                case 'P': { //Termination ON
                    if (BufferCount == 1) {
                        io_turn_termination_on();
                    } else {
                        uart_writeByte('!');
                    }
                } break;

                case 'p': { //Termination OFF
                    if (BufferCount == 1) {
                        io_turn_termination_off();
                    } else {
                        uart_writeByte('!');
                    }
                } break;

                case 'W': { //V+ ON
                    if (BufferCount == 1) {
                        io_turn_voltage_on();
                    } else {
                        uart_writeByte('!');
                    }
                } break;

                case 'w': { //V+ OFF
                    if (BufferCount == 1) {
                        io_turn_voltage_off();
                    } else {
                        uart_writeByte('!');
                    }
                } break;

            }

            uart_writeByte('\n');
            BufferCount = 0;

        } else {
            if (BufferCount < BUFFER_MAX) {
                Buffer[BufferCount] = data;
                BufferCount++;
            } else {
                BufferCount = 255; //overflow
            }
        }

        io_led_inactive();
    }
}