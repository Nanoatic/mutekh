#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dsrl/dsrl.h>
#include <dsrl/dsrl-lua.h>

/* 
 * Lua representation of TCG
 */
#define LUA_DSRL_TCG_HANDLE "lua.dsrl_tcg"

void check_lua_tcg (lua_State *L, int index)
{
    if ((lua_type(L, index) == LUA_TTABLE) && lua_getmetatable(L, index)) {
        lua_getfield(L, LUA_REGISTRYINDEX, LUA_DSRL_TCG_HANDLE);
        if (lua_rawequal(L, -1, -2)) {
            lua_pop(L, 2);
            return;
        }
    }
    luaL_argerror(L, index, "`"LUA_DSRL_TCG_HANDLE"' expected");
}
void check_empty_tcg (lua_State *L, int index)
{
    lua_getfield(L, index, "dsrl_tcg");
    if (!lua_isnil(L, -1))
        luaL_argerror(L, index, "`"LUA_DSRL_TCG_HANDLE"' has already been run");
    lua_pop(L, 1);
}

static int new_tcg (lua_State *L)
{
    lua_newtable(L);
    luaL_getmetatable(L, LUA_DSRL_TCG_HANDLE);
    lua_setmetatable(L, -2);

    lua_pushvalue(L, 1);
    lua_setfield(L, -2, "name");

    return 1;
}

static int load_tcg (lua_State *L)
{
    const char *scriptfile = luaL_checkstring(L, 1);
    if (luaL_dofile(L, scriptfile) != 0)
    {
        printk("%s\n", lua_tostring(L, -1));
        //luaL_error(L, "dsrl script %s failed", scriptfile);
        lua_pushnil(L);
        return 1;
    }
    /* check a tcg is on stack */
    check_lua_tcg(L, -1);
    _dsrl_debug("\tdone\n");
    return 1;
}

/*
 * Run TCG 
 */
#define RUN_DSRL_TCG_HANDLE "run.dsrl_tcg"

