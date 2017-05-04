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

#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "console_pv.h"

#ifdef HAVE_ALLOCA_H
# include <alloca.h>
#endif

#ifdef CONFIG_LIBTERMUI_FILEOPS
# include <stdio.h>
#endif

termui_err_t
termui_con_process(struct termui_con_ctx_s *con)
{
  const char		*rline;
  char			line_[CONFIG_LIBTERMUI_CON_MAXLINELEN], *line = line_;
  termui_err_t		res;

  if (!(rline = termui_getline_process(con->bhv)))
    return -EIO;

  /* skip blank line */
  if (!*(rline += strspn(rline, " \t\n")))
    return 0;

  /* skip comment */
  if (*rline == '#')
    return 0;

  termui_getline_history_addlast(con->bhv);

  /* copy line to writable buffer for args spliting */
  strncpy(line, rline, CONFIG_LIBTERMUI_CON_MAXLINELEN);

  res = termui_con_execute(con, line);
  termui_con_printf(con, "\n");

  return res;
}

termui_err_t
termui_con_execute(struct termui_con_ctx_s *con, char *line)
{
  struct termui_con_parse_ctx_s ctx;
  struct termui_con_entry_s *e;
  struct termui_con_entry_s *root = *con->root;
  void *argctx = NULL;
  termui_err_t res = 0;

  if (!(e = termui_con_find_entry(con, root, &line)))
    {
      termui_con_printf(con, "error: Command not found.\n");
      return -ENOSYS;
    }

#ifdef CONFIG_LIBTERMUI_CON_ALIAS
  while (e->flag & TERMUI_CON_FLAG_ISALIAS)
    {
      const char *path = e->desc;

      if (!(e = termui_con_find_entry(con, root, (char**)&path)))
	{
	  termui_con_printf(con, "error: Invalid alias to `%1A%s%A'.\n", path);
	  return -ENOSYS;
	}
    }
#endif

  if (!(e->flag & TERMUI_CON_FLAG_ISCMD))
    {
      termui_con_printf(con, "error: `%1A%s%A' is not a command.\n", e->cmd);
      return -ENOSYS;
    }

#ifdef CONFIG_LIBTERMUI_CON_ACL
  if (!(~e->acl & con->acl))
    {
      termui_con_printf(con, "error: Access denied to `%1A%s%A' command.\n", e->cmd);
      return -EACCES;
    }
#endif

  termui_con_split_args(line, &ctx);

  /* allocate args parsing context */
  if (e->opts_ctx_size)
    {
      argctx = alloca(e->opts_ctx_size);
      memset(argctx, 0, e->opts_ctx_size);
    }

  termui_con_parse_opts(e, &ctx, con);

  /* parse args using args descriptors */
  if (termui_con_check_opts(e, &ctx, argctx, con))
    res = -ENOSYS;

  if (!res)
    res = e->u.process(con, argctx, ctx.paramc, ctx.argv, ctx.used);

  /* arg context cleanup callback */
  if (e->opts_cleanup)
    e->opts_cleanup(con, argctx, ctx.used);

  return res;
}

#ifdef CONFIG_LIBTERMUI_FILEOPS
termui_err_t termui_con_execute_file(struct termui_con_ctx_s *con,
				     const char *file, termui_bool_t err_abort)
{
  char		buffer[CONFIG_LIBTERMUI_CON_MAXLINELEN + 1];
  char		*line;
  termui_size_t	n = 1;
  FILE		*in;

  /* open script file */
  if (!(in = fopen(file, "r")))
    {
      termui_con_printf(con, "error: Unable to open `%s' file\n", file);
      return -ENOENT;
    }

  /* read line from file */
  while ((line = fgets(buffer, CONFIG_LIBTERMUI_CON_MAXLINELEN, in)))
    {
      line[CONFIG_LIBTERMUI_CON_MAXLINELEN] = '\0';

      /* skip blank line and comments */
      line += strspn(line, " \t\n");

      if (*line == '\0' || *line == '#')
	continue;

      /* execute command */
      if (termui_con_execute(con, line) < 0)
	{
	  termui_con_printf(con, "error: Batch command execution failed at `%s:%u'\n", file, n);

	  /* abort on error */
	  if (err_abort)
	    return -ECANCELED;
	}

      n++;
    }

  fclose(in);

  return 0;
}
#endif

