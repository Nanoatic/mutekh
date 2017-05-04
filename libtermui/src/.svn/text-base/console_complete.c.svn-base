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

#include <termui/strings.h>
#include "console_pv.h"

void termui_con_complete_apply(struct termui_con_ctx_s *con,
			       struct termui_term_behavior_s *bhv,
			       struct termui_con_complete_ctx_s *ctx)
{
  if (ctx->count == 0)
    {
      /* no candidate */
      termui_term_beep(con->tm);
    }
  else
    {
      const char *c0 = ctx->candidate[0];
      termui_strlen_t len, min = strlen(c0);
      termui_strlen_t fixed_len = ctx->end - ctx->start;
      termui_comp_index_t i;

      /* get common max match length of candidates */
      for (i = 1; i < ctx->count; i++)
	{
	  len = termui_str_match_len(ctx->candidate[i], c0);
	  min = len < min ? len : min;
	}

      /* min contain candidate common prefix size */

      if (fixed_len < min)
	{
	  termui_strlen_t oklen = termui_str_match_len(ctx->start, c0);
	  termui_strlen_t dellen;

	  if (oklen > fixed_len)
	    oklen = fixed_len;	/* oklen contain lenght of correct text already in place */

	  if ((dellen = fixed_len - oklen))
	    {
	      termui_getline_move_backward(bhv, dellen); /* remove non matching tail if any */
	      termui_getline_delete(bhv, dellen);
	    }

	  /* complete with new tail */
	  termui_getline_insert(bhv, c0 + oklen, min - oklen);
	}

      ctx->start += min;

      if (ctx->count == 1)
	{
	  if (fixed_len <= min && ctx->suffix)
	    {
	      /* insert separator char at end of successfully completed token */
	      termui_getline_insert(bhv, &ctx->suffix, 1);
	      ctx->start++;
	    }
	}
      else if (fixed_len >= min)
	{
	  termui_cursor_pos_t w, h;

	  /* no completion possible, beep & display candidates list */
	  if (termui_term_getsize(con->tm, &w, &h))
	    w = 80;

	  termui_term_beep(con->tm);
	  termui_con_printf(con, "\n");

	  len = w;
	  for (i = 0; i < ctx->count; i++)
	    {
	      const char *space = "                 ";
	      const char *cand = ctx->candidate[i];
	      termui_strlen_t alen, clen = strlen(cand);

	      /* get aligned len */
	      alen = ((((clen) - 1) | ((16) - 1)) + 1);

	      if (alen + 1 > len)
		{
		  termui_con_printf(con, "\n");
		  len = w;
		}

	      termui_con_puts(con, cand);
	      termui_con_puts(con, space + 16 - (alen - clen));
	      len -= alen + 1;
	    }

	  termui_con_printf(con, "\n");
	  termui_getline_reprompt(bhv);
	}
    }
}

enum termui_con_comp_match_e
termui_con_comp_match(const struct termui_con_complete_ctx_s *pctx,
		      const char *token, termui_strlen_t *len_)
{
  termui_strlen_t len = termui_str_match_len(token, pctx->start);
  termui_strlen_t fixed_len = pctx->end - pctx->start;                 
  enum termui_con_comp_match_e res;                                    

  if (len < fixed_len)
    {                 
      res = token[len]
        ? termui_con_match_dont
        : termui_con_match_token;
    }                            
  else
    {                            
      /* strip match len to cursor */
      len = fixed_len;               

      res = token[len]
        ? termui_con_match_partial
        : termui_con_match_complete;
    }                               

  if (len_ != NULL)
    *len_ = len;   

  return res;
}

termui_bool_t
termui_con_comp_substr(struct termui_con_complete_ctx_s *ctx, const char *token)
{
  char end = *ctx->end;
  termui_bool_t res;

  if (ctx->start >= ctx->end)
    return 0;

  /* FIXME write a dedicated algorithm and impose minimal len */
  *ctx->end = 0;
  res = !!strstr(token, ctx->start);
  *ctx->end = end;
  return res;
}

static struct termui_con_entry_s *
termui_con_complete_cmd(struct termui_con_ctx_s *con,
			struct termui_con_complete_ctx_s *ctx)
{
  struct termui_con_entry_s *h, *e;
  termui_bool_t substring = 0;
  termui_bool_t matchok;

