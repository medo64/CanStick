#ifndef IO_H
#define IO_H

void io_init(void);

#define io_led_active()          LC5 = 1
#define io_led_inactive()        LC5 = 0
#define io_led_toggle()          LC5 = !LA1

#define io_out_terminationOn()   LC4 = 0
#define io_out_terminationOff()  LC4 = 1
#define io_out_getTermination()  !LC4

#define io_out_powerOn()         LB5 = 0
#define io_out_powerOff()        LB5 = 1
#define io_out_getPower()        !LB5

#endif
