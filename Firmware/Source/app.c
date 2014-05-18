#include <p18cxxx.h>
#include <stdbool.h>


#include "config.h"
#include "io.h"
#include "uart.h"
#include "can.h"


#define BUFFER_MAX  64
uint8_t Buffer[BUFFER_MAX];
uint8_t BufferCount = 0;

bool Echo = false;

void processUart(void);


void main(void) {    
    init();
    io_init();

    for (uint8_t i = 0; i < 3; i++) {
        io_led_active();
        wait_short();
        io_led_inactive();
        wait_short();
    }

    uart_init(115200);
    can_init_125k();


    while (true) {
        ClrWdt();

        processUart();

        CAN_MESSAGE message;
        if (can_read(&message)) {
            io_led_active();

            if (message.Flags.IsExtended) {
                uart_writeHexUInt8(message.Header.ID3);
                uart_writeHexUInt8(message.Header.ID2);
                uart_writeHexUInt8(message.Header.ID1);
                uart_writeHexUInt8(message.Header.ID0);
            } else {
                uart_writeHexDigit(message.Header.ID1);
                uart_writeHexUInt8(message.Header.ID0);
            }
            if (!message.Flags.RemoteRequest) {
                uart_writeByte(':');
                for (uint8_t i = 0; i < message.Flags.Length; i++) {
                    uart_writeHexUInt8(message.Data[i]);
                }
            } else {
            }
            uart_writeByte('\n');

            io_led_inactive();
        }
    }
}


void processUart() {
    ClrWdt();
    while (uart_canRead()) {
        io_led_active();
        uint8_t data = uart_readByte();

        if (Echo) { uart_writeByte(data); }

        if (((data == '\r') || (data == '\n')) && (BufferCount > 0)) {

            switch (Buffer[0]) {

                case '?': { //Status
                    if (BufferCount == 1) {
                        uart_writeByte('?');
                        if (io_out_getPower()) { uart_writeByte('W'); } else { uart_writeByte('w'); }
                        if (io_out_getTermination()) { uart_writeByte('P'); } else { uart_writeByte('p'); }
                        if (Echo) { uart_writeByte('O'); } else { uart_writeByte('o'); }
                    } else {
                        uart_writeByte('!');
                        uart_writeByte('n');
                    }
                } break;

                case 'O': { //Echo ON
                    if (BufferCount == 1) {
                        Echo = true;
                    } else {
                        uart_writeByte('!');
                        uart_writeByte('n');
                    }
                } break;

                case 'o': { //Echo OFF
                    if (BufferCount == 1) {
                        Echo = false;
                    } else {
                        uart_writeByte('!');
                        uart_writeByte('n');
                    }
                } break;

                case 'P': { //Termination ON
                    if (BufferCount == 1) {
                        io_out_terminationOn();
                    } else {
                        uart_writeByte('!');
                        uart_writeByte('n');
                    }
                } break;

                case 'p': { //Termination OFF
                    if (BufferCount == 1) {
                        io_out_terminationOff();
                    } else {
                        uart_writeByte('!');
                        uart_writeByte('n');
                    }
                } break;

                case 'S': { //Speed
                    if ((BufferCount == 2) && (Buffer[1] == '*')) { //auto-baud
                        //
                    } else if (BufferCount > 1) {
                        uint16_t number = 0;
                        uint8_t digit;
                        for (uint8_t i = 1; i < BufferCount; i++) {
                            digit = Buffer[i] - 0x30;
                            if (digit < 10) {
                                number *= 10;
                                number += digit;
                            } else {
                                number = 0;
                                break;
                            }
                        }
                        switch (number) {
                            case 20: can_init_20k(); break;
                            case 50: can_init_50k(); break;
                            case 125: can_init_125k(); break;
                            case 250: can_init_250k(); break;
                            case 500: can_init_500k(); break;
                            case 800: can_init_800k(); break;
                            case 1000: can_init_1000k(); break;
                            default: uart_writeByte('!'); uart_writeByte('v'); break;
                        }
                    } else {
                        uart_writeByte('S');
                        uart_writeUInt16(can_getSpeed());
                    }
                } break;

                case 'W': { //V+ ON
                    if (BufferCount == 1) {
                        io_out_powerOn();
                    } else {
                        uart_writeByte('!');
                        uart_writeByte('n');
                    }
                } break;

                case 'w': { //V+ OFF
                    if (BufferCount == 1) {
                        io_out_powerOff();
                    } else {
                        uart_writeByte('!');
                        uart_writeByte('n');
                    }
                } break;

                default:
                    uart_writeByte('!');
                    uart_writeByte('c');
                    break;

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