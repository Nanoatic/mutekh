
#include <hexo/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <mutek/mem_alloc.h>
#include <mutek/printk.h>
#include <hexo/cpu.h>
#include <hexo/lock.h>

void __cyg_profile_func_enter (void *this_fn,
			       void *call_site);

void __cyg_profile_func_exit  (void *this_fn,
			       void *call_site);

static bool_t mutek_instrument_trace_flag = 0;

#ifdef CONFIG_MUTEK_MEMALLOC_GUARD_INSTRUMENT
static bool_t mutek_instrument_memalloc_guard = 0;
bool_t mem_guard_check(void) __attribute__((unused));
#endif

static lock_t mutek_instrument_lock = LOCK_INITIALIZER;

__attribute__ ((no_instrument_function))
void __cyg_profile_func_enter (void *this_fn,
			       void *call_site)
{
  if (mutek_instrument_trace_flag)
    {
      mutek_instrument_trace_flag = 0;
      lock_spin(&mutek_instrument_lock);
      printk(">>>>> cpu(%i) [f:%p]   Called from [f:%p]\n", cpu_id(), this_fn, call_site);
      lock_release(&mutek_instrument_lock);
      mutek_instrument_trace_flag = 1;
    }

#ifdef CONFIG_MUTEK_MEMALLOC_GUARD_INSTRUMENT
  if (mutek_instrument_memalloc_guard && mem_guard_check())
    {
      printk("Memory guard check failed on function call [f:%p] called from [f:%p]",
	     this_fn, call_site);
      abort();
    }
#endif
}

__attribute__ ((no_instrument_function))
void __cyg_profile_func_exit  (void *this_fn,
			       void *call_site)
{
#ifdef CONFIG_MUTEK_MEMALLOC_GUARD_INSTRUMENT
  if (mutek_instrument_memalloc_guard && mem_guard_check())
    {
      printk("Memory guard check failed on function return [f:%p] called from [f:%p]",
	     this_fn, call_site);
      abort();
    }
#endif

  if (mutek_instrument_trace_flag)
    {
      mutek_instrument_trace_flag = 0;
      lock_spin(&mutek_instrument_lock);
      printk("  <<< [f:%p]   Called from [f:%p]\n", this_fn, call_site);
      lock_release(&mutek_instrument_lock);
      mutek_instrument_trace_flag = 1;
    }
}

bool_t
mutek_instrument_trace(bool_t state)
{
	bool_t old = mutek_instrument_trace_flag;
	mutek_instrument_trace_flag = state;
	return old;
}

void
mutek_instrument_alloc_guard(bool_t state)
{
#ifdef CONFIG_MUTEK_MEMALLOC_GUARD_INSTRUMENT
   mutek_instrument_memalloc_guard = state;
#endif
}

