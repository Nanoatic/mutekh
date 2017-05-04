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

    Key field based container functions set

*/

#ifndef GPCT_KEYFIELD_H_
#define GPCT_KEYFIELD_H_

#include <gpct/_platform.h>
#include <gpct/_cont_hash_cmp.h>

/***********************************************************************
 *      define all lookup and sorting functions prototypes
 */

/* Search the first item with a matching key field value */
/* backslash-block */
#define GPCT_CONT_PROTO_LOOKUP(name, prefix, keyfield)                  
GPCT_UNUSED name##_index_t                                                          
prefix##_lookup         (name##_root_t *root,                           
                         gpct_##name##_##keyfield##_arg_t value)

/* Search the first item with a matching key field value */
/* backslash-block */
#define GPCT_CONT_PROTO_REMOVE_KEY(name, prefix, keyfield)                  
GPCT_UNUSED gpct_error_t                                                          
prefix##_remove_key     (name##_root_t *root,                           
                         gpct_##name##_##keyfield##_arg_t value)

/* Search the last item with a matching key field value */
/* backslash-block */
#define GPCT_CONT_PROTO_LOOKUP_LAST(name, prefix, keyfield)             
GPCT_UNUSED name##_index_t                                                          
prefix##_lookup_last    (name##_root_t *root,                           
                         gpct_##name##_##keyfield##_arg_t value)

/* Search the next item with a matching key field value */
/* backslash-block */
#define GPCT_CONT_PROTO_LOOKUP_NEXT(name, prefix, keyfield)             
GPCT_UNUSED name##_index_t                                                          
prefix##_lookup_next    (name##_root_t *root,                           
                         name##_index_t index,                          
                         gpct_##name##_##keyfield##_arg_t value)

/* Search the previous item with a matching key field value */
/* backslash-block */
#define GPCT_CONT_PROTO_LOOKUP_PREV(name, prefix, keyfield)             
GPCT_UNUSED name##_index_t                                                          
prefix##_lookup_prev    (name##_root_t *root,                           
                         name##_index_t index,                          
                         gpct_##name##_##keyfield##_arg_t value)

/* Insert an item in ascending order sorted list */
/* backslash-block */
#define GPCT_CONT_PROTO_INSERT_ASCEND(name, prefix, keyfield)           
GPCT_UNUSED void                                                                  
prefix##_insert_ascend  (name##_root_t *root,                           
                         name##_item_t item)

/* Insert an item in descending order sorted list */
/* backslash-block */
#define GPCT_CONT_PROTO_INSERT_DESCEND(name, prefix, keyfield)          
GPCT_UNUSED void                                                                  
prefix##_insert_descend (name##_root_t *root,                           
                         name##_item_t item)

/* Sort items in container in ascending order */
/* backslash-block */
#define GPCT_CONT_PROTO_SORT_ASCEND(name, prefix, keyfield)             
GPCT_UNUSED void                                                                    
prefix##_sort_ascend    (name##_root_t *root)

/* Sort items in container in descending order */
/* backslash-block */
#define GPCT_CONT_PROTO_SORT_DESCEND(name, prefix, keyfield)            
GPCT_UNUSED void                                                                    
prefix##_sort_descend   (name##_root_t *root)

/* Merge to containers sorted in ascending order */
/* backslash-block */
#define GPCT_CONT_PROTO_MERGE_ASCEND(name, prefix, keyfield)             
GPCT_UNUSED void                                                                    
prefix##_merge_ascend    (name##_root_t *dest, name##_root_t *src)

/* Merge to containers sorted in descending order */
/* backslash-block */
#define GPCT_CONT_PROTO_MERGE_DESCEND(name, prefix, keyfield)            
GPCT_UNUSED void                                                                    
prefix##_merge_descend   (name##_root_t *dest, name##_root_t *src)

