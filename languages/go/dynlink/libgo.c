/* go-type-identity.c -- hash and equality identity functions.

   Copyright 2009 The Go Authors. All rights reserved.
   Use of this source code is governed by a BSD-style
   license that can be found in the LICENSE file.  */

#include <stddef.h>

#include "go-type.h"

/* Typedefs for accesses of different sizes.  */

typedef int QItype __attribute__ ((mode (QI)));
typedef int HItype __attribute__ ((mode (HI)));
typedef int SItype __attribute__ ((mode (SI)));
typedef int DItype __attribute__ ((mode (DI)));

/* An identity hash function for a type.  This is used for types where
   we can simply use the type value itself as a hash code.  This is
   true of, e.g., integers and pointers.  */

size_t
__go_type_hash_identity (const void *key, size_t key_size)
{
  switch (key_size)
    {
    case 1:
      return *(const QItype *) key;
    case 2:
      return *(const HItype *) key;
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
      return *(const SItype *) key;
    default:
      return *(const DItype *) key;
    }
}

/* An identity equality function for a type.  This is used for types
   where we can check for equality by checking that the values have
   the same bits.  */

_Bool
__go_type_equal_identity (const void *k1, const void *k2, size_t key_size)
{
  return __builtin_memcmp (k1, k2, key_size) == 0;
}


/* go-type-interface.c -- hash and equality interface functions.

   Copyright 2009 The Go Authors. All rights reserved.
   Use of this source code is governed by a BSD-style
   license that can be found in the LICENSE file.  */

#include "interface.h"

/* A hash function for an interface.  */

size_t
__go_type_hash_interface (const void *vval,
			  size_t key_size __attribute__ ((unused)))
{
  const struct __go_interface *val;
  const struct __go_type_descriptor *descriptor;
  size_t size;

  val = (const struct __go_interface *) vval;
  if (val->__methods == NULL)
    return 0;
  descriptor = (const struct __go_type_descriptor *) val->__methods[0];
  size = descriptor->__size;
  if (__go_is_pointer_type (descriptor))
    return descriptor->__hashfn (&val->__object, size);
  else
    return descriptor->__hashfn (val->__object, size);
}

/* An equality function for an interface.  */

_Bool
__go_type_equal_interface (const void *vv1, const void *vv2,
			   size_t key_size __attribute__ ((unused)))
{
  const struct __go_interface *v1;
  const struct __go_interface *v2;
  const struct __go_type_descriptor* v1_descriptor;
  const struct __go_type_descriptor* v2_descriptor;

  v1 = (const struct __go_interface *) vv1;
  v2 = (const struct __go_interface *) vv2;
  if (v1->__methods == NULL || v2->__methods == NULL)
    return v1->__methods == v2->__methods;
  v1_descriptor = (const struct __go_type_descriptor *) v1->__methods[0];
  v2_descriptor = (const struct __go_type_descriptor *) v2->__methods[0];
  if (!__go_type_descriptors_equal (v1_descriptor, v2_descriptor))
    return 0;
  if (__go_is_pointer_type (v1_descriptor))
    return v1->__object == v2->__object;
  else
    return v1_descriptor->__equalfn (v1->__object, v2->__object,
				     v1_descriptor->__size);
}


/* go-type-string.c -- hash and equality string functions.

   Copyright 2009 The Go Authors. All rights reserved.
   Use of this source code is governed by a BSD-style
   license that can be found in the LICENSE file.  */

#include "go-string.h"

/* A string hash function for a map.  */

size_t
__go_type_hash_string (const void *vkey,
		       size_t key_size __attribute__ ((unused)))
{
  size_t ret;
  const struct __go_string *key;
  size_t len;
  size_t i;
  const unsigned char *p;

  ret = 5381;
  key = (const struct __go_string *) vkey;
  len = key->__length;
  for (i = 0, p = key->__data; i < len; i++, p++)
    ret = ret * 33 + *p;
  return ret;
}

/* A string equality function for a map.  */

_Bool
__go_type_equal_string (const void *vk1, const void *vk2,
			size_t key_size __attribute__ ((unused)))
{
  const struct __go_string *k1;
  const struct __go_string *k2;

  k1 = (const struct __go_string *) vk1;
  k2 = (const struct __go_string *) vk2;
  return (k1->__length == k2->__length
	  && __builtin_memcmp (k1->__data, k2->__data, k1->__length) == 0);
}


/* go-typedesc-equal.c -- return whether two type descriptors are equal.

   Copyright 2009 The Go Authors. All rights reserved.
   Use of this source code is governed by a BSD-style
   license that can be found in the LICENSE file.  */

/* Compare type descriptors for equality.  This is necessary because
   types may have different descriptors in different shared libraries.
   Also, unnamed types may have multiple type descriptors even in a
   single shared library.  */

_Bool
__go_type_descriptors_equal (const struct __go_type_descriptor *td1,
			     const struct __go_type_descriptor *td2)
{
  if (td1 == td2)
    return 1;
  /* In a type switch we can get a NULL descriptor.  */
  if (td1 == NULL || td2 == NULL)
    return 0;
  if (td1->__code != td2->__code || td1->__hash != td2->__hash)
    return 0;
  if (td1->__uncommon != NULL && td1->__uncommon->__name != NULL)
    {
      if (td2->__uncommon == NULL || td2->__uncommon->__name == NULL)
	return 0;
      return (__go_ptr_strings_equal (td1->__uncommon->__name,
				      td2->__uncommon->__name)
	      && __go_ptr_strings_equal (td1->__uncommon->__pkg_path,
					 td2->__uncommon->__pkg_path));
    }
  if (td2->__uncommon != NULL && td2->__uncommon->__name != NULL)
    return 0;
  return __go_ptr_strings_equal (td1->__reflection, td2->__reflection);
}
