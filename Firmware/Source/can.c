#include <pic18f25k80.h>
#include <stdbool.h>
#include <stdint.h>
#include <GenericTypeDefs.h>
#include "config.h"
#include "io.h"

void can_init_125k() {
    TRISB2 = 0;
    TRISB3 = 1;

    //set to Configuration mode
    CANCON = 0b10000000;
    while ((CANSTAT & 0b11100000) != 0b10000000);

    BRGCON2bits.SEG2PHTS =  1; //freely programmable SEG2PH

    BRGCON1bits.BRP      = 15;
    BRGCON2bits.PRSEG    =  1; //1 Tq
    BRGCON2bits.SEG1PH   =  6; //3 Tq
    BRGCON3bits.SEG2PH   =  5; //3 Tq
    BRGCON1bits.SJW      =  1; //1 Tq

    ECANCONbits.MDSEL = 2; //Enhanced FIFO mode

    //set to Normal mode
    CANCON = 0b00000000;
    while ((CANSTAT & 0b11100000) != 0b00000000);
}


void can_write(void) {
    while (TXB0CONbits.TXREQ);

    io_led_toggle();

    TXB0SIDL = 0x60;
    TXB0SIDH = 0x00;
    TXB0D0 = 65;
    TXB0DLCbits.DLC = 1;
    TXB0CONbits.TXREQ = 1;

    io_led_toggle();
}