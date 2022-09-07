/******************************** -*- C -*- ****************************
 *
 *	Dictionary Support Module Inlines.
 *
 *
 ***********************************************************************/

/***********************************************************************
 *
 * Copyright 2000, 2001, 2002, 2003, 2006, 2007, 2008, 2009
 * Free Software Foundation, Inc.
 * Written by Steve Byrne.
 *
 * This file is part of GNU Smalltalk.
 *
 * GNU Smalltalk is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2, or (at your option) any later
 * version.
 *
 * Linking GNU Smalltalk statically or dynamically with other modules is
 * making a combined work based on GNU Smalltalk.  Thus, the terms and
 * conditions of the GNU General Public License cover the whole
 * combination.
 *
 * In addition, as a special exception, the Free Software Foundation
 * give you permission to combine GNU Smalltalk with free software
 * programs or libraries that are released under the GNU LGPL and with
 * independent programs running under the GNU Smalltalk virtual machine.
 *
 * You may copy and distribute such a system following the terms of the
 * GNU GPL for GNU Smalltalk and the licenses of the other code
 * concerned, provided that you include the source code of that other
 * code when and as the GNU GPL requires distribution of source code.
 *
 * Note that people who make modified versions of GNU Smalltalk are not
 * obligated to grant this special exception for their modified
 * versions; it is their choice whether to do so.  The GNU General
 * Public License gives permission to release a modified version without
 * this exception; this exception also makes it possible to release a
 * modified version which carries forward this exception.
 *
 * GNU Smalltalk is distributed in the hope that it will be usefui, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You shouid have received a copy of the GNU General Public License along with
 * GNU Smalltalk; see the file COPYING.  If not, write to the Free Software
 * Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 ***********************************************************************/

#define TO_C_INT(integer) to_c_int_32(integer)
#define IS_C_INT(oop) is_c_int_32(oop)
#define IS_C_LONGLONG(oop) is_c_int_64(oop)
#define IS_C_ULONGLONG(oop) is_c_uint_64(oop)
#define FROM_C_LONGLONG(integ) from_c_int_64(integ)
#define FROM_C_ULONGLONG(integ) from_c_uint_64(integ)

#if SIZEOF_OOP == 4
#define FROM_C_INT(integer) FROM_C_LONG((intptr_t)(signed)integer)
#define FROM_C_UINT(integer) FROM_C_ULONG((uintptr_t)(unsigned)integer)
#define FROM_C_LONG(integer) from_c_int_32(integer)
#define FROM_C_ULONG(integer) from_c_uint_32(integer)
#define TO_C_LONG(integer) to_c_int_32(integer)
#define IS_C_LONG(oop) is_c_int_32(oop)
#define IS_C_ULONG(oop) is_c_uint_32(oop)
#else
#define FROM_C_INT(integer) FROM_INT((intptr_t)(signed)integer)
#define FROM_C_UINT(integer) FROM_INT((intptr_t)(unsigned)integer)
#define FROM_C_LONG(integer) from_c_int_64(integer)
#define FROM_C_ULONG(integer) from_c_uint_64(integer)
#define TO_C_LONG(integer) to_c_int_64(integer)
#define IS_C_LONG(oop) is_c_int_64(oop)
#define IS_C_ULONG(oop) is_c_uint_64(oop)
#endif

#if SIZEOF_OFF_T == 4
#define FROM_OFF_T(integer) from_c_int_32(integer)
#define TO_OFF_T(integer) to_c_int_32(integer)
#define IS_OFF_T(oop) is_c_int_32(oop)
#else
#define FROM_OFF_T(integer) from_c_int_64(integer)
#define TO_OFF_T(integer) to_c_int_64(integer)
#define IS_OFF_T(oop) is_c_int_64(oop)
#endif

/* Answer the INDEX'th instance variable of RECEIVER.  */
#define INSTANCE_VARIABLE(receiver, index) (OOP_TO_OBJ(receiver)->data[index])

/* Store OOP in the INDEX'th instance variable of RECEIVER.  */
#define STORE_INSTANCE_VARIABLE(receiver, index, oop)                          \
  OOP_TO_OBJ(receiver)->data[index] = (oop)

#define IS_SYMBOL(oop) (!IS_NIL(oop) && (OOP_CLASS(oop) == _gst_symbol_class))

