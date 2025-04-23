#ifndef STDIO_H
#define STDIO_H

#include <stdarg.h>
#include <stddef.h>

// Basic printf-like function
int kprintf(const char *format, ...);

// Basic character input (will be implemented with UART)
char kgetc(void);

// Simple blocking character read
char kgetc_blocking(void);

#endif // STDIO_H