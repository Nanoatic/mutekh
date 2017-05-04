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

TEST_OBJS += test_bitops

INC_bitops		= gpct/_bitops.h
SRC_bitops		= test_bitops.t

#bitops_gcc_CFLAGS		= -U__GNUC__ -D__GNUC__=2
#INC_bitops_gcc			= gpct/_bitops.h
#SRC_bitops_gcc			= test_bitops.t
#
#bitops_nogcc_CFLAGS		= -U__GNUC__ -Dinline=__inline__ -ansi
#INC_bitops_nogcc		= gpct/_bitops.h
#SRC_bitops_nogcc		= test_bitops.t

