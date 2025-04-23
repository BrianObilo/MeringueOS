#ifndef KERNEL_H
#define KERNEL_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// External symbols defined by the linker script
extern char _kernel_start;
extern char _text_end;
extern char _rodata_start;
extern char _rodata_end;
extern char _rodata_load;
extern char _data_start;
extern char _data_end;
extern char _data_load;
extern char _bss_start;
extern char _bss_end;
extern char _stack_top;
extern char _stack_bottom;
extern char _pmm_bitmap_start;
extern char _pmm_bitmap_end;
extern char _kernel_end;

// Boot parameters structure (will be populated by bootloader)
typedef struct {
    void* uefi_memory_map;      // Pointer to the UEFI memory map
    uint64_t map_size;          // Total size of the map in bytes
    uint64_t map_desc_size;     // Size of each EFI_MEMORY_DESCRIPTOR
    uint32_t map_desc_version;  // Version of the descriptor structure
    uint64_t kernel_phys_start; // Physical start address of the kernel image
    uint64_t kernel_phys_end;   // Physical end address of the kernel image
    // Add other necessary fields like framebuffer info, command line, etc.
} KERNEL_BOOT_PARAMS;

// Main kernel entry point
void kernel_main(KERNEL_BOOT_PARAMS *params);

// Debug function called from boot_debug
void boot_debug_copy(void *dest, void *src, size_t size);

#endif // KERNEL_H