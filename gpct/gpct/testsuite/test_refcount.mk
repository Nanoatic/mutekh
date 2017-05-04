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
	test_refcount_RING			\
	test_refcount_DLIST			\
	test_refcount_CLIST			\
	test_refcount_BLIST			\
	test_refcount_SLIST			\
	test_refcount_HASHLIST

DEF_refcount_CLIST		= $(DEF_CLIST) -DTESTTYPE="struct test_object_s" -DGPCT_CONFIG_ATOMIC_PTHREAD_SPIN
test_refcount_CLIST_CFLAGS	= -DGPCT_CONFIG_ATOMIC_PTHREAD_SPIN -D_XOPEN_SOURCE=600 -D_REENTRANT
INC_refcount_CLIST		= $(INC_CLIST)
SRC_refcount_CLIST		= test_refcount.t

DEF_refcount_DLIST		= $(DEF_DLIST) -DTESTTYPE="struct test_object_s"
test_refcount_DLIST_CFLAGS	= -DGPCT_CONFIG_ATOMIC_PTHREAD_MUTEX -D_XOPEN_SOURCE=600 -D_REENTRANT
INC_refcount_DLIST		= $(INC_DLIST)
SRC_refcount_DLIST		= test_refcount.t

DEF_refcount_BLIST		= $(DEF_BLIST) -DTESTTYPE="struct test_object_s"
test_refcount_BLIST_CFLAGS	= -DGPCT_CONFIG_ATOMIC_GCC
INC_refcount_BLIST		= $(INC_BLIST)
SRC_refcount_BLIST		= test_refcount.t

DEF_refcount_SLIST		= $(DEF_SLIST) -DTESTTYPE="struct test_object_s"
test_refcount_SLIST_CFLAGS	= -DGPCT_CONFIG_ATOMIC_NONE
INC_refcount_SLIST		= $(INC_SLIST)
SRC_refcount_SLIST		= test_refcount.t

DEF_refcount_HASHLIST		= $(DEF_HASHLIST) $(DEF_KEY_STRING) -DTESTTYPE="struct test_object_s"
test_refcount_HASHLIST_CFLAGS	= -DGPCT_CONFIG_ATOMIC_NONE
INC_refcount_HASHLIST		= $(INC_HASHLIST)
SRC_refcount_HASHLIST		= test_refcount.t

DEF_refcount_RING		= $(DEF_RING) -DTESTTYPE="struct test_object_s *"
test_refcount_RING_CFLAGS	= -DGPCT_CONFIG_ATOMIC_NONE
INC_refcount_RING		= $(INC_RING)
SRC_refcount_RING		= test_refcount.t

