#include <pic18f25k80.h>

#include "device.h"

DEVICE_TYPE cachedDeviceType = DEVICE_UNKNOWN;

DEVICE_TYPE device_getType() {
    if (cachedDeviceType == DEVICE_UNKNOWN) {
        TRISA5 = 0; TRISA3 = 0;
        ANSEL4 = 0; ANSEL3 = 0;

        if (PORTAbits.RA5 && !PORTAbits.RA3) {
            cachedDeviceType = DEVICE_CANSTICK;
        } else if (PORTAbits.RA5 && PORTAbits.RA3) {
            cachedDeviceType = DEVICE_CANSTICK_JACK;
        } else if (!PORTAbits.RA5 && !PORTAbits.RA3) {
            cachedDeviceType = DEVICE_CANSTICK_MINI;
        }
    }
    return cachedDeviceType;
}
