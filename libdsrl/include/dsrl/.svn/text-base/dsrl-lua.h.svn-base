#ifndef _DSRL_LUA_H_
#define _DSRL_LUA_H_

#include <dsrl/dsrl-private.h>

#define DSRL_RESOURCE(resource)                                      \
    static error_t check_type_##resource (lua_State *L, size_t ires) \
    {                                                                \
        if (lua_istable(L, ires) && lua_getmetatable(L, ires)) {     \
            lua_getfield(L, LUA_REGISTRYINDEX, "lua."#resource);     \
            if (lua_rawequal(L, -1, -2)) {                           \
                lua_pop(L, 2);                                       \
                return 0;                                            \
            }                                                        \
            lua_pop(L, 2);                                           \
        }                                                            \
        lua_pushstring(L, "`lua."#resource"' required");             \
        return 1;                                                    \
    }                                                                \
    error_t check_##resource (lua_State *L, size_t ires)             \
    {                                                                \
        if (check_type_##resource(L, ires) == 0)                     \
            return check_fields_##resource(L, ires);                 \
        return 1;                                                    \
    }                                                                \
    int new_##resource (lua_State *L)                                \
    {                                                                \
        check_lua_tcg(L, 1);                                         \
        lua_getfield(L, 1, #resource);                               \
        if (!lua_istable(L, -1)){                                    \
            lua_pop(L, 1);                                           \
            lua_newtable(L);                                         \
            lua_pushvalue(L, -1);                                    \
            lua_setfield(L, 1, #resource);                           \
        }                                                            \
        size_t iresources = lua_gettop(L);                           \
        const char *name = luaL_checkstring(L, 2);                   \
        luaL_argcheck(L, lua_istable(L, 3), 3, "table expected");    \
        luaL_getmetatable(L, "lua."#resource);                       \
        lua_setmetatable(L, 3);                                      \
        lua_pushvalue(L, 2);                                         \
        lua_setfield(L, 3, "name");                                  \
        lua_pushvalue(L, 3);                                         \
        lua_setfield(L, 1, name);                                    \
        lua_pushvalue(L, 3);                                         \
        size_t i = luaL_getn(L, iresources);                         \
        lua_rawseti(L, iresources, i+1);                             \
        return 1;                                                    \
    }                                                                \
    static void createmeta_##resource(lua_State *L)                  \
    {                                                                \
        _dsrl_debug("_createmeta_"#resource"\n");                    \
        luaL_newmetatable(L, "lua."#resource);                       \
        lua_pushstring(L, "__index");                                \
        lua_pushvalue(L, -2);                                        \
        lua_settable(L, -3);                                         \
        lua_pushstring(L, "__metatable");                            \
        lua_pushvalue(L, -2);                                        \
        lua_settable(L, -3);                                         \
     }


# define BUILD_RESOURCE_PROTO(resource) \
    size_t build_##resource(lua_State *L, size_t ires, resource##_t **p)

#define DSRL_RESOURCE_PROTO(resource)                      \
    BUILD_RESOURCE_PROTO(resource);                        \
    error_t check_##resource (lua_State *L, size_t index); \
    int new_##resource (lua_State *L);

DSRL_RESOURCE_PROTO(dsrl_const);
DSRL_RESOURCE_PROTO(dsrl_barrier);
DSRL_RESOURCE_PROTO(dsrl_file);
DSRL_RESOURCE_PROTO(dsrl_memspace);
DSRL_RESOURCE_PROTO(dsrl_io_memspace);
DSRL_RESOURCE_PROTO(dsrl_mwmr);
DSRL_RESOURCE_PROTO(dsrl_exec);
DSRL_RESOURCE_PROTO(dsrl_task);

void luaopen_dsrl_resources(lua_State *L);

#endif