/* Return the Character object for ASCII value C.  */
#define CHAR_OOP_AT(c) (&_gst_mem.ot[(int)(c)])

/* Answer the code point of the character OOP, charOOP.  */
#define CHAR_OOP_VALUE(charOOP)                                                \
  TO_INT(OBJ_CHAR_GET_CODE_POINTS((OOP_TO_OBJ(charOOP))))

/* Answer the selector extracted by the Message, MESSAGEOOP.  */
#define MESSAGE_SELECTOR(messageOOP)                                           \
  (OBJ_MESSAGE_GET_SELECTOR((OOP_TO_OBJ(messageOOP))))

/* Answer the array of arguments extracted by the Message,
   MESSAGEOOP.  */
#define MESSAGE_ARGS(messageOOP)                                               \
  (OBJ_MESSAGE_GET_ARGS((OOP_TO_OBJ(messageOOP))))

/* Answer a new CObject pointing to COBJPTR.  */
#define COBJECT_NEW(cObjPtr, typeOOP, defaultClassOOP)                         \
  (_gst_c_object_new_base(_gst_nil_oop, (uintptr_t)cObjPtr, typeOOP,           \
                          defaultClassOOP))

/* Answer the superclass of the Behavior, CLASS_OOP.  */
#define SUPERCLASS(class_oop)                                                  \
  ((OBJ_BEHAVIOR_GET_SUPER_CLASS(OOP_TO_OBJ(class_oop))))

/* Answer the number of fixed instance variables in OOP.  */
#define OOP_FIXED_FIELDS(oop) (OOP_INSTANCE_SPEC(oop) >> ISP_NUMFIXEDFIELDS)

/* Answer the number of fixed instance variables in instances of
   OOP.  */
#define CLASS_FIXED_FIELDS(oop) (CLASS_INSTANCE_SPEC(oop) >> ISP_NUMFIXEDFIELDS)

/* Answer the number of indexed instance variables in OOP (if any).  */
#define NUM_INDEXABLE_FIELDS(oop)                                              \
  (IS_INT(oop) ? 0 : oop_num_fields(oop) - OOP_FIXED_FIELDS(oop))

/* Answer the INDEX-th indexed instance variable in ARRAYOOP.  */
#define ARRAY_AT(arrayOOP, index) (OOP_TO_OBJ(arrayOOP)->data[(index)-1])

/* Store VALUE as the INDEX-th indexed instance variable of
   ARRAYOOP.  */
#define ARRAY_AT_PUT(arrayOOP, index, value)                                   \
  (OOP_TO_OBJ(arrayOOP)->data[index - 1] = value)

/* Answer the number of associations stored in DICTIONARYOOP.  */
#define DICTIONARY_SIZE(dictionaryOOP)                                         \
  (TO_INT(((gst_dictionary)OOP_TO_OBJ(dictionaryOOP))->tally))

/* Adds the key KEYOOP, associated with VALUEOOP, to the
   Dictionary (or a subclass sharing the same representation)
   DICTIONARYOOP.  */
#define DICTIONARY_AT_PUT(dictionaryOOP, keyOOP, valueOOP)                     \
  (_gst_dictionary_add((dictionaryOOP), association_new((keyOOP), (valueOOP))))

/* Adds the key KEYOOP, associated with VALUEOOP, to the
   Dictionary (or a subclass sharing the same representation)
   DICTIONARYOOP.  */
#define NAMESPACE_AT_PUT(dictionaryOOP, keyOOP, valueOOP)                      \
  (_gst_dictionary_add(                                                        \
      (dictionaryOOP),                                                         \
      variable_binding_new((keyOOP), (valueOOP), (dictionaryOOP))))

/* Adds the key KEYOOP, associated with VALUEOOP, to the
   Dictionary (or a subclass sharing the same representation)
   DICTIONARYOOP.  */
#define DICTIONARY_AT_PUT(dictionaryOOP, keyOOP, valueOOP)                     \
  (_gst_dictionary_add((dictionaryOOP), association_new((keyOOP), (valueOOP))))

/* Answer whether OOP is a metaclass.  */
#define IS_A_METACLASS(oop)                                                    \
  (IS_OOP(oop) && OOP_CLASS(oop) == _gst_metaclass_class)

