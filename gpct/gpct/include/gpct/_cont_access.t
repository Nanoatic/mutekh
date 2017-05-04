/* -*- c -*-

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
    02110-1301 USA

    Copyright Alexandre Becoulet <alexandre.becoulet@free.fr> (C) 2009

    Container main access functions stuff

*/

#ifndef GPCT_CONT_ACCESS_H_
#define GPCT_CONT_ACCESS_H_

#include <stdarg.h>

#include <gpct/_platform.h>
#include <gpct/_cont_lock.h>
#include <gpct/_cont_refcount.h>
#include <gpct/_cont_foreach.h>

/***********************************************************************
 *      container access functions prototypes
 */

/* Itialize an item and mark it as not being part of a container. */
/* backslash-block */
#define GPCT_CONT_PROTO_ORPHAN(name, prefix)                    
GPCT_UNUSED void                                                
prefix##_orphan(name##_item_t item)

/* Check if an item is currently part of a container */
/* backslash-block */
#define GPCT_CONT_PROTO_ISORPHAN(name, prefix)                  
GPCT_UNUSED gpct_bool_t                                         
prefix##_isorphan(name##_item_t item)

/* Check for null index */
/* backslash-block */
#define GPCT_CONT_PROTO_ISNULL(name, prefix)                    
GPCT_UNUSED gpct_bool_t                                                     
prefix##_isnull (name##_index_t index)

/* Check index validity */
/* backslash-block */
#define GPCT_CONT_PROTO_ISVALID(name, prefix)                   
GPCT_UNUSED gpct_bool_t                                                     
prefix##_isvalid(name##_root_t *root, name##_index_t index)

/* Return container empty state */
/* backslash-block */
#define GPCT_CONT_PROTO_ISEMPTY(name, prefix)                   
GPCT_UNUSED gpct_bool_t                                                     
prefix##_isempty(name##_root_t *root)

/* Return container full state */
/* backslash-block */
#define GPCT_CONT_PROTO_ISFULL(name, prefix)                   
GPCT_UNUSED gpct_bool_t                                                     
prefix##_isfull(name##_root_t *root)

/* Get the item at the specified index. */
/* backslash-block */
#define GPCT_CONT_PROTO_GET(name, prefix)                       
GPCT_UNUSED name##_item_t                                                   
prefix##_get    (name##_root_t *root, name##_index_t index)

/* Get the pointer to item at the specified index. */
/* backslash-block */
#define GPCT_CONT_PROTO_GETPTR(name, prefix)                    
GPCT_UNUSED name##_itembase_t *                                             
prefix##_getptr (name##_root_t *root, name##_index_t index)

/* Get item index from pointer to item. */
/* backslash-block */
#define GPCT_CONT_PROTO_GETINDEX(name, prefix)                  
GPCT_UNUSED name##_index_t                                                  
prefix##_getindex(name##_root_t *root, name##_itembase_t *ptr)

/* Allocate a new unused item in the container and return its index */
/* backslash-block */
#define GPCT_CONT_PROTO_ALLOC(name, prefix)                     
GPCT_UNUSED name##_index_t                                                  
prefix##_alloc (name##_root_t *root, ...)

/* Set an item and replace the old item at the specified index */
/* backslash-block */
#define GPCT_CONT_PROTO_SET(name, prefix)                       
GPCT_UNUSED gpct_error_t                                                    
prefix##_set    (name##_root_t *root,                           
                 name##_index_t index,                          
                 name##_item_t item)

/* Return the index of the next item */
/* backslash-block */
#define GPCT_CONT_PROTO_NEXT(name, prefix)                      
GPCT_UNUSED name##_index_t                                                  
prefix##_next   (name##_root_t *root, name##_index_t index)

/* Return the index of the previous item */
/* backslash-block */
#define GPCT_CONT_PROTO_PREV(name, prefix)                      
GPCT_UNUSED name##_index_t                                                  
prefix##_prev   (name##_root_t *root, name##_index_t index)

