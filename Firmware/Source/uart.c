#include <pic18f25k80.h>
#include <stdbool.h>
#include <stdint.h>
#include <GenericTypeDefs.h>
#include "config.h"


void resetRx() {
    CREN1 = 0; //disable continous receive, also clears errors
    CREN1 = 1; //enable continous receive
    unsigned char dummyRead;
    dummyRead = RCREG1;
    dummyRead = RCREG1;
    dummyRead = RCREG1; //read data to clear FIFO
    SPEN1 = 0; //disable USART.
    SPEN1 = 1; //enable USART.
}


void uart_init(uint32_t desiredBaudRate) { //must be 19200 or less
    UINT16_VAL spbrg;
    spbrg.Val = (uint16_t)(_XTAL_FREQ / desiredBaudRate / 4);
    SPBRG1 = spbrg.v[0];
    SPBRGH1 = spbrg.v[1];
    BRG161 = 1; //16-bit
    BRGH1  = 1; //high speed
    SYNC1  = 0; //asynchronous mode
    SPEN1  = 1; //serial port enabled
    TXEN1  = 1;
    CREN1  = 1;
    resetRx();
}

bool uart_canRead() {
    return (!RC1IF) ? false : true;
}

bool uart_canWrite() {
    return (!TXIF) ? false : true;
}

unsigned char uart_readByte() {
    if (FERR1) { resetRx(); } //framing error
    if (OERR1) { resetRx(); } //overrun error
    while (!RC1IF) { } //wait until something is received
    unsigned char data = RCREG1;
    return data;
}

void uart_writeByte(unsigned char value) {
    while (!TXIF) { } //wait until buffer is empty
    TXREG1 = value;
}

void uart_writeBytes(unsigned char *value, unsigned char count) {
    for (unsigned char i=0; i<count; i++) {
        while (!TXIF) { } //wait until buffer is empty
        TXREG1 = value[i];
    }
}
