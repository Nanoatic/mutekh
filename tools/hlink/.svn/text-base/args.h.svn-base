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

#ifndef ARGS_H_
# define ARGS_H_

/*
** action to be performed if an arg is matched
*/

enum			args_action_e
{
  args_no_action,
  args_varint_set,
  args_vardbl_set,
  args_varstr_set,
  args_func_call
};

/*
** arg descriptor struct
*/

struct			args_list_s
{
  const char		*str_short;
  const char		*str_long;
  const char		*explanation;		/* description displayed in help msg */

  unsigned int		param_count;		/* expected param count */
  enum args_action_e	action;			/* action to perform */

  union
  {
    void		*void_;
    double		*dbl;
    unsigned int	*num;			/* ptr to int for param writing */
    char		**str;			/* ptr to char* for param writing */
    int			(*func)(char	**argv);/* ptr to function called. 
						   if function return non 0, 
						   program exit */
  }			value_ptr;
  unsigned int		value;

  unsigned int		exclude_mask;
};

extern unsigned int args_exclude;

/*
** functions
*/


int args_parse(int argc, char **argv);

int args_check_mandatory(unsigned int mask);

extern int args_help(char **dummy);

/* 
** param list filled by args_parse, NULL terminated. 
** contain non option args
*/

extern struct args_list_s	argslist_g[];
extern char			**args_param_list_g;
extern char			*args_title_g;
extern char			*args_copyright_g;
extern char			*args_usage_g;


#endif /* !ARGS_H_ */
