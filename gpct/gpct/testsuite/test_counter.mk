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
	test_counter_HASHLIST_SCALAR		\
	test_counter_CLIST_SCALAR		\
	test_counter_DLIST_SCALAR		\
	test_counter_BLIST_SCALAR		\
	test_counter_SLIST_SCALAR

DEF_counter_CLIST_SCALAR	= $(DEF_CLIST) $(DEF_KEY_SCALAR) -DTESTTYPE="struct test_object_s"
INC_counter_CLIST_SCALAR	= $(INC_CLIST)
SRC_counter_CLIST_SCALAR	= test_counter.t

DEF_counter_DLIST_SCALAR	= $(DEF_DLIST) $(DEF_KEY_SCALAR) -DTESTTYPE="struct test_object_s"
INC_counter_DLIST_SCALAR	= $(INC_DLIST)
SRC_counter_DLIST_SCALAR	= test_counter.t

DEF_counter_BLIST_SCALAR	= $(DEF_BLIST) $(DEF_KEY_SCALAR) -DTESTTYPE="struct test_object_s"
INC_counter_BLIST_SCALAR	= $(INC_BLIST)
SRC_counter_BLIST_SCALAR	= test_counter.t

DEF_counter_SLIST_SCALAR	= $(DEF_SLIST) $(DEF_KEY_SCALAR) -DTESTTYPE="struct test_object_s"
INC_counter_SLIST_SCALAR	= $(INC_SLIST)
SRC_counter_SLIST_SCALAR	= test_counter.t

DEF_counter_HASHLIST_SCALAR	= $(DEF_HASHLIST) $(DEF_KEY_SCALAR) -DTESTTYPE="struct test_object_s"
INC_counter_HASHLIST_SCALAR	= $(INC_HASHLIST)
SRC_counter_HASHLIST_SCALAR	= test_counter.t

