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
# along with this program.  If not, see <http://www.gnu.org/licenses/>

TEST_OBJS +=					\
	test_locks_PTHREAD_MUTEX		\
	test_locks_PTHREAD_RMUTEX		\
	test_locks_PTHREAD_RWLOCK		\
	test_locks_PTHREAD_SPIN			\
	test_locks_GCC_SPIN			\
	test_locks_GCC_RWSPIN


DEF_locks_PTHREAD_MUTEX		= -DTESTLOCK=PTHREAD_MUTEX
test_locks_PTHREAD_MUTEX_CFLAGS	= -D_REENTRANT
INC_locks_PTHREAD_MUTEX		= gpct/lock_pthread_mutex.h
SRC_locks_PTHREAD_MUTEX		= test_locks.t

DEF_locks_PTHREAD_RMUTEX	= -DTESTLOCK=PTHREAD_RMUTEX
test_locks_PTHREAD_RMUTEX_CFLAGS= -D_XOPEN_SOURCE=500 -D_REENTRANT
INC_locks_PTHREAD_RMUTEX	= gpct/lock_pthread_rmutex.h
SRC_locks_PTHREAD_RMUTEX	= test_locks.t

DEF_locks_PTHREAD_RWLOCK	= -DTESTLOCK=PTHREAD_RWLOCK
test_locks_PTHREAD_RWLOCK_CFLAGS= -D_REENTRANT
INC_locks_PTHREAD_RWLOCK	= gpct/lock_pthread_rwlock.h
SRC_locks_PTHREAD_RWLOCK	= test_locks.t

DEF_locks_PTHREAD_SPIN		= -DTESTLOCK=PTHREAD_SPIN
test_locks_PTHREAD_SPIN_CFLAGS	= -D_XOPEN_SOURCE=600 -D_REENTRANT
INC_locks_PTHREAD_SPIN		= gpct/lock_pthread_spin.h
SRC_locks_PTHREAD_SPIN		= test_locks.t

DEF_locks_GCC_SPIN		= -DTESTLOCK=GCC_SPIN
INC_locks_GCC_SPIN		= gpct/lock_gcc_spin.h
SRC_locks_GCC_SPIN		= test_locks.t

DEF_locks_GCC_RWSPIN		= -DTESTLOCK=GCC_RWSPIN
INC_locks_GCC_RWSPIN		= gpct/lock_gcc_rwspin.h
SRC_locks_GCC_RWSPIN		= test_locks.t

