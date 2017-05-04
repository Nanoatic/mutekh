
#include <hexo/types.h>
#include <hexo/cpu.h>

#if defined(CONFIG_LIBC_ASSERT)

ssize_t printk(const char *format, ...);

static bool_t already_failed = 0;

void
__assert_fail(const char *file,
			  uint_fast16_t line,
			  const char *func,
			  const char *expr)
{
  if (!already_failed)
    {
      already_failed = 1;
      printk("Assertion failed at %s:%u:%s(): (%s) is false\n", file, line, func, expr);
      cpu_trap();
      while (1)
	;
    }
}

#endif
