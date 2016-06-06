#ifndef DEVICE_H
#define DEVICE_H

#include <stdint.h>

typedef enum DEVICE_TYPE {
    DEVICE_UNKNOWN,
    DEVICE_CANSTICK,
    DEVICE_CANSTICK_JACK,
    DEVICE_CANSTICK_MINI
} DEVICE_TYPE;

DEVICE_TYPE device_getType();

#endif
