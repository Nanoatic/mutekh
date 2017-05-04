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

TEST_OBJS +=				\
	test_darray_DARRAY_char	\
	test_darray_DRING_char

DEF_darray_DARRAY_char		= -DTESTTYPE="char" $(DEF_DARRAY)
test_darray_DARRAY_char_CFLAGS	= $(CFLAGS_PTHREAD_MUTEX)
INC_darray_DARRAY_char		= $(INC_DARRAY)
SRC_darray_DARRAY_char		= test_darray.t

DEF_darray_DRING_char		= -DTESTTYPE="char" $(DEF_DRING)
test_darray_DRING_char_CFLAGS	= $(CFLAGS_PTHREAD_MUTEX)
INC_darray_DRING_char		= $(INC_DRING)
SRC_darray_DRING_char		= test_darray.t

