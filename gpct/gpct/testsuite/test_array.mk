# Copyright (C) 2009 Alexandre Becoulet
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

TEST_OBJS +=			\
	test_array_RING_char	\
	test_array_RING_int	\
	test_array_ARRAY_char	\
	test_array_ARRAY_int

DEF_array_RING_char		= $(DEF_RING) -DTESTTYPE="char"
test_array_RING_char_CFLAGS	= $(CFLAGS_PTHREAD_MUTEX)
INC_array_RING_char		= $(INC_RING)
SRC_array_RING_char		= test_array.t

DEF_array_RING_int		= $(DEF_RING) -DTESTTYPE="int"
INC_array_RING_int		= $(INC_RING)
SRC_array_RING_int		= test_array.t

DEF_array_ARRAY_char		= $(DEF_ARRAY) -DTESTTYPE="char"
test_array_ARRAY_char_CFLAGS	= $(CFLAGS_PTHREAD_MUTEX)
INC_array_ARRAY_char		= $(INC_ARRAY)
SRC_array_ARRAY_char		= test_array.t

DEF_array_ARRAY_int		= $(DEF_ARRAY) -DTESTTYPE="int"
INC_array_ARRAY_int		= $(INC_ARRAY)
SRC_array_ARRAY_int		= test_array.t

