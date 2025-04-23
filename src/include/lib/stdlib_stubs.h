#ifndef STDLIB_STUBS_H
#define STDLIB_STUBS_H

#include <stddef.h>
#include <stdint.h>

// Constants for overflow detection only if not already defined
#ifndef ULONG_MAX
#define ULONG_MAX ((unsigned long)~0UL)
#endif

#ifndef UINT64_MAX
#define UINT64_MAX ((uint64_t)~0ULL)
#endif

// String conversion functions
unsigned long long simple_strtoull(const char *nptr, char **endptr, int base);
unsigned long simple_strtoul(const char *nptr, char **endptr, int base);

// Abort function
void abort(void) __attribute__((noreturn));

#endif // STDLIB_STUBS_H