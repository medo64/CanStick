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


void can_init_20k(void);
void can_init_50k(void);
void can_init_125k(void);
void can_init_250k(void);
void can_init_500k(void);
void can_init_800k(void);
void can_init_1000k(void);
uint16_t can_getSpeed();

bool can_read(CAN_MESSAGE* message);
void can_write();

#endif
