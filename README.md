# ARM OS - Educational Operating System for AArch64

This is an educational operating system for the ARM AArch64 architecture, designed to run on the QEMU virt platform.

## Features

- Physical Memory Manager (PMM): Bitmap-based 4KB frame allocator
- Kernel Heap Allocator: First-fit free list allocator with coalescing
- Exception Handling: Complete exception vector table implementation
- Console I/O: PL011 UART driver
- Shell: Basic command-line interface with memory inspection commands
- Libc: Minimal implementation of essential functions

## Building

Prerequisites:
- AArch64 cross-compiler (aarch64-linux-gnu-gcc)
- QEMU with AArch64 support (qemu-system-aarch64)

To build the OS:

```bash
make
```

To run the OS in QEMU:

```bash
make qemu
```

To debug with GDB:

```bash
# Terminal 1:
make debug

# Terminal 2:
aarch64-linux-gnu-gdb build/kernel8.elf -ex "target remote localhost:1234"
```

## Shell Commands

- `help` - Display help information
- `memdump <addr> [len]` - Dump memory contents
- `peek <addr> [size]` - Read a value from memory
- `poke <addr> <value> [size]` - Write a value to memory
- `alloc <size>` - Allocate memory using the kernel heap
- `free <addr>` - Free previously allocated memory
- `pmm_info` - Display Physical Memory Manager information

## Architecture

The OS follows a modular design with the following components:

- `boot`: Boot code and kernel entry point
- `exceptions`: Exception handling mechanisms
- `lib`: Basic library functions (string, stdio, etc.)
- `memory`: Physical and virtual memory management
- `shell`: Command-line interface
- `ui`: Text User Interface (TUI)

## License

This project is provided for educational purposes.