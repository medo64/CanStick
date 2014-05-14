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

    RXB0CONbits.RXM1 = 1; //receive all messages
    
    while (true) {
        ClrWdt();

        processUart();
        //can_write();

        if (RXB0CONbits.RXFUL) {
            io_led_toggle();

            CAN_MESSAGE message;

            if (RXB0SIDLbits.EXID) { //extended
                message.Header.ID = ((uint32_t)RXB0SIDHbits.SID << 21) | ((uint32_t)RXB0SIDLbits.SID << 18) | ((uint32_t)RXB0SIDLbits.EID << 16) | ((uint32_t)RXB0EIDHbits.EID << 8) | ((uint32_t)RXB0EIDLbits.EID);
                message.Flags.IsExtended = true;
            } else {
                message.Header.ID = (RXB0SIDHbits.SID << 3) | RXB0SIDLbits.SID;
                message.Flags.IsExtended = false;
            }
            if (!RXB0CONbits.RTRRO) {
                message.Flags.RemoteRequest = false;
                message.Flags.Length = RXB0DLCbits.DLC;
                unsigned char* data = (unsigned char*)&RXB0D0;
                for (unsigned char i = 0; i < message.Flags.Length; i++) {
                    message.Data[i] = *(data + i);
                }
            } else {
                message.Flags.RemoteRequest = true;
                message.Flags.Length = 0;
            }

            if (message.Flags.IsExtended) {
                writeNumberAsHex(message.Header.ID3);
                writeNumberAsHex(message.Header.ID2);
                writeNumberAsHex(message.Header.ID1);
                writeNumberAsHex(message.Header.ID0);
            } else {
                writeNumberAsHex(message.Header.ID1);
                writeNumberAsHex(message.Header.ID0);
            }
            if (!message.Flags.RemoteRequest) {
                uart_writeByte(':');
                for (unsigned char i = 0; i < message.Flags.Length; i++) {
                    writeNumberAsHex(message.Data[i]);
                }
            } else {
                uart_writeByte('!');
            }
            uart_writeByte('\n');

            RXB0CONbits.RXFUL = 0;
            wait_short();
        }
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