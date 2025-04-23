#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "memory/kheap.h"
#include "memory/frame_alloc.h"
#include "lib/string.h"
#include "lib/stdio.h"

// Header for memory blocks (both allocated and free)
typedef struct heap_block {
    size_t size;          // Size of the data area *excluding* this header
    bool is_free;         // True if block is free, false if allocated
    struct heap_block *next; // Pointer to the next block in the heap (physical order)
    struct heap_block *prev; // Pointer to the previous block in the heap (physical order)
    struct heap_block *next_free; // Pointer to the next free block in the free list
    struct heap_block *prev_free; // Pointer to the previous free block in the free list
} heap_block_t;

#define HEAP_HEADER_SIZE sizeof(heap_block_t)
#define HEAP_MIN_BLOCK_SIZE (HEAP_HEADER_SIZE * 2) // Minimum size to allow splitting

// Head of the free list (doubly linked)
static heap_block_t *free_list_head = NULL;
static heap_block_t *heap_start = NULL;
static heap_block_t *heap_end = NULL;

// --- Free List Management ---

static void add_to_free_list(heap_block_t *block) {
    block->is_free = true;
    block->next_free = free_list_head;
    block->prev_free = NULL;
    if (free_list_head) {
        free_list_head->prev_free = block;
    }
    free_list_head = block;
}

static void remove_from_free_list(heap_block_t *block) {
    if (block->prev_free) {
        block->prev_free->next_free = block->next_free;
    } else {
        free_list_head = block->next_free; // It was the head
    }
    if (block->next_free) {
        block->next_free->prev_free = block->prev_free;
    }
    block->is_free = false; // Mark as not free after removal
    block->next_free = NULL;
    block->prev_free = NULL;
}

// --- Heap Expansion ---

static bool expand_heap(size_t min_expand_size) {
    // Request at least a page, or more if needed
    size_t pages_needed = (min_expand_size + HEAP_HEADER_SIZE + PAGE_SIZE - 1) / PAGE_SIZE;
    if (pages_needed == 0) pages_needed = 1;

    kprintf("KHeap: Expanding heap by %zu pages\n", pages_needed);

    heap_block_t *new_block = NULL;
    for (size_t i = 0; i < pages_needed; ++i) {
        void *frame = alloc_frame();
        if (!frame) {
            kprintf("KHeap Error: Failed to allocate frame during expansion!\n");
            // If we allocated some frames but not all, we should ideally free them
            // or add what we got. For simplicity here, we fail.
            return false;
        }

        heap_block_t *current_block = (heap_block_t *)frame;
        current_block->size = PAGE_SIZE - HEAP_HEADER_SIZE;
        current_block->is_free = true;
        current_block->next = NULL; // Will be linked below or by coalesce

        if (!new_block) {
            new_block = current_block; // Keep track of the first new block
        }

        // Link the new block into the main heap structure
        if (heap_end) {
            current_block->prev = heap_end;
            heap_end->next = current_block;
        } else {
            // This is the very first block in the heap
            heap_start = current_block;
            current_block->prev = NULL;
        }
        heap_end = current_block;
    }

    if (!new_block) return false; // Should not happen if alloc_frame succeeded

    // Coalesce the first new block with the previous block if it was free
    if (new_block->prev && new_block->prev->is_free) {
        heap_block_t *prev_block = new_block->prev;
        remove_from_free_list(prev_block); // Remove old free block
        prev_block->size += new_block->size + HEAP_HEADER_SIZE;
        prev_block->next = new_block->next;
        if (new_block->next) {
            new_block->next->prev = prev_block;
        }
        if (heap_end == new_block) {
             heap_end = prev_block;
        }
        new_block = prev_block; // The merged block is now the 'new' block to add
    }

    // Add the (potentially merged) new block to the free list
    add_to_free_list(new_block);

    return true;
}

// --- Coalescing ---

static heap_block_t* coalesce(heap_block_t *block) {
    if (!block || !block->is_free) return block;

    heap_block_t *current = block;

    // Coalesce with next block if it's free
    if (current->next && current->next->is_free) {
        heap_block_t *next_block = current->next;
        kprintf("KHeap: Coalescing forward %p (%zu) with %p (%zu)\n", 
                current, current->size, next_block, next_block->size);
        remove_from_free_list(next_block); // Remove next block from free list
        current->size += next_block->size + HEAP_HEADER_SIZE;
        current->next = next_block->next;
        if (current->next) {
            current->next->prev = current;
        }
        if (heap_end == next_block) {
            heap_end = current;
        }
        // next_block is now merged into current, clear it for safety
        memset(next_block, 0, HEAP_HEADER_SIZE);
    }

    // Coalesce with previous block if it's free
    if (current->prev && current->prev->is_free) {
        heap_block_t *prev_block = current->prev;
        kprintf("KHeap: Coalescing backward %p (%zu) with %p (%zu)\n", 
                prev_block, prev_block->size, current, current->size);
        remove_from_free_list(prev_block); // Previous block is already in free list, remove it
        prev_block->size += current->size + HEAP_HEADER_SIZE;
        prev_block->next = current->next;
        if (prev_block->next) {
            prev_block->next->prev = prev_block;
        }
         if (heap_end == current) {
            heap_end = prev_block;
        }
        // current is now merged into prev_block, clear it for safety
        memset(current, 0, HEAP_HEADER_SIZE);
        current = prev_block; // The result of the coalesce is the previous block
    }

    return current; // Return the potentially larger coalesced block
}

