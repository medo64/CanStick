#include <p18cxxx.h>
#include <stdbool.h>


#include "config.h"
#include "io.h"
#include "uart.h"
#include "can.h"


#define UART_BUFFER_MAX  64
uint8_t UartBuffer[UART_BUFFER_MAX];
uint8_t UartBufferCount = 0;

#define CAN_BUFFER_MAX  255
CAN_MESSAGE CanBuffer[CAN_BUFFER_MAX];
uint8_t CanBufferStart = 0;
uint8_t CanBufferEnd = 0;
uint8_t CanBufferCount = 0;

bool Echo = false;
bool AutoReport = false;


void processUart(void);
void reportNextMessage(void);


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

        if (CanBufferCount < CAN_BUFFER_MAX) {
            if (can_readAsync(&CanBuffer[CanBufferEnd])) {
                io_led_active();

                CanBufferEnd++;
                CanBufferCount++;
                if (AutoReport) {
                    reportNextMessage();
                    uart_writeByte('\n');
                }

                io_led_inactive();
            }
        }
    }
}


void processUart() {
    uint8_t data;
    while (uart_readByteAsync(&data)) {
        ClrWdt();

        if (Echo) { uart_writeByte(data); }

        if ((data == '\n') || (data == '\r')) {
            if (UartBufferCount > 0) {
                io_led_active();

                switch (UartBuffer[0]) {

                    case ':': { //commands/options
                        if (UartBufferCount > 1) {
                            switch (UartBuffer[1]) {

                                case '?': { //device name
                                    if (UartBufferCount == 2) {
                                        uart_writeString("CanStick A");
                                    } else {
                                        uart_writeString("!n");
                                    }
                                } break;

                                case '#': { //Reset
                                    if (UartBufferCount == 2) {
                                        uart_writeByte('\n');
                                        Reset();
                                    } else {
                                        uart_writeString("!n");
                                    }
                                } break;

                                case 'E': { //Echo on
                                    if (UartBufferCount == 2) {
                                        Echo = true;
                                    } else {
                                        uart_writeString("!n");
                                    }
                                } break;

                                case 'e': { //echo off
                                    if (UartBufferCount == 2) {
                                        Echo = false;
                                    } else {
                                        uart_writeString("!n");
                                    }
                                } break;

#if defined(DEVICE_CANSTICK)
                                case 'P': { //Power on
                                    if (UartBufferCount == 2) {
                                        io_out_powerOn();
                                    } else {
                                        uart_writeString("!n");
                                    }
                                } break;

                                case 'p': { //Power off
                                    if (UartBufferCount == 2) {
                                        io_out_powerOff();
                                    } else {
                                        uart_writeString("!n");
                                    }
                                } break;
#endif

                                case 'R': { //Auto-report on
                                    if (UartBufferCount == 2) {
                                        AutoReport = true;
                                    } else {
                                        uart_writeString("!n");
                                    }
                                } break;

                                case 'r': { //Auto-report off
                                    if (UartBufferCount == 2) {
                                        AutoReport = false;
                                    } else {
                                        uart_writeString("!n");
                                    }
                                } break;

                                case 'S': { //Speed
                                    if ((UartBufferCount == 3) && (UartBuffer[2] == '*')) { //auto-baud
                                        //TODO
                                    } else if (UartBufferCount > 2) {
                                        uint16_t number = 0;
                                        uint8_t digit;
                                        for (uint8_t i = 2; i < UartBufferCount; i++) {
                                            digit = UartBuffer[i] - 0x30;
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
                                        uart_writeString("!n");
                                    }
                                } break;

                                case 's': { //return speed
                                    if (UartBufferCount == 2) {
                                        uart_writeUInt16(can_getSpeed());
                                    } else {
                                        uart_writeString("!n");
                                    }
                                } break;

#if defined(DEVICE_CANSTICK)
                                case 'T': { //Termination on
                                    if (UartBufferCount == 2) {
                                        io_out_terminationOn();
                                    } else {
                                        uart_writeString("!n");
                                    }
                                } break;

                                case 't': { //Termination off
                                    if (UartBufferCount == 2) {
                                        io_out_terminationOff();
                                    } else {
                                        uart_writeString("!n");
                                    }
                                } break;
#endif

                                default: {
                                    uart_writeString("!k");
                                } break;

                            }
                        } else { //current status
#if defined(DEVICE_CANSTICK)
                            if (io_out_getPower()) { uart_writeByte('P'); } else { uart_writeByte('p'); }
                            if (io_out_getTermination()) { uart_writeByte('T'); } else { uart_writeByte('t'); }
#endif
                            if (Echo) { uart_writeByte('E'); } else { uart_writeByte('e'); }
                            if (AutoReport) { uart_writeByte('R'); } else { uart_writeByte('r'); }
                        }
                    } break;


                    case '?': { //CAN status
                        if (UartBufferCount == 1) {
                            CAN_STATUS status = can_getStatus();
                            if (status.TxOff) {
                                uart_writeByte('X');
                            } else if (status.TxPassive) {
                                uart_writeByte('T');
                            } else if (status.TxWarning) {
                                uart_writeByte('t');
                            }
                            if (status.RxPassive) {
                                uart_writeByte('R');
                            } else if (status.RxWarning) {
                                uart_writeByte('r');
                            }
                            if (status.RxOverflow) {
                                uart_writeByte('O');
                            } else if (status.RxOverflowWarning) {
                                uart_writeByte('o');
                            }
                            if ((TXERRCNT > 0) || (RXERRCNT > 0)) {
                                uart_writeByte(':');
                                uart_writeHexUInt8(TXERRCNT);
                                uart_writeHexUInt8(RXERRCNT);
                            }
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
                        for (; (i <= 8) && (i < UartBufferCount); i++) { //8 id bytes max + 1 byte extra
                            uint8_t value = UartBuffer[i];
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
                            for (; (j <= 16) && (i < UartBufferCount); i++, j++) { //16 data bytes max + 1 byte extra
                                uint8_t value = UartBuffer[i];
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

                        if (i == UartBufferCount) { //read whole buffer, send message
                            if (tentativeSend) {
                                can_writeAsync(message);
                            } else {
                                if (!can_write(message)) {
                                    uart_writeString("!t");
                                }
                            }
                        } else {
                            uart_writeString("!l");
                        }
                    } break;

                    
                    default:
                        uart_writeString("!c");
                        break;

                }

                uart_writeByte('\n');
                UartBufferCount = 0;
                io_led_inactive();
                break;

            } else { //read next message
                reportNextMessage();
                uart_writeByte('\n');
            }

        } else {
            if (UartBufferCount < UART_BUFFER_MAX) {
                UartBuffer[UartBufferCount] = data;
                UartBufferCount++;
            } else {
                UartBufferCount = 255; //overflow
            }
        }
    }
}


void reportNextMessage() {
    if (CanBufferCount > 0) {
        CAN_MESSAGE message = CanBuffer[CanBufferStart];
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
        CanBufferStart++;
        CanBufferCount--;
    }
}
