#ifndef IO_H
#define IO_H

void io_init(void);

void io_led_active(void);
void io_led_inactive(void);
void io_led_toggle(void);

void io_turn_termination_on(void);
void io_turn_termination_off(void);

void io_turn_voltage_on(void);
void io_turn_voltage_off(void);

#endif
