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

/** Blocking write of a multiple bytes. */
void uart_writeBytes(uint8_t *value, uint8_t count);

/** Blocking write of a null terminated string. */
void uart_writeString(const uint8_t* text);

/** Blocking write of a 8-bit unsigned integer. */
void uart_writeUInt8(uint8_t number);

/** Blocking write of a 16-bit unsigned integer. */
void uart_writeUInt16(uint16_t number);

/** Blocking write of a hex digit. */
void uart_writeHexDigit(uint8_t value);

/** Blocking write of a byte as two hex digits. */
void uart_writeHexUInt8(uint8_t value);

#endif
