
#include <lua/lua.h>
#include <lua/lauxlib.h>

lua_State	*ls;

int cmd_print(lua_State *st)
{
  unsigned int	i;

  for (i = 1; i <= lua_gettop(st); i++)
    {
      switch (lua_type(st, i))
	{
	case LUA_TNUMBER:
	  printk("(lua num %i)\n", lua_tonumber(st, i));
	  break;
	case LUA_TSTRING:
	  printk("(lua str %s)\n", lua_tostring(st, i));
	  break;
	default:
	  printk("(lua type %i)\n", lua_type(st, i));
	}
    }

  return 0;
}

void app_start()
{
  const char	*cmd = "for i = 1, 6, 1 do print (\"coucou\", i); end";

  ls = luaL_newstate();

  lua_pushstring(ls, "print");
  lua_pushcfunction(ls, cmd_print);
  lua_settable(ls, LUA_GLOBALSINDEX);

  luaL_loadbuffer(ls, cmd, strlen(cmd), cmd);
  lua_pcall(ls, 0, LUA_MULTRET, 0);
}

