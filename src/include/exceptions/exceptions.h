#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdint.h>
#include <stdbool.h>

// Structure to mimic the saved context on the stack
// Order must match the stp/str sequence in save_context
typedef struct {
    uint64_t regs[31];   // x0-x30
    uint64_t spsr_el1;   // Saved Program Status Register
    uint64_t elr_el1;    // Exception Link Register
    uint64_t sp_el0;     // Stack Pointer for EL0
} saved_registers_t;

// C-level exception handlers
void handle_sync_exception(saved_registers_t *context);
void handle_irq(saved_registers_t *context);
void handle_fiq(saved_registers_t *context);
void handle_serror(saved_registers_t *context);

// Panic function - halts the system with an error message
void panic(const char *message);

#endif // EXCEPTIONS_H