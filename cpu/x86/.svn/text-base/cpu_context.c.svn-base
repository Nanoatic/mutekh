
#include <hexo/error.h>
#include <hexo/local.h>
#include <hexo/context.h>
#include <hexo/segment.h>
#include <hexo/interrupt.h>

#include <cpu/hexo/pmode.h>

#ifdef CONFIG_HEXO_USERMODE
/* current context local storage segment descriptor, used to restore
   segment register when returning from user mode */
CPU_LOCAL cpu_x86_segsel_t *cpu_tls_seg = 0;
#endif

/* context local storage segment descriptor */
CONTEXT_LOCAL cpu_x86_segsel_t ctx_tls_seg;

error_t
cpu_context_bootstrap(struct context_s *context)
{
  cpu_x86_segsel_t	tls_seg;

  /* get a new segment descriptor for tls */
  if (!(tls_seg = cpu_x86_segment_alloc((uintptr_t)context->tls,
					arch_contextdata_size(),
					CPU_X86_SEG_DATA_UP_RW)))
    return -ENOMEM;

  /* load current tls segment */
  cpu_x86_dataseggs_use(tls_seg, 0);

  CONTEXT_LOCAL_SET(__context_data_base, context->tls);
  CONTEXT_LOCAL_SET(ctx_tls_seg, tls_seg);

  return 0;
}

error_t
cpu_context_init(struct context_s *context, context_entry_t *entry, void *param)
{
  cpu_x86_segsel_t	tls_seg;

  /* get a new segment descriptor for tls */
  if (!(tls_seg = cpu_x86_segment_alloc((uintptr_t)context->tls,
					arch_contextdata_size(),
					CPU_X86_SEG_DATA_UP_RW)))
    return -ENOMEM;

  CONTEXT_LOCAL_TLS_SET(context->tls, __context_data_base, context->tls);
  CONTEXT_LOCAL_TLS_SET(context->tls, ctx_tls_seg, tls_seg);

  context->stack_ptr = (reg_t*)((uintptr_t)context->stack_end -
                                CONFIG_HEXO_STACK_ALIGN);

  /* push param */
  *--context->stack_ptr = (uintptr_t)param;

  /* push context entry function return pointer */
  *--context->stack_ptr = (uintptr_t)0;

  /* push execution pointer */
  *--context->stack_ptr = (uintptr_t)entry;	/* EIP */

  /* initial frame pointer */
  *--context->stack_ptr = (reg_t)NULL;

  /* push default flags, interrupts are disabled */
  *--context->stack_ptr = 0x00000046;	/* EFLAGS */

  // FIXME MMU

  /* push tls segment index */
  *--context->stack_ptr = tls_seg << 3;	/* GS */

  return 0;
}

void
cpu_context_destroy(struct context_s *context)
{
  /* free tls segment descriptor */
  cpu_x86_segdesc_free(CONTEXT_LOCAL_TLS_GET(context->tls, ctx_tls_seg));
}

# if defined(CONFIG_HEXO_USERMODE)

void __attribute__((noreturn))
cpu_context_set_user(uintptr_t kstack, uintptr_t ustack,
		     user_entry_t *entry, void *param)
{
  reg_t *us = (reg_t*)ustack;

  cpu_interrupt_disable();

  /* push param */
  *--us = param;
  /* push fake return address */
  *--us = 0;

  CPU_LOCAL_ADDR(cpu_tss)->esp0 = kstack;
#  ifdef CONFIG_CPU_X86_SYSENTER
  cpu_x86_write_msr(SYSENTER_ESP_MSR, kstack);
#  endif

  asm volatile (
		"movl	%0, %%ds	\n"
		"movl	%0, %%es	\n"
		"movl	%0, %%fs	\n"
		"movl	%0, %%gs	\n"
#  ifdef CONFIG_CPU_X86_SYSENTER
		"pushl	%3		\n" /* EFLAGS */
		"popf			\n"
		"sysexit		\n"
		:
		: "r" ((ARCH_GDT_USER_DATA_INDEX << 3) | 3)
		, "c" (us)
		, "d" (entry)
#  else
		"pushl %0		\n" /* SS */
		"pushl %1		\n" /* ESP */
		"pushl %4		\n" /* EFLAGS */
		"pushl %2		\n" /* CS */
		"pushl %3		\n" /* EIP */
		"iret			\n"
		:
		: "r" ((ARCH_GDT_USER_DATA_INDEX << 3) | 3)
		, "r" (us)
		, "r" ((ARCH_GDT_USER_CODE_INDEX << 3) | 3)
		, "r" (entry)
#  endif
		, "i" (0x0)	/* FIXME enable interrupts */
		);

  while (1)
    ;
}

#endif

