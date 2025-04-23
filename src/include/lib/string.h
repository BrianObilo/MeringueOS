#ifndef STRING_H
#define STRING_H

#include <stddef.h>

void* memset(void *s, int c, size_t n);
void* memcpy(void *dest, const void *src, size_t n);
size_t strlen(const char *s);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);
char* strtok(char *str, const char *delim);
size_t strspn(const char *s, const char *accept);
char* strpbrk(const char *s, const char *reject);

#endif // STRING_H