  ctx->suffix = 0;
  h = e = *con->root;

  while (e != NULL)
    {
      if (e->flag & TERMUI_CON_FLAG_HIDDEN)
	goto next;

#ifdef CONFIG_LIBTERMUI_CON_ACL
      if (!(~e->acl & con->acl))
	goto next;
#endif

      matchok = 0;

      if (substring)
	{
	  if (termui_con_comp_substr(ctx, e->cmd))
	    matchok = 1;
	}
      else
	{
	  termui_strlen_t len;

	  switch (termui_con_comp_match(ctx, e->cmd, &len))
	    {
	    case termui_con_match_token:
	    case termui_con_match_complete:
	      ctx->count = 0;

	      /* if full name match for group, jump to sub group list */
	      if (e->flag & TERMUI_CON_FLAG_ISGROUP)
		{
		  ctx->start += len;
		  h = e = e->u.subdir;
		  continue;
		}

	      /* if full name match a command, end command completion */
	      matchok = 2;
	      break;

	    case termui_con_match_partial:
	      /* partial match, add to candidate */
	      matchok = 1;
	      break;

	    case termui_con_match_dont:
	      break;
	    }
	}

      if (matchok)
	{
	  if (!(e->flag & TERMUI_CON_FLAG_ISGROUP))
	    ctx->suffix = ' ';
	  ctx->candidate[ctx->count++] = e->cmd;
	  if (ctx->count == CONFIG_LIBTERMUI_CON_MAXCOMPLETE)
	    return NULL;
	}

      if (matchok == 2)
	return e;

    next:

      /* try substring completion if still no candidates */
      if (e->next == NULL && ctx->count == 0)
	{
	  e = h;
	  h = NULL;
	  substring = 1;
	  continue;
	}

      e = e->next;
    }

  return NULL;
}

TERMUI_CON_ARGS_COLLECT_PROTOTYPE(termui_con_collect_cmd)
{
  termui_con_complete_cmd(con, cctx);

  return NULL;
}

static void
termui_con_complete_opt(struct termui_con_ctx_s *con,
			struct termui_con_entry_s *cmd,
			struct termui_con_complete_ctx_s *ctx,
			termui_con_bits_t mask)
{
  struct termui_con_opts_s *desc = cmd->opts_desc;
  termui_strlen_t len = ctx->end - ctx->start;
  termui_comp_index_t i;

  if (desc == NULL)
    return;

  for (i = 0; ctx->count < CONFIG_LIBTERMUI_CON_MAXCOMPLETE; i++)
    {
      struct termui_con_opts_s *opt = desc + i;

      if (!opt->id)
	break;

      if (!(opt->id & mask))
	continue;

#ifdef CONFIG_LIBTERMUI_CON_ACL
      if (!(~opt->acl & con->acl))
	continue;
#endif

      if (opt->str_long != NULL &&
	  /* not a candidate if match length do not reach cursor position */
	  termui_str_match_len(opt->str_long, ctx->start) >= len)
	ctx->candidate[ctx->count++] = opt->str_long;

      if ((len > 1 || opt->str_long == NULL) && opt->str_short != NULL &&
	  termui_str_match_len(opt->str_short, ctx->start) >= len)
	ctx->candidate[ctx->count++] = opt->str_short;
    }
}

