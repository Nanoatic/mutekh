/*
 * This file is part of MutekH.
 * 
 * MutekH is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; version 2.1 of the License.
 * 
 * MutekH is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with MutekH; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 * Copyright (c) UPMC, Lip6, SoC
 *         Nicolas Pouillon <nipo@ssji.net>, 2009-2010
 */

#include <mutek/mem_alloc.h>
#include <mutek/semaphore.h>

#include <stdio.h>
#include <hexo/types.h>

#include <capsule_api.h>
#include <capsule_abs_itf.h>

#define CAP_STACK_SIZE 4096*4

#if 0
#define dprintk(x...) printk(x)
#else
#define dprintk(x...) do{}while(0)
#endif

static bool_t probes_blocked = 0;

#define CONTAINER_LOCK_group_queue HEXO_SPIN

CONTAINER_TYPE(group_queue, SLIST,
struct capsule_group_s
{
    CONTAINER_ENTRY_TYPE(SLIST) list_entry;

    struct capsule_group_s *parent;
    struct semaphore_s join;
}, list_entry);

struct cpu_ctxt_s
{
    struct sched_context_s context;

    char stack[CAP_STACK_SIZE];

    size_t job_count;
    size_t joins;
    size_t probes;
};

CONTAINER_FUNC(capsule_queue, SLIST, static inline, capsule_queue, list_entry);
CONTAINER_FUNC(group_queue, SLIST, static inline, group_queue, list_entry);

int_fast8_t main(int_fast8_t, char**);

struct semaphore_s free_cpu;
struct semaphore_s has_job;
static group_queue_root_t free_groups;
static capsule_queue_root_t jobs;
static capsule_queue_root_t free_jobs;

CONTEXT_LOCAL capsule_ctxt_t *cur_job;
struct cpu_ctxt_s *cpu_ctxts[CONFIG_CPU_MAXCOUNT];

static inline struct cpu_ctxt_s *get_cpu_ctxt()
{
    uintptr_t p = (uintptr_t)sched_get_current();
    p -= offsetof(struct cpu_ctxt_s, context);
    return (struct cpu_ctxt_s *)p;
}

capsule_ctxt_t *capsule_ctxt_get(void)
{
    capsule_ctxt_t *r = CONTEXT_LOCAL_GET(cur_job);
    dprintk("%s %p %p, cpu%d, cls = %p, tls = %p\n",
            __FUNCTION__, r, CONTEXT_LOCAL_GET(cur_job),
            cpu_id(), CPU_GET_CLS(),
            CONTEXT_GET_TLS());
    assert(r);
    return r;
}

capsule_rc_t capsule_sys_init(void)
{
    dprintk("%s\n", __FUNCTION__);
    return 0;
}

void run_main();

static void main_runner(void *unused)
{
    run_main();
    printk("%s main ended\n", __FUNCTION__);

    cpu_trap();

    cpu_interrupt_disable();
    sched_lock();
    sched_context_exit();
}

static inline struct capsule_group_s *group_create(
    struct capsule_group_s *parent)
{
    struct capsule_group_s *group = group_queue_pop(&free_groups);
    if ( !group ) {
        group = mem_alloc(sizeof(*group), mem_scope_sys);
    }

    semaphore_init(&group->join, 1);
    group->parent = parent;

    return group;
}

static inline void group_release(struct capsule_group_s *group)
{
    assert(semaphore_value(&group->join) == 0);
    semaphore_destroy(&group->join);

    group_queue_push(&free_groups, group);
}

static struct capsule_ctxt_s *ctxt_create()
{
    struct capsule_ctxt_s *c = capsule_queue_pop(&free_jobs);

    if ( c == NULL )
        c = mem_alloc(sizeof(*c), mem_scope_sys);

    c->private_dtor = NULL;
    c->group = NULL;

    return c;
}

static void do_one_job(struct capsule_ctxt_s *job)
{
    struct cpu_ctxt_s *cpu = get_cpu_ctxt();

    assert(job);

    struct capsule_ctxt_s *old = CONTEXT_LOCAL_GET(cur_job);

/*         dprintk("%s setting cur to %p, cpu%d, cls = %p, tls = %p\n", */
/*                 __FUNCTION__, job, */
/*                 cpu_id(), CPU_GET_CLS(), */
/*                 CONTEXT_GET_TLS()); */
    CONTEXT_LOCAL_SET(cur_job, job);
    dprintk("%s on %d job %p\n", __FUNCTION__, cpu_id(), job);
/*         dprintk("%s cur %p\n", __FUNCTION__, capsule_ctxt_get()); */
    job->func(job->arg);
    dprintk("%s on %d job %p done\n", __FUNCTION__, cpu_id(), job);

    if ( job->private_dtor != NULL )
        job->private_dtor(job->priv);

    job->private_dtor = NULL;

    semaphore_give(&job->group->join, 1);
    capsule_queue_push(&free_jobs, job);

    CONTEXT_LOCAL_SET(cur_job, old);

    cpu->job_count++;
}

CONTEXT_ENTRY(capsule_sys_runner)
{
	sched_unlock();
    cpu_interrupt_enable();

    dprintk("%s on %d %p\n", __FUNCTION__, cpu_id(), param);

    semaphore_give(&free_cpu, 1);

    while (1) {
//        dprintk("%s on %d wait\n", __FUNCTION__, cpu_id());
        semaphore_take(&has_job, 1);
//        dprintk("%s on %d todo\n", __FUNCTION__, cpu_id());
        struct capsule_ctxt_s *job = capsule_queue_pop(&jobs);
        if ( job )
            do_one_job(job);

        semaphore_give(&free_cpu, 1);
    }
}