/* Return the index of the first item in the container */
/* backslash-block */
#define GPCT_CONT_PROTO_HEAD(name, prefix)                      
GPCT_UNUSED name##_index_t                                                  
prefix##_head   (name##_root_t *root)

/* Return the index of the last item in the container */
/* backslash-block */
#define GPCT_CONT_PROTO_TAIL(name, prefix)                      
GPCT_UNUSED name##_index_t                                                  
prefix##_tail   (name##_root_t *root)

/* Return the current items count in the container */
/* backslash-block */
#define GPCT_CONT_PROTO_COUNT(name, prefix)                     
GPCT_UNUSED size_t                                                          
prefix##_count  (name##_root_t *root)

/* Return the maximum items count in the container */
/* backslash-block */
#define GPCT_CONT_PROTO_SIZE(name, prefix)                      
GPCT_UNUSED size_t                                                          
prefix##_size(name##_root_t *root)

/* Change the maximum items count (realloc) */
/* backslash-block */
#define GPCT_CONT_PROTO_RESIZE(name, prefix)                    
GPCT_UNUSED gpct_error_t                                                    
prefix##_resize(name##_root_t *root, size_t size)

/* Insert an item before an other item in the container */
/* backslash-block */
#define GPCT_CONT_PROTO_INSERT_PRE(name, prefix)                
GPCT_UNUSED size_t                                                          
prefix##_insert_pre(name##_root_t *root, name##_index_t index,  
                    name##_item_t item)

/* Insert an item next to an other item in the container */
/* backslash-block */
#define GPCT_CONT_PROTO_INSERT_POST(name, prefix)               
GPCT_UNUSED size_t                                                          
prefix##_insert_post(name##_root_t *root, name##_index_t index, 
                     name##_item_t item)

/* Delete the item at the given index in the container */
/* backslash-block */
#define GPCT_CONT_PROTO_REMOVE(name, prefix)                    
GPCT_UNUSED gpct_error_t                                                    
prefix##_remove (name##_root_t *root, name##_index_t index)

/* Insert an item before the container first item */
/* backslash-block */
#define GPCT_CONT_PROTO_PUSH(name, prefix)                      
GPCT_UNUSED size_t                                                          
prefix##_push   (name##_root_t *root, name##_item_t item)

/* Insert an item after the container last item */
/* backslash-block */
#define GPCT_CONT_PROTO_PUSHBACK(name, prefix)                  
GPCT_UNUSED size_t                                                          
prefix##_pushback(name##_root_t *root, name##_item_t item)

/* Remove and get the container first item */
/* backslash-block */
#define GPCT_CONT_PROTO_POP(name, prefix)                       
GPCT_UNUSED name##_item_t                                                   
prefix##_pop    (name##_root_t  *root)

/* Remove and get the container last item */
/* backslash-block */
#define GPCT_CONT_PROTO_POPBACK(name, prefix)                   
GPCT_UNUSED name##_item_t                                                   
prefix##_popback(name##_root_t  *root)

/* Insert several items before the container first item */
/* backslash-block */
#define GPCT_CONT_PROTO_PUSH_ARRAY(name, prefix)                
GPCT_UNUSED size_t                                                          
prefix##_push_array     (name##_root_t *root,                   
                         name##_item_t *item,                   
                         size_t size)

/* Insert several items after the container last item */
/* backslash-block */
#define GPCT_CONT_PROTO_PUSHBACK_ARRAY(name, prefix)            
GPCT_UNUSED size_t                                                          
prefix##_pushback_array (name##_root_t *root,                   
                         name##_item_t *item,                   
                         size_t size)

/* Remove several items from the _cont_access.head and copy them to an array */
/* backslash-block */
#define GPCT_CONT_PROTO_POP_ARRAY(name, prefix)                 
GPCT_UNUSED size_t                                                          
prefix##_pop_array      (name##_root_t  *root,                  
                         name##_item_t *item,                   
                         size_t size)

