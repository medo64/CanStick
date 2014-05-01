#ifndef UART_H
#define UART_H

#include <stdbool.h>
#include <stdint.h>


/** Initializes UART. */
void uart_init(uint32_t desiredBaudRate);

/** Returns true if there is data waiting to be read. */
bool uart_canRead();

/** Returns true if there is no data currently being written. */
bool uart_canWrite();

/** Blocking read of a single byte. */
unsigned char uart_readByte();

/** Blocking write of a single byte. */
void uart_writeByte(unsigned char value);

/** Blocking writes of a multiple bytes. */
void uart_writeBytes(unsigned char *value, unsigned char count);


#endif
