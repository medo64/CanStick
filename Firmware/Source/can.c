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


typedef union {
    struct {
        uint8_t COMSTAT;
    };
    struct {
        CAN_STATUS STATUS;
    };
} CAN_RAW_STATUS;


CAN_RX* RxRegisters[8] = { (CAN_RX*)&RXB0CON, (CAN_RX*)&RXB1CON, (CAN_RX*)&B0CON, (CAN_RX*)&B1CON, (CAN_RX*)&B2CON, (CAN_RX*)&B3CON, (CAN_RX*)&B4CON, (CAN_RX*)&B5CON };
uint16_t speed = 0;


void can_preinit() {
    TRISB2 = 0;
    TRISB3 = 1;
    CIOCONbits.ENDRHI = 1; //drive Vdd when recessive

    CANCON = 0b10000000; //set to Configuration mode
    while ((CANSTAT & 0b11100000) != 0b10000000);

    BRGCON2bits.SEG2PHTS =  1; //freely programmable SEG2PH
    ECANCONbits.MDSEL = 2; //enhanced FIFO mode

    for (uint8_t i = 0; i < (sizeof(RxRegisters) / sizeof(RxRegisters[0])); i++) {
        (*RxRegisters[i]).CON.RXM1 = 1; //receive all messages
    }
}

void can_postinit() {
    CANCON = 0b00000000; //set to Normal mode
    while ((CANSTAT & 0b11100000) != 0b00000000);
}


void can_init_20k() {
    can_preinit();
    BRGCON1bits.BRP    = 59;
    BRGCON2bits.PRSEG  = 7; //8 Tq
    BRGCON2bits.SEG1PH = 7; //8 Tq
    BRGCON3bits.SEG2PH = 2; //3 Tq
    BRGCON1bits.SJW    = 2; //3 Tq
    can_postinit();
    speed = 20;
}

void can_init_50k() {
    can_preinit();
    BRGCON1bits.BRP    = 29;
    BRGCON2bits.PRSEG  = 6; //7 Tq
    BRGCON2bits.SEG1PH = 5; //6 Tq
    BRGCON3bits.SEG2PH = 1; //2 Tq
    BRGCON1bits.SJW    = 1; //2 Tq
    can_postinit();
    speed = 50;
}

void can_init_125k() {
    can_preinit();
    BRGCON1bits.BRP    = 11;
    BRGCON2bits.PRSEG  = 6; //7 Tq
    BRGCON2bits.SEG1PH = 5; //6 Tq
    BRGCON3bits.SEG2PH = 1; //2 Tq
    BRGCON1bits.SJW    = 1; //2 Tq
    can_postinit();
    speed = 125;
}

void can_init_250k() {
    can_preinit();
    BRGCON1bits.BRP    = 5;
    BRGCON2bits.PRSEG  = 6; //7 Tq
    BRGCON2bits.SEG1PH = 5; //6 Tq
    BRGCON3bits.SEG2PH = 1; //2 Tq
    BRGCON1bits.SJW    = 1; //2 Tq
    can_postinit();
    speed = 250;
}

void can_init_500k() {
    can_preinit();
    BRGCON1bits.BRP    = 2;
    BRGCON2bits.PRSEG  = 6; //7 Tq
    BRGCON2bits.SEG1PH = 5; //6 Tq
    BRGCON3bits.SEG2PH = 1; //2 Tq
    BRGCON1bits.SJW    = 1; //2 Tq
    can_postinit();
    speed = 500;
}

void can_init_800k() {
    can_preinit();
    BRGCON1bits.BRP    = 2;
    BRGCON2bits.PRSEG  = 3; //4 Tq
    BRGCON2bits.SEG1PH = 2; //3 Tq
    BRGCON3bits.SEG2PH = 1; //2 Tq
    BRGCON1bits.SJW    = 1; //2 Tq
    can_postinit();
    speed = 800;
}

void can_init_1000k() {
    can_preinit();
    BRGCON1bits.BRP    = 2;
    BRGCON2bits.PRSEG  = 2; //3 Tq
    BRGCON2bits.SEG1PH = 1; //2 Tq
    BRGCON3bits.SEG2PH = 1; //2 Tq
    BRGCON1bits.SJW    = 0; //1 Tq
    can_postinit();
    speed = 1000;
}

uint16_t can_getSpeed() {
    return speed;
}


CAN_STATUS can_getStatus() {
    CAN_RAW_STATUS status;
    status.COMSTAT = COMSTAT;
    return status.STATUS;
}


bool can_readAsync(CAN_MESSAGE* message) {
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
            for (uint8_t i = 0; i < (*message).Flags.Length; i++) {
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

void can_read(CAN_MESSAGE* message) {
    while (!can_readAsync(message));
}


bool can_writeAsync(CAN_MESSAGE message) {
    if (TXB0CONbits.TXREQ) { return false; }

    if (message.Flags.IsExtended) {
        TXB0EIDLbits.EID = message.Header.ID & 0xFF;
        TXB0EIDHbits.EID = (message.Header.ID >> 8) & 0xFF;
        TXB0SIDLbits.EID = (message.Header.ID >> 16) & 0x03;
        TXB0SIDLbits.SID = (message.Header.ID >> 18) & 0x07;
        TXB0SIDHbits.SID = (message.Header.ID >> 21);
    } else {
        TXB0SIDLbits.SID = message.Header.ID & 0x07;
        TXB0SIDHbits.SID = message.Header.ID >> 3;
    }

    TXB0SIDLbits.EXIDE = message.Flags.IsExtended;
    TXB0DLCbits.DLC = message.Flags.Length;
    TXB0DLCbits.TXRTR = message.Flags.RemoteRequest;

    TXB0D0 = message.Data[0];
    TXB0D1 = message.Data[1];
    TXB0D2 = message.Data[2];
    TXB0D3 = message.Data[3];
    TXB0D4 = message.Data[4];
    TXB0D5 = message.Data[5];
    TXB0D6 = message.Data[6];
    TXB0D7 = message.Data[7];

    TXB0CONbits.TXREQ = 1;

    return true;
}

bool can_write(CAN_MESSAGE message) {
    while (!can_writeAsync(message)) {
        if (COMSTATbits.TXBO || COMSTATbits.TXBP) { return false; } //break away if device is bus passive
    }
    return true;
}