// --- Public API ---

void kheap_init() {
    free_list_head = NULL;
    heap_start = NULL;
    heap_end = NULL;
    
    // Pre-allocate some initial pages
    expand_heap(PAGE_SIZE * 4); // Pre-allocate 16KB
    
    kprintf("KHeap: Initialized.\n");
}

void* kmalloc(size_t size) {
    if (size == 0) {
        return NULL;
    }

    // Ensure minimum allocation size and alignment (e.g., align to 8 or 16 bytes)
    // For simplicity, let's align to sizeof(void*)
    size_t alignment = sizeof(void*);
    size = (size + alignment - 1) & ~(alignment - 1);

    // Add space for the header
    size_t total_size_needed = size + HEAP_HEADER_SIZE;

    // First-fit search
    heap_block_t *current_free = free_list_head;
    heap_block_t *best_fit = NULL;

    while (current_free) {
        if (current_free->size >= size) { // Found a block large enough
            best_fit = current_free;
            break; // First fit
        }
        current_free = current_free->next_free;
    }

    // If no block found, try to expand the heap
    if (!best_fit) {
        if (!expand_heap(total_size_needed)) {
            kprintf("KHeap Error: Failed to expand heap for allocation of size %zu\n", size);
            return NULL; // Expansion failed
        }
        // Retry finding a block (the new block should be suitable)
        current_free = free_list_head;
         while (current_free) {
            if (current_free->size >= size) {
                best_fit = current_free;
                break;
            }
            current_free = current_free->next_free;
        }

        if (!best_fit) {
             kprintf("KHeap Error: Still no suitable block after expansion!\n");
             return NULL; // Should not happen if expansion succeeded
        }
    }

    // We found a suitable block (best_fit)
    remove_from_free_list(best_fit); // Remove it from the free list

    // Check if we can split the block
    if (best_fit->size >= size + HEAP_MIN_BLOCK_SIZE) {
        // Split the block
        size_t remaining_size = best_fit->size - size - HEAP_HEADER_SIZE;
        heap_block_t *new_free_block = (heap_block_t *)((uint8_t *)best_fit + HEAP_HEADER_SIZE + size);

        new_free_block->size = remaining_size;
        new_free_block->is_free = true; // Will be added to free list
        new_free_block->next = best_fit->next;
        new_free_block->prev = best_fit;

        if (new_free_block->next) {
            new_free_block->next->prev = new_free_block;
        } else {
            heap_end = new_free_block; // It's the new end of the heap
        }

        best_fit->size = size; // Adjust size of the allocated block
        best_fit->next = new_free_block;

        // Add the new smaller free block to the free list
        add_to_free_list(new_free_block);
        kprintf("KHeap: Split block %p. Allocated %zu, remaining %zu at %p\n", 
                best_fit, best_fit->size, new_free_block->size, new_free_block);

    } else {
        // Cannot split, allocate the whole block
        kprintf("KHeap: Allocated whole block %p (%zu) for size %zu\n", 
                best_fit, best_fit->size, size);
    }

    best_fit->is_free = false;

    // Return pointer to the data area (after the header)
    void *data_ptr = (void *)((uint8_t *)best_fit + HEAP_HEADER_SIZE);
    // Optionally zero the allocated memory
    memset(data_ptr, 0, best_fit->size);

    // kprintf("KHeap: kmalloc(%zu) -> %p\n", size, data_ptr);
    return data_ptr;
}

void kfree(void *ptr) {
    if (!ptr) {
        return;
    }

    // Get the header from the pointer
    heap_block_t *block = (heap_block_t *)((uint8_t *)ptr - HEAP_HEADER_SIZE);

    // Basic validation
    if (block->is_free) {
        kprintf("KHeap Warning: Double free detected for pointer %p\n", ptr);
        return;
    }
    // More robust validation would involve checking magic numbers in the header
    // or ensuring the block pointer is within the known heap range [heap_start, heap_end].

    kprintf("KHeap: kfree(%p) - block %p, size %zu\n", ptr, block, block->size);
    block->is_free = true;

    // Attempt to coalesce with neighbors
    heap_block_t *coalesced_block = coalesce(block);

    // Add the (potentially coalesced) block back to the free list
    // Check if it wasn't already added by coalesce (if coalesce returned the same block)
    if (coalesced_block == block) {
         add_to_free_list(coalesced_block);
         kprintf("KHeap: Added block %p (%zu) to free list\n", 
                 coalesced_block, coalesced_block->size);
    } else {
         // Coalesce already handled adding the merged block (prev_block)
         add_to_free_list(coalesced_block); // Ensure the final coalesced block is on the list
         kprintf("KHeap: Added coalesced block %p (%zu) to free list\n", 
                 coalesced_block, coalesced_block->size);
    }
}