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
#include <assert.h>
#include <stdlib.h>

#include "console_pv.h"

static termui_bool_t termui_strnzcmp(const char *str, const char *asciiz, termui_strlen_t n)
{
  while (n && *asciiz && *str == *asciiz)
    str++, asciiz++, n--;

  return n || *asciiz;
}

static termui_strlen_t termui_con_next_arg(char **line, char quote)
{
  char *end = *line;
  char *start = end;

  while (1)
    {
      switch (*end)
	{
	case (' '):
	case ('\t'):
	case ('\n'):
	  if (quote)
	    break;
	  *line = end + 1;
	  return end - start;

	case ('\0'):
	  *line = end;
	  return end - start;

	case ('\''):
	case ('"'):
	  if (quote != *end)
	    break;
	  *line = end + 1;
	  return end - start;
	}

      end++;
    }
}

void termui_con_split_args(char *line, struct termui_con_parse_ctx_s *ctx)
{
  termui_arg_index_t argc = 0;

  while (*line && argc < TERMUI_CON_MAX_ARGS)
    {
      char q = 0;
      line += strspn(line, " \t\n");

      if (!*line)
	break;

      switch (*line)
	{
	case ('\''):
	case ('"'):
	  line++;
	  q = line[-1];

	default:
	  memset(ctx->attr + argc, 0, sizeof(ctx->attr[argc]));
	  ctx->argv[argc] = line;
	  ctx->attr[argc++].len = termui_con_next_arg(&line, q);
	}
    }

  ctx->argc = argc;
}

void
termui_con_parse_opts(const struct termui_con_entry_s *e,
		      struct termui_con_parse_ctx_s *ctx,
		      struct termui_con_ctx_s *con)
{
  struct termui_con_opts_s *desc = e->opts_desc;
  termui_arg_index_t i, k, l, paramc = 0;

  ctx->used = 0;
  ctx->needed = e->opts_mandatory & ~e->opts_disabled;
  ctx->excluded = e->opts_disabled;

  for (k = 0; k < ctx->argc; ++k)
    {
      struct termui_con_opts_s *opt = NULL;
      char *arg = ctx->argv[k];
      termui_strlen_t arglen = ctx->attr[k].len;
      termui_arg_sindex_t short_opt;

      l = k;

      if (desc == NULL || arg[0] != '-')
	{
	  /* add to arguments list */
	  /* not an option arg, argv untouched, opt = 0 */
	  ctx->attr[k].mask = paramc++;
	  continue;
	}

      if (arglen == 1)
	{
	  ctx->attr[k].type = TERMUI_CON_OPT_TYPE_1SC;
	  continue;
	}

      if (arg[1] == '-')
	{
	  if (arglen == 2)
	    {
	      /* '--' found, no more options */
	      desc = NULL;
	      ctx->attr[k].type = TERMUI_CON_OPT_TYPE_2SC;
	      continue;
	    }

	  /* long option lookup */
	  short_opt = -1;

	  for (i = 0; desc[i].id; i++)
	    {
	      if (desc[i].str_long &&
		  !(desc[i].id & e->opts_disabled) &&
#ifdef CONFIG_LIBTERMUI_CON_ACL
		  (~desc[i].acl & con->acl) &&
#endif
		  !termui_strnzcmp(arg, desc[i].str_long, arglen))
		break;
	    }
	}

      /* short option lookup */
      else
	{
	  short_opt = 1;
	next_short:
	  {
	    if (short_opt >= ctx->attr[l].len)
	      continue;

	    for (i = 0; desc[i].id; i++)
	      {
		if (desc[i].str_short &&
		    !(desc[i].id & e->opts_disabled) &&
#ifdef CONFIG_LIBTERMUI_CON_ACL
		    (~desc[i].acl & con->acl) &&
#endif
		    desc[i].str_short[1] == arg[short_opt])
		  break;
	      }
	  }
	}

      opt = desc + i;

      if (desc[i].id == 0 || /* not found */
	  ctx->attr[l].mask & opt->id)     /* short option repeated */
	{
	  ctx->attr[l].type = TERMUI_CON_OPT_TYPE_BAD;
	  continue;
	}

      ctx->attr[l].type = TERMUI_CON_OPT_TYPE_OPT;
      ctx->attr[l].mask |= opt->id;
      ctx->used |= opt->id;
      ctx->needed |= opt->depend & ~e->opts_disabled;
      ctx->excluded |= opt->exclude;

      for (i = 0; i < opt->param_cnt; i++)
	{
	  ctx->attr[++k].mask = opt->id;
	  ctx->attr[k].type = TERMUI_CON_OPT_TYPE_OPT_ARG;
	}

      if (short_opt++ > 0)
	{
	  opt = NULL;
	  goto next_short;
	}
    }
}

struct termui_con_opts_s *
termui_con_opt_form_id(const struct termui_con_entry_s *e,
		       termui_con_bits_t id)
{
  struct termui_con_opts_s	*desc;
  termui_arg_index_t i;

  if ((desc = e->opts_desc))
    {
      /* find option entry from mask */
      for (i = 0; desc[i].id != 0; i++)
	{
	  assert(!(desc[i].id & (desc[i].id - 1)));
	  if (desc[i].id == id)
	    return desc + i;
	}
    }

  return NULL;
}

static const char*
termui_con_opt_str(struct termui_con_opts_s *opt)
{
  return opt->str_long != NULL
    ? opt->str_long : opt->str_short;
}