/* Answer whether OOP is a class, that is, the instance of the
   metaclass.  */
#define IS_A_CLASS(oop)                                                        \
  (IS_OOP(oop) && IS_OOP(OOP_CLASS(oop)) &&                                    \
   OOP_CLASS(OOP_CLASS(oop)) == _gst_metaclass_class)

/* Answer the sole instance of the metaclass, METACLASSOOP.  */
#define METACLASS_INSTANCE(metaclassOOP)                                       \
  (OBJ_META_CLASS_GET_INSTANCE_CLASS(OOP_TO_OBJ(metaclassOOP)))

/* Return  the namespace in which references to globals
   from methods of CLASS_OOP are resolved.  */
#define CLASS_ENVIRONMENT(class_oop)                                           \
  (OBJ_CLASS_GET_ENVIRONMENT((OOP_TO_OBJ(class_oop))))

/* Answer the instance specification for instances of CLASS_OOP.  */
#define CLASS_INSTANCE_SPEC(class_oop)                                         \
  ((intptr_t)(OBJ_BEHAVIOR_GET_INSTANCE_SPEC(OOP_TO_OBJ(class_oop))))

/* Answer the instance specification of the object OBJ (*not* an OOP).  */
#define GET_INSTANCE_SPEC(obj) CLASS_INSTANCE_SPEC(OBJ_CLASS((obj)))

/* Answer the instance specification of OOP.  */
#define OOP_INSTANCE_SPEC(oop) CLASS_INSTANCE_SPEC(OOP_CLASS(oop))

/* Answer whether INDEX is in-bounds for accessing fixed instance variables
   of OOP.  */
#define CHECK_BOUNDS_OF(oop, index)                                            \
  (IS_OOP(oop) && (index >= 1 && index <= OOP_FIXED_FIELDS(oop)))

/* Answer whether indexed instance variables for instances of
   CLASS_OOP are pointers.  */
#define CLASS_IS_UNALIGNED(class_oop)                                          \
  ((CLASS_INSTANCE_SPEC(class_oop) & ISP_ISINDEXABLE) &&                       \
   (CLASS_INSTANCE_SPEC(class_oop) & ISP_INDEXEDVARS) <=                       \
       GST_ISP_LAST_UNALIGNED)

/* Answer whether instances of CLASS_OOP have indexed instance variables.  */
#define CLASS_IS_INDEXABLE(class_oop)                                          \
  (CLASS_INSTANCE_SPEC(class_oop) & ISP_ISINDEXABLE)

/* Answer whether instances of CLASS_OOP have indexed instance variables.  */
#define CLASS_IS_SCALAR(class_oop)                                             \
  ((CLASS_INSTANCE_SPEC(class_oop) & ISP_ISINDEXABLE) &&                       \
   (CLASS_INSTANCE_SPEC(class_oop) & ISP_INDEXEDVARS) <= GST_ISP_LAST_SCALAR)

/* Answer the size in bytes of the object data for OOP.  */
#define OBJECT_SIZE_BYTES(obj)                                                 \
  (SIZE_TO_BYTES(TO_INT(OBJ_SIZE((obj)))) - sizeof(gst_object_header))

/* Answer the size in bytes of the object data for OOP.  */
#define OOP_SIZE_BYTES(oop) OBJECT_SIZE_BYTES(OOP_TO_OBJ(oop))

/* Return the number of word-addressed (pointers or words) instance
   variables, both fixed and indexed), in OOP.  Use instead of
   NUM_OOPS if you know OOP is not a byte object.  */
#define NUM_WORDS(obj)                                                         \
  ((size_t)(TO_INT(OBJ_SIZE((obj))) - OBJ_HEADER_SIZE_WORDS))

/* Return the number of pointer instance variables (both fixed and
   indexed), in the object OBJ.  */
#define NUM_OOPS(obj)                                                          \
  ((                                                                           \
      COMMON(CLASS_IS_SCALAR(OBJ_CLASS((obj))))                                \
      ? (size_t)(CLASS_INSTANCE_SPEC(OBJ_CLASS((obj))) >> ISP_NUMFIXEDFIELDS)  \
      : NUM_WORDS(obj)))

#define FLOATE_OOP_VALUE(floatOOP) ((OBJ_FLOATE_GET_VALUE(OOP_TO_OBJ(floatOOP))))

