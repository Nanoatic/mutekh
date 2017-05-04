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

    Copyright (c) 2010, Nicolas Pouillon, <nipo@ssji.net>
*/

#include <hexo/error.h>
#include <hexo/local.h>
#include <pthread.h>

struct pthread_key_s
{
  bool_t allocated;
  void (*destructor)(void *);
};

CONTEXT_LOCAL const void *_key_values[CONFIG_PTHREAD_KEYS_MAX];

static pthread_mutex_t _keys_lock = PTHREAD_MUTEX_INITIALIZER;
static struct pthread_key_s _keys[CONFIG_PTHREAD_KEYS_MAX];

error_t pthread_key_create(pthread_key_t *key, void (*destructor)(void *))
{
    error_t i = 0;

    pthread_mutex_lock(&_keys_lock);

    for ( i=0; i<CONFIG_PTHREAD_KEYS_MAX; ++i ) {
        if ( _keys[i].allocated )
            continue;

        _keys[i].allocated = 1;
        _keys[i].destructor = destructor;
        *key = i;
        goto done;
    }
    i = -EAGAIN;

  done:
    pthread_mutex_unlock(&_keys_lock);

    return i;
}

error_t pthread_key_delete(pthread_key_t key)
{
    error_t ret = 0;

    pthread_mutex_lock(&_keys_lock);
    
    if ( _keys[key].allocated ) {
        _keys[key].allocated = 0;
        _keys[key].destructor = NULL;
    } else {
        ret = -EINVAL;
    }

    pthread_mutex_unlock(&_keys_lock);
    return ret;
}

void _pthread_keys_init(struct pthread_s *thread)
{
    size_t i;
    const void **_values = CONTEXT_LOCAL_ADDR(_key_values[0]);

    for ( i=0; i<CONFIG_PTHREAD_KEYS_MAX; ++i )
        _values[i] = NULL;
}

void _pthread_keys_cleanup(struct pthread_s *thread)
{
    size_t iter;
    const void **_values = CONTEXT_LOCAL_ADDR(_key_values[0]);

    pthread_mutex_lock(&_keys_lock);
    
    for ( iter=0; iter<CONFIG_PTHREAD_KEYS_DESTRUCTOR_ITERATIONS; ++iter ) {
        bool_t done = 0;
        size_t i;

        for ( i=0; i<CONFIG_PTHREAD_KEYS_MAX; ++i ) {
            void *data = (void*)_values[i];

            if ( (!_keys[i].allocated)
                 || (_keys[i].destructor == NULL)
                 || (data == NULL) )
                continue;

            _values[i] = NULL;
            _keys[i].destructor(data);
            done = 1;
        }

        if ( !done )
            break;
    }

    pthread_mutex_unlock(&_keys_lock);
}