static int run_tcg (lua_State *L)
{
    /* check everything */
    check_lua_tcg(L, 1);
    check_empty_tcg(L, 1);

    _dsrl_debug("checked tcg.\n");

#define check_tcg_resource(resource)                        \
    lua_getfield(L, 1, #resource);                          \
    if (lua_istable(L, -1)) {                               \
        size_t i##resource = lua_gettop(L);                 \
        size_t n##resource = luaL_getn(L, i##resource);     \
        size_t i;                                           \
        for (i=1; i<=n##resource; i++)                      \
        {                                                   \
            lua_rawgeti(L, i##resource, i);                 \
            size_t iresource = lua_gettop(L);               \
            if (check_##resource(L, iresource) != 0)        \
            {                                               \
                size_t ierror = lua_gettop(L);              \
                luaL_Buffer b;                              \
                luaL_buffinit(L, &b);                       \
                luaL_addstring(&b, #resource" `");          \
                /*lua_pushvalue(L, iname);                  */\
                /*luaL_addvalue(&b);                        */\
                /*luaL_addstring(&b, "': ");                */\
                lua_pushvalue(L, ierror);                   \
                luaL_addvalue(&b);                          \
                luaL_pushresult(&b);                        \
                lua_error(L);                               \
            }                                               \
            lua_pop(L, 1);                                  \
        }                                                   \
        _dsrl_debug("#"#resource": %d\n", n##resource);     \
    } else if (!lua_isnil(L, -1))                           \
        luaL_error(L, #resource": expected to be a table"); \
    lua_pop(L, 1);

    check_tcg_resource(dsrl_const);
    check_tcg_resource(dsrl_barrier);
    check_tcg_resource(dsrl_file);
    check_tcg_resource(dsrl_memspace);
    check_tcg_resource(dsrl_io_memspace);
    check_tcg_resource(dsrl_mwmr);
    _dsrl_debug("checked standard resources.\n");

    check_tcg_resource(dsrl_exec);
    _dsrl_debug("checked exec resources.\n");
    check_tcg_resource(dsrl_task);
    _dsrl_debug("checked task resources.\n");

    /* build the tcg */
    dsrl_tcg_t *dsrl_tcg = (dsrl_tcg_t*)malloc(sizeof(dsrl_tcg_t));
    dsrl_tcg_t **d = (dsrl_tcg_t**)lua_newuserdata(L, sizeof(dsrl_tcg_t*));
    *d = dsrl_tcg;
    luaL_getmetatable(L, RUN_DSRL_TCG_HANDLE);
    lua_setmetatable(L, -2);
    lua_pushvalue(L, -1);
    lua_setfield(L, 1, "dsrl_tcg");

    _dsrl_debug("built dsrl_tcg userdata\n");

    cpu_cycle_t cycle_count = -cpu_cycle_count();

# define CREATE_RESOURCE(resource)\
    build_##resource(L, iresource, &resource)

# define CALLBACK_RESOURCE(addr, size)
#define build_tcg_resource(resource)                                                    \
    lua_getfield(L, 1, #resource);                                                      \
    if (lua_istable(L, -1)){                                                            \
        size_t i##resource = lua_gettop(L);                                             \
        size_t n##resource = luaL_getn(L, i##resource);                                 \
        dsrl_tcg->resource = (resource##_t**)malloc(n##resource*sizeof(resource##_t*)); \
        dsrl_tcg->n_##resource = n##resource;                                           \
        size_t i;                                                                       \
        for (i=1; i<=n##resource; i++)                                                  \
        {                                                                               \
            lua_rawgeti(L, i##resource, i);                                             \
            size_t iresource = lua_gettop(L);                                           \
            resource##_t *resource;                                                     \
            lua_getfield(L, iresource, "_addr");                                        \
            lua_getfield(L, iresource, "_size");                                        \
            if (lua_isnil(L, -1) || lua_isnil(L, -1))                                   \
            {                                                                           \
                lua_pop(L, 2);                                                          \
                size_t _size = CREATE_RESOURCE(resource);                               \
                lua_pushinteger(L, (uintptr_t)resource);                                \
                lua_setfield(L, iresource, "_addr");                                    \
                lua_pushinteger(L, _size);                                              \
                lua_setfield(L, iresource, "_size");                                    \
            } else {                                                                    \
                size_t _size = (size_t)lua_tointeger(L, -1);                            \
                resource = (resource##_t*)lua_tointeger(L, -2);                         \
                lua_pop(L, 2);                                                          \
                CALLBACK_RESOURCE(resource, _size);                                     \
            }                                                                           \
            lua_pop(L, 1);                                                              \
            dsrl_tcg->resource[i-1] = resource;                                         \
        }                                                                               \
    }                                                                                   \
    lua_pop(L, 1);

    build_tcg_resource(dsrl_const);
    _dsrl_debug("const, ");
    build_tcg_resource(dsrl_barrier);
    _dsrl_debug("barrier, ");
    build_tcg_resource(dsrl_file);
    _dsrl_debug("file, ");
    build_tcg_resource(dsrl_memspace);
    _dsrl_debug("memspace, ");
    build_tcg_resource(dsrl_io_memspace);
    _dsrl_debug("io_memspace, ");
    build_tcg_resource(dsrl_mwmr);
    _dsrl_debug("mwmr.\n");
    _dsrl_debug("built standard resources.\n");

    build_tcg_resource(dsrl_exec);
    _dsrl_debug("built exec resources.\n");
    build_tcg_resource(dsrl_task);
    _dsrl_debug("built task resources.\n");

    cycle_count += cpu_cycle_count();
    _dsrl_debug("run_tcg load cycles: %d\n", cycle_count);

    /* at last, launch the task */
    size_t itask;
    for (itask = 0; itask<dsrl_tcg->n_dsrl_task; itask++)
    {
        _dsrl_debug("launch task #%d\n", itask);
        CPU_INTERRUPT_SAVESTATE_DISABLE;
        sched_context_start(&dsrl_tcg->dsrl_task[itask]->context);
        CPU_INTERRUPT_RESTORESTATE;
    }
    return 1;
}
static int gc_tcg (lua_State *L)
{
    _dsrl_debug("gc_tcg: nothing yet...\n");
    /* TODO */
    return 0;
}

/*
 * Util
 */
static int list (lua_State *L)
{
    luaL_checktype(L, 1, LUA_TTABLE);
    lua_getglobal(L, "print");
    size_t iprint = lua_gettop(L);
    lua_pushnil(L);  /* first key */
    while (lua_next(L, 1)) {
        lua_pushvalue(L, iprint);
        lua_pushvalue(L, -3);  /* key */
        lua_pushvalue(L, -3);  /* value */
        lua_call(L, 2, 1);
        if (!lua_isnil(L, -1))
            return 1;
        lua_pop(L, 2);  /* remove value and result */
    }
    return 0;
}

/* 
 * Register types and their capabilities
 */
static const luaL_Reg lua_tcglib_m[] = {
    {"barrier"      , new_dsrl_barrier},
    {"const"        , new_dsrl_const},
    {"memspace"     , new_dsrl_memspace},
    {"file"         , new_dsrl_file},
    {"io_memspace"  , new_dsrl_io_memspace},
    {"mwmr"         , new_dsrl_mwmr},
    {"exec"         , new_dsrl_exec},
    {"task"         , new_dsrl_task},
    {"run"          , run_tcg},
    {NULL           , NULL}
};

static const luaL_Reg run_tcglib_m[] = {
    //{"__gc" , gc_tcg},
    {NULL   , NULL}
};

void luaopen_tcg_resources (lua_State *L)
{
    luaL_newmetatable(L, LUA_DSRL_TCG_HANDLE);
    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);
    lua_pushstring(L, "__metatable");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);
    luaL_openlib(L, NULL, lua_tcglib_m, 0);

    luaL_newmetatable(L, RUN_DSRL_TCG_HANDLE);
    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);
    lua_pushstring(L, "__metatable");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);
    luaL_openlib(L, NULL, run_tcglib_m, 0);

    luaopen_dsrl_resources(L);
}

static const luaL_Reg lua_dsrllib_f[] = {
    {"new_tcg",     new_tcg},
    {"load_tcg",    load_tcg},
    {"list",        list},
    {NULL, NULL}
};

void luaopen_dsrl (lua_State *L)
{
    /* register the two functions which create a new tcg */
    luaL_openlib(L, "dsrl", lua_dsrllib_f, 0);
    /* register the metatables for the subtypes of tcg */
    luaopen_tcg_resources(L);
}
