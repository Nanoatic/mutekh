#ifndef MUTEK_PRINTK_H_
#define MUTEK_PRINTK_H_

#if defined(CONFIG_MUTEK_PRINTK)

#include <stdarg.h>
#include <libc/formatter.h>

void printk_set_output(printf_output_func_t *f, void *ctx);
ssize_t printk(const char *format, ...);
inline ssize_t vprintk(const char *format, va_list ap);
void hexdumpk(uintptr_t address, const void *data, size_t len);
void writek(const char *data, size_t len);

#else /* no printk */

static inline
void printk_set_output(printf_output_func_t *f, void *ctx)
{}

static inline
ssize_t printk(const char *format, ...)
{
	return 0;
}

static inline
void hexdumpk(uintptr_t address, const void *data, size_t len)
{
	return 0;
}

static inline
inline ssize_t vprintk(const char *format, va_list ap)
{
	return 0;
}

static inline
void writek(const char *data, size_t len)
{
}

#endif /* printk */

#define PRINTK_RET(val, ...)			\
do {						\
	printk(__VA_ARGS__);			\
	return (val);				\
} while (0)

#endif

