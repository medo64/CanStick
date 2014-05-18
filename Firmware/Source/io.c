#include <p18cxxx.h>

void io_init() {
    LA0 = 1; //turn off termination
    LA1 = 0; //turn on LED
    LC3 = 1; //turn off V+

    TRISA0 = 0;
    TRISA1 = 0;
    TRISC3 = 0;
}
