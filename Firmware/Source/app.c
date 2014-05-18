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
        if (can_readAsync(&message)) {
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
            }
            uart_writeByte('\n');

            io_led_inactive();
        }
    }
}


void processUart() {
    ClrWdt();
    while (uart_canRead()) {
        uint8_t data = uart_readByte();

        if (Echo) { uart_writeByte(data); }

        if (((data == '\r') || (data == '\n')) && (BufferCount > 0)) {
            io_led_active();

            switch (Buffer[0]) {

                case '?': {
                    if (BufferCount == 1) {
                        uart_writeString("CanStick A");
                    } else {
                        uart_writeString("!n");
                    }
                } break;

                case '#': { //Reset
                    if (BufferCount == 1) {
                        uart_writeByte('\n');
                        Reset();
                    } else {
                        uart_writeString("!n");
                    }
                } break;

                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                case 'A':
                case 'B':
                case 'C':
                case 'D':
                case 'E':
                case 'F':
                case 'a':
                case 'b':
                case 'c':
                case 'd':
                case 'e':
                case 'f': {
                    CAN_MESSAGE message;
                    message.Header.ID = 0;
                    message.Flags.RemoteRequest = 1;

                    bool tentativeSend = false;
                    bool hasError = false;

                    uint8_t i = 0;

                    //get id
                    for (; (i <= 8) && (i < BufferCount); i++) { //8 id bytes max + 1 byte extra
                        uint8_t value = Buffer[i];
                        if (value == ':') { //data follows
                            message.Flags.RemoteRequest = 0;
                            i++;
                            break;
                        } else if (value == '~') { //send only if there is a free slot
                            tentativeSend = true;
                            i++;
                            break;
                        } else if ((value >= 0x30) && (value <= 0x39)) { //decimal hex value
                            value -= 0x30;
                        } else if ((value >= 0x41) && (value <= 0x46)) { //uppercase hex value
                            value -= 0x37;
                        } else if ((value >= 0x61) && (value <= 0x66)) { //lowercase hex value
                            value -= 0x57;
                        } else {
                            hasError = true;
                            uart_writeString("!i");
                            break;
                        }
                        message.Header.ID <<= 4;
                        message.Header.ID |= value;
                    }
                    if (hasError) { break; }

                    if (message.Header.ID > 0x1FFFFFFF) {
                        uart_writeString("!io");
                        break;
                    }
                    message.Flags.IsExtended = (i > 4) || (message.Header.ID >= 0x7FF);

                    if (!message.Flags.RemoteRequest) { //read data
                        uint8_t j = 0;
                        for (; (j <= 16) && (i < BufferCount); i++, j++) { //16 data bytes max + 1 byte extra
                            uint8_t value = Buffer[i];
                            if (value == '~') { //send only if there is a free slot
                                tentativeSend = true;
                                i++;
                                break;
                            } else if ((value >= 0x30) && (value <= 0x39)) { //decimal hex value
                                value -= 0x30;
                            } else if ((value >= 0x41) && (value <= 0x46)) { //uppercase hex value
                                value -= 0x37;
                            } else if ((value >= 0x61) && (value <= 0x66)) { //lowercase hex value
                                value -= 0x57;
                            } else {
                                hasError = true;
                                uart_writeString("!d");
                                break;
                            }
                            uint8_t b = j / 2;
                            message.Data[b] <<= 4;
                            message.Data[b] |= value;
                        }
                        if (hasError) { break; }

                        if (j % 2 == 0) {
                            message.Flags.Length = j / 2;
                        } else {
                            uart_writeString("!dl");
                            break;
                        }
                    }

                    if (i == BufferCount) { //read whole buffer, send message
                        if (tentativeSend) {
                            can_writeAsync(message);
                        } else {
                            can_write(message);
                        }
                    } else {
                        uart_writeString("!l");
                    }
                } break;

                case 'i': { //Status
                    if (BufferCount == 1) {
                        uart_writeByte('i');
                        if (io_out_getPower()) { uart_writeByte('W'); } else { uart_writeByte('w'); }
                        if (io_out_getTermination()) { uart_writeByte('P'); } else { uart_writeByte('p'); }
                        if (Echo) { uart_writeByte('O'); } else { uart_writeByte('o'); }
                    } else {
                        uart_writeString("!n");
                    }
                } break;

                case 'O': { //Echo ON
                    if (BufferCount == 1) {
                        Echo = true;
                    } else {
                        uart_writeString("!n");
                    }
                } break;

                case 'o': { //Echo OFF
                    if (BufferCount == 1) {
                        Echo = false;
                    } else {
                        uart_writeString("!n");
                    }
                } break;

                case 'P': { //Termination ON
                    if (BufferCount == 1) {
                        io_out_terminationOn();
                    } else {
                        uart_writeString("!n");
                    }
                } break;

                case 'p': { //Termination OFF
                    if (BufferCount == 1) {
                        io_out_terminationOff();
                    } else {
                        uart_writeString("!n");
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
                            default: uart_writeString("!v"); break;
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
                        uart_writeString("!n");
                    }
                } break;

                case 'w': { //V+ OFF
                    if (BufferCount == 1) {
                        io_out_powerOff();
                    } else {
                        uart_writeString("!n");
                    }
                } break;

                default:
                    uart_writeString("!c");
                    break;

            }

            uart_writeByte('\n');
            BufferCount = 0;
            io_led_inactive();

        } else {
            if (BufferCount < BUFFER_MAX) {
                Buffer[BufferCount] = data;
                BufferCount++;
            } else {
                BufferCount = 255; //overflow
            }
        }
    }
}