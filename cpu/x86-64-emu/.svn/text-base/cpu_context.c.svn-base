
#include <hexo/error.h>
#include <hexo/context.h>

error_t
cpu_context_bootstrap(struct context_s *context)
{
  /* set context local storage base pointer */
  CPU_LOCAL_SET(__context_data_base, context->tls);

  return 0;
}

void __x86_64_context_entry(void);

asm(
    "__x86_64_context_entry:	\n"
    "	pop	%rdi		\n" /* entry function param */
    "	retq			\n" /* entry function address */
    );

error_t
cpu_context_init(struct context_s *context, context_entry_t *entry, void *param)
{
  context->stack_ptr = (reg_t*)((uintptr_t)context->stack_end -
                                CONFIG_HEXO_STACK_ALIGN);

  /* push entry function address and param arg */
  *--context->stack_ptr = (reg_t)entry;
  *--context->stack_ptr = (reg_t)param;

  /* push fake entry point address */
  *--context->stack_ptr = (uintptr_t)&__x86_64_context_entry;

  /* initial frame pointer */
  *--context->stack_ptr = (reg_t)NULL;

  /* push default flags */
#if defined(CONFIG_DEBUG)
  *--context->stack_ptr = 0x00040246;	/* EFLAGS with alignment chk */
#else
  *--context->stack_ptr = 0x00000246;	/* EFLAGS */
#endif

  /* push tls address */
  *--context->stack_ptr = (reg_t)context->tls;

  return 0;
}

void
cpu_context_destroy(struct context_s *context)
{
}

