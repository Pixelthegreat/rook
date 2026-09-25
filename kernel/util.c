#include <rook/core.h>
#include <rook/util.h>

/* copy memory */
extern void *memcpy(void *dst, const void *src, size_t n) {

	for (size_t i = 0; i < n; i++)
		*(uint8_t *)dst++ = *(uint8_t *)src++;
	return dst;
}

/* set memory */
extern void *memset(void *dst, int c, size_t n) {

	for (size_t i = 0; i < n; i++)
		*(uint8_t *)dst++ = (uint8_t)c;
	return dst;
}

/* move memory */
extern void *memmove(void *dst, const void *src, size_t n) {

	for (size_t i = 0; i < n; i++) {

		if (src > dst) *(uint8_t *)(dst+i) = *(uint8_t *)(src+i);
		else *(uint8_t *)(dst+n-i-1) = *(uint8_t *)(src+n-i-1);
	}
	return dst;
}

/* compare memory */
extern int memcmp(const void *s1, const void *s2, size_t n) {

	for (size_t i = 0; i < n; i++) {

		int a = (int)*(uint8_t *)s1++, b = (int)*(uint8_t *)s2++;
		if (a != b) return a - b;
	}
	return 0;
}

/* get string length */
extern size_t strlen(const char *s) {

	size_t n = 0;
	while (*s++) n++;
	return n;
}

/* compare strings */
extern int strcmp(const char *s1, const char *s2) {

	for (; *s1 == *s2 && *s1; s1++, s2++);
	return *(unsigned char *)s1 - *(unsigned char *)s2;
}

/* compare strings to point */
extern int strncmp(const char *a, const char *b, size_t n) {

	if (!n--) return 0;
	for (; *a && *b && n && *a == *b; a++, b++, n--);
	return *(unsigned char *)a - *(unsigned char *)b;
}

/* copy string */
extern char *strcpy(char *restrict dst, const char *restrict src) {

	while (*src) *dst++ = *src++;
	*dst = 0;

	return dst;
}

/* find first instance of character in string */
extern char *strchr(const char *s, int c) {

	do {
		if (*s == c) return (char *)s;
		s++;
	} while (*s);

	return NULL;
}

/* find last instance of character in string */
extern char *strrchr(const char *s, int c) {

	const char *p = NULL;
	do {
		if (*s == c) p = s;
		s++;
	} while (*s);

	return (char *)p;
}
