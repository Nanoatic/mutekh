/*
    This file is part of libtermui.

    libtermui is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libtermui is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with libtermui.  If not, see <http://www.gnu.org/licenses/>.

    Copyright 2006, Alexandre Becoulet <alexandre.becoulet@free.fr>

    Completion handler for libtermui

*/

#include <string.h>
#include <ctype.h>

#include <lua/lua.h>

#include <termui/term.h>
#include <termui/bhv.h>
#include <termui/getline.h>

static inline int
match_len(const char *a, const char *b)
{
  unsigned int	i = 0;

  while (a[i] && (a[i] == b[i]))
    i++;

  return i;
}

#define MAX_CANDIDATES 256

TERMUI_GETLINE_FCN_COMPLETE(lua_complete)
{
  lua_State *luast = priv;
  const char *start = termui_getline_line_start(bhv);
  const char *cursor = termui_getline_line_cursor(bhv);
  const char *path = cursor;
  int table = LUA_GLOBALSINDEX;
  char *buf, *next;
  int plen;			/* prefix len */
  int count = 0;		/* candidate count */

  /* find table path to complete in line */
  while (path > start && (isalnum(path[-1]) || path[-1] == '.' || path[-1] == '_'))
    path--;

  char buf_[cursor - path + 1];
  buf = buf_;

  memcpy(buf, path, cursor - path);
  buf[cursor - path] = 0;

  /* traverse intermediate tables */
  while ((next = strchr(buf, '.')))
    {
      *next++ = 0;

      lua_getfield(luast, table, buf);

      if (table != LUA_GLOBALSINDEX)
	lua_remove(luast, table);

      if (!lua_istable(luast, -1))
	{
	  termui_term_beep(bhv->tm);
	  return;
	}

      table = lua_gettop(luast);
      buf = next;
    }

  plen = strlen(buf);
  lua_pushnil(luast);

  /* explore table to complete to find candidates */
  while (lua_next(luast, table))
    {
      const char *key;

      /* discard value */
      lua_pop(luast, 1);

      /* discard non string keys */
      if (!lua_isstring(luast, -1))
	continue;

      key = lua_tostring(luast, -1);

      /* discard non matching string keys */
      if (*buf && strncmp(key, buf, plen))
	continue;

      /* keep matching key */
      lua_pushvalue(luast, -1);
      count++;
    }

  switch (count)
    {
      /* no candidate */
    case (0):
      termui_term_beep(bhv->tm);
      break;

      /* one candidate */
    case (1): {
      const char *str = lua_tostring(luast, -1);

      termui_getline_insert(bhv, str + plen, lua_objlen(luast, -1) - plen);
      lua_gettable(luast, table);

      switch (lua_type(luast, -1))
	{
	case LUA_TTABLE:
	  termui_getline_insert(bhv, ".", 1);
	  break;
	case LUA_TFUNCTION:
	  termui_getline_insert(bhv, "()", 2);
	  termui_getline_move_backward(bhv, 1);
	  break;
	}

      lua_pop(luast, 1);
      break;
    }

      /* more than one candidates */
    default: {
      const char *common = lua_tostring(luast, -1);
      int mlen, len = lua_objlen(luast, -1);

      termui_term_printf(bhv->tm, "\n");

      /* display key list and find longest common len */
      while (count--)
	{
	  const char *str = lua_tostring(luast, -1);
	  mlen = match_len(common, str);
	  if (mlen < len)
	    len = mlen;
	  termui_term_printf(bhv->tm, "%s\n", str);
	  lua_pop(luast, 1);
	}

      termui_getline_reprompt(bhv);

      if (mlen > plen)
	termui_getline_insert(bhv, common + plen, len - plen);
    }

    }

  /* cleanup stack */
  if (table != LUA_GLOBALSINDEX)
    lua_remove(luast, table);
}

