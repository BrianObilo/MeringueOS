#ifndef FRAME_ALLOC_H
#define FRAME_ALLOC_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "kernel.h"

// Define page size (typically 4KB for AArch64)
#define PAGE_SIZE 4096
#define PAGE_SHIFT 12

// Define the RAM region for QEMU virt machine
#define PMM_RAM_BASE 0x40000000

// Note: Linker symbols are now included from kernel.h

// Initialize the physical memory manager
void frame_alloc_init(const KERNEL_BOOT_PARAMS *params);

// Allocate a physical frame, returns NULL if no free frames
void* alloc_frame(void);

// Free a previously allocated physical frame
void free_frame(void *frame);

// Get information about memory
uint64_t pmm_get_total_memory(void);
uint64_t pmm_get_free_memory(void);
uint64_t pmm_get_highest_usable_address(void);

#endif // FRAME_ALLOC_H