/* Remove several items from the container tail and copy them to an array */
/* backslash-block */
#define GPCT_CONT_PROTO_POPBACK_ARRAY(name, prefix)             
GPCT_UNUSED size_t                                                          
prefix##_popback_array  (name##_root_t  *root,                  
                         name##_item_t *item,                   
                         size_t size)

/* Container iterator function to be used with prefix##_foreach() function */
/* backslash-block */
#define CONTAINER_ITERATOR(name, fcn)                           
intptr_t (fcn) (name##_item_t item, va_list ap)

/* Iterate over the whole container by calling an iterator function.
   Stop iteration if the iterator function return non zero. */
/* backslash-block */
#define GPCT_CONT_PROTO_FOREACH(name, prefix)                   
GPCT_UNUSED intptr_t                                                        
prefix##_foreach        (name##_root_t *root,                   
                         CONTAINER_ITERATOR(name, * const fcn), ...)

/* Iterate over the whole container by calling an iterator function.
   Stop iteration if the iterator function return non zero. */
/* backslash-block */
#define GPCT_CONT_PROTO_FOREACH_REVERSE(name, prefix)           
GPCT_UNUSED intptr_t                                                        
prefix##_foreach_reverse(name##_root_t *root,                   
                         CONTAINER_ITERATOR(name, * const fcn), ...)

/* Remove all items from the container */
/* backslash-block */
#define GPCT_CONT_PROTO_CLEAR(name, prefix)                     
GPCT_UNUSED void                                                            
prefix##_clear          (name##_root_t *root)

/* Init the container root */
/* backslash-block */
#define GPCT_CONT_PROTO_INIT(name, prefix)                      
GPCT_UNUSED gpct_error_t                                                    
prefix##_init           (name##_root_t *root)

/* Destroy the container root */
/* backslash-block */
#define GPCT_CONT_PROTO_DESTROY(name, prefix)                   
GPCT_UNUSED void                                                            
prefix##_destroy        (name##_root_t *root)

/* Check container root consitency */
/* backslash-block */
#define GPCT_CONT_PROTO_CHECK(name, prefix)                     
GPCT_UNUSED gpct_error_t                                                    
prefix##_check          (name##_root_t *root)



/***********************************************************************
 *      container functions prototypes macro
 */

/* backslash-region-begin */
#define CONTAINER_PROTOTYPE(name, attr, prefix)

attr GPCT_CONT_PROTO_ISORPHAN(name, prefix);
attr GPCT_CONT_PROTO_ORPHAN(name, prefix);
attr GPCT_CONT_PROTO_ISNULL(name, prefix);
attr GPCT_CONT_PROTO_ISVALID(name, prefix);
attr GPCT_CONT_PROTO_ISEMPTY(name, prefix);
attr GPCT_CONT_PROTO_GET(name, prefix);
attr GPCT_CONT_PROTO_GETPTR(name, prefix);
attr GPCT_CONT_PROTO_GETINDEX(name, prefix);
attr GPCT_CONT_PROTO_ALLOC(name, prefix);
attr GPCT_CONT_PROTO_SET(name, prefix);
attr GPCT_CONT_PROTO_NEXT(name, prefix);
attr GPCT_CONT_PROTO_PREV(name, prefix);
attr GPCT_CONT_PROTO_HEAD(name, prefix);
attr GPCT_CONT_PROTO_TAIL(name, prefix);
attr GPCT_CONT_PROTO_COUNT(name, prefix);
attr GPCT_CONT_PROTO_SIZE(name, prefix);
attr GPCT_CONT_PROTO_RESIZE(name, prefix);
attr GPCT_CONT_PROTO_INSERT_PRE(name, prefix);
attr GPCT_CONT_PROTO_INSERT_POST(name, prefix);
attr GPCT_CONT_PROTO_CLEAR(name, prefix);
attr GPCT_CONT_PROTO_REMOVE(name, prefix);
attr GPCT_CONT_PROTO_PUSH(name, prefix);
attr GPCT_CONT_PROTO_PUSHBACK(name, prefix);
attr GPCT_CONT_PROTO_POP(name, prefix);
attr GPCT_CONT_PROTO_POPBACK(name, prefix);
attr GPCT_CONT_PROTO_PUSH_ARRAY(name, prefix);
attr GPCT_CONT_PROTO_PUSHBACK_ARRAY(name, prefix);
attr GPCT_CONT_PROTO_POP_ARRAY(name, prefix);
attr GPCT_CONT_PROTO_POPBACK_ARRAY(name, prefix);
attr GPCT_CONT_PROTO_FOREACH(name, prefix);
attr GPCT_CONT_PROTO_FOREACH_REVERSE(name, prefix);
attr GPCT_CONT_PROTO_WRLOCK(name, prefix);
attr GPCT_CONT_PROTO_RDLOCK(name, prefix);
attr GPCT_CONT_PROTO_UNLOCK(name, prefix);
attr GPCT_CONT_PROTO_INIT(name, prefix);
attr GPCT_CONT_PROTO_DESTROY(name, prefix);
attr GPCT_CONT_PROTO_CHECK(name, prefix);
/* backslash-region-end */

