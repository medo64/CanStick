#ifndef CONFIG_H
#define CONFIG_H

#if defined(DEVICE_CANSTICK) == defined(DEVICE_CANJACK)
    #error Must select either CanStick or CanJack device configuration.
#endif


#define _XTAL_FREQ 48000000

void init(void);
void wait_short(void);
void reset(void);

#endif
