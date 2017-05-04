#include <stdio.h>
#include <stddef.h>
#include <hexo/endian.h>

#include <hexo/error.h>
#include <hexo/types.h>
#include <hexo/context.h>
#include <hexo/interrupt.h>
#include <hexo/cpu.h>

#include <dsrl/dsrl-private.h>

/*
 * Wait syscall
 */
static inline dsrl_task_t* context_to_dsrl_task(struct sched_context_s *ctx)
{
    uintptr_t p = (uintptr_t)ctx;
    p -= offsetof(dsrl_task_t, context);
    return (dsrl_task_t*)p;
}

#define DSRL_SCHED_WAIT(n) void (n)(volatile uint32_t *addr, uint32_t val)
typedef DSRL_SCHED_WAIT(sched_wait_fcn_t);

#define endian_cpu32(x) (x)

#define DECLARE_WAIT(endianness, name, cmp)                                              \
    static SCHED_CANDIDATE_FCN(wait_##name##endianness##_f)                              \
    {                                                                                    \
        dsrl_task_t* task = context_to_dsrl_task(sched_ctx);                             \
                                                                                         \
        cpu_dcache_invld((void*)task->wait_addr);                                        \
        return (endian_##endianness##32(*(task->wait_addr)) cmp task->wait_val);         \
    }                                                                                    \
                                                                                         \
    static DSRL_SCHED_WAIT(dsrl_sched_wait_##name##_##endianness)                        \
    {                                                                                    \
        dsrl_task_t* current = context_to_dsrl_task(sched_get_current());                \
        current->wait_val = val;                                                         \
        current->wait_addr = addr;                                                       \
        while (!wait_##name##endianness##_f(&current->context)) {                        \
            cpu_interrupt_disable();                                                     \
            sched_context_candidate_fcn(&current->context, wait_##name##endianness##_f); \
            sched_context_switch();                                                      \
            sched_context_candidate_fcn(&current->context, NULL);                        \
            cpu_interrupt_enable();                                                      \
        }                                                                                \
    }

DECLARE_WAIT(le, eq, ==)
DECLARE_WAIT(le, ne, !=)
DECLARE_WAIT(le, le, <=)
DECLARE_WAIT(le, ge, >=)
DECLARE_WAIT(le, lt, <)
DECLARE_WAIT(le, gt, >)

DECLARE_WAIT(be, eq, ==)
DECLARE_WAIT(be, ne, !=)
DECLARE_WAIT(be, le, <=)
DECLARE_WAIT(be, ge, >=)
DECLARE_WAIT(be, lt, <)
DECLARE_WAIT(be, gt, >)

DECLARE_WAIT(cpu, eq, ==)
DECLARE_WAIT(cpu, ne, !=)
DECLARE_WAIT(cpu, le, <=)
DECLARE_WAIT(cpu, ge, >=)
DECLARE_WAIT(cpu, lt, <)
DECLARE_WAIT(cpu, gt, >)

#define POINTER_WAIT(endianness, name)    dsrl_sched_wait_##name##_##endianness

static sched_wait_fcn_t* const sched_wait_table[3][6] = 
{
    {
        POINTER_WAIT(le, eq),
        POINTER_WAIT(le, ne),
        POINTER_WAIT(le, le),
        POINTER_WAIT(le, ge),
        POINTER_WAIT(le, lt),
        POINTER_WAIT(le, gt)
    }, {
        POINTER_WAIT(be, eq),
        POINTER_WAIT(be, ne),
        POINTER_WAIT(be, le),
        POINTER_WAIT(be, ge),
        POINTER_WAIT(be, lt),
        POINTER_WAIT(be, gt)
    }, {
        POINTER_WAIT(cpu, eq),
        POINTER_WAIT(cpu, ne),
        POINTER_WAIT(cpu, le),
        POINTER_WAIT(cpu, ge),
        POINTER_WAIT(cpu, lt),
        POINTER_WAIT(cpu, gt)
    }
};

static reg_t dsrl_syscall_wait(size_t endianness, size_t name, volatile uint32_t *addr, uint32_t val)
{
    if ((endianness > 3) || (name > 6))
    {
        printk("bad parameters for wait syscall!\n");
        return -1;
    }

    sched_wait_fcn_t *f = sched_wait_table[endianness][name];
    f(addr, val);

    return 0;
}

/* 
 * Others syscall
 */
static reg_t dsrl_syscall_invalid(void)
{
    printk("invalid syscall\n");
    return -1;
}

static reg_t dsrl_syscall_yield(void)
{
    sched_context_switch();
    return 0;
}

/* Syscall table */
struct dsrl_syscall_s
{
    reg_t (*call)();
    uint_fast8_t argc;
};
#define DSRL_SYSCALL_NUMBER 3
static struct dsrl_syscall_s dsrl_syscall_table[DSRL_SYSCALL_NUMBER] = 
{
    { .argc = 0, .call = dsrl_syscall_invalid },
    { .argc = 0, .call = dsrl_syscall_yield },
    { .argc = 4, .call = dsrl_syscall_wait },
};

/* Syscall handler */
CPU_SYSCALL_HANDLER(dsrl_syscall_handler)
{
    register reg_t service asm ("$2");
    struct dsrl_syscall_s *f;

#if 0
    printk("syscall handler: service=%d, end=%d, name=%d, add=0x%x, val=%d\n", 
            service, regtable[4], regtable[5], regtable[6], regtable[7]);
#endif

    if (service < DSRL_SYSCALL_NUMBER)
        f = &dsrl_syscall_table[service];
    else
        f = &dsrl_syscall_table[0];

    switch (f->argc)
    {
        case 0:
            regtable[2] = f->call();
            break;
        case 4:
            regtable[2] = f->call(regtable[4],
                    regtable[5],
                    regtable[6],
                    regtable[7]);
            break;
        default:
            regtable[2] = dsrl_syscall_table[0].call();
            break;
    }
}

