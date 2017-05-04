
#include <stdio.h>
#include <errno.h>
#include <mutek/mem_alloc.h>
#include <mutek/printk.h>
#include <lua/lua.h>

int poke(lua_State *st)
{
    if (lua_gettop(st) < 2)
        return -1;

    uint32_t *addr = (void*)lua_tonumber(st, 1);
    uint32_t data = lua_tonumber(st, 2);

    *addr = data;

    return 0;
}

int peek(lua_State *st)
{
    if (lua_gettop(st) < 1)
        return -1;

    uint32_t *addr = (void*)lua_tonumber(st, 1);
    lua_pushnumber(st, *addr);

    return 1;
}

void init_mem_shell(lua_State* luast)
{
    lua_register(luast, "peek", peek);
    lua_register(luast, "poke", poke);
}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

