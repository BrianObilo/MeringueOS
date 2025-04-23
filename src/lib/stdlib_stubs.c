#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "lib/stdlib_stubs.h"
#include "lib/stdio.h"

// Simple implementation supporting base 10 and base 16 (0x prefix)
unsigned long long simple_strtoull(const char *nptr, char **endptr, int base) {
    unsigned long long result = 0;
    bool negative = false; // Ignored for unsigned, but good practice
    const char *orig_nptr = nptr;

    // Skip leading whitespace
    while (*nptr == ' ' || *nptr == '\t' || *nptr == '\n' ||
           *nptr == '\r' || *nptr == '\f' || *nptr == '\v') {
        nptr++;
    }

    // Handle optional sign (ignored for unsigned)
    if (*nptr == '+') {
        nptr++;
    } else if (*nptr == '-') {
        negative = true; // Keep track even if ignored
        nptr++;
    }

    // Determine base
    if ((base == 0 || base == 16) && *nptr == '0' && (nptr[1] == 'x' || nptr[1] == 'X')) {
        base = 16;
        nptr += 2;
    } else if (base == 0) {
        base = 10;
    }

    if (base < 2 || base > 16) { // Simplified: only support up to base 16
        if (endptr) *endptr = (char *)orig_nptr;
        return 0; // Invalid base
    }

    unsigned long long cutoff = UINT64_MAX / base;
    unsigned long long cutlim = UINT64_MAX % base;

    while (true) {
        int digit;
        char c = *nptr;

        if (c >= '0' && c <= '9') {
            digit = c - '0';
        } else if (c >= 'a' && c <= 'z') {
            digit = c - 'a' + 10;
        } else if (c >= 'A' && c <= 'Z') {
            digit = c - 'A' + 10;
        } else {
            break; // Invalid character
        }

        if (digit >= base) {
            break; // Invalid digit for base
        }

        // Basic overflow check
        if (result > cutoff || (result == cutoff && (unsigned long long)digit > cutlim)) {
            result = UINT64_MAX; // Indicate overflow
            // Consume remaining valid digits for endptr
            while (true) {
                 c = *(++nptr);
                 if (c >= '0' && c <= '9') digit = c - '0';
                 else if (c >= 'a' && c <= 'z') digit = c - 'a' + 10;
                 else if (c >= 'A' && c <= 'Z') digit = c - 'A' + 10;
                 else break;
                 if (digit >= base) break;
            }
            break; // Exit main loop after overflow
        }

        result = result * base + digit;
        nptr++;
    }

    if (endptr) {
        *endptr = (char *)nptr;
    }

    // Apply sign if needed (though result is unsigned)
    // if (negative && result != UINT64_MAX) {
    //     // Standard strtoull doesn't negate, but strtoll would
    // }

    return result;
}

// Simple wrapper for unsigned long
unsigned long simple_strtoul(const char *nptr, char **endptr, int base) {
    unsigned long long val = simple_strtoull(nptr, endptr, base);
    // Check for overflow against ULONG_MAX if needed, though simple_strtoull already checks against UINT64_MAX
    if (val > ULONG_MAX) {
        return ULONG_MAX;
    }
    return (unsigned long)val;
}

// Basic abort function - enters infinite loop
void abort(void) {
    kprintf("ABORT CALLED!\n");
    // Disable interrupts?
    while(1) {}
}