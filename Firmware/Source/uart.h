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
uint8_t uart_readByte();

/** Blocking write of a single byte. */
void uart_writeByte(uint8_t value);

/** Blocking writes of a multiple bytes. */
void uart_writeBytes(uint8_t *value, uint8_t count);

void uart_writeUInt8(uint8_t number);
void uart_writeUInt16(uint16_t number);

void uart_writeHexDigit(uint8_t number);
void uart_writeHexUInt8(uint8_t number);


#endif
