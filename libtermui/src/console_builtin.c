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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "console_pv.h"

static inline const char*
termui_con_null_str(const char *str)
{
  return str ? str : "";
}

static void
termui_con_usage(struct termui_con_ctx_s *con,
		 const struct termui_con_entry_s *e)
{
  termui_con_printf(con,
		    "\n"
		    "%1A%s%A - %s\n",
		    e->cmd, termui_con_null_str(e->desc));

  if (e->longdesc)
    {
      termui_con_printf(con,
		     "\n"
		     "%1ADescription%A: \n");
      termui_con_printf(con, e->longdesc);
      termui_con_printf(con, "\n");
    }
}

static void
termui_con_opts_usage(struct termui_con_ctx_s *con,
		      const struct termui_con_entry_s *e)
{
  struct termui_con_opts_s *desc = e->opts_desc;
  termui_arg_index_t i;
  termui_bool_t done = 0;

  if (desc == NULL)
    return;

  for (i = 0; desc[i].id; i++)
    {
      struct termui_con_opts_s *opt = desc + i;

      if ((opt->id & e->opts_disabled))
	continue;		/* skip entry disabled options */

#ifdef CONFIG_LIBTERMUI_CON_ACL
      if (!(~opt->acl & con->acl))
	continue;
#endif

      if (!done++)
	termui_con_printf(con,
			  "\n"
			  "%1AOptions%A:\n");

      termui_con_printf(con, "  %-5s%-16s%s\n",
			termui_con_null_str(opt->str_short),
			termui_con_null_str(opt->str_long),
			termui_con_null_str(opt->desc));
    }

}

/************************************************************************/

TERMUI_CON_COMMAND_PROTOTYPE(termui_con_builtin_help)
{
  struct termui_con_entry_s	*e = NULL;

  /* show detailed help about command(s) */

  if (argc && (e = termui_con_find_entry(con, *con->root, argv)))
    {
      switch (e->flag & TERMUI_CON_FLAG_TYPE)
	{
	case TERMUI_CON_FLAG_ISCMD:
	  termui_con_usage(con, e);
	  termui_con_opts_usage(con, e);
	  return 0;

	case TERMUI_CON_FLAG_ISGROUP:
	  termui_con_usage(con, e);
	  return 0;

	case TERMUI_CON_FLAG_ISALIAS:
	  termui_con_printf(con, "\n"
			    "%1A%s%A is an alias for %1A%s%A.\n", e->cmd, e->desc);
	  return 0;
	}
    }

  if (e == NULL)
    termui_con_printf(con, "error: No such entry.\n");

  /* show general help message */
  termui_con_printf(con,
		    /*********************************** 79 cols ***********************************/
		    "\n"
		    "Type `%1Alist%A [group]' to show available commands list.\n"
		    "Type `%1Ahelp%A [command ...]' to display detailed command help.\n"
		    "\n"
		    "This console user interface is driven by libtermui (C) 2005-2010 A.Becoulet\n"
		    );

  return 0;
}

/************************************************************************/

/* 
 *  Write available commands table to console
*/

static termui_bool_t
termui_con_command_table_r(struct termui_con_ctx_s *con,
			   struct termui_con_entry_s *root,
			   uint_fast8_t depth, const char *prefix,
			   termui_con_bits_t flag_mask)
{
  char		buf[256];
  termui_bool_t n = 0;

  for (; root; root = root->next)
    {
      const char *description = "No description available";

      if (!(root->flag & flag_mask))
	continue;

      if (root->desc)
	description = root->desc;

      snprintf(buf, 256, "%s%s",
	       prefix, root->cmd);

      if (root->flag & TERMUI_CON_FLAG_ISGROUP)
	{
#ifdef CONFIG_LIBTERMUI_CON_ACL
	  if (~root->acl & con->acl)
	    {
#endif
	      if (n)
		termui_con_printf(con, "\n");
	      termui_con_command_table_r(con, root->u.subdir, depth + 1, buf, flag_mask);
	      n = 0;
#ifdef CONFIG_LIBTERMUI_CON_ACL
	    }
	  else
	    termui_con_printf(con, "\n  %31A%-32s%A %36AAccess denied%a\n\n", buf);
#endif
	}
#ifdef CONFIG_LIBTERMUI_CON_ALIAS
      else if (root->flag & TERMUI_CON_FLAG_ISALIAS)
	{
	  termui_con_printf(con, "  %33A%-32s%A %36AAlias to %A%-19s\n",
			 buf, description);
	  n = 1;
	}
#endif
      else
	{
#ifdef CONFIG_LIBTERMUI_CON_ACL
	  termui_con_bits_t acl = ~root->acl & con->acl;

	  if (acl)
	    {
#endif
	      termui_con_printf(con, "  %-32s %36A%-32s%A\n", buf, description);
	      n = 1;
#ifdef CONFIG_LIBTERMUI_CON_ACL
	    }
	  else if (flag_mask & TERMUI_CON_FLAG_DENIED)
	    {
	      termui_con_printf(con, "  %31A%-32s%A %36AAccess denied%A\n", buf);
	      n = 1;
	    }
#endif
	}
    }

  if (n)
    termui_con_printf(con, "\n");

  return n;
}

