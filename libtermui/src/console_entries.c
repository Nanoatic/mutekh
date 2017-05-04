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
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include "console_pv.h"

static void
termui_con_register_tree(struct termui_con_entry_s **root,
			 struct termui_con_entry_s *list,
			 termui_size_t max_count);

static struct termui_con_entry_s **
termui_con_register_single(struct termui_con_entry_s **root,
			   struct termui_con_entry_s *entry)
{
  struct termui_con_entry_s	**p = root;

  if (!(entry->flag & TERMUI_CON_FLAG_REGISTERED))
    {
      entry->next = NULL;
      entry->prev = p;
      if (root)
	*root = entry;

      entry->flag |= TERMUI_CON_FLAG_REGISTERED;

      if (entry->flag & TERMUI_CON_FLAG_ISGROUP)
	termui_con_register_tree(NULL, entry->u.subdir, -1);
    }

  return &entry->next;
}

/* 
 * Update command tree links and setup root. Old root is discarded
 */

static void
termui_con_register_tree(struct termui_con_entry_s **root,
		      struct termui_con_entry_s *list,
		      termui_size_t max_count)
{
  struct termui_con_entry_s	*e;

  /* setup local linked list */

  for (e = list; e->flag && max_count--; e++)
    root = termui_con_register_single(root, e);

  if (root)
    *root = NULL;
}

/* 
 * Append a command tree at end of current root list.
 */

void
termui_con_append(struct termui_con_entry_s **root,
			 struct termui_con_entry_s *list,
			 termui_size_t max_count)
{
  struct termui_con_entry_s	*i, **p = root;

  for (i = *root; i; i = i->next)
    p = &i->next;

  termui_con_register_tree(p, list, max_count);
}

void
termui_con_register(struct termui_con_ctx_s *con,
		    struct termui_con_entry_s *list)
{
  termui_con_append(con->root, list, -1);
}

/* 
 * Unregister a single entry and return next
 */

static inline struct termui_con_entry_s *
termui_con_unregister_entry(struct termui_con_entry_s *e)
{
  struct termui_con_entry_s *next = e->next;

  if (next != NULL)
    next->prev = e->prev;

  if (e->prev != NULL)
    *e->prev = next;

  if (e->flag & TERMUI_CON_FLAG_ALLOCATED)
    {
      if (e->desc)
	free((void*)e->desc);
      if (e->longdesc)
	free((void*)e->longdesc);
      free(e);
    }

  return next;
}

/* 
 * Unregsiter a command tree
 */

void
termui_con_unregister(struct termui_con_entry_s *list)
{
  struct termui_con_entry_s	*e;

  /* setup local linked list */

  for (e = list; e->flag; e++)
    {
      if (!(e->flag & TERMUI_CON_FLAG_REGISTERED))
	continue;

      e->flag &= ~TERMUI_CON_FLAG_REGISTERED;

      if (e->flag & TERMUI_CON_FLAG_ISGROUP)
	termui_con_unregister(e->u.subdir);

      termui_con_unregister_entry(e);
    }
}

static inline termui_bool_t
termui_con_cmdname_char(char c)
{
  return isalnum(c) || c == '_' || c == '.';
}

struct termui_con_entry_s *
termui_con_find_entry(struct termui_con_ctx_s *con,
		      struct termui_con_entry_s *root,
		      char **path_)
{
  char *path = *path_;

  /* FIXME can not return groups */

  while (root)
    {
      termui_strlen_t len = strlen(root->cmd);

#ifdef CONFIG_LIBTERMUI_CON_ACL
      if (~root->acl & con->acl)
#endif
	if (!strncmp(root->cmd, path, len))
	  {
	    path += len;

	    if (root->flag & TERMUI_CON_FLAG_ISGROUP)
	      {
		root = root->u.subdir;
		continue;
	      }

	    if (termui_con_cmdname_char(*path))
	      return NULL;

	    *path_ = path;
	    return root;
	  }

      root = root->next;
    }

  return NULL;
}

