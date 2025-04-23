#ifndef UART_H
#define UART_H

#include <stdint.h>

// Initialize UART (PL011 for QEMU virt)
void uart_init(void);

// Send a character
void uart_putc(char c);

// Get a character (non-blocking)
char uart_getc(void);

// Check if data is available to read
int uart_is_data_available(void);

#endif // UART_H