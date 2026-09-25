#ifndef ROOK_UTIL_H
#define ROOK_UTIL_H

#include <rook/core.h>

/*
 * String functions
 */
extern void *memcpy(void *dst, const void *src, size_t n);
extern void *memset(void *dst, int c, size_t n);
extern void *memmove(void *dst, const void *src, size_t n);
extern int memcmp(const void *s1, const void *s2, size_t n);

extern size_t strlen(const char *s);
extern int strcmp(const char *s1, const char *s2);
extern int strncmp(const char *s1, const char *s2, size_t n);
extern char *strcpy(char *restrict dst, const char *restrict src);
extern char *strchr(const char *s, int c);
extern char *strrchr(const char *s, int c);

#endif /* ROOK_UTIL_H */
