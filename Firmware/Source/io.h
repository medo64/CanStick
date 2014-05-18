#ifndef IO_H
#define IO_H

void io_init(void);

#define io_led_active()          LA1 = 1
#define io_led_inactive()        LA1 = 0
#define io_led_toggle()          LA1 = !LA1

#define io_out_terminationOn()   LA0 = 0
#define io_out_terminationOff()  LA0 = 1
#define io_out_getTermination()  !LA0

#define io_out_powerOn()         LC3 = 0
#define io_out_powerOff()        LC3 = 1
#define io_out_getPower()        !LC3

#endif
