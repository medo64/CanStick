#include <p18cxxx.h>
#include <stdbool.h>


#include "config.h"
#include "io.h"
#include "uart.h"
#include "can.h"


#define BUFFER_MAX   64
unsigned char Buffer[BUFFER_MAX];
unsigned char BufferCount = 0;

bool Echo = false;

void processUart(void);


void writeNumberAsHex(unsigned char number) {
    unsigned char data[2];
    data[0] = 0x30 + (number >> 4);
    data[1] = 0x30 + (number & 0xF);
    if (data[0] > 0x39) { data[0] += 7; }
    if (data[1] > 0x39) { data[1] += 7; }
    uart_writeBytes(data, 2);
}

void writeDigitAsHex(unsigned char number) {
    unsigned char data = 0x30 + (number & 0xF);
    if (data > 0x39) { data += 7; }
    uart_writeByte(data);
}


void main(void) {    
    init();
    io_init();

    for (unsigned char i = 0; i < 3; i++) {
        io_led_active();
        wait_short();
        io_led_inactive();
        wait_short();
    }

    uart_init(115200);
    can_init_125k();

    io_turn_termination_on();
    
    while (true) {
        ClrWdt();

        processUart();

        CAN_MESSAGE message;
        if (can_read(&message)) {
            io_led_active();

            if (message.Flags.IsExtended) {
                writeNumberAsHex(message.Header.ID3);
                writeNumberAsHex(message.Header.ID2);
                writeNumberAsHex(message.Header.ID1);
                writeNumberAsHex(message.Header.ID0);
            } else {
                writeDigitAsHex(message.Header.ID1);
                writeNumberAsHex(message.Header.ID0);
            }
            if (!message.Flags.RemoteRequest) {
                uart_writeByte(':');
                for (unsigned char i = 0; i < message.Flags.Length; i++) {
                    writeNumberAsHex(message.Data[i]);
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