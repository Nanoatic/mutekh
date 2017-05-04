#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hexo/segment.h>

#include <dsrl/dsrl-lua.h>
#include <cpu/tls.h>

/*
 * checking utilities
 */
#define RESOURCE_CHECK_FIELDS(resource) error_t check_fields_##resource(lua_State *L, size_t ires)
#define CHECK_EXTERNAL(ires)                         \
    do {                                             \
        lua_getfield(L, ires, "_addr");              \
        lua_getfield(L, ires, "_size");              \
        if (!lua_isnil(L, -1) && !lua_isnil(L, -2)){ \
            lua_pop(L, 2);                           \
            return 0;                                \
        }                                            \
        lua_pop(L, 2);                               \
    } while(0)

#define CHECK_FIELD(field, type, ires)                                            \
    do {                                                                          \
        lua_getfield(L, ires, #field);                                            \
        if (!lua_is##type(L, -1))                                                 \
        {                                                                         \
            if (lua_isnil(L, -1))                                                 \
            lua_pushstring(L, "field `"#field"' not existing");                   \
            else                                                                  \
            lua_pushstring(L, "error on field `"#field"' (expecting `"#type"')"); \
            return 1;                                                             \
        }                                                                         \
    lua_pop(L, 1);                                                                \
    } while(0)

/*
 * building utilities
 */
#define GET_FIELD(field, type, ires)                  \
({                                                    \
    lua_getfield(L, ires, #field);                    \
    lua_to##type(L, -1);                              \
})
#define GET_INTEGER_FIELD(field, ires)                \
({                                                    \
    size_t res = GET_FIELD(field, integer, ires);     \
    lua_pop(L, 1);                                    \
    res;                                              \
})
#define GET_STRING_FIELD(field, ires)                 \
({                                                    \
    const char *res = GET_FIELD(field, string, ires); \
    lua_pop(L, 1);                                    \
    res;                                              \
})
#define TEST_BOOL_FIELD(field, ires)       \
    ({                                     \
     bool_t res = 0;                       \
     lua_getfield(L, ires, #field);        \
     if (lua_isnumber(L, -1))              \
        res = (lua_tointeger(L, -1) != 0); \
     lua_pop(L, 1);                        \
     res;                                  \
     })

/* build prototype */
# define RESOURCE_BUILD(resource) size_t build_##resource(lua_State *L, size_t ires, resource##_t **p)
/* Alloc resource
 * - skip shared param here, only cached
 */
# define ALLOC_CACHED(resource, size) \
    (resource##_t*)mem_alloc(size, (mem_scope_cpu))
# define ALLOC_UNCACHED(resource, size) \
    (resource##_t*)mem_alloc(size, (mem_scope_sys))

# define RESOURCE_ALLOC(resource, add_size, ires)                         \
    resource##_t *res;                                                    \
    do {                                                                  \
        if (TEST_BOOL_FIELD(cached, ires))                                \
        res = ALLOC_CACHED (resource, sizeof(resource##_t) + add_size);   \
        else                                                              \
        res = ALLOC_UNCACHED (resource, sizeof(resource##_t) + add_size); \
        *p = res;                                                         \
    } while(0)

#define RESOURCE_GET_BUFFER(resource) \
    (dsrl_buffer_t)((uintptr_t)res + sizeof(resource##_t))

/* Barrier */
RESOURCE_CHECK_FIELDS(dsrl_barrier)
{
    CHECK_EXTERNAL(ires);
    CHECK_FIELD(max, number, ires);
    return 0;
}
DSRL_RESOURCE(dsrl_barrier)
RESOURCE_BUILD(dsrl_barrier)
{
    RESOURCE_ALLOC(dsrl_barrier, 0, ires);
    size_t n = GET_INTEGER_FIELD(max, ires);
    res->count = n;
    res->max = n;
    res->serial = 0;
    res->lock = 0; 
    return sizeof(dsrl_barrier_t);
}
/* Const */
RESOURCE_CHECK_FIELDS(dsrl_const)
{
    CHECK_EXTERNAL(ires);
    CHECK_FIELD(value, number, ires);
    return 0;
}
DSRL_RESOURCE(dsrl_const)
RESOURCE_BUILD(dsrl_const)
{
    *p = (dsrl_const_t*)GET_INTEGER_FIELD(value, ires);
    return 0;
}
/* Memspace */
RESOURCE_CHECK_FIELDS(dsrl_memspace)
{
    CHECK_EXTERNAL(ires);
    CHECK_FIELD(size, number, ires);
    return 0;
}
DSRL_RESOURCE(dsrl_memspace)
RESOURCE_BUILD(dsrl_memspace)
{
    size_t size = GET_INTEGER_FIELD(size, ires);
    RESOURCE_ALLOC(dsrl_memspace, size, ires);
    res->size = size;
    res->buffer = RESOURCE_GET_BUFFER(dsrl_memspace);
    return (sizeof(dsrl_memspace_t) + size);
}
/* IOmemspace */
RESOURCE_CHECK_FIELDS(dsrl_io_memspace)
{
    CHECK_EXTERNAL(ires);
    CHECK_FIELD(size, number, ires);
    CHECK_FIELD(mmap, number, ires);
    return 0;
}
DSRL_RESOURCE(dsrl_io_memspace)
RESOURCE_BUILD(dsrl_io_memspace)
{
    RESOURCE_ALLOC(dsrl_io_memspace, 0, ires);
    res->size = GET_INTEGER_FIELD(size, ires);
    res->buffer = (dsrl_buffer_t)GET_INTEGER_FIELD(mmap, ires);
    return (sizeof(dsrl_io_memspace_t) + res->size);
}
/* File (this is not really part of original srl api...) */
RESOURCE_CHECK_FIELDS(dsrl_file)
{
    CHECK_EXTERNAL(ires);
    CHECK_FIELD(file, string, ires);
    //CHECK_FIELD(size, number, ires);
    return 0;
}
DSRL_RESOURCE(dsrl_file)
RESOURCE_BUILD(dsrl_file)
{
    const char* filename = GET_STRING_FIELD(file, ires);
#if 1
    struct stat st;
    if (stat(filename, &st) != 0)
    {
        lua_pushfstring(L, "`stat()' failed on file `%s'\n", filename);
        lua_error(L);
    }
    _dsrl_debug("file is %d bytes\n", st.st_size);
    size_t size = st.st_size;
    RESOURCE_ALLOC(dsrl_file, size, ires);
#else
    size_t size = GET_INTEGER_FIELD(size, ires);
    RESOURCE_ALLOC(dsrl_file, size, ires);
#endif
    res->size = size;
    res->buffer = RESOURCE_GET_BUFFER(dsrl_file);

    FILE *f = fopen(filename, "r");
    if (f==NULL)
        luaL_error(L, "opening file %s failed", filename);
    fread(res->buffer, res->size, 1, f);
    fclose(f);
    return (sizeof(dsrl_file_t) + size);
}
/* Mwmr */
RESOURCE_CHECK_FIELDS(dsrl_mwmr)
{
    CHECK_EXTERNAL(ires);
    CHECK_FIELD(width, number, ires);
    CHECK_FIELD(depth, number, ires);
    return 0;
}
DSRL_RESOURCE(dsrl_mwmr)
RESOURCE_BUILD(dsrl_mwmr)
{
    size_t width = GET_INTEGER_FIELD(width, ires);
    size_t depth = GET_INTEGER_FIELD(depth, ires);
    size_t size = width * depth;
    RESOURCE_ALLOC(dsrl_mwmr, size, ires);
    res->width = width;
    res->depth = depth;
    res->gdepth = size;
    res->buffer = RESOURCE_GET_BUFFER(dsrl_mwmr);

    res->status.free_tail = 0;
    res->status.free_head = 0;
    res->status.free_size = size;
    res->status.data_tail = 0;
    res->status.data_head = 0;
    res->status.data_size = 0;
    return (sizeof(dsrl_mwmr_t) + size);
}

/* exec */
RESOURCE_CHECK_FIELDS(dsrl_exec)
{
    CHECK_EXTERNAL(ires);
    CHECK_FIELD(file, string, ires);
    return 0;
}
DSRL_RESOURCE(dsrl_exec)
RESOURCE_BUILD(dsrl_exec)
{
    /* exec is private to the kernel */
    dsrl_exec_t *res = (dsrl_exec_t*)malloc(sizeof(dsrl_exec_t));
    *p = res;

    res->execname = GET_STRING_FIELD(file, ires);

    /* load the exec in memory */
    _dsrl_debug("\tload exec in memory\n");
    if (rtld_open(&res->dynobj, res->execname) != 0)
        luaL_error(L, "dlopen failed on %s", res->execname);
    else
        _dsrl_debug("\tentrypoint is @%p\n", res->dynobj->elf.entrypoint);

    return 0;
}
/* Task */
RESOURCE_CHECK_FIELDS(dsrl_task)
{
    lua_getfield(L, ires, "exec");
    size_t iexec = lua_gettop(L);
    if (check_type_dsrl_exec(L, iexec) == 0)
    {
        if (check_fields_dsrl_exec(L, iexec) != 0)
            return 1;
    } else
        return 1;    
    lua_pop(L, 1);

    CHECK_FIELD(func, string, ires);
    CHECK_FIELD(sstack, number, ires);
    CHECK_FIELD(cpuid, number, ires);
    CHECK_FIELD(tty, number, ires);
    CHECK_FIELD(args, table, ires);

    /* check the args table are dsrl resources */
    lua_getfield(L, ires, "args");
    /* args table is numerically indexed */
    size_t iargs = lua_gettop(L);
    size_t nargs = luaL_getn(L, iargs);
    size_t i;
    for (i=1; i<=nargs; i++)
    {
        lua_rawgeti(L, iargs, i);
        size_t iarg = lua_gettop(L);

#define CHECK_ARG(resource)                                 \
        do {                                                \
            if (check_type_##resource(L, iarg) == 0)        \
            {                                               \
                if (check_fields_##resource(L, iarg) != 0)  \
                {                                           \
                    size_t ierror = lua_gettop(L);          \
                    luaL_Buffer b;                          \
                    luaL_buffinit(L, &b);                   \
                    lua_pushfstring(L, "arg #%d: ", nargs); \
                    luaL_addvalue(&b);                      \
                    lua_pushvalue(L, ierror);               \
                    luaL_addvalue(&b);                      \
                    luaL_pushresult(&b);                    \
                    return 1;                               \
                }                                           \
                lua_pop(L, 1);                              \
                goto next_arg;                              \
            }                                               \
            lua_pop(L, 1); /* pop the error message */      \
        } while (0)

        CHECK_ARG(dsrl_const);
        CHECK_ARG(dsrl_barrier);
        CHECK_ARG(dsrl_file);
        CHECK_ARG(dsrl_memspace);
        CHECK_ARG(dsrl_io_memspace);
        CHECK_ARG(dsrl_mwmr);

        lua_pushfstring(L, "arg #%d: not a dsrl resource", i);
        return 1;
next_arg:
        continue;
    }
    /* pop the args table */
    lua_pop(L, 1);
    return 0;
}
DSRL_RESOURCE(dsrl_task)

static CONTEXT_ENTRY(dsrl_run_task)
{
    dsrl_task_t *task = param;

    _dsrl_debug("dsrl_run_task\n");

    // set the syscall handler for the current CPU
    // that's why migration should be forbidden
    CPU_SYSCALL_HANDLER(dsrl_syscall_handler);
    cpu_syscall_sethandler(dsrl_syscall_handler);

    /* setup tls */
    tls_init_tp(task->tp);

#if defined(CONFIG_CPU_MIPS) && __mips >= 32
    /* allow user to access count and count_cc registers */
    reg_t hwrena = cpu_mips_mfc0(7, 0);
    hwrena |= 0x0000000c; // count and count_cc are respectively #2 and #3
    cpu_mips_mtc0(7, 0, hwrena);

    /* allow user to access FPU coprocessor */
    reg_t status = cpu_mips_mfc0(12, 0);
    status |= 0x20000000; // FPU is cp1
    cpu_mips_mtc0(12, 0, status);
#endif

    sched_unlock();
    cpu_interrupt_enable();

    /* application must become PIC independently */
    typedef void* dsrl_func_t (void*);
    dsrl_func_t *f = task->exec->dynobj->elf.entrypoint;
    f(task->args_ptr);

    // should not happen (at least if previously we went in user mode)
    cpu_interrupt_disable();
    sched_lock();
    sched_context_exit();
}
RESOURCE_BUILD(dsrl_task)
{
    /* task is private to the kernel */
    dsrl_task_t *res = (dsrl_task_t*)malloc(sizeof(dsrl_task_t));
    *p = res;

    res->funcname = GET_STRING_FIELD(func, ires);

    res->cpuid  = GET_INTEGER_FIELD(cpuid, ires);
    assert(res->cpuid < CONFIG_CPU_MAXCOUNT);
    res->tty = GET_INTEGER_FIELD(tty, ires);

    size_t sstack = GET_INTEGER_FIELD(sstack, ires);

    /* Fill the args */
    lua_getfield(L, ires, "args");
    /* args table is numerically indexed */
    size_t iargs = lua_gettop(L);
    size_t nargs = luaL_getn(L, iargs);

    _dsrl_debug("dsrl_task_build\n");
    _dsrl_debug("\t#args: %d\n", nargs);

    /* Build the stacks according to the number of args */
    uintptr_t stack;
    size_t  stack_size;

    uintptr_t stack_args;
    /* make rooms for arguments (func, tty and user args) */
    size_t  stack_args_size = sstack - 4*2 - 4*nargs;
    stack_args = (uintptr_t)ALLOC_CACHED(uintptr, sstack);

    _dsrl_debug("\t@stack_args: %x, stack_args_size: %d, stack_size: %d\n", 
            stack_args, stack_args_size, sstack);

    stack_size = stack_args_size;
    stack = stack_args;

    res->args_ptr = (uintptr_t*)(stack_args + stack_args_size);
    res->args_ptr[0] = res->tty;

    size_t i;
    for (i=0; i<nargs; i++)
    {
        /* reuse the field _addr to find the address of arguments */
        lua_rawgeti(L, iargs, i+1);
        size_t iarg = lua_gettop(L);
        lua_getfield(L, iarg, "_addr");
        res->args_ptr[i+2] = lua_tointeger(L, -1);
        lua_pop(L, 2);
    }
    /* pop the args table */
    lua_pop(L, 1);

    /* get the exec */
    lua_getfield(L, ires, "exec");
    size_t iexec = lua_gettop(L);
    lua_getfield(L, iexec, "_addr");
    res->exec = lua_tointeger(L, -1);
    lua_pop(L, 2);

    /* look for the function symbol */
    if (rtld_sym(res->exec->dynobj, res->funcname, &res->func) != 0)
        luaL_error(L, "dlsym failed on %s", res->funcname);
    else
        _dsrl_debug("\tfunc is @%p\n", res->func);
    res->args_ptr[1] = res->func;

    size_t tls_size;
    if (rtld_tls_size(res->exec->dynobj, &tls_size) != 0)
        luaL_error(L, "rtld_tls_size failed on %s", res->exec->execname);
    /* tls can be cached since it is thread local */
    res->tls = (uintptr_t)ALLOC_CACHED(uintptr, tls_size);

    if (rtld_tls_init(res->exec->dynobj, res->tls, &res->tp) != 0)
        luaL_error(L, "rtld_tls_init failed on %s", res->exec->execname);
    else
        _dsrl_debug("\ttls is @%p and tp is @%p\n", res->tls, res->tp);

    /* build the mutekH sched context */
    _dsrl_debug("\tBuild sched context\n");

    context_init(&res->context.context,
            (void*)stack, (void*)(stack + stack_size),
            dsrl_run_task, res);
    sched_context_init(&res->context);
    sched_affinity_single(&res->context, res->cpuid);

    return 0;
}

/*
 * Register dsrl resources
 */
void luaopen_dsrl_resources(lua_State *L)
{
#define REGISTER_DSRL_RESOURCE(resource) createmeta_##resource(L)
    REGISTER_DSRL_RESOURCE(dsrl_const);
    REGISTER_DSRL_RESOURCE(dsrl_barrier);
    REGISTER_DSRL_RESOURCE(dsrl_file);
    REGISTER_DSRL_RESOURCE(dsrl_io_memspace);
    REGISTER_DSRL_RESOURCE(dsrl_memspace);
    REGISTER_DSRL_RESOURCE(dsrl_mwmr);
    REGISTER_DSRL_RESOURCE(dsrl_exec);
    REGISTER_DSRL_RESOURCE(dsrl_task);

    rtld_init();
}
