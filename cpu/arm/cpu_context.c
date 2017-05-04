
#include <hexo/error.h>
#include <hexo/context.h>
#include <hexo/interrupt.h>

#if !defined(CONFIG_CPU_ARM_TLS_IN_C15)
CPU_LOCAL void *__context_data_base;
#endif

error_t
cpu_context_bootstrap(struct context_s *context)
{
  /* set context local storage register base pointer */
#if defined(CONFIG_CPU_ARM_TLS_IN_C15)
    THUMB_TMP_VAR;
    
	asm volatile (
        THUMB_TO_ARM
        "mcr p15,0,%0,c13,c0,4 \n\t"
        ARM_TO_THUMB
        /*:*/ THUMB_OUT(:)
        : "r" (context->tls));
#else
	__context_data_base = context->tls;
#endif

	return 0;
}



/* fake context entry point, pop entry function param and entry function
   address from stack and perform jump to real entry function.  We
   need to do this to pass an argument to the context entry function. */
void __arm_context_entry(void);

asm(
    ".type __arm_context_entry, %function \n"
#if defined(__thumb__)
	".code	16                  \n"
	".thumb_func            	\n"
#endif
    "__arm_context_entry:		\n"
    "	pop {r0, pc}			\n"
    ".size __arm_context_entry, .-__arm_context_entry \n"
    );



/* context init function */

error_t
cpu_context_init(struct context_s *context, context_entry_t *entry, void *param)
{
  context->stack_ptr = (reg_t*)((uintptr_t)context->stack_end -
                                CONFIG_HEXO_STACK_ALIGN);

  /* push entry function address and param arg */
  *--context->stack_ptr = (uintptr_t)entry;
  *--context->stack_ptr = (uintptr_t)param;

  /* fake entry point */
  *--context->stack_ptr = (uintptr_t)&__arm_context_entry;

  /* frame pointer */
  *--context->stack_ptr = 0;

  /* super32, interrupts are disabled */
  *--context->stack_ptr = 0x000000d3;

  /* context local storage address */
  *--context->stack_ptr = (uintptr_t)context->tls;

  return 0;
}



void
cpu_context_destroy(struct context_s *context)
{
#if 0
  reg_t		*stack = (reg_t*)context->stack_ptr;
#endif
}

