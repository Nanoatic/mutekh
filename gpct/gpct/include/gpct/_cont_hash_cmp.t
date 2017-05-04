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

    Hash and compare mechanisms

*/

#ifndef GPCT_HASH_COMPARE_H_
#define GPCT_HASH_COMPARE_H_

#include <string.h>
#include <ctype.h>

#include <gpct/_platform.h>

/***********************************************************************
 *      Test, hash and compare algorithms
 */

typedef uintptr_t gpct_key_hash_t;

/* backslash-region-begin */
#define GPCT_HASH_LIB_PROTO(attr)

attr gpct_key_hash_t
gpct_rawdata_hash_1(const uint8_t *blob, size_t size);

attr gpct_key_hash_t
gpct_rawdata_hash_2(const uint16_t *blob, size_t size);

attr gpct_key_hash_t
gpct_rawdata_hash_4(const uint32_t *blob, size_t size);

attr gpct_key_hash_t
gpct_str_hash(const char *str);

attr gpct_key_hash_t
gpct_strcase_hash(const char *str);
/* backslash-region-end */

/* backslash-region-begin */
#define GPCT_HASH_LIB_FUNC(attr)

attr gpct_key_hash_t
gpct_rawdata_hash_1(const uint8_t *blob, size_t size)
{
  gpct_key_hash_t   val = 0;
  size_t            i;

  for (i = 0; i < size; i++)
    val = blob[i] ^ (val << 1);

  return val;
}

attr gpct_key_hash_t
gpct_rawdata_hash_2(const uint16_t *blob, size_t size)
{
  gpct_key_hash_t   val = 0;
  size_t            i;

  for (i = 0; i < size / 2; i++)
    val = blob[i] ^ (val << 1);

  return val;
}

attr gpct_key_hash_t
gpct_rawdata_hash_4(const uint32_t *blob, size_t size)
{
  gpct_key_hash_t   val = 0;
  size_t            i;

  for (i = 0; i < size / 4; i++)
    val = blob[i] ^ (val << 1);

  return val;
}

attr gpct_key_hash_t
gpct_str_hash(const char *str)
{
  gpct_key_hash_t       val = 0;

  while (*str)
    val = (gpct_key_hash_t)*str++ ^ (val << 3) ^ val;

  return val;
}

attr gpct_key_hash_t
gpct_strcase_hash(const char *str)
{
  gpct_key_hash_t       val = 0;

  while (*str)
    val = (gpct_key_hash_t)tolower(*str++) ^ (val << 3) ^ val;

  return val;
}
/* backslash-region-end */

#ifdef CPGT_CONFIG_USE_LIB
 GPCT_HASH_LIB_PROTO()
#else
 GPCT_HASH_LIB_FUNC(GPCT_UNUSED static)
#endif

/* raw data dispatch function */
GPCT_ALWAYS_INLINE GPCT_INTERNAL gpct_key_hash_t
gpct_rawdata_hash(const void *blob, size_t size)
{
  if (!(size & 3)
#ifdef GPCT_CONFIG_NONALIGNED_ACCESS
      && !(addr & 3)
#endif
      )
    return gpct_rawdata_hash_4((const uint32_t *)blob, size);

  if (!(size & 1)
#ifdef GPCT_CONFIG_NONALIGNED_ACCESS
      && !(addr & 1)
#endif
      )
    return gpct_rawdata_hash_2((const uint16_t *)blob, size);

  return gpct_rawdata_hash_1((const uint8_t *)blob, size);
}


/***********************************************************************
 *      hash and compare shortcut macros used by container code
 */

#define GPCT_CONT_HASH_ARG(name, keyfield, arg)             \
  gpct_##name##_##keyfield##_hash(gpct_##name##_##keyfield##_access_arg(arg))

#define GPCT_CONT_HASH_FIELD(name, keyfield, item)              \
  gpct_##name##_##keyfield##_hash(gpct_##name##_##keyfield##_access_field(item))

#define GPCT_CONT_COMPARE_FIELD_ARG(name, keyfield, item, arg)    \
  gpct_##name##_##keyfield##_compare(gpct_##name##_##keyfield##_access_field(item), \
				     gpct_##name##_##keyfield##_access_arg(arg))

#define GPCT_CONT_COMPARE_FIELDS(name, keyfield, item1, item2)  \
  gpct_##name##_##keyfield##_compare(gpct_##name##_##keyfield##_access_field(item1), \
				     gpct_##name##_##keyfield##_access_field(item2))


/***********************************************************************
 *      define hash and compare functions prototypes
 */

/* compare key values */
#define GPCT_CONT_PROTO_COMPARE(name, keyfield)                         \
intmax_t                                                                \
gpct_##name##_##keyfield##_compare(gpct_##name##_##keyfield##_type_t a,	\
                                   gpct_##name##_##keyfield##_type_t b)

/* compute key value hash */
#define GPCT_CONT_PROTO_HASH(name, keyfield)				\
gpct_key_hash_t                                                         \
gpct_##name##_##keyfield##_hash(gpct_##name##_##keyfield##_type_t a)

/* get key value from item ptr */
#define GPCT_CONT_PROTO_ACCESS_KF(name, keyfield)			\
gpct_##name##_##keyfield##_type_t		                        \
gpct_##name##_##keyfield##_access_field(name##_item_t name##_item)

/* get key value from passed lookup argument */
#define GPCT_CONT_PROTO_ACCESS_ARG(name, keyfield)			\
gpct_##name##_##keyfield##_type_t		                        \
gpct_##name##_##keyfield##_access_arg(gpct_##name##_##keyfield##_arg_t a)


/***********************************************************************
 *      contained field value access
 */

/*
 * Container item is the value to consider for hash and compare, no
 * field actually exist here
 */

/* backslash-region-begin */
#define GPCT_CONT_KEY_DIRECT_KF_ACCESS(name, testalgo, ...)

typedef name##_item_t gpct_##name##__key_t;

GPCT_CPP_CONCAT(GPCT_CONT_KEY_, testalgo, _TYPE)(name, , __VA_ARGS__);

GPCT_ALWAYS_INLINE GPCT_INTERNAL GPCT_CONT_PROTO_ACCESS_KF(name, )
  { return name##_item; }
/* backslash-region-end */


/*
 * Container item is a struct and one of its member is the value to
 * consider for hash and compare.
 */

/* backslash-region-begin */
#define GPCT_CONT_KEY_MEMBER_KF_ACCESS(name, testalgo, keyfield, ...)

typedef typeof((*(name##_item_t*)0).keyfield) gpct_##name##_##keyfield##_key_t;

GPCT_CPP_CONCAT(GPCT_CONT_KEY_, testalgo, _TYPE)(name, keyfield, __VA_ARGS__);

GPCT_ALWAYS_INLINE GPCT_INTERNAL GPCT_CONT_PROTO_ACCESS_KF(name, keyfield)
  { return name##_item.keyfield; }
/* backslash-region-end */


/*
 * Container item is a pointer to a struct and one of its member is
 * the value to consider for hash and compare.
 */

/* backslash-region-begin */
#define GPCT_CONT_KEY_PTR_KF_ACCESS(name, testalgo, keyfield, ...)

typedef typeof(((name##_item_t)0)->keyfield) gpct_##name##_##keyfield##_key_t;

GPCT_CPP_CONCAT(GPCT_CONT_KEY_, testalgo, _TYPE)(name, keyfield, __VA_ARGS__);

GPCT_ALWAYS_INLINE GPCT_INTERNAL GPCT_CONT_PROTO_ACCESS_KF(name, keyfield)
  { return name##_item->keyfield; }
/* backslash-region-end */


/*
 * User code is provided to get value to consider for hash and compare.
 */

/* backslash-region-begin */
#define GPCT_CONT_KEY_CUSTOM_KF_ACCESS(name, testalgo, custom_expr, keyfield, ...)

typedef typeof(custom_expr) gpct_##name##_##keyfield##_key_t;

GPCT_CPP_CONCAT(GPCT_CONT_KEY_, testalgo, _TYPE)(name, keyfield, __VA_ARGS__);

GPCT_ALWAYS_INLINE GPCT_INTERNAL GPCT_CONT_PROTO_ACCESS_KF(name, keyfield)
  { return custom_expr; }
/* backslash-region-end */

/***********************************************************************
 *       lookup function argument access mechanisms
 */

/*
 * Lookup value passed to the container access function has the same
 * type as the value in the container.
 */

/* backslash-region-begin */
#define GPCT_CONT_KEY_DIRECT_ARG_ACCESS(name, keyfield)

 typedef gpct_##name##_##keyfield##_type_t
  gpct_##name##_##keyfield##_arg_t;

GPCT_ALWAYS_INLINE GPCT_INTERNAL GPCT_CONT_PROTO_ACCESS_ARG(name, keyfield)
  { return a; }
/* backslash-region-end */


/*
 * Lookup value passed to the container access function is a pointer
 * to an actual value with the same type as the value in the container.
 */

/* backslash-region-begin */
#define GPCT_CONT_KEY_PTR_ARG_ACCESS(name, keyfield)

 typedef gpct_##name##_##keyfield##_type_t *
  gpct_##name##_##keyfield##_arg_t;

GPCT_ALWAYS_INLINE GPCT_INTERNAL GPCT_CONT_PROTO_ACCESS_ARG(name, keyfield)
  { return *a; }
/* backslash-region-end */

/***********************************************************************
 *      hash and compare mechanisms
 */

/* FIXME intmax_t is bad (too long) */

/*
 * Scalar types with automatic cast,
 * item structure must be defined
 */

/* backslash-region-begin */
#define GPCT_CONT_KEY_SCALAR_TYPE(name, keyfield, ...)
 typedef gpct_##name##_##keyfield##_key_t
  gpct_##name##_##keyfield##_type_t;

GPCT_ALWAYS_INLINE GPCT_INTERNAL GPCT_CONT_PROTO_COMPARE(name, keyfield)
  { return (intmax_t)a - (intmax_t)b; }

GPCT_ALWAYS_INLINE GPCT_INTERNAL GPCT_CONT_PROTO_HASH(name, keyfield)
  { return a; }

GPCT_CONT_KEY_DIRECT_ARG_ACCESS(name, keyfield)
/* backslash-region-end */


/*
 * user specified scalar type
 */

/* backslash-region-begin */
#define GPCT_CONT_KEY_SCALARTYPE_TYPE(name, keyfield, type)
 typedef type gpct_##name##_##keyfield##_type_t;

GPCT_ALWAYS_INLINE GPCT_INTERNAL GPCT_CONT_PROTO_COMPARE(name, keyfield)
  { return (intmax_t)a - (intmax_t)b; }

GPCT_ALWAYS_INLINE GPCT_INTERNAL GPCT_CONT_PROTO_HASH(name, keyfield)
  { return a; }

GPCT_CONT_KEY_DIRECT_ARG_ACCESS(name, keyfield)
/* backslash-region-end */


/*
 * string
 */

/* backslash-region-begin */
#define GPCT_CONT_KEY_STRING_TYPE(name, keyfield, ...)
 typedef const char * gpct_##name##_##keyfield##_type_t;

GPCT_ALWAYS_INLINE GPCT_INTERNAL GPCT_CONT_PROTO_COMPARE(name, keyfield)
  { return strcmp(a, b); }

GPCT_ALWAYS_INLINE GPCT_INTERNAL GPCT_CONT_PROTO_HASH(name, keyfield)
  { return gpct_str_hash(a); }

GPCT_CONT_KEY_DIRECT_ARG_ACCESS(name, keyfield)
/* backslash-region-end */


/*
 * string ignoring case
 */

/* backslash-region-begin */
#define GPCT_CONT_KEY_CASESTRING_TYPE(name, keyfield, ...)
 typedef const char * gpct_##name##_##keyfield##_type_t;

GPCT_ALWAYS_INLINE GPCT_INTERNAL GPCT_CONT_PROTO_COMPARE(name, keyfield)
  { return strcasecmp(a, b); }

GPCT_ALWAYS_INLINE GPCT_INTERNAL GPCT_CONT_PROTO_HASH(name, keyfield)
  { return gpct_strcase_hash(a); }

GPCT_CONT_KEY_DIRECT_ARG_ACCESS(name, keyfield)
/* backslash-region-end */


/*
 * aggregate
 * item structure must be defined
 */

/* backslash-region-begin */
#define GPCT_CONT_KEY_AGGREGATE_TYPE(name, keyfield, ...)
 typedef gpct_##name##_##keyfield##_key_t
   gpct_##name##_##keyfield##_type_t;

GPCT_ALWAYS_INLINE GPCT_INTERNAL GPCT_CONT_PROTO_COMPARE(name, keyfield)
  { return memcmp(&a, &b, sizeof(((name##_item_t)0)->keyfield)); }

GPCT_ALWAYS_INLINE GPCT_INTERNAL GPCT_CONT_PROTO_HASH(name, keyfield)
  { return gpct_rawdata_hash(&a, sizeof(((name##_item_t)0)->keyfield)); }

GPCT_CONT_KEY_PTR_ARG_ACCESS(name, keyfield)
/* backslash-region-end */


/*
 * blob
 */

/* backslash-region-begin */
#define GPCT_CONT_KEY_BLOB_TYPE(name, keyfield, size)
 typedef const void * gpct_##name##_##keyfield##_type_t;

GPCT_ALWAYS_INLINE GPCT_INTERNAL GPCT_CONT_PROTO_COMPARE(name, keyfield)
  { return memcmp(a, b, size); }

GPCT_ALWAYS_INLINE GPCT_INTERNAL GPCT_CONT_PROTO_HASH(name, keyfield)
  { return gpct_rawdata_hash(a, size); }

GPCT_CONT_KEY_DIRECT_ARG_ACCESS(name, keyfield)
/* backslash-region-end */

#endif

