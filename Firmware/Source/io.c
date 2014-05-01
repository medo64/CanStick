#include <p18cxxx.h>

void io_init() {
    LA0 = 1; //turn off termination
    LA1 = 0; //turn on LED
    LC3 = 1; //turn off V+

    TRISA0 = 0;
    TRISA1 = 0;
    TRISC3 = 0;
}


void io_led_active() {
    LA1 = 1;
}

void io_led_inactive() {
    LA1 = 0;
}

void io_led_toggle() {
    LA1 = !LA1;
}


void io_turn_termination_on() {
    LA0 = 0;
}

void io_turn_termination_off() {
    LA0 = 1;
}


void io_turn_voltage_on() {
    LC3 = 0;
}

void io_turn_voltage_off() {
    LC3 = 1;
}
