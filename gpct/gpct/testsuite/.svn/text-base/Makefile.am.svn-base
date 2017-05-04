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

#### container algorithms

DEF_DLIST	= -DTESTCONT=DLIST -DTESTTYPE_EXTRA=list_entry
INC_DLIST	= gpct/cont_dlist.h

DEF_CLIST	= -DTESTCONT=CLIST -DTESTTYPE_EXTRA=list_entry
INC_CLIST	= gpct/cont_clist.h

DEF_BLIST	= -DTESTCONT=BLIST -DTESTTYPE_EXTRA=list_entry
INC_BLIST	= gpct/cont_blist.h

DEF_SLIST	= -DTESTCONT=SLIST -DTESTTYPE_EXTRA=list_entry
INC_SLIST	= gpct/cont_slist.h

DEF_HASHLIST	= -DTESTCONT=HASHLIST -DTESTTYPE_EXTRA=list_entry,5
INC_HASHLIST	= gpct/cont_hashlist.h

DEF_RING	= -DTESTCONT=RING -DTESTTYPE_EXTRA=8
INC_RING	= gpct/cont_ring.h

DEF_DRING	= -DTESTCONT=DRING -DTESTTYPE_EXTRA=4,16
INC_DRING	= gpct/cont_dring.h

DEF_ARRAY	= -DTESTCONT=ARRAY -DTESTTYPE_EXTRA=8
INC_ARRAY	= gpct/cont_array.h

DEF_DARRAY	= -DTESTCONT=DARRAY -DTESTTYPE_EXTRA=4,16
INC_DARRAY	= gpct/cont_darray.h

CFLAGS_PTHREAD_MUTEX = -DCONTAINER_LOCK_test=PTHREAD_MUTEX -D_REENTRANT

#### container keyfield hash/compare mechanisms

DEF_KEY_SCALARTYPE	= -DTESTALGO=SCALARTYPE -DTESTKEY=id -DTESTKEYTYPE_EXTRA=int -DTESTFUNC_EXTRA=id
DEF_KEY_SCALAR		= -DTESTALGO=SCALAR -DTESTKEY=id -DTESTFUNC_EXTRA=id
DEF_KEY_STRING		= -DTESTALGO=STRING -DTESTKEY=name -DTESTFUNC_EXTRA=name
DEF_KEY_CASESTRING	= -DTESTALGO=CASESTRING -DTESTKEY=name -DTESTFUNC_EXTRA=name
DEF_KEY_BLOB		= -DTESTALGO=BLOB -DTESTKEY=blob -DTESTKEYTYPE_EXTRA=12 -DTESTFUNC_EXTRA=blob
DEF_KEY_AGGREGATE	= -DTESTALGO=AGGREGATE -DTESTKEY=agg -DTESTFUNC_EXTRA=agg

TEST_OBJS=

include test_bitops.mk
include test_bitmap.mk
include test_insert.mk
include test_hash.mk
include test_counter.mk
include test_refcount.mk
include test_array.mk
include test_darray.mk
include test_locks.mk

####

CPPFLAGS=-I$(top_srcdir)/include -I$(top_builddir)/include -DGPCT_CONFIG_NODEPRECATED -D_GNU_SOURCE
CFLAGS=-save-temps
LDFLAGS=-lpthread

TESTS = $(TEST_OBJS)
check_PROGRAMS = $(TEST_OBJS)

EXTRA_DIST = test_array.t test_bitmap.t test_bitops.t test_counter.t	\
	     test_darray.t test_hash.t test_insert.t test_refcount.t \
	     test_locks.t testsuite.h

CLEANFILES = $(TEST_OBJS:=.c) $(TEST_OBJS:=.i) $(TEST_OBJS:=.s)

test_%.c: $(SRC_$*) $(INC)/$(INC_$*)
	m4 -P -DTESTNAME=$* $(DEF_$*) -DINCLUDE=$(INC_$*) $(SRC_$*) > $@

.PRECIOUS: test_%.c

