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

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>

#include "term_pv.h"
#include "console_pv.h"

#ifdef CONFIG_LIBTERMUI_FILEOPS
# include <stdio.h>
#endif

#ifdef HAVE_ALLOCA_H
# include <alloca.h>
#endif

static TERMUI_GETLINE_FCN_PROMPT(termui_con_prompt)
{
  struct termui_con_ctx_s *con = priv;

  return termui_term_printf(con->tm, con->prompt);
}

struct termui_con_ctx_s *
termui_con_alloc(struct termui_con_entry_s **root,
		 termui_iostream_t in, termui_iostream_t out,
		 const char *type)
{
  struct termui_con_ctx_s *con;

  if (!(con = malloc(sizeof (struct termui_con_ctx_s))))
    goto err;

  if (!(con->tm = termui_term_alloc(in, out, con)))
    goto err_free;

  termui_term_set(con->tm, type);

  if (!(con->bhv = termui_getline_alloc(con->tm, CONFIG_LIBTERMUI_CON_MAXLINELEN)))
    goto err_term;

#ifdef CONFIG_LIBTERMUI_CON_COMPLETION
  termui_getline_complete_init(con->bhv, termui_con_complete);
#endif

  termui_getline_history_init(con->bhv, 64);

#ifdef CONFIG_LIBTERMUI_CON_ACL
  con->acl = -1;
#endif
  con->root = root;
  con->prompt = "$ ";

  termui_getline_setprompt(con->bhv, termui_con_prompt);

  return con;

#if 0
 err_bhv:
  termui_getline_free(con->bhv);
#endif
 err_term:
  termui_term_free(con->tm);
 err_free:
  free(con);
 err:
  return NULL;
}

/* 
 *  Free console ressources
 */

void
termui_con_free(struct termui_con_ctx_s *con)
{
  termui_getline_free(con->bhv);
  termui_term_free(con->tm);
  free(con);
}

/* Set console prompt string */

void termui_con_set_prompt(struct termui_con_ctx_s *con,
			   const char *prompt)
{
  con->prompt = prompt;
}

#ifdef HAVE_ARPA_TELNET_H
/* do basic telnet protocol handshaking */

void termui_con_telnet_setup(struct termui_con_ctx_s *con)
{
  termui_term_telnet_bhv_init(con->bhv);
  termui_term_telnet_send_setup(con->tm);
}
#endif

/* 
 *  Wait for user entry and return line content
 */

const char *
termui_con_input(struct termui_con_ctx_s *con,
		 const char *prompt)
{
  const char	*old_prompt, *res;

  old_prompt = con->prompt;
  if (prompt != NULL)
    con->prompt = prompt;
  res = termui_getline_process(con->bhv);
  con->prompt = old_prompt;

  return res;
}

termui_ssize_t
termui_con_printf(struct termui_con_ctx_s *con,
		  const char *fmt, ...)
{
  termui_ssize_t res = 0;
  va_list list;

  if (con)
    {
      va_start(list, fmt);
      res = termui_term_printf_va(con->tm, fmt, list);
      va_end(list);
    }

  return res;
}

termui_ssize_t termui_con_puts(struct termui_con_ctx_s *con, const char *string)
{
  return con ? termui_term_puts(con->tm, string) : 0;
}

void *
termui_con_get_private(struct termui_con_ctx_s *con)
{
  return con->pv;
}

void
termui_con_set_private(struct termui_con_ctx_s *con,
		       void *pv)
{
  con->pv = pv;
}

#ifdef CONFIG_LIBTERMUI_CON_ACL
void
termui_con_set_acl(struct termui_con_ctx_s *con,
		   termui_con_bits_t acl_mask)
{
  con->acl = acl_mask;
}
#endif

#ifdef CONFIG_LIBTERMUI_FILEOPS

termui_err_t termui_con_save_history(struct termui_con_ctx_s *con, FILE *file)
{
  const char *str;
  termui_size_t i = 0;

  while ((str = termui_getline_history_get(con->bhv, i++)) != NULL)
    fprintf(file, "%s\n", str);

  return TERMUI_TERM_RET_OK;
}

termui_err_t termui_con_load_history(struct termui_con_ctx_s *con, FILE *file)
{
  char buf[CONFIG_LIBTERMUI_CON_MAXLINELEN];

  while (fgets(buf, CONFIG_LIBTERMUI_CON_MAXLINELEN, file) != NULL)
    {
      buf[strlen(buf) - 1] = '\0';
      termui_getline_history_add(con->bhv, buf);
    }

  return 0;
}

#endif