TERMUI_CON_OPT_DECL(termui_con_builtin_list_opts) =
{
#ifdef CONFIG_LIBTERMUI_CON_ALIAS
  TERMUI_CON_OPT_ENTRY("-s", "--alias", 0x1,
		   TERMUI_CON_HELP("Show aliases", NULL)),
#endif
#ifdef CONFIG_LIBTERMUI_CON_ACL
  TERMUI_CON_OPT_ENTRY("-d", "--denied", 0x2,
		   TERMUI_CON_HELP("Show unavailable entries", NULL)),
#endif
  TERMUI_CON_LIST_END
};

TERMUI_CON_COMMAND_PROTOTYPE(termui_con_builtin_list)
{
  termui_con_bits_t flag_mask = TERMUI_CON_FLAG_ISCMD | TERMUI_CON_FLAG_ISGROUP;
  struct termui_con_entry_s *root = *con->root;

#ifdef CONFIG_LIBTERMUI_CON_ALIAS
  if (used & 0x1)
    flag_mask |= TERMUI_CON_FLAG_ISALIAS;
#endif
#ifdef CONFIG_LIBTERMUI_CON_ACL
  if (used & 0x2)
    flag_mask |= TERMUI_CON_FLAG_DENIED;
#endif

  if (argc)
    {
      char *path = *argv;
      root = termui_con_find_entry(con, root, &path);

      if (root == NULL || !(root->flag & TERMUI_CON_FLAG_ISGROUP))
	{
	  termui_con_printf(con, "error: No such group `%1A%s%A'.\n", *argv);
	  return -ENOENT;
	}
    }

  termui_con_printf(con,
		 "\n"
		 "  %1A%-32s %-35s%A\n"
		 "--------------------------------------------------------------------------------\n",
		 "Command path", "Description");

  termui_con_command_table_r(con, root, 0, "", flag_mask);

  return 0;
}

/************************************************************************/

#ifdef CONFIG_LIBTERMUI_CON_ALIAS

TERMUI_CON_COMMAND_PROTOTYPE(termui_con_builtin_alias)
{
  char *name;
  struct termui_con_entry_s *alias;

  if (argc > 1)
    name = argv[1];
  else if ((name = strrchr(argv[0], '.')))
    name++;
  else
    {
      termui_con_printf(con, "error: Can not self alias\n");
      return -ENOENT;
    }

  if (!(alias = malloc(sizeof(*alias))))
    return -ENOENT;

  alias->flag = TERMUI_CON_FLAG_ISALIAS | TERMUI_CON_FLAG_ALLOCATED;
  alias->desc = strdup(argv[0]);
  alias->longdesc = NULL;
  strncpy(alias->cmd, name, CONFIG_LIBTERMUI_CON_MAXCMDLEN);
  alias->cmd[CONFIG_LIBTERMUI_CON_MAXCMDLEN] = 0;

  termui_con_append(con->root, alias, 1);

  return 0;
}

#endif

/************************************************************************/

#ifdef CONFIG_LIBTERMUI_FILEOPS

TERMUI_CON_COMMAND_PROTOTYPE(termui_con_builtin_batch)
{
  termui_arg_index_t i;
  termui_err_t res = 0;

  for (i = 0; i < argc; i++)
    if (termui_con_execute_file(con, argv[i], 1) == -ENOENT)
      res = -ENOENT;

  return res;
}

#endif

/************************************************************************/

TERMUI_CON_COMMAND_PROTOTYPE(termui_con_builtin_quit)
{
  return -ECANCELED;
}

