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

TEMPLATES = _bitops.t _cont_access.t _cont_array.t _cont_bintree.t	\
	_cont_bintree_iter.t _cont_bintree_noup.t _cont_bintree_up.t	\
	_cont_bitmap.t _cont_entry.t _cont_foreach.t _cont_hash_cmp.t	\
	_cont_keyfield.t _cont_lists.t _cont_lock.t _cont_lookup.t	\
	_cont_pushpop_array.t _cont_ring.t _object.t cont_array.t	\
	cont_bintree.t cont_bitmap.t cont_blist.t cont_clist.t		\
	cont_darray.t cont_dbitmap.t cont_dlist.t cont_dring.t		\
	cont_hashlist.t cont_pbintree.t cont_ring.t cont_slist.t	\
	object_refcount.t object_simple.t _bitops.t _cont_access.t	\
	_cont_array.t _cont_bintree.t

.t.h:
	cat $< | $(top_srcdir)/build/backslash.pl > $@

BUILT_SOURCES = $(TEMPLATES:.t=.h)
CLEANFILES = $(BUILT_SOURCES)

pkgincludedir = $(includedir)/gpct
pkginclude_HEADERS = $(BUILT_SOURCES) _atomic.h _atomic_gcc.h		\
	_atomic_none.h _atomic_pthread.h _cont_refcount.h _helpers.h	\
	_lock_gcc.h _platform.h lock_gcc_rwspin.h lock_gcc_spin.h	\
	lock_pthread_mutex.h lock_pthread_rmutex.h			\
	lock_pthread_rwlock.h lock_pthread_spin.h

EXTRA_DIST = $(TEMPLATES)

