#include <p18cxxx.h>
#include <stdint.h>
#include "config.h"


#pragma config RETEN     = OFF
#pragma config INTOSCSEL = HIGH
#if defined(DEVICE_CANSTICK)
    #pragma config SOSCSEL  SOSCSEL = DIG
#elif defined(DEVICE_CANJACK)
    #pragma config SOSCSEL   = HIGH
#endif
#pragma config XINST     = OFF

#if defined(DEVICE_CANSTICK)
    #pragma config FOSC      = EC3
    #pragma config PLLCFG    = OFF
#elif defined(DEVICE_CANJACK)
    #pragma config FOSC      = HS2
    #pragma config PLLCFG    = ON
#endif

#pragma config FCMEN     = OFF
#pragma config IESO      = OFF

#pragma config PWRTEN    = ON
#pragma config BOREN     = SBORDIS
#pragma config BORV      = 0
#pragma config BORPWR    = HIGH

#pragma config WDTEN     = SWDTDIS
#pragma config WDTPS     = 1048576

#pragma config CANMX     = PORTB
#pragma config MSSPMSK   = MSK7
#pragma config MCLRE     = OFF

#pragma config STVREN    = ON
#pragma config BBSIZ     = BB2K

#pragma config CP0       = OFF
#pragma config CP1       = OFF
#pragma config CP2       = OFF
#pragma config CP3       = OFF

#pragma config CPB       = OFF
#pragma config CPD       = OFF

#pragma config WRT0      = OFF
#pragma config WRT1      = OFF
#pragma config WRT2      = OFF
#pragma config WRT3      = OFF

#pragma config WRTC      = OFF
#pragma config WRTB      = OFF
#pragma config WRTD      = OFF

#pragma config EBTR0     = OFF
#pragma config EBTR1     = OFF
#pragma config EBTR2     = OFF
#pragma config EBTR3     = OFF

#pragma config EBTRB     = OFF


void Delay10KTCYx(unsigned char count) {
    do {
        _delay(10000);
    } while(--count != 0);
}


void init(void) {
    //disable interrupts
    GIE = 0;

    //wait for PLL lock
    PLLEN = 1;
    while (!OSCCONbits.OSTS);
    Delay10KTCYx(255);

    REFOCONbits.RODIV3 = 0;
    REFOCONbits.RODIV2 = 0;
    REFOCONbits.RODIV1 = 0;
    REFOCONbits.RODIV0 = 0;
    REFOCONbits.ROSEL = 1;
    REFOCONbits.ROSSLP = 1;
    REFOCONbits.ROON = 1;
    TRISC3 = 0;

    //clear all outputs
    LATA = 0b00000000;
    LATB = 0b00000000;
    LATC = 0b00001000;
}

void wait_short(void) {
    Delay10KTCYx(192);
}

void reset(void) {
    Reset();
}