void app_start()
{
    size_t i;

    semaphore_init(&free_cpu, 0);
    semaphore_init(&has_job, 0);
    group_queue_init(&free_groups);
    capsule_queue_init(&jobs);
    capsule_queue_init(&free_jobs);

    dprintk("%s waiting\n", __FUNCTION__);
    for (i=0; i<50000; ++i)
        asm volatile("nop");

    memset(cpu_ctxts, 0, sizeof(*cpu_ctxts)*CONFIG_CPU_MAXCOUNT);

	CPU_INTERRUPT_SAVESTATE_DISABLE;
    for ( i=0; i<arch_get_cpu_count(); ++i ) {
        struct cpu_ctxt_s *ctx = mem_alloc(sizeof(*ctx), mem_scope_sys);

        memset(ctx, 0, sizeof(*ctx));

        cpu_ctxts[i] = ctx;

        dprintk("%s %d/%d %p\n",
                __FUNCTION__, i, arch_get_cpu_count(),
                ctx);

        context_init( &ctx->context.context,
                      ctx->stack,
                      ctx->stack + CAP_STACK_SIZE,
                      capsule_sys_runner,
                      ctx );

        sched_context_init( &ctx->context );
        sched_affinity_single( &ctx->context, i );
        sched_context_start( &ctx->context );


        group_queue_push(&free_groups, mem_alloc(sizeof(struct capsule_group_s), mem_scope_sys));
    }

	CPU_INTERRUPT_RESTORESTATE;

    semaphore_take(&free_cpu, 1);
    capsule_ctxt_t *out = ctxt_create();
    out->func = main_runner;
    out->group = group_create(NULL);
    capsule_divide(out, NULL);
}

capsule_rc_t capsule_sys_init_warmup(void)
{
    size_t i;

    for ( i=0; i<arch_get_cpu_count()*2; ++i ) {
        struct capsule_ctxt_s *job = mem_alloc(sizeof(*job), mem_scope_sys);
        capsule_queue_push(&free_jobs, job);
    }

    return 0;
}

void capsule_sys_destroy(void)
{

}

/**
   Globally disable probes
 */
void capsule_sys_block(void)
{
    probes_blocked = 1;
}

void capsule_sys_unblock(void)
{
    probes_blocked = 0;
}

#ifdef CONFIG_LIBC_STREAM
void capsule_sys_dump_all_stats(FILE * stream)
{
    size_t job_count = 0;
    size_t joins = 0;
    size_t probes = 0;
    size_t i;
    fprintf(stream, "Where\tprobes\tjobs\tjoins\n");

    for ( i=0; i<arch_get_cpu_count(); ++i ) {
        fprintf(stream, "CPU%d\t%d\t%d\t%d\n",
                i,
                cpu_ctxts[i]->probes,
                cpu_ctxts[i]->job_count,
                cpu_ctxts[i]->joins);
        probes += cpu_ctxts[i]->probes;
        job_count += cpu_ctxts[i]->job_count;
        joins += cpu_ctxts[i]->joins;
    }
    fprintf(stream, "Global\t%d\t%d\t%d\n",
            probes,
            job_count,
            joins);

    fprintf(stream, "\n");
    fprintf(stream, "Total free context at end: %d\n", capsule_queue_count(&free_jobs));
}
#endif

void capsule_sys_reset_all_stats()
{
    size_t i;

    for ( i=0; i<arch_get_cpu_count(); ++i ) {
        cpu_ctxts[i]->probes = 0;
        cpu_ctxts[i]->job_count = 0;
        cpu_ctxts[i]->joins = 0;
    }
}

void capsule_probe(capsule_ctxt_func_t func, capsule_ctxt_t ** ctxt)
{
    *ctxt = NULL;

    struct cpu_ctxt_s *cpu = get_cpu_ctxt();
    capsule_ctxt_t *cur = capsule_ctxt_get();

    cpu->probes++;

    if ( probes_blocked ) {
        return;
    }
    if (semaphore_try_take(&free_cpu, 1) != 0) {
        return;
    }

    *ctxt = ctxt_create();

    (*ctxt)->func = func;
    dprintk("%s %p group %p\n", __FUNCTION__, *ctxt, cur->group);
    (*ctxt)->group = cur->group;
    semaphore_give(&cur->group->join, -1);
}

void capsule_divide(capsule_ctxt_t * ctxt, void * arg)
{
    dprintk("%s %p\n", __FUNCTION__, ctxt);
    ctxt->arg = arg;

    capsule_queue_push(&jobs, ctxt);
    semaphore_give(&has_job, 1);
}

void capsule_group_split(void)
{
    capsule_ctxt_t *cur = capsule_ctxt_get();
    struct capsule_group_s *group = group_create(cur->group);
    cur->group = group;
}

void capsule_group_join(void)
{
    capsule_ctxt_t *cur = capsule_ctxt_get();
    struct capsule_group_s *group = cur->group;
    struct capsule_group_s *parent = group->parent;

    struct cpu_ctxt_s *cpu = get_cpu_ctxt();

    cpu->joins++;

    dprintk("group_join count: %d\n", semaphore_value(&group->join));

    // wait all other threads
    _capsule_semaphore_take(&group->join, 1);

    dprintk("group_joined count: %d, parent: %p\n", semaphore_value(&group->join), parent);

    if ( parent ) {
        group_release(group);
        cur->group = parent;
    } else {
        semaphore_give(&group->join, 1);
    }
}


void _capsule_semaphore_take(struct semaphore_s *sem, int_fast8_t val)
{
    // wait all other threads
    while ( semaphore_try_take(sem, val) != 0 ) {
        struct capsule_ctxt_s *job = capsule_queue_pop(&jobs);
        if ( job )
            do_one_job(job);
    }
}
