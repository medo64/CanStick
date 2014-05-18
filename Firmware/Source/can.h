#ifndef CAN_H
#define	CAN_H

#include <stdint.h>


typedef union {
    struct {
        uint32_t ID;
    };
    struct {
        uint8_t  ID0;
        uint8_t  ID1;
        uint8_t  ID2;
        uint8_t  ID3;
    };
} CAN_ID;

typedef struct {
    unsigned Length        : 4;
    unsigned               : 2;
    unsigned IsExtended    : 1;
    unsigned RemoteRequest : 1;
} CAN_FLAGS;

typedef  struct {
    CAN_ID    Header;
    CAN_FLAGS Flags;
    uint8_t   Data[8];
} CAN_MESSAGE;


typedef struct {
    unsigned RxOrTxWarning     : 1;
    unsigned RxWarning         : 1;
    unsigned TxWarning         : 1;
    unsigned RxPassive         : 1;
    unsigned TxPassive         : 1;
    unsigned TxOff             : 1;
    unsigned RxOverflow        : 1;
    unsigned RxOverflowWarning : 1;
} CAN_STATUS;


/** Initializes CAN module at 20 kbps. */
void can_init_20k(void);

/** Initializes CAN module at 50 kbps. */
void can_init_50k(void);

/** Initializes CAN module at 125 kbps. */
void can_init_125k(void);

/** Initializes CAN module at 250 kbps. */
void can_init_250k(void);

/** Initializes CAN module at 500 kbps. */
void can_init_500k(void);

/** Initializes CAN module at 800 kbps. */
void can_init_800k(void);

/** Initializes CAN module at 1000 kbps. */
void can_init_1000k(void);

/** Returns speed of CAN module. */
uint16_t can_getSpeed();


/** Returns CAN status. */
CAN_STATUS can_getStatus(void);


/** Blocking read of CAN message.  */
void can_read(CAN_MESSAGE* message);

/** Tries to read CAN message. Returns true if successful. */
bool can_readAsync(CAN_MESSAGE* message);

/** Blocking write of CAN message.  */
void can_write(CAN_MESSAGE message);

/** Tries to write CAN message. Returns true if successful. */
bool can_writeAsync(CAN_MESSAGE message);

#endif
