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

#ifndef CONSOLE_PV_H_
#define CONSOLE_PV_H_

#include <unistd.h>

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <termui/types.h>
#include <termui/term.h>
#include <termui/getline.h>

#include <termui/bhv.h>
#include <termui/console.h>

#ifdef CONFIG_LIBTERMUI_CON_ACL
# define TERMUI_COND_CON_ACL(x) x
#else
# define TERMUI_COND_CON_ACL(x) 0
#endif

struct				termui_con_ctx_s
{
  struct termui_term_behavior_s	*bhv;
  struct termui_term_s			*tm;
  void				*pv;
#ifdef CONFIG_LIBTERMUI_CON_ACL
  termui_con_bits_t		acl;
#endif
  struct termui_con_entry_s	**root;
  const char			*prompt;
};

/*
 * termui_con_complete.c
 */

TERMUI_GETLINE_FCN_COMPLETE(termui_con_complete);

/** return true if power of two */
static inline termui_bool_t
termui_is_powtwo(termui_con_bits_t x)
{
  return !(x & (x - 1));
}

/** return next bit set only */
static inline termui_con_bits_t
termui_next_bit(termui_con_bits_t x)
{
  return x & ~(x - 1);
}


#define TERMUI_CON_MAX_ARGS		64
#define TERMUI_CON_MAX_OPTS		32

/* 
 * termui_con_args.c
 */

struct termui_con_parse_ctx_s
{
  /* input args count, strings, lenghts */
  termui_arg_index_t		argc;
  char				*argv[TERMUI_CON_MAX_ARGS];
  struct {
    /* option mask or non option arg index */
    termui_con_bits_t	mask:TERMUI_CON_MAX_OPTS;
    /* arg string len */
    termui_con_bits_t	len:13;
    /* arg type */
    termui_con_bits_t	type:3;
  }				attr[TERMUI_CON_MAX_ARGS];

  /* parsed non option args count */
  termui_arg_index_t		paramc;
  /* parsed options */
  termui_con_bits_t		used;
  termui_con_bits_t		needed;
  termui_con_bits_t		excluded;
};

enum {
  TERMUI_CON_OPT_TYPE_ARG, /* non option argument */
  TERMUI_CON_OPT_TYPE_OPT, /* option first argument */
  TERMUI_CON_OPT_TYPE_OPT_ARG, /* option extra argument */
  TERMUI_CON_OPT_TYPE_BAD,     /* unrecognized option */
  TERMUI_CON_OPT_TYPE_2SC,     /* -- */
  TERMUI_CON_OPT_TYPE_1SC,     /* - */
};

termui_err_t
termui_con_check_opts(const struct termui_con_entry_s *e,
		      struct termui_con_parse_ctx_s *ctx,
		      void *argctx, struct termui_con_ctx_s *con);

void
termui_con_split_args(char *line, struct termui_con_parse_ctx_s *ctx);

void
termui_con_parse_opts(const struct termui_con_entry_s *e,
		      struct termui_con_parse_ctx_s *ctx,
		      struct termui_con_ctx_s *con);

struct termui_con_opts_s *
termui_con_opt_form_id(const struct termui_con_entry_s *e,
		       termui_con_bits_t mask);

struct termui_con_entry_s *
termui_con_find_entry(struct termui_con_ctx_s *con,
		      struct termui_con_entry_s *root,
		      char **path_);

#endif /* CONSOLE_H_ */