/* Test if the container is sorted in ascending order */
/* backslash-block */
#define GPCT_CONT_PROTO_SORTED_ASCEND(name, prefix, keyfield)            
GPCT_UNUSED gpct_bool_t                                                              
prefix##_issorted_ascend   (name##_root_t *root)

/* Test if the container is sorted in descending order */
/* backslash-block */
#define GPCT_CONT_PROTO_SORTED_DESCEND(name, prefix, keyfield)            
GPCT_UNUSED gpct_bool_t                                                              
prefix##_issorted_descend   (name##_root_t *root)

/* Sort items in container in ascending order with a stable algorithm */
/* backslash-block */
#define GPCT_CONT_PROTO_SORT_STABLE_ASCEND(name, prefix, keyfield)      
void                                                                    
prefix##_sort_stable_ascend    (name##_root_t *root)

/* Sort items in container in descending order with a stable algorithm */
/* backslash-block */
#define GPCT_CONT_PROTO_SORT_STABLE_DESCEND(name, prefix, keyfield)     
void                                                                    
prefix##_sort_stable_descend   (name##_root_t *root)

/***********************************************************************
 *      User level lookup functions prototypes macro
 */

/* backslash-region-begin */
#define CONTAINER_KEY_PROTOTYPE(name, attr, prefix, keyfield)

GPCT_UNUSED attr GPCT_CONT_PROTO_LOOKUP(name, prefix, keyfield);
GPCT_UNUSED attr GPCT_CONT_PROTO_LOOKUP_LAST(name, prefix, keyfield);
GPCT_UNUSED attr GPCT_CONT_PROTO_LOOKUP_NEXT(name, prefix, keyfield);
GPCT_UNUSED attr GPCT_CONT_PROTO_LOOKUP_PREV(name, prefix, keyfield);
GPCT_UNUSED attr GPCT_CONT_PROTO_INSERT_ASCEND(name, prefix, keyfield);
GPCT_UNUSED attr GPCT_CONT_PROTO_INSERT_DESCEND(name, prefix, keyfield);
GPCT_UNUSED attr GPCT_CONT_PROTO_MERGE_ASCEND(name, prefix, keyfield);
GPCT_UNUSED attr GPCT_CONT_PROTO_MERGE_DESCEND(name, prefix, keyfield);
GPCT_UNUSED attr GPCT_CONT_PROTO_SORT_ASCEND(name, prefix, keyfield);
GPCT_UNUSED attr GPCT_CONT_PROTO_SORT_DESCEND(name, prefix, keyfield);
GPCT_UNUSED attr GPCT_CONT_PROTO_SORT_STABLE_ASCEND(name, prefix, keyfield);
GPCT_UNUSED attr GPCT_CONT_PROTO_SORT_STABLE_DESCEND(name, prefix, keyfield);
/* backslash-region-end */

/***********************************************************************
 *      User level container types and functions defintions macro
 */

/* backslash-block */
#define         CONTAINER_KEY_TYPE(name, access, testalgo, ...)
  GPCT_CPP_CONCAT(GPCT_CONT_KEY_, access, _KF_ACCESS)(name, testalgo, __VA_ARGS__);

/* backslash-block */
#define         CONTAINER_KEY_FUNC(name, algo, attr, prefix, ...)
  GPCT_CPP_CONCAT(GPCT_CONT_KEY_, algo, _FUNC)(attr, name, prefix, CONTAINER_LOCK_##name, __VA_ARGS__)

/* backslash-block */
#define         CONTAINER_KEY_FUNC_NOLOCK(name, algo, attr, prefix, ...)
  GPCT_CPP_CONCAT(GPCT_CONT_KEY_, algo, _FUNC)(attr, name, prefix, NOLOCK, __VA_ARGS__) 

/* backslash-block */
#define         CONTAINER_KEY_FUNC_LOCK(name, algo, attr, prefix, lockname, ...) 
  GPCT_CPP_CONCAT(GPCT_CONT_KEY_, algo, _FUNC)(attr, name, prefix, lockname, __VA_ARGS__) 

#endif

