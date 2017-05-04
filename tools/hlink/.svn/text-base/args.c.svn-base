/*

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
    02110-1301 USA

    Copyright Alexandre Becoulet <alexandre.becoulet@free.fr>

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "args.h"

char				**args_param_list_g;

/*
** command line arguments handling functions
*/

static void args_disp_copyright()
{
  printf("%s\n"
	 "Copyright %s\n\n",
	 args_title_g, args_copyright_g);
}

static void args_disp_option(unsigned int i)
{
  printf("    %-8s%-16s%s\n",
	 argslist_g[i].str_short, argslist_g[i].str_long,
	 argslist_g[i].explanation);
}

static void args_disp_usage()
{
  unsigned int		i;

  printf("Usage: %s\n"
	 "List of available options: \n",
	 args_usage_g);

  for (i = 0; argslist_g[i].str_short; i++)
    args_disp_option(i);
}

int args_help(char **dummy)
{
  args_disp_copyright();
  args_disp_usage();
  return 1;
}

unsigned int args_exclude;

int		args_parse(int		argc,
			   char		**argv)
{
  int			args_ok = 1;
  unsigned int		param_list_cnt = 0;
  
  args_exclude = 0;

  if (!(args_param_list_g = (char**)calloc((argc + 1), sizeof (char **))))
    return 1;

  while (argc)
    {
      unsigned int	i;

      /* look for possible option in args list */

      if (args_ok)
	{
	  if (!strcmp(*argv, "--"))
	    {
	      args_ok = 0;
	      argc--, argv++;
	      continue;
	    }
	  for (i = 0; argslist_g[i].str_short; i++)
	    if (**argv && (!strcmp(argslist_g[i].str_short, *argv)
			   || !strcmp(argslist_g[i].str_long, *argv)))
	      break;
	}

      if (args_ok && argslist_g[i].str_short)
	{
	  
	  if (argc < argslist_g[i].param_count + 1)
	    {
	      /* error, invalid option usage */
	      fprintf(stderr, "Invalid use of %s option, try --help.\n", *argv);
	      return (1);
	    }

	  /* test exclusion mask */
	  if (args_exclude & argslist_g[i].exclude_mask)
	    {
	      int	j;

	      /* display list of excluded options */
	      fprintf(stderr, "Use of %s not allowed with: ", *argv);
	      for (j = 0; argslist_g[j].str_short; j++)
		if (argslist_g[i].exclude_mask
		    & argslist_g[j].exclude_mask)
		  fprintf(stderr, "%s %s ", argslist_g[j].str_short,
			  argslist_g[j].str_long);
	      fprintf(stderr, "\n");
	      return (1);
	    }
	  else
	    /* set entry exclusion bits */
	    args_exclude |= argslist_g[i].exclude_mask;

	  /* determine action for this arg */
	  switch (argslist_g[i].action)
	    {
	    case args_varint_set:	/* int set */
	      if (argslist_g[i].param_count)
		*argslist_g[i].value_ptr.num = strtol(argv[1], 0, 0);
	      else
		*argslist_g[i].value_ptr.num |= argslist_g[i].value;
	      break;

	    case args_vardbl_set:
	      *argslist_g[i].value_ptr.dbl = strtod(argv[1], 0);
	      break;

	    case args_varstr_set:	/* string set */
	      *argslist_g[i].value_ptr.str = argv[1];
	      break;

	    case args_func_call:	/* function call */
	      if (argslist_g[i].value_ptr.func(argv))
		return (1);
	      break;

	    case args_no_action:
	      ;
	    }

	  /* skip to next arg */
	  argc -= argslist_g[i].param_count + 1;
	  argv += argslist_g[i].param_count + 1;

	}
      else
	{
	  if (args_ok && (**argv == '-'))
	    {	  
	      /* error, invalid option */
	      fprintf(stderr, "Invalid option: %s, try --help.\n", *argv);
	      return (1);
	    }
	  else
	    /* add to param list */
	    args_param_list_g[param_list_cnt++] = *argv;
	  argc--, argv++;
	}

    }

  return (0);
}

int args_check_mandatory(unsigned int mask)
{
  unsigned int i;

  mask &= ~args_exclude;

  if (!mask)
    return 0;

  fprintf(stderr, "Some madatory options are missing, choose among:\n");

  for (i = 0; argslist_g[i].str_short; i++)
    if (argslist_g[i].exclude_mask & mask)
      args_disp_option(i);

  return 1;
}

