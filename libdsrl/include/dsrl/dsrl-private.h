#ifndef _DSRL_PRIVATE_H_
#define _DSRL_PRIVATE_H_

#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include <dsrl/dsrl-types.h>

#include <mutek/printk.h>

#if defined(CONFIG_LIBDSRL_DEBUG)
# define _dsrl_debug printk
#else
# define _dsrl_debug(...)
#endif

void check_lua_tcg (lua_State *L, int index);

#endif
