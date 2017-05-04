#ifndef _DSRL_TYPES_H_
#define _DSRL_TYPES_H_

#include <hexo/types.h>
#include <mutek/scheduler.h>

#include <libelf/rtld.h>

/*
 * exec resource (rtld)
 */
typedef struct {
    const char *execname;
    struct dynobj_rtld_s *dynobj;
} dsrl_exec_t;

/*
 * Task resource
 */
typedef struct {
    const char *funcname;
    uintptr_t tty;
    cpu_id_t cpuid;

    /* Hexo context */
    struct sched_context_s context;
    uintptr_t *args_ptr;

    /* dynamic loader */
    dsrl_exec_t *exec;
    uintptr_t func;
    uintptr_t tls;
    uintptr_t tp;

    /* Sched wait syscall */
    volatile uint32_t *wait_addr;
    uint32_t    wait_val;
} dsrl_task_t;

/**********************
 * standard resources *
 **********************/

/* Const resource */
typedef uint32_t dsrl_const_t;

/* Barrier resource */
typedef struct {
    int8_t max;
    int8_t count;
    volatile int32_t lock;
    volatile uint32_t serial;
} dsrl_barrier_t;

/* Memspace, IOMemspace and file resource */
typedef void* dsrl_buffer_t;
typedef struct {
    uint32_t size;
    dsrl_buffer_t buffer;
} dsrl_memspace_t;
typedef dsrl_memspace_t dsrl_io_memspace_t;
typedef dsrl_memspace_t dsrl_file_t;

/* Mwmr resource */
typedef struct {
     uint32_t free_tail; // bytes 
     uint32_t free_head; // bytes 
     uint32_t free_size; // bytes 
     uint32_t data_tail; // bytes 
     uint32_t data_head; // bytes 
     uint32_t data_size; // bytes 
} soclib_mwmr_status_s;
typedef struct {
    size_t width;
    size_t depth;
    size_t gdepth;
    soclib_mwmr_status_s status;
    dsrl_buffer_t buffer;
} dsrl_mwmr_t;

/*
 * TCG
 */
typedef struct dsrl_tcg_s{
	dsrl_barrier_t	**dsrl_barrier;
	dsrl_const_t	**dsrl_const;
	dsrl_memspace_t **dsrl_file;
	dsrl_memspace_t **dsrl_memspace;
	dsrl_memspace_t **dsrl_io_memspace;
	dsrl_mwmr_t		**dsrl_mwmr;
	dsrl_exec_t		**dsrl_exec;
	dsrl_task_t		**dsrl_task;
	size_t n_dsrl_barrier;
	size_t n_dsrl_const;
	size_t n_dsrl_file;
	size_t n_dsrl_memspace;
	size_t n_dsrl_io_memspace;
	size_t n_dsrl_mwmr;
	size_t n_dsrl_exec;
	size_t n_dsrl_task;
} dsrl_tcg_t;

#endif
