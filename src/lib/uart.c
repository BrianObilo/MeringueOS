#include "lib/uart.h"

// QEMU virt PL011 UART registers
#define UART_BASE       0x09000000
#define UART_DR         ((volatile uint32_t*)(UART_BASE + 0x00))
#define UART_FR         ((volatile uint32_t*)(UART_BASE + 0x18))
#define UART_IBRD       ((volatile uint32_t*)(UART_BASE + 0x24))
#define UART_FBRD       ((volatile uint32_t*)(UART_BASE + 0x28))
#define UART_LCRH       ((volatile uint32_t*)(UART_BASE + 0x2C))
#define UART_CR         ((volatile uint32_t*)(UART_BASE + 0x30))
#define UART_IMSC       ((volatile uint32_t*)(UART_BASE + 0x38))

// Flag register bits
#define UART_FR_RXFE    0x10    // Receive FIFO empty
#define UART_FR_TXFF    0x20    // Transmit FIFO full

// Line control register bits
#define UART_LCRH_FEN   0x10    // Enable FIFOs
#define UART_LCRH_WLEN_8 0x60   // 8 bits word length

// Control register bits
#define UART_CR_UARTEN  0x01    // UART enable
#define UART_CR_TXE     0x100   // Transmit enable
#define UART_CR_RXE     0x200   // Receive enable

void uart_init(void) {
    // Disable UART while configuring
    *UART_CR = 0;
    
    // Configure baud rate: 115200 baud
    // Assuming 48MHz clock, divisor = 48000000/(16*115200) = 26.041666...
    // Integer part = 26
    // Fractional part = 0.041666... * 64 = 2.66... ≈ 3
    *UART_IBRD = 26;
    *UART_FBRD = 3;
    
    // Configure line control: 8 bits, no parity, 1 stop bit, FIFOs enabled
    *UART_LCRH = UART_LCRH_WLEN_8 | UART_LCRH_FEN;
    
    // Mask all interrupts initially
    *UART_IMSC = 0;
    
    // Enable UART, transmit and receive
    *UART_CR = UART_CR_UARTEN | UART_CR_TXE | UART_CR_RXE;
}

void uart_putc(char c) {
    // Wait for FIFO to have space
    while (*UART_FR & UART_FR_TXFF);
    
    // Send character
    *UART_DR = c;
    
    // If it's a newline, also send a carriage return
    if (c == '\n') {
        uart_putc('\r');
    }
}

char uart_getc(void) {
    // If receive FIFO is empty, return 0
    if (*UART_FR & UART_FR_RXFE) {
        return 0;
    }
    
    // Read and return character
    return *UART_DR;
}

int uart_is_data_available(void) {
    // Check if receive FIFO is not empty
    return !(*UART_FR & UART_FR_RXFE);
}