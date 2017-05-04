
#include <hexo/error.h>
#include <hexo/context.h>

error_t
cpu_context_bootstrap(struct context_s *context)
{
  /* set context local storage base pointer */
  CPU_LOCAL_SET(__context_data_base, context->tls);

  return 0;
}

void __avr_context_entry(void);

asm(
    "__avr_context_entry:	\n"
    "	pop	r24		\n" /* entry function param */
    "	pop	r25		\n" /* entry function param */
    "	ret			\n" /* entry function address */
    );

error_t
cpu_context_init(struct context_s *context, context_entry_t *entry, void *param)
{
  context->stack_ptr = (reg_t*)((uintptr_t)context->stack_end -
                                CONFIG_HEXO_STACK_ALIGN);

  /* push entry function address and param arg */
  *--context->stack_ptr = (reg_t)CPU_AVR_HI8(entry);
  *--context->stack_ptr = (reg_t)CPU_AVR_LO8(entry);

  /* push entry function address and param arg */
  *--context->stack_ptr = (reg_t)CPU_AVR_HI8(param);
  *--context->stack_ptr = (reg_t)CPU_AVR_LO8(param);

  /* push fake entry point address */
  *--context->stack_ptr = (reg_t)CPU_AVR_HI8(__avr_context_entry);
  *--context->stack_ptr = (reg_t)CPU_AVR_LO8(__avr_context_entry);

  context->stack_ptr -= 2;	/* r28 and r29 */

  /* push default flags */
  *--context->stack_ptr = 0x00;	/* SREG with INT disabled */

  /* push tls address */
  *--context->stack_ptr = (reg_t)CPU_AVR_HI8(context->tls);
  *--context->stack_ptr = (reg_t)CPU_AVR_LO8(context->tls);

  return 0;
}

void
cpu_context_destroy(struct context_s *context)
{
}

