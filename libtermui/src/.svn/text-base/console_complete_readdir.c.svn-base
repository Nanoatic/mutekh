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

*/

#define _BSD_SOURCE

#include "console_pv.h"

#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef HAVE_SYS_STAT_H
# include <sys/stat.h>
#endif

#ifdef HAVE_FNMATCH_H
# include <fnmatch.h>
#endif

TERMUI_CON_ARGS_CLEANUP_PROTOTYPE(termui_con_readdir_cleanup)
{
  if (ctx)
    free(ctx);
}

TERMUI_CON_ARGS_COLLECT_PROTOTYPE(termui_con_readdir_collect)
{
  termui_bool_t substr = 1;
  char *slash, *buf = NULL;
  char *start = cctx->start;
  termui_size_t buf_use = 0;
  termui_size_t buf_len = 0;
  struct dirent *e;
  DIR *d;

  cctx->suffix = ' ';

  /* find basename */

  for (slash = cctx->end; *slash != '/' && slash > cctx->start; slash--)
    ;

  if (slash > cctx->start)
    {
      /* "base/xxx" */
      *slash = 0;
      d = opendir(cctx->start);
      *slash = '/';
      cctx->start = slash + 1;
    }
  else if (*slash == '/')
    {
      /* "/xxx" */
      d = opendir("/");
      cctx->start = slash + 1;
    }
  else
    {
      /* "" or "xxx" */
      d = opendir(".");
    }

  if (d == NULL)
    return NULL;

  while ((e = readdir(d)))
    {
      termui_bool_t matchok = 0;

      /* skip hidden files */
      if (e->d_name[0] == '.' && cctx->start[0] != '.')
	continue;

      /* try substring match */
      if (substr)
	{
	  if (termui_con_comp_substr(cctx, e->d_name))
	    matchok = 1;
	}

      /* try prefix match */
      if (termui_con_comp_match(cctx, e->d_name, NULL) > 0)
	{
	  matchok = 2;
	}
#ifdef HAVE_FNMATCH_H
      /* try pattern match */
      else if (cctx->start < cctx->end &&
	      !fnmatch(cctx->start, e->d_name, 0))
	{
	  matchok = 2;
	}
#endif
      if (matchok)
	{
	  termui_strlen_t len = strlen(e->d_name) + 1;

	  switch (e->d_type)
	    {
	    case DT_DIR:
	      cctx->suffix = '/';
	      break;

#ifdef HAVE_SYS_STAT_H
	    case DT_LNK: {
	      termui_strlen_t baselen = cctx->start - start;
	      char fullpath[baselen + 1 + len];
	      struct stat st;

	      memcpy(fullpath, start, baselen);
	      fullpath[baselen] = '/';
	      memcpy(fullpath + baselen + 1, e->d_name, len);
	      if (!stat(fullpath, &st) && S_ISDIR(st.st_mode))
		cctx->suffix = '/';
	    }
#endif
	    }

	  if (substr && matchok == 2)
	    {
	      substr = 0;
	      cctx->count = 0;
	      buf_use = 0;
	    }

	  if (buf_use + len > buf_len)
	    {
	      char *b;

	      buf_len += 256;
	      if (!(b = realloc(buf, buf_len)))
		continue;
	      if (b != buf)
		{
		  termui_comp_index_t i;

		  /* adjust previous entries on realloc */
		  for (i = 0; i < cctx->count; i++)
		    cctx->candidate[i] += b - buf;
		  buf = b;
		}
	    }

	  memcpy(buf + buf_use, e->d_name, len);
	  cctx->candidate[cctx->count++] = buf + buf_use;
	  buf_use += len;
	}
    }

  closedir(d);
  return buf;
}

