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

    Copyright 2010, Alexandre Becoulet <alexandre.becoulet@free.fr>

*/

#ifndef TERM_UI_TYPE_H_
#define TERM_UI_TYPE_H_

#include <hexo/types.h>
#include <hexo/error.h>

typedef uint32_t		termui_con_bits_t;
typedef bool_t			termui_bool_t;
typedef error_t			termui_err_t;
typedef int_fast16_t		termui_key_t;
typedef uint_fast8_t		termui_hist_index_t;
typedef uint_fast8_t		termui_cursor_pos_t;
typedef uint_fast8_t		termui_arg_index_t;
typedef int_fast8_t		termui_arg_sindex_t;
typedef uint_fast8_t		termui_comp_index_t;
typedef int_fast8_t		termui_comp_sindex_t;
typedef uint_fast16_t		termui_strlen_t;

typedef struct device_s *	termui_iostream_t;

/* compiler native types, used for printf format */
typedef __compiler_sint_t	termui_int_t;
typedef __compiler_sint_t	termui_long_int_t;
typedef __compiler_slonglong_t	termui_long_long_int_t;

typedef size_t			termui_size_t;
typedef ssize_t			termui_ssize_t;

struct term_behavior_s;
struct termui_con_complete_ctx_s;
struct termui_con_opts_s;
struct termui_con_ctx_s;
struct termui_con_entry_s;

#endif /* TYPE_H_ */