TERMUI_GETLINE_FCN_COMPLETE(termui_con_complete)
{
  struct termui_con_ctx_s	*con = priv;
  struct termui_con_entry_s	*cmd;
  struct termui_con_parse_ctx_s pctx;
  struct termui_con_complete_ctx_s cctx = { };
  struct termui_con_complete_func_s *cfunc;
  termui_con_bits_t id, missing;
  termui_comp_sindex_t i;
  termui_bool_t opt_fallback = 0;

  /* find/complete command at line start */
  cctx.start = (char*)termui_getline_line_start(bhv);
  cctx.end = (char*)termui_getline_line_cursor(bhv);
  cctx.start += strspn(cctx.start, " \t"); /* skip blank */

  cmd = termui_con_complete_cmd(con, &cctx);
  termui_con_complete_apply(con, bhv, &cctx);

  cctx.suffix = ' ';

  if (cmd == NULL)
    return;

  /* parse/complete arguments */
  termui_con_split_args(cctx.start, &pctx);
  termui_con_parse_opts(cmd, &pctx, con);

  /* missing options mask */
  missing = pctx.needed & ~pctx.used;

  for (i = -1; i + 1 < pctx.argc; i++)
    if (cctx.end <= pctx.argv[i + 1])
      break;
  /* i is edited argv index, -1 if before first arg */

  /* default: we are completing before any arg */
  cfunc = &cmd->complete;
  cctx.count = 0;
  cctx.start = cctx.end;
  id = 0;

  if (i < 0)
    {
      /* before first arg */
      if (missing)
	goto missing_opt;
    }
  else
    {
      if (cctx.end <= pctx.argv[i] + pctx.attr[i].len)
	{
	  /* we are completing inside the i arg */
	  cctx.start = pctx.argv[i];

	  switch (pctx.attr[i].type)
	    {
	    case TERMUI_CON_OPT_TYPE_ARG:
	      /* complete non option arg */
	      opt_fallback = 1;
	      id = pctx.attr[i].mask;
	      if (id >= cmd->args_max)
		cfunc = NULL;
	      break;

	    case TERMUI_CON_OPT_TYPE_1SC:
	    case TERMUI_CON_OPT_TYPE_2SC:
	    case TERMUI_CON_OPT_TYPE_OPT:
	    case TERMUI_CON_OPT_TYPE_BAD: {
	      /* complete option name */
	      termui_con_complete_opt(con, cmd, &cctx, ~pctx.excluded);
	      cfunc = NULL;
	      break;
	    }

	    case TERMUI_CON_OPT_TYPE_OPT_ARG: {
	      /* complete option extra arg */
	      struct termui_con_opts_s *opt = termui_con_opt_form_id(cmd, pctx.attr[i].mask);
	      cfunc = opt ? &opt->complete : NULL;

	      while (pctx.attr[i - 1 - id].type == TERMUI_CON_OPT_TYPE_OPT_ARG)
		id++;
	      break;
	    }

	    }
	}
      else
	{
	  /* we are completing after the i arg */

	  switch (pctx.attr[i].type)
	    {
	    case TERMUI_CON_OPT_TYPE_OPT_ARG:
	      /* find extra arg index */
	      while (pctx.attr[i - id].type == TERMUI_CON_OPT_TYPE_OPT_ARG)
		id++;

	    case TERMUI_CON_OPT_TYPE_OPT: {
	      struct termui_con_opts_s *opt = termui_con_opt_form_id(cmd, pctx.attr[i].mask);
	      if (!opt)
		return;
	      if (id < opt->param_cnt) {
		cfunc = &opt->complete;
		break;
	      }

	    case TERMUI_CON_OPT_TYPE_BAD:
	      if (missing)
		goto missing_opt;

	      opt_fallback = 1;
	      /* assume next arg is not option related, find index */
	      for (id = 0; !id && i > 0; i--)
		if (pctx.attr[i - 1].type == TERMUI_CON_OPT_TYPE_ARG)
		  id = pctx.attr[i - 1].mask + 1;
	      if (id >= cmd->args_max)
		cfunc = NULL;
	      break;

	    case TERMUI_CON_OPT_TYPE_1SC:
	    case TERMUI_CON_OPT_TYPE_2SC:
	    case TERMUI_CON_OPT_TYPE_ARG:
	      if (missing)
		goto missing_opt;

	      /* complete empty next non option arg */
	      opt_fallback = 1;
	      id = pctx.attr[i].mask + 1;
	      if (id >= cmd->args_max)
		cfunc = NULL;
	      break;

	      /* insert missing required option */
	      missing_opt:
	      termui_con_complete_opt(con, cmd, &cctx, termui_next_bit(missing));
	      cfunc = NULL;
	    }

	    }
	}
    }

  {
    void *data = NULL;

    if (cfunc != NULL && cfunc->collect != NULL)
	data = cfunc->collect(con, &cctx, id);

    if (cctx.count > 0)
      {
	termui_con_complete_apply(con, bhv, &cctx);
	if (cfunc != NULL && cfunc->cleanup != NULL)
	  cfunc->cleanup(con, data, id);
      }
    else if (opt_fallback)
      {
	/* fallback to options completion */
	termui_con_complete_opt(con, cmd, &cctx, ~pctx.excluded);
	termui_con_complete_apply(con, bhv, &cctx);
      }

  }
}

