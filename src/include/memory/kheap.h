#ifndef KHEAP_H
#define KHEAP_H

#include <stddef.h>
#include <stdint.h>

// Initialize the kernel heap
void kheap_init(void);

// Allocate a block of memory
void *kmalloc(size_t size);

// Free a previously allocated block
void kfree(void *ptr);

#endif // KHEAP_H