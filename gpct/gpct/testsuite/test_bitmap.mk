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

TEST_OBJS +=					\
	test_bitmap_uint8_t			\
	test_bitmap_uint16_t			\
	test_bitmap_uint32_t			\
	test_bitmap_uint64_t

DEF_bitmap_uint8_t		= -DTESTTYPE="uint8_t"
test_bitmap_uint8_t_CFLAGS	= $(CFLAGS_PTHREAD_MUTEX)
INC_bitmap_uint8_t		= gpct/cont_bitmap.h
SRC_bitmap_uint8_t		= test_bitmap.t

DEF_bitmap_uint16_t		= -DTESTTYPE="uint16_t"
INC_bitmap_uint16_t		= gpct/cont_bitmap.h
SRC_bitmap_uint16_t		= test_bitmap.t

DEF_bitmap_uint32_t		= -DTESTTYPE="uint32_t"
INC_bitmap_uint32_t		= gpct/cont_bitmap.h
SRC_bitmap_uint32_t		= test_bitmap.t

DEF_bitmap_uint64_t		= -DTESTTYPE="uint64_t"
INC_bitmap_uint64_t		= gpct/cont_bitmap.h
SRC_bitmap_uint64_t		= test_bitmap.t

