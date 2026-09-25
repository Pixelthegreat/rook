#include <rook/core.h>
#include <rook/kernel/util.h>

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

/* better strncpy */
extern void rk_strncpy(char *dst, size_t size, const char *src) {

	size_t len = strlen(src);
	if (len > size-1) len = size-1;

	memcpy(dst, src, len);
	dst[len] = 0;
}

/* integer size ids */
enum {
	HH = 0, H, I, L, LL, P, Z,
};

struct buffer_info {
	char *data;
	size_t position;
	size_t size;
};

/* add character to buffer */
static void add_char(struct buffer_info *buffer, char c) {

	if (buffer->position >= buffer->size-1)
		return;
	buffer->data[buffer->position++] = c;
	buffer->data[buffer->position] = 0;
}

/* format specifier handlers */
static void handler_c(struct buffer_info *buffer, va_list args, int modifier, int number);
static void handler_d(struct buffer_info *buffer, va_list args, int modifier, int number);
static void handler_s(struct buffer_info *buffer, va_list args, int modifier, int number);
static void handler_p(struct buffer_info *buffer, va_list args, int modifier, int number);
static void handler_u(struct buffer_info *buffer, va_list args, int modifier, int number);
static void handler_x(struct buffer_info *buffer, va_list args, int modifier, int number);

static void (*handlers[256])(struct buffer_info *, va_list, int, int) = {
	['c'] = handler_c,
	['d'] = handler_d,
	['i'] = handler_d,
	['s'] = handler_s,
	['p'] = handler_p,
	['u'] = handler_u,
	['x'] = handler_x,
};

/* character */
static void handler_c(struct buffer_info *buffer, va_list args, int modifier, int number) {

	char ch = (char)va_arg(args, int);
	add_char(buffer, ch);
}

/* integer */
static void handler_d(struct buffer_info *buffer, va_list args, int modifier, int number) {

	long long v;
	switch (modifier) {
		case HH:
		case H:
		case I:
			v = (long long)(va_arg(args, int)); break;
		case L: v = (long long)(va_arg(args, long)); break;
		case LL: v = va_arg(args, long long); break;
		default: v = 0; break;
	}
	if (!v) {
		add_char(buffer, '0');
		return;
	}
	if (v < 0) {
		add_char(buffer, '-');
		v = -v;
	}
	char mbuf[64];
	size_t n = 0;
	while (v) {
		mbuf[n++] = (char)(v % 10) + '0';
		v /= 10;
	}
	while (n--) add_char(buffer, mbuf[n]);
}

/* string */
static void handler_s(struct buffer_info *buffer, va_list args, int modifier, int number) {

	const char *s = va_arg(args, const char *);
	if (!s) return;

	while (*s) add_char(buffer, *s++);
}

/* pointer */
static void handler_p(struct buffer_info *buffer, va_list args, int modifier, int number) {

	add_char(buffer, '0');
	add_char(buffer, 'x');

	return handler_x(buffer, args, P, number);
}

/* unsigned integer */
static void handler_u(struct buffer_info *buffer, va_list args, int modifier, int number) {

	unsigned long long v;
	switch (modifier) {
		case HH:
		case H:
		case I:
			v = (unsigned long long)(va_arg(args, unsigned int)); break;
		case L: v = (unsigned long long)(va_arg(args, unsigned long)); break;
		case LL: v = va_arg(args, unsigned long long); break;
		case Z: v = (unsigned long long)va_arg(args, size_t); break;
		default: v = 0;
	}
	if (!v) {
		add_char(buffer, '0');
		return;
	}
	char mbuf[64];
	size_t n = 0;
	while (v) {
		mbuf[n++] = (char)(v % 10) + '0';
		v /= 10;
	}
	while (n--) add_char(buffer, mbuf[n]);
}

/* unsigned hexadecimal integer */
static void handler_x(struct buffer_info *buffer, va_list args, int modifier, int number) {

	const char *hex_chars = "0123456789abcdef";

	unsigned long long v;
	switch (modifier) {
		case HH:
		case H:
		case I:
			v = (unsigned long long)(va_arg(args, unsigned int)); break;
		case L: v = (unsigned long long)(va_arg(args, unsigned long)); break;
		case LL: v = va_arg(args, unsigned long long); break;
		case Z: v = (unsigned long long)va_arg(args, size_t); break;
		default: v = 0;
	}
	if (!v) {
		add_char(buffer, '0');
		return;
	}
	char mbuf[64];
	size_t n = 0;
	size_t snum = (size_t)number;
	if (!snum) snum = 1;
	while (v) {
		mbuf[n++] = hex_chars[v & 0xf];
		v >>= 4;
	}
	if (n < snum) {
		snum -= n;
		while (snum--) add_char(buffer, '0');
	}
	while (n--) add_char(buffer, mbuf[n]);
}

/* print formatted string with argument list */
extern int vsnprintf(char *buffer, size_t size, const char *format, va_list args) {

	struct buffer_info buffer_info = {
		.data = buffer,
		.position = 0,
		.size = size,
	};
	while (*format) {

		char c = *format++;

		/* format character */
		if (c == '%') {

			c = *format++;

			/* number */
			int number = 0;
			while (c >= '0' && c <= '9') {

				number = (number * 10) + ((int)c - '0');
				c = *format++;
			}

			/* size modifier */
			int modifier = I;
			while (c == 'h') {

				if (modifier == I) modifier = H;
				else if (modifier == H) modifier = HH;
				c = *format++;
			}
			while (c == 'l') {

				if (modifier == I) modifier = L;
				else if (modifier == L) modifier = LL;
				c = *format++;
			}
			while (c == 'z') {

				modifier = Z;
				c = *format++;
			}

			/* format specifier */
			if (handlers[c]) handlers[c](
					&buffer_info,
					args,
					modifier,
					number
			);
			else add_char(&buffer_info, c);
		}
		else add_char(&buffer_info, c);
	}
	return (int)buffer_info.position;
}

/* print formatted string */
extern int snprintf(char *buffer, size_t size, const char *format, ...) {

	va_list args;
	va_start(args, format);
	int n = vsnprintf(buffer, size, format, args);
	va_end(args);

	return n;
}
