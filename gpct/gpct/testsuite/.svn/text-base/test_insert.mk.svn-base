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
	test_insert_DLIST_SCALAR		\
	test_insert_DLIST_SCALARTYPE		\
	test_insert_DLIST_STRING		\
	test_insert_DLIST_CASESTRING		\
	test_insert_DLIST_BLOB		\
	test_insert_DLIST_AGGREGATE		\
						\
	test_insert_RING_SCALAR		\
	test_insert_DRING_SCALAR		\
	test_insert_ARRAY_SCALAR		\
	test_insert_DARRAY_SCALAR		\
	test_insert_CLIST_SCALAR		\
	test_insert_BLIST_SCALAR		\
	test_insert_SLIST_SCALAR

DEF_insert_DLIST_SCALAR		= $(DEF_DLIST) $(DEF_KEY_SCALAR) -DTESTTYPE="struct test_object_s"
test_insert_DLIST_SCALAR_CFLAGS	= $(CFLAGS_PTHREAD_MUTEX)
INC_insert_DLIST_SCALAR		= $(INC_DLIST)
SRC_insert_DLIST_SCALAR		= test_insert.t

DEF_insert_CLIST_SCALAR		= $(DEF_CLIST) $(DEF_KEY_SCALAR) -DTESTTYPE="struct test_object_s"
test_insert_CLIST_SCALAR_CFLAGS	= 
INC_insert_CLIST_SCALAR		= $(INC_CLIST)
SRC_insert_CLIST_SCALAR		= test_insert.t

DEF_insert_DLIST_SCALARTYPE	= $(DEF_DLIST) $(DEF_KEY_SCALARTYPE) -DTESTTYPE="struct test_object_s"
INC_insert_DLIST_SCALARTYPE	= $(INC_DLIST)
SRC_insert_DLIST_SCALARTYPE	= test_insert.t

DEF_insert_DLIST_STRING	=	$(DEF_DLIST) $(DEF_KEY_STRING) -DTESTTYPE="struct test_object_s"
INC_insert_DLIST_STRING	=	$(INC_DLIST)
SRC_insert_DLIST_STRING	=	test_insert.t

DEF_insert_DLIST_CASESTRING	= $(DEF_DLIST) $(DEF_KEY_CASESTRING) -DTESTTYPE="struct test_object_s"
INC_insert_DLIST_CASESTRING	= $(INC_DLIST)
SRC_insert_DLIST_CASESTRING	= test_insert.t

DEF_insert_DLIST_BLOB		= $(DEF_DLIST) $(DEF_KEY_BLOB) -DTESTTYPE="struct test_object_s"
INC_insert_DLIST_BLOB		= $(INC_DLIST)
SRC_insert_DLIST_BLOB		= test_insert.t

DEF_insert_DLIST_AGGREGATE	= $(DEF_DLIST) $(DEF_KEY_AGGREGATE) -DTESTTYPE="struct test_object_s" -DTESTAGG
INC_insert_DLIST_AGGREGATE	= $(INC_DLIST)
SRC_insert_DLIST_AGGREGATE	= test_insert.t

DEF_insert_RING_SCALAR		= $(DEF_RING) $(DEF_KEY_SCALAR) -DTESTTYPE="struct test_object_s *"
test_insert_RING_SCALAR_CFLAGS	= $(CFLAGS_PTHREAD_MUTEX)
INC_insert_RING_SCALAR		= $(INC_RING)
SRC_insert_RING_SCALAR		= test_insert.t

DEF_insert_DRING_SCALAR		= $(DEF_DRING) $(DEF_KEY_SCALAR) -DTESTTYPE="struct test_object_s *"
test_insert_DRING_SCALAR_CFLAGS	= $(CFLAGS_PTHREAD_MUTEX)
INC_insert_DRING_SCALAR		= $(INC_DRING)
SRC_insert_DRING_SCALAR		= test_insert.t

DEF_insert_ARRAY_SCALAR		= $(DEF_ARRAY) $(DEF_KEY_SCALAR) -DTESTTYPE="struct test_object_s *"
test_insert_ARRAY_SCALAR_CFLAGS	= $(CFLAGS_PTHREAD_MUTEX)
INC_insert_ARRAY_SCALAR		= $(INC_ARRAY)
SRC_insert_ARRAY_SCALAR		= test_insert.t

DEF_insert_DARRAY_SCALAR	= $(DEF_DARRAY) $(DEF_KEY_SCALAR) -DTESTTYPE="struct test_object_s *"
test_insert_DARRAY_SCALAR_CFLAGS= $(CFLAGS_PTHREAD_MUTEX)
INC_insert_DARRAY_SCALAR	= $(INC_DARRAY)
SRC_insert_DARRAY_SCALAR	= test_insert.t

DEF_insert_BLIST_SCALAR		= $(DEF_BLIST) $(DEF_KEY_SCALAR) -DTESTTYPE="struct test_object_s"
test_insert_BLIST_SCALAR_CFLAGS	= $(CFLAGS_PTHREAD_MUTEX)
INC_insert_BLIST_SCALAR		= $(INC_BLIST)
SRC_insert_BLIST_SCALAR		= test_insert.t

DEF_insert_SLIST_SCALAR		= $(DEF_SLIST) $(DEF_KEY_SCALAR) -DTESTTYPE="struct test_object_s"
test_insert_SLIST_SCALAR_CFLAGS	= $(CFLAGS_PTHREAD_MUTEX)
INC_insert_SLIST_SCALAR		= $(INC_SLIST)
SRC_insert_SLIST_SCALAR		= test_insert.t

