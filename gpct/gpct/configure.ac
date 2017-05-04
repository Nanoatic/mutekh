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

AC_INIT(gpct, 1.0)
AC_PREREQ(2.50)

AC_CONFIG_AUX_DIR(build)
AM_INIT_AUTOMAKE(gpct, 1.0)
AM_CONFIG_HEADER(config.h)

AC_LANG(C)
AC_PROG_CC

#AM_PROG_LIBTOOL 

AC_STDC_HEADERS
AC_CHECK_HEADERS(assert.h, AC_DEFINE([HAVE_ASSERT_H]))

AC_CHECK_PROG(PERL,perl,perl,no)
if test x$PERL = xno ; then
   AC_MSG_ERROR(Cannot find perl interpreter.)
fi

AC_C_CONST
AC_C_INLINE

AC_ARG_ENABLE(examples, AC_HELP_STRING(--disable-examples, [Disable compilation of examples]), disable_examples=yes)
AM_CONDITIONAL(EXAMPLES, test x$disable_examples != xyes)

AC_OUTPUT([
  Makefile
  doc/Makefile
  include/Makefile
  include/gpct/Makefile
  examples/Makefile
  testsuite/Makefile
])