/***********************************************************************
 *      container types and functions defintions macro
 */

/* Define type associated with a container */
/* backslash-block */
#define         CONTAINER_TYPE(name, algo, type, ...)
  typedef gpct_empty_t gpct_lock_CONTAINER_LOCK_##name##_type_t;
  GPCT_CPP_CONCAT(GPCT_CONT_, algo, _TYPE)(name, type, __VA_ARGS__)
  GPCT_LOCK_EMPTY_LOCK_FUNC(gpct_lock_CONTAINER_LOCK_##name)
  GPCT_CONT_OBJECT_EMPTY_FUNC(name, CONTAINER_OBJ_##name)
  extern name##_item_t name##_item; /* fake object for custom_expr to be always valid for typeof() */

/* backslash-block */
#define         CONTAINER_ENTRY_TYPE(algo)
  GPCT_CPP_CONCAT(gpct_, algo, _entry_t)


/* define all container access functions */

/* backslash-block */
#define         CONTAINER_FUNC_NOLOCK(name, algo, attr, prefix, ...)
  GPCT_CPP_CONCAT(GPCT_CONT_, algo, _FUNC)(attr, name, prefix, NOLOCK, __VA_ARGS__)


/* backslash-block */
#define         CONTAINER_FUNC_LOCK(name, algo, attr, prefix, lockname, ...)
  GPCT_CPP_CONCAT(GPCT_CONT_, algo, _FUNC)(attr, name, prefix, lockname, __VA_ARGS__)


/* backslash-block */
#define         CONTAINER_FUNC(name, algo, attr, prefix, ...)
  GPCT_CPP_CONCAT(GPCT_CONT_, algo, _FUNC)(attr, name, prefix, CONTAINER_LOCK_##name, __VA_ARGS__)
  GPCT_CONT_LOCK_FUNC(attr, name, prefix, CONTAINER_LOCK_##name)


/* Default container object intializer */
/* backslash-block */
#define         CONTAINER_ROOT_INITIALIZER(name, algo) 
  GPCT_CPP_CONCAT(GPCT_CONT_, algo, _ROOT_INITIALIZER)(name, CONTAINER_LOCK_##name)


/* backslash-block */
#define         CONTAINER_ROOT_DECLARATOR(name, algo, symbol) 
  GPCT_CPP_CONCAT(GPCT_CONT_, algo, _ROOT_DECLARATOR)(name, CONTAINER_LOCK_##name, symbol)


/* backslash-block */
#define         CONTAINER_DEF(algo, test)
  GPCT_CPP_CONCAT(GPCT_CONT_, algo, _##test)

#endif

