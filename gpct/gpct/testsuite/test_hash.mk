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
	test_hash_HASHLIST_SCALAR		\
	test_hash_HASHLIST_SCALARTYPE		\
	test_hash_HASHLIST_STRING		\
	test_hash_HASHLIST_CASESTRING		\
	test_hash_HASHLIST_BLOB		\
						\
	test_hash_RING_SCALAR			\
	test_hash_CLIST_SCALAR		\
	test_hash_DLIST_SCALAR		\
	test_hash_DLIST_BLOB			\
	test_hash_BLIST_SCALAR		\
	test_hash_SLIST_SCALAR

#	test_hash_HASHLIST_AGGREGATE		

DEF_hash_RING_SCALAR		= $(DEF_RING) $(DEF_KEY_SCALAR) -DTESTTYPE="struct test_object_s *"
test_hash_RING_SCALAR_CFLAGS	= $(CFLAGS_PTHREAD_MUTEX)
INC_hash_RING_SCALAR		= $(INC_RING)
SRC_hash_RING_SCALAR		= test_hash.t

DEF_hash_CLIST_SCALAR		= $(DEF_CLIST) $(DEF_KEY_SCALAR) -DTESTTYPE="struct test_object_s"
test_hash_CLIST_SCALAR_CFLAGS	= $(CFLAGS_PTHREAD_MUTEX)
INC_hash_CLIST_SCALAR		= $(INC_CLIST)
SRC_hash_CLIST_SCALAR		= test_hash.t

DEF_hash_DLIST_SCALAR		= $(DEF_DLIST) $(DEF_KEY_SCALAR) -DTESTTYPE="struct test_object_s"
INC_hash_DLIST_SCALAR		= $(INC_DLIST)
SRC_hash_DLIST_SCALAR		= test_hash.t

DEF_hash_BLIST_SCALAR		= $(DEF_BLIST) $(DEF_KEY_SCALAR) -DTESTTYPE="struct test_object_s"
INC_hash_BLIST_SCALAR		= $(INC_BLIST)
SRC_hash_BLIST_SCALAR		= test_hash.t

DEF_hash_SLIST_SCALAR		= $(DEF_SLIST) $(DEF_KEY_SCALAR) -DTESTTYPE="struct test_object_s"
INC_hash_SLIST_SCALAR		= $(INC_SLIST)
SRC_hash_SLIST_SCALAR		= test_hash.t

DEF_hash_HASHLIST_SCALAR	= $(DEF_HASHLIST) $(DEF_KEY_SCALAR) -DTESTTYPE="struct test_object_s"
test_hash_HASHLIST_SCALAR_CFLAGS= $(CFLAGS_PTHREAD_MUTEX)
INC_hash_HASHLIST_SCALAR	= $(INC_HASHLIST)
SRC_hash_HASHLIST_SCALAR	= test_hash.t

DEF_hash_HASHLIST_SCALARTYPE	= $(DEF_HASHLIST) $(DEF_KEY_SCALARTYPE) -DTESTTYPE="struct test_object_s"
INC_hash_HASHLIST_SCALARTYPE	= $(INC_HASHLIST)
SRC_hash_HASHLIST_SCALARTYPE	= test_hash.t

DEF_hash_HASHLIST_STRING	= $(DEF_HASHLIST) $(DEF_KEY_STRING) -DTESTTYPE="struct test_object_s"
INC_hash_HASHLIST_STRING	= $(INC_HASHLIST)
SRC_hash_HASHLIST_STRING	= test_hash.t

DEF_hash_HASHLIST_CASESTRING	= $(DEF_HASHLIST) $(DEF_KEY_CASESTRING) -DTESTTYPE="struct test_object_s"
INC_hash_HASHLIST_CASESTRING	= $(INC_HASHLIST)
SRC_hash_HASHLIST_CASESTRING	= test_hash.t

DEF_hash_HASHLIST_BLOB		= $(DEF_HASHLIST) $(DEF_KEY_BLOB) -DTESTTYPE="struct test_object_s"
INC_hash_HASHLIST_BLOB		= $(INC_HASHLIST)
SRC_hash_HASHLIST_BLOB		= test_hash.t

DEF_hash_HASHLIST_AGGREGATE	= $(DEF_HASHLIST) $(DEF_KEY_AGGREGATE) -DTESTTYPE="struct test_object_s" -DTESTAGG
INC_hash_HASHLIST_AGGREGATE	= $(INC_HASHLIST)
SRC_hash_HASHLIST_AGGREGATE	= test_hash.t

DEF_hash_DLIST_BLOB		= $(DEF_DLIST) $(DEF_KEY_BLOB) -DTESTTYPE="struct test_object_s"
INC_hash_DLIST_BLOB		= $(INC_DLIST)
SRC_hash_DLIST_BLOB		= test_hash.t