static void
termui_con_opts_print(struct termui_con_ctx_s *con,
		      struct termui_con_opts_s *desc,
		      termui_con_bits_t mask)
{
  termui_arg_index_t j;

  for (j = 0; desc[j].id; j++)
    {
      if (desc[j].id & mask)
	{
	  if (desc[j].str_short != NULL)
	    {
	      termui_con_puts(con, desc[j].str_short);
	      termui_con_puts(con, " ");
	    }
	  if (desc[j].str_long != NULL)
	    {
	      termui_con_puts(con, desc[j].str_long);
	      termui_con_puts(con, " ");
	    }
	}
    }

  termui_con_printf(con, "%A\n");
}

termui_err_t
termui_con_check_opts(const struct termui_con_entry_s *e,
		      struct termui_con_parse_ctx_s *ctx,
		      void *argctx, struct termui_con_ctx_s *con)
{
  struct termui_con_opts_s *desc = e->opts_desc;
  termui_con_bits_t mask, used = 0;
  termui_bool_t err = 0;
  termui_arg_index_t k;

  ctx->paramc = 0;

  for (k = 0; k < ctx->argc; k++)
    {
      termui_strlen_t arglen = ctx->attr[k].len;
      mask = ctx->attr[k].mask;

      ctx->argv[k][arglen] = 0;

      switch (ctx->attr[k].type)
	{
	case TERMUI_CON_OPT_TYPE_BAD:
	  /* error, invalid option */
	  termui_con_printf(con, "error: Invalid option: `%1A%s%A'\n", ctx->argv[k]);
	  return -1;

	case TERMUI_CON_OPT_TYPE_1SC:
	case TERMUI_CON_OPT_TYPE_ARG:
	  /* non option argument */
	  ctx->argv[ctx->paramc++] = ctx->argv[k];

	case TERMUI_CON_OPT_TYPE_2SC:
	  continue;

	case TERMUI_CON_OPT_TYPE_OPT_ARG:
	  abort();

	case TERMUI_CON_OPT_TYPE_OPT:

	  while (mask)
	    {
	      struct termui_con_opts_s *opt;
	      termui_con_bits_t m;

	      if ((k + 1 < ctx->argc) &&
		  (ctx->attr[k + 1].type == TERMUI_CON_OPT_TYPE_OPT_ARG) &&
		  (ctx->attr[k + 1].mask & mask))
		/* handle option with contiguous associated arguments first */
		m = ctx->attr[k + 1].mask;
	      else
		/* pickup any matched option */
		m = termui_next_bit(mask);

	      opt = termui_con_opt_form_id(e, m);
	      assert(opt);

	      /* test dependency mask */
	      if (~ctx->used & opt->depend & ~e->opts_disabled)
		{
		  /* display list of excluded options */
		  termui_con_printf(con, "error: Use of `%1A%s%A' requires: %1A",
				    termui_con_opt_str(opt));
		  termui_con_opts_print(con, desc, opt->depend & ~e->opts_disabled);
		  err = 1;
		}

	      /* test exclusion mask */
	      if (ctx->used & opt->exclude & ~opt->id)
		{
		  /* display list of excluded options */
		  termui_con_printf(con, "error: Use of `%1A%s%A' not allowed with: %1A",
				    termui_con_opt_str(opt));
		  termui_con_opts_print(con, desc, opt->exclude & ~e->opts_disabled);
		  err = 1;
		}

	      /* test single use */
	      if (used & opt->exclude & opt->id)
		{
		  termui_con_printf(con, "error: Multiple use of `%1A%s%A' not allowed\n",
				    termui_con_opt_str(opt));
		  err = 1;
		}

	      if (opt->parse_args)
		{
		  termui_arg_index_t j;

		  if (ctx->argc - k - 1 < opt->param_cnt)
		    {
		      termui_con_printf(con, "error: at least %u argument%s expected to `%1A%s%A' option\n",
					opt->param_cnt, opt->param_cnt > 1 ? "s" : "",
					termui_con_opt_str(opt));
		      err = 1;
		      goto end_loop;
		    }

		  /* nul terminate option args before parse */
		  for (j = k + 1; j < k + opt->param_cnt + 1; j++)
		    ctx->argv[j][ctx->attr[j].len] = 0;

		  /* use parse handler for this option */
		  if (opt->parse_args(con, argctx, ctx->argv + k + 1))
		    {
		      termui_con_printf(con, "error: Invalid argument to `%1A%s%A' option\n", 
					termui_con_opt_str(opt));

		      err = 1;
		    }

		  /* skip to next arg */
		  k += opt->param_cnt;
		}

	      /* set entry exclusion bits */
	      mask ^= m;
	      used |= opt->id;
	    }
	}
    }

 end_loop:

  /* check arguments count */
  if (ctx->paramc > e->args_max)
    {
      termui_con_printf(con, "error: Too many arguments `%1A%s%A ...'\n", ctx->argv[e->args_max]);
      err = 1;
    }

  if (ctx->paramc < e->args_min)
    {
      termui_con_printf(con, "error: at least %u argument%s expected, %u found\n",
			e->args_min, e->args_min > 1 ? "s" : "", ctx->paramc);
      err = 1;
    }

  /* check mandatory options */
  mask = ~ctx->used & e->opts_mandatory & ~e->opts_disabled;

  if (mask)
    {
      termui_con_printf(con, "error: Missing option: %1A");
      termui_con_opts_print(con, desc, mask);
      err = 1;
    }

  return -err;
}

