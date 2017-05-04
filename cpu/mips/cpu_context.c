
#include <stdlib.h>
#include <hexo/error.h>
#include <hexo/context.h>
#include <hexo/interrupt.h>

error_t
cpu_context_bootstrap(struct context_s *context)
{
    /* set context local storage base pointer */
    CPU_LOCAL_SET(__context_data_base, context->tls);

    return 0;
}


/* fake context entry point, pop entry function param and entry
   function address from stack and perform jump to real entry
   function.  We need to do this to pass an argument to the context
   entry function through register. */
void __mips_context_entry(void);

asm(
        ".set push                 \n"
        ".set noreorder            \n"
        ".set noat                 \n"
	".type __mips_context_entry, @function \n"
        "__mips_context_entry:     \n"
        "   lw      $4,     0($sp) \n" /* entry function param */
        "   lw      $1,     4($sp) \n" /* entry function address */
        "   jr      $1             \n"
        "   addiu   $sp,    2*4    \n"
	".size __mips_context_entry, .-__mips_context_entry \n"
        ".set pop                  \n"
   );



/* context init function */
error_t
cpu_context_init(struct context_s *context, context_entry_t *entry, void *param)
{
    /* Mips ABI only requires 4 free words in the stack. */
    context->stack_ptr = (reg_t*)((uintptr_t)context->stack_end -
                                  __MIN(sizeof (reg_t) * 4, CONFIG_HEXO_STACK_ALIGN));

    /* push entry function address and param arg */
    *--context->stack_ptr = (uintptr_t)entry;
    *--context->stack_ptr = (uintptr_t)param;

    /* fake entry point */
    *--context->stack_ptr = (uintptr_t)&__mips_context_entry;

    /* frame pointer */
    *--context->stack_ptr = 0;

    /* status register, interrupts are disabled */
#ifdef CONFIG_COMPILE_SOFTFLOAT
    *--context->stack_ptr = 0x0000ff00;
#else
    *--context->stack_ptr = 0x2000ff00;
#endif

    /* context local storage address */
    *--context->stack_ptr = (uintptr_t)context->tls;

    return 0;
}



void
cpu_context_destroy(struct context_s *context)
{
#if 0
    reg_t        *stack = (reg_t*)context->stack_ptr;
#endif
}

#if defined(CONFIG_HEXO_USERMODE)
void __attribute__((noreturn))
cpu_context_set_user(uintptr_t kstack, uintptr_t ustack,
        user_entry_t *entry, void *param)
{
    cpu_interrupt_disable();

    CONTEXT_LOCAL_SET(context_kstack, kstack);

    asm volatile (
            ".set push                     \n"
            ".set noat                     \n"
            /* set stack */
            "   move    $sp,    %[ustack]  \n"
            /* set arg */
            "   move    $4,     %[param]   \n"
            "   addiu   $sp,    -4*4       \n"
            "   move    $t9,    %[entry]   \n"
# if __mips >= 32
            /* entrypoint goes into epc */
            "   mtc0    $t9,    $14        \n"
            "   mfc0    $1,     $12        \n"
            /* status.ksu=10; status.exl=1; status.ie=1 */
#if 0
            /* if supervisor mode is not implemented,
             * ksu.0=R0="3rd bit of r_status" is ignored.
             * No need to reset it then.
             */
            "   andi    $1,     0xFFFFFFF7 \n"
#endif
            "   ori     $1,     0x00000013 \n"
            "   mtc0    $1,     $12        \n"
            ".set noreorder                \n"
            /* switch to user mode at @EPC with interrupt enabled */
            "   eret                       \n"
# else
            /* set previous state as user */
            "   mfc0    $1,     $12        \n"
            "   ori     $1,     0xC        \n"
            "   mtc0    $1,     $12        \n"
            /* "restore" user mode and jump */
            ".set noreorder                \n"
            "   jr      $t9                \n"
            "   rfe                        \n"
# endif
            ".set pop                      \n"
            :
            : [ustack]  "r" (ustack)
            , [entry]   "r" (entry)
            , [param]   "r" (param)
            );
}
#endif

// Local Variables:
// tab-width: 4;
// c-basic-offset: 4;
// c-file-offsets:((innamespace . 0)(inline-open . 0));
// indent-tabs-mode: nil;
// End:
//
// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

