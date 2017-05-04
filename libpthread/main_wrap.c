/*
    This file is part of MutekH.

    MutekH is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    MutekH is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with MutekH; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

    Copyright Alexandre Becoulet <alexandre.becoulet@lip6.fr> (c) 2006

*/

#include <hexo/cpu.h>
#include <hexo/init.h>
#include <pthread.h>

pthread_t pthread_main;

error_t main(size_t argc, char **argv);

void * pthread_main_func(void *pv)
{
  char *argv[] = { "a.out", NULL };
  main(1, argv);
  return NULL;
}

void app_start()
{
  if (cpu_isbootstrap())
    pthread_create(&pthread_main, NULL, pthread_main_func, NULL);
}

