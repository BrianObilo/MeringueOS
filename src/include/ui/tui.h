#ifndef TUI_H
#define TUI_H

// Initialize the text user interface
int tui_init(void);

// Write data to the TUI (terminal)
void tui_write(const char *data, size_t len);

#endif // TUI_H