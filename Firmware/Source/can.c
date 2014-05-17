#include <pic18f25k80.h>
#include <stdbool.h>
#include <stdint.h>
#include <GenericTypeDefs.h>

#include "config.h"
#include "can.h"
#include "io.h"


typedef struct {
    struct {
        unsigned       : 1;
        unsigned       : 1;
        unsigned       : 1;
        unsigned       : 1;
        unsigned       : 1;
        unsigned RTRRO : 1;
        unsigned RXM1  : 1;
        unsigned RXFUL : 1;
    } CON;
    struct {
        unsigned SID  : 8;
    } SIDH;
    struct {
        unsigned EID  : 2;
        unsigned      : 1;
        unsigned EXID : 1;
        unsigned      : 1;
        unsigned SID  : 3;
    } SIDL;
    struct {
        unsigned EID  : 8;
    } EIDH;
    struct {
        unsigned EID  : 8;
    } EIDL;
    struct {
        unsigned DLC  : 4;
        unsigned      : 2;
        unsigned      : 1;
        unsigned      : 1;
    } DLC;
    uint8_t D[8];
} CAN_RX;


CAN_RX* RxRegisters[8] = { (CAN_RX*)&RXB0CON, (CAN_RX*)&RXB1CON, (CAN_RX*)&B0CON, (CAN_RX*)&B1CON, (CAN_RX*)&B2CON, (CAN_RX*)&B3CON, (CAN_RX*)&B4CON, (CAN_RX*)&B5CON };


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

    ECANCONbits.MDSEL = 2; //enhanced FIFO mode

    for (uint8_t i = 0; i < (sizeof(RxRegisters) / sizeof(RxRegisters[0])); i++) {
        (*RxRegisters[i]).CON.RXM1 = 1; //receive all messages
    }

    //set to Normal mode
    CANCON = 0b00000000;
    while ((CANSTAT & 0b11100000) != 0b00000000);
}


#define CON_RXFUL  0x80
#define SIDL_EXID  0x08

bool can_read(CAN_MESSAGE* message) {
    CAN_RX* root = RxRegisters[CANCON & 0x0F];

    if ((*root).CON.RXFUL) {
        if ((*root).SIDL.EXID) { //extended
            (*message).Header.ID = ((uint32_t)(*root).SIDH.SID << 21) | ((uint32_t)(*root).SIDL.SID << 18) | ((uint32_t)(*root).SIDL.EID << 16) | ((uint32_t)(*root).EIDH.EID << 8) | ((uint32_t)(*root).EIDL.EID);
            (*message).Flags.IsExtended = true;
        } else {
            (*message).Header.ID = ((*root).SIDH.SID << 3) | (*root).SIDL.SID;
            (*message).Flags.IsExtended = false;
        }
        if (!(*root).CON.RTRRO) {
            (*message).Flags.RemoteRequest = false;
            (*message).Flags.Length = (*root).DLC.DLC;
            for (unsigned char i = 0; i < (*message).Flags.Length; i++) {
                (*message).Data[i] = (*root).D[i];
            }
        } else {
            (*message).Flags.RemoteRequest = true;
            (*message).Flags.Length = 0;
        }
        (*root).CON.RXFUL = 0;
        return true;
    } else {
        return false;
    }
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