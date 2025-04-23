#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "ui/tui.h"
#include "lib/stdio.h"

// This is a placeholder implementation for the TUI
// In the full version, this would integrate with a framebuffer

int tui_init(void) {
    kprintf("TUI: Initializing...\n");
    kprintf("TUI: Using fallback console mode (no framebuffer)\n");
    
    // Return success for now - we'll just use UART output
    return 0;
}

void tui_write(const char *data, size_t len) {
    // For now, just forward to UART output via kprintf
    for (size_t i = 0; i < len; i++) {
        kprintf("%c", data[i]);
    }
}