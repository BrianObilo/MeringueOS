#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "lib/string.h"

void* memset(void *s, int c, size_t n) {
    unsigned char *p = (unsigned char *)s;
    unsigned char uc = (unsigned char)c;
    while (n-- > 0) {
        *p++ = uc;
    }
    return s;
}

void* memcpy(void *dest, const void *src, size_t n) {
    unsigned char *d = (unsigned char *)dest;
    const unsigned char *s = (const unsigned char *)src;
    while (n-- > 0) {
        *d++ = *s++;
    }
    return dest;
}

size_t strlen(const char *s) {
    size_t len = 0;
    while (*s++) {
        len++;
    }
    return len;
}

int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
    while (n > 0 && *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
        n--;
    }
    if (n == 0) {
        return 0;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

// Note: strtok is not re-entrant due to the static pointer!
static char *strtok_last;

char* strtok(char *str, const char *delim) {
    char *token;

    if (str == NULL) {
        str = strtok_last;
    }
    if (str == NULL) {
        return NULL; // No more tokens
    }

    // Skip leading delimiters
    str += strspn(str, delim);
    if (*str == '\0') {
        strtok_last = NULL;
        return NULL;
    }

    // Find the end of the token
    token = str;
    str = strpbrk(token, delim);
    if (str == NULL) {
        // This token is the last one
        strtok_last = NULL;
    } else {
        // Terminate the token and save the position for the next call
        *str = '\0';
        strtok_last = str + 1;
    }

    return token;
}

// Helper for strtok: length of initial segment consisting of accept characters
size_t strspn(const char *s, const char *accept) {
    size_t count = 0;
    const char *a;
    bool found;

    while (*s) {
        found = false;
        for (a = accept; *a; a++) {
            if (*s == *a) {
                found = true;
                break;
            }
        }
        if (!found) {
            break;
        }
        count++;
        s++;
    }
    return count;
}

// Helper for strtok: find first occurrence of reject character
char* strpbrk(const char *s, const char *reject) {
    const char *r;
    while (*s) {
        for (r = reject; *r; r++) {
            if (*s == *r) {
                return (char *)s;
            }
        }
        s++;
    }
    return NULL;
}