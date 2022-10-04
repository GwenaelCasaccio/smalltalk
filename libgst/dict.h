/******************************** -*- C -*- ****************************
 *
 *	Dictionary Support Module Definitions.
 *
 *
 ***********************************************************************/

/***********************************************************************
 *
 * Copyright 2000, 2001, 2002, 2003, 2005, 2006, 2008, 2009
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
 * GNU Smalltalk is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * GNU Smalltalk; see the file COPYING.  If not, write to the Free Software
 * Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 ***********************************************************************/

#ifndef GST_DICT_H
#define GST_DICT_H

/***********************************************************************
 *
 *	Below are the structural definitions for several of the important
 *	objects present in the Smalltalk system.  Their C representation
 *	corresponds exactly with their Smalltalk representation.
 *
 ***********************************************************************/

#define COBJECT_ANON_TYPE FROM_INT(-1)
#define COBJECT_CHAR_TYPE FROM_INT(0)
#define COBJECT_UNSIGNED_CHAR_TYPE FROM_INT(1)
#define COBJECT_SHORT_TYPE FROM_INT(2)
#define COBJECT_UNSIGNED_SHORT_TYPE FROM_INT(3)
#define COBJECT_LONG_TYPE FROM_INT(4)
#define COBJECT_UNSIGNED_LONG_TYPE FROM_INT(5)
#define COBJECT_FLOAT_TYPE FROM_INT(6)
#define COBJECT_DOUBLE_TYPE FROM_INT(7)
#define COBJECT_STRING_TYPE FROM_INT(8)
#define COBJECT_SMALLTALK_TYPE FROM_INT(9)
#define COBJECT_INT_TYPE FROM_INT(10)
#define COBJECT_UNSIGNED_INT_TYPE FROM_INT(11)

/*    3                   2                   1
    1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |    # fixed fields             |      unused       |I| kind  |1|
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

   I'm moving it to bits 13-30 (it used to stay at bits 5-30),
   allocating space for more flags in case they're needed.
   If you change ISP_NUMFIXEDFIELDS you should modify Behavior.st too.
   Remember to shift by ISP_NUMFIXEDFIELDS-1 there, since Smalltalk does
   not see GST_ISP_INTMARK!!  */

enum {
  ISP_PROXY = 0x80000000,

  ISP_NUMFIXEDFIELDS = 13,

  /* Set if the instances of the class have indexed instance
     variables.  */
  ISP_ISINDEXABLE = 32,

  /* These represent the shape of the indexed instance variables of
     the instances of the class.  */
  ISP_INDEXEDVARS = 62,
  ISP_SHAPE = 30,

  /* Set to 1 to mark a SmallInteger.  */
  GST_ISP_INTMARK = 1
};

extern OOP _gst_abstract_namespace_class ATTRIBUTE_HIDDEN;
extern OOP _gst_array_class ATTRIBUTE_HIDDEN;
extern OOP _gst_arrayed_collection_class ATTRIBUTE_HIDDEN;
extern OOP _gst_association_class ATTRIBUTE_HIDDEN;
extern OOP _gst_behavior_class ATTRIBUTE_HIDDEN;
extern OOP _gst_binding_dictionary_class ATTRIBUTE_HIDDEN;
extern OOP _gst_block_closure_class ATTRIBUTE_HIDDEN;
extern OOP _gst_block_context_class ATTRIBUTE_HIDDEN;
extern OOP _gst_boolean_class ATTRIBUTE_HIDDEN;
extern OOP _gst_byte_array_class ATTRIBUTE_HIDDEN;
extern OOP _gst_c_callable_class ATTRIBUTE_HIDDEN;
extern OOP _gst_c_func_descriptor_class ATTRIBUTE_HIDDEN;
extern OOP _gst_c_callback_descriptor_class ATTRIBUTE_HIDDEN;
extern OOP _gst_c_object_class ATTRIBUTE_HIDDEN;
extern OOP _gst_c_object_type_ctype ATTRIBUTE_HIDDEN;
extern OOP _gst_c_type_class ATTRIBUTE_HIDDEN;
extern OOP _gst_callin_process_class ATTRIBUTE_HIDDEN;
extern OOP _gst_char_class ATTRIBUTE_HIDDEN;
extern OOP _gst_character_array_class ATTRIBUTE_HIDDEN;
extern OOP _gst_class_class ATTRIBUTE_HIDDEN;
extern OOP _gst_class_description_class ATTRIBUTE_HIDDEN;
extern OOP _gst_collection_class ATTRIBUTE_HIDDEN;
extern OOP _gst_compiled_block_class ATTRIBUTE_HIDDEN;
extern OOP _gst_compiled_code_class ATTRIBUTE_HIDDEN;
extern OOP _gst_compiled_method_class ATTRIBUTE_HIDDEN;
extern OOP _gst_context_part_class ATTRIBUTE_HIDDEN;
extern OOP _gst_continuation_class ATTRIBUTE_HIDDEN;
extern OOP _gst_date_class ATTRIBUTE_HIDDEN;
extern OOP _gst_deferred_variable_binding_class ATTRIBUTE_HIDDEN;
extern OOP _gst_dictionary_class ATTRIBUTE_HIDDEN;
extern OOP _gst_directed_message_class ATTRIBUTE_HIDDEN;
extern OOP _gst_message_lookup_class ATTRIBUTE_HIDDEN;
extern OOP _gst_false_class ATTRIBUTE_HIDDEN;
extern OOP _gst_file_descriptor_class ATTRIBUTE_HIDDEN;
extern OOP _gst_file_segment_class ATTRIBUTE_HIDDEN;
extern OOP _gst_file_stream_class ATTRIBUTE_HIDDEN;
extern OOP _gst_float_class ATTRIBUTE_HIDDEN;
extern OOP _gst_floatd_class ATTRIBUTE_HIDDEN;
extern OOP _gst_floate_class ATTRIBUTE_HIDDEN;
extern OOP _gst_floatq_class ATTRIBUTE_HIDDEN;
extern OOP _gst_fraction_class ATTRIBUTE_HIDDEN;
extern OOP _gst_hashed_collection_class ATTRIBUTE_HIDDEN;
extern OOP _gst_homed_association_class ATTRIBUTE_HIDDEN;
extern OOP _gst_identity_dictionary_class ATTRIBUTE_HIDDEN;
extern OOP _gst_identity_set_class ATTRIBUTE_HIDDEN;
extern OOP _gst_integer_class ATTRIBUTE_HIDDEN;
extern OOP _gst_iterable_class ATTRIBUTE_HIDDEN;
extern OOP _gst_interval_class ATTRIBUTE_HIDDEN;
extern OOP _gst_large_integer_class ATTRIBUTE_HIDDEN;
extern OOP _gst_large_negative_integer_class ATTRIBUTE_HIDDEN;
extern OOP _gst_large_positive_integer_class ATTRIBUTE_HIDDEN;
extern OOP _gst_large_zero_integer_class ATTRIBUTE_HIDDEN;
extern OOP _gst_link_class ATTRIBUTE_HIDDEN;
extern OOP _gst_linked_list_class ATTRIBUTE_HIDDEN;
extern OOP _gst_lookup_key_class ATTRIBUTE_HIDDEN;
extern OOP _gst_lookup_table_class ATTRIBUTE_HIDDEN;
extern OOP _gst_magnitude_class ATTRIBUTE_HIDDEN;
extern OOP _gst_memory_class ATTRIBUTE_HIDDEN;
extern OOP _gst_message_class ATTRIBUTE_HIDDEN;
extern OOP _gst_metaclass_class ATTRIBUTE_HIDDEN;
extern OOP _gst_method_context_class ATTRIBUTE_HIDDEN;
extern OOP _gst_method_dictionary_class ATTRIBUTE_HIDDEN;
extern OOP _gst_method_info_class ATTRIBUTE_HIDDEN;
extern OOP _gst_namespace_class ATTRIBUTE_HIDDEN;
extern OOP _gst_number_class ATTRIBUTE_HIDDEN;
extern OOP _gst_object_class ATTRIBUTE_HIDDEN;
extern OOP _gst_object_memory_class ATTRIBUTE_HIDDEN;
extern OOP _gst_ordered_collection_class ATTRIBUTE_HIDDEN;
extern OOP _gst_positionable_stream_class ATTRIBUTE_HIDDEN;
extern OOP _gst_process_class ATTRIBUTE_HIDDEN;
extern OOP _gst_processor_scheduler_class ATTRIBUTE_HIDDEN;
extern OOP _gst_read_stream_class ATTRIBUTE_HIDDEN;
extern OOP _gst_read_write_stream_class ATTRIBUTE_HIDDEN;
extern OOP _gst_root_namespace_class ATTRIBUTE_HIDDEN;
extern OOP _gst_semaphore_class ATTRIBUTE_HIDDEN;
extern OOP _gst_sequenceable_collection_class ATTRIBUTE_HIDDEN;
extern OOP _gst_set_class ATTRIBUTE_HIDDEN;
extern OOP gst_small_integer_class;
extern OOP _gst_smalltalk_dictionary ATTRIBUTE_HIDDEN;
extern OOP _gst_sorted_collection_class ATTRIBUTE_HIDDEN;
extern OOP _gst_stream_class ATTRIBUTE_HIDDEN;
extern OOP _gst_string_class ATTRIBUTE_HIDDEN;
extern OOP _gst_sym_link_class ATTRIBUTE_HIDDEN;
extern OOP _gst_symbol_class ATTRIBUTE_HIDDEN;
extern OOP _gst_system_dictionary_class ATTRIBUTE_HIDDEN;
extern OOP _gst_time_class ATTRIBUTE_HIDDEN;
extern OOP _gst_true_class ATTRIBUTE_HIDDEN;
extern OOP _gst_undefined_object_class ATTRIBUTE_HIDDEN;
extern OOP _gst_unicode_character_class ATTRIBUTE_HIDDEN;
extern OOP _gst_unicode_string_class ATTRIBUTE_HIDDEN;
extern OOP _gst_variable_binding_class ATTRIBUTE_HIDDEN;
extern OOP _gst_weak_array_class ATTRIBUTE_HIDDEN;
extern OOP _gst_weak_set_class ATTRIBUTE_HIDDEN;
extern OOP _gst_weak_key_dictionary_class ATTRIBUTE_HIDDEN;
extern OOP _gst_weak_value_lookup_table_class ATTRIBUTE_HIDDEN;
extern OOP _gst_weak_identity_set_class ATTRIBUTE_HIDDEN;
extern OOP _gst_weak_key_identity_dictionary_class ATTRIBUTE_HIDDEN;
extern OOP _gst_weak_value_identity_dictionary_class ATTRIBUTE_HIDDEN;
extern OOP _gst_write_stream_class ATTRIBUTE_HIDDEN;
extern OOP _gst_processor_oop[100] ATTRIBUTE_HIDDEN;
extern OOP _gst_key_hash_oop ATTRIBUTE_HIDDEN;
extern OOP _gst_debug_information_class ATTRIBUTE_HIDDEN;
extern OOP _gst_key_hash_class ATTRIBUTE_HIDDEN;

/* The size of the indexed instance variables corresponding to the
   various instanceSpec values declared in gstpriv.h.  */
extern signed char _gst_log2_sizes[32] ATTRIBUTE_HIDDEN;

/* Creates a new instance of the Dictionary class with room for SIZE
   items.  */
extern OOP _gst_dictionary_new(int size) ATTRIBUTE_HIDDEN;

/* Creates a new instance of the BindingDictionary class with room for SIZE
   items.  The object has the ENVIRONMENTOOP environment (which for
   dictionary of class variables is the class that hosts the dictionary).  */
extern OOP _gst_binding_dictionary_new(int size,
                                       OOP environmentOOP) ATTRIBUTE_HIDDEN;

/* Gets the method dictionary associated with CLASS_OOP, and returns
   it.  If the methodDictionary associated with CLASS_OOP is nil, one
   is created and installed into that class.  */
extern OOP _gst_valid_class_method_dictionary(OOP class_oop) ATTRIBUTE_HIDDEN;

/* This returns the dictionary of class variables for CLASS_OOP */
extern OOP
_gst_class_variable_dictionary(OOP class_oop) ATTRIBUTE_PURE ATTRIBUTE_HIDDEN;

/* This finds the key SYMBOL into the dictionary POOLOOP and, if any,
   in all of its super-namespaces.  Returns the association.  */
extern OOP _gst_namespace_association_at(OOP poolOOP, OOP symbol)
    ATTRIBUTE_PURE ATTRIBUTE_HIDDEN;

/* This finds the key SYMBOL into the dictionary POOLOOP and, if any,
   in all of its super-namespaces.  Returns the value.  */
extern OOP _gst_namespace_at(OOP poolOOP,
                             OOP symbol) ATTRIBUTE_PURE ATTRIBUTE_HIDDEN;

/* Adds the Association in ASSOCIATIONOOP to the Dictionary (or a
   subclass sharing the same representation) DICTIONARYOOP.  */
extern OOP _gst_dictionary_add(OOP dictionaryOOP,
                               OOP associationOOP) ATTRIBUTE_HIDDEN;

/* Answer the number of slots that are in a dictionary of
   OLDNUMFIELDS items after growing it.  */
extern size_t new_num_fields(size_t oldNumFields);

/* Create a new instance of method dictionary. */
extern OOP _gst_method_dictionary_new(size_t wanted_size);

/* Adds the KeyOOP and the ValueOOP in the MethodDictionaryOOP */
extern OOP _gst_method_dictionary_at_put(OOP methodDictionaryOOP, OOP keyOOP, OOP valueOOP);

/* Create a new instance of CLASSOOP (an IdentityDictionary subclass) and answer it. */
extern OOP _gst_identity_dictionary_new(OOP classOOP, int size);

/* Look for the value associated to KEYOOP in IDENTITYDICTIONARYOOP
   and answer it or, if not found, _gst_nil_oop.  */
extern OOP _gst_identity_dictionary_at(OOP identityDictionaryOOP, OOP keyOOP)
    ATTRIBUTE_PURE ATTRIBUTE_HIDDEN;

/* Creates a String object starting from the NUL-terminated string
   S.  */
extern OOP _gst_string_new(const char *s) ATTRIBUTE_HIDDEN;

/* Creates a String object starting from the NUL-terminated wide
   string S.  */
extern OOP _gst_unicode_string_new(const wchar_t *s) ATTRIBUTE_HIDDEN;

/* Look in the Smalltalk dictionary for a class whose name is in the
   Symbol CLASSNAMEOOP.  */
extern OOP _gst_find_class(OOP classNameOOP) ATTRIBUTE_PURE ATTRIBUTE_HIDDEN;

/* Look for an implementation of SELECTOR (a Symbol) into CLASS_OOP's
   method dictionary or in the method dictionary of a superclass.  */
extern OOP _gst_find_class_method(OOP class_oop,
                                  OOP selector) ATTRIBUTE_PURE ATTRIBUTE_HIDDEN;

/* Create a new Message object for the given SELECTOR (a Symbol) and
   Array of arguments.  */
extern OOP _gst_message_new_args(OOP selectorOOP,
                                 OOP argsArray) ATTRIBUTE_HIDDEN;

/* Returns the name of CLASS_OOP (a Smalltalk Class) */
extern OOP _gst_get_class_symbol(OOP class_oop) ATTRIBUTE_PURE ATTRIBUTE_HIDDEN;

/* Create and return an exact copy of OOP, which can be any kind
   of OOP.  Builtin OOPs and integers are returned unchanged,
   while for other objects this is a "shallow copy"; all the
   instance variables in the new object are the exact same ones
   that are in the original object.  */
extern OOP _gst_object_copy(OOP oop) ATTRIBUTE_HIDDEN;

/* Returns the array of the names of the instance variables of
   CLASS_OOP (a Smalltalk Class).  */
extern OOP
_gst_instance_variable_array(OOP class_oop) ATTRIBUTE_PURE ATTRIBUTE_HIDDEN;

/* Returns the array of the names of the pool dictionaries of
   CLASS_OOP (a Smalltalk Class).  */
extern OOP
_gst_shared_pool_dictionary(OOP class_oop) ATTRIBUTE_PURE ATTRIBUTE_HIDDEN;

/* Creates a new CObject pointing to cObjOfs bytes in BASEOOP (or
   at the absolute address cObjOfs if BASEOOP is NULL), extracting the class
   to be instantiated from the CType, TYPEOOP, or using the provided
   class if TYPEOOP is nil.  */
extern OOP _gst_c_object_new_base(OOP baseOOP, uintptr_t cObjOfs, OOP typeOOP,
                                  OOP defaultClassOOP) ATTRIBUTE_HIDDEN;

/* Creates a new String with LEN indexed instance variables.  */
extern OOP _gst_new_string(size_t len) ATTRIBUTE_HIDDEN;

/* Creates a new ByteArray containing LEN bytes starting at BYTES.  */
extern OOP _gst_byte_array_new(const gst_uchar *bytes,
                               size_t len) ATTRIBUTE_HIDDEN;

/* Creates a new String containing LEN characters starting at S.  */
extern OOP _gst_counted_string_new(const char *s, size_t len) ATTRIBUTE_HIDDEN;

/* Adds the key KEYOOP, associated with VALUEOOP, to the
   IdentityDictionary (or a subclass sharing the same representation)
   IDENTITYDICTIONARYOOP.  */
extern OOP _gst_identity_dictionary_at_put(OOP identityDictionaryOOP,
                                           OOP keyOOP,
                                           OOP valueOOP) ATTRIBUTE_HIDDEN;

/* Allocates and returns a new C (NULL-terminated) string that has the same
   contents as STRINGOOP.  Even if there are embedded NULs, the
   allocated area has always a size of "stringOOP size + 1" bytes.  */
extern char *_gst_to_cstring(OOP stringOOP) ATTRIBUTE_HIDDEN;

/* Allocates and returns a new wide C string that has the same
   contents as STRINGOOP.  Even if there are embedded NULs, the
   allocated area has always a size of "unicodeStringOOP size + 1"
   bytes.  */
extern wchar_t *_gst_to_wide_cstring(OOP unicodeStringOOP) ATTRIBUTE_HIDDEN;

/* Allocates and returns a new memory block that has the same contents
   as BYTEARRAYOOP.  Even if there are embedded NULs, the allocated
   area has always a size of "byteArrayOOP size" bytes.  */
extern gst_uchar *_gst_to_byte_array(OOP byteArrayOOP) ATTRIBUTE_HIDDEN;

/* Creates the kernel classes of the Smalltalk system.  Operates in two
   passes: pass1 creates the class objects, but they're not completely
   initialized.  pass2 finishes the initialization process.  The garbage
   collector can NOT run during this time.  */
extern void _gst_init_dictionary(void) ATTRIBUTE_HIDDEN;

/* Copies the first bytes of S into STRINGOOP (as many bytes as the
   OOP can hold).  */
extern void _gst_set_oopstring(OOP stringOOP, const char *s) ATTRIBUTE_HIDDEN;

/* Copies the first bytes of S into STRINGOOP (as many bytes as the
   OOP can hold).  */
extern void _gst_set_oop_unicode_string(OOP stringOOP,
                                        const wchar_t *s) ATTRIBUTE_HIDDEN;

/* Set the instance variables of the FileStream object,
   FILESTREAMOOP.  If BUFFERED is true, the variables
   inherited by Streams are set to a 1024-bytes String.  */
extern void _gst_set_file_stream_file(OOP fileStreamOOP, int fd,
                                      OOP fileNameOOP, OOP isPipe,
                                      int access,
                                      bool buffered) ATTRIBUTE_HIDDEN;

/* Copies the first bytes of BYTES into BYTEARRAYOOP (as many bytes as
   the OOP can hold).  */
extern void _gst_set_oop_bytes(OOP byteArrayOOP,
                               gst_uchar *bytes) ATTRIBUTE_HIDDEN;

/* Frees the memory pointed to by the CObject, COBJOOP.  */
extern void _gst_free_cobject(OOP cObjOOP) ATTRIBUTE_HIDDEN;

/* Loads the contents of the global variables from the Smalltalk dictionary
   after an image has been restored.  PRIM_TABLE_MATCHES if true if the
   table of primitives is already set up correctly.  */
extern bool _gst_init_dictionary_on_image_load(
    bool prim_table_matches) ATTRIBUTE_HIDDEN;

/* Transforms a primitive name into a primitive index, looking up
   the VMPrimitives dictionary.  */
extern int _gst_resolve_primitive_name(char *name) ATTRIBUTE_HIDDEN;

/* Entry point for the profiler.  */
extern void _gst_record_profile(OOP oldMethod, OOP newMethod,
                                int ipOffset) ATTRIBUTE_HIDDEN;

/* Scramble the bits of X.  */
extern uintptr_t scramble(uintptr_t x) ATTRIBUTE_HIDDEN;

/* Checks to see if TESTEDOOP is a subclass of CLASS_OOP, returning
   true if it is.  */
extern bool is_a_kind_of(OOP testedOOP, OOP class_oop)  ATTRIBUTE_HIDDEN;

/* Stores the VALUE Object (which must be an appropriate Integer for
   byte or word objects) into the INDEX-th indexed instance variable
   of the Object pointed to by OOP.  Returns whether the INDEX is
   correct and the VALUE has the appropriate class and/or range.  */
extern bool index_oop_put_spec(OOP oop, gst_object object,
                               size_t index,
                               OOP value,
                               intptr_t instanceSpec) ATTRIBUTE_HIDDEN;

/* Stores the VALUE Object (which must be an appropriate Integer for
   byte or word objects) into the INDEX-th indexed instance variable
   of the Object pointed to by OOP.  Returns whether the INDEX is
   correct and the VALUE has the appropriate class and/or range.  */
extern bool index_oop_put(OOP oop, size_t index, OOP value) ATTRIBUTE_HIDDEN;

/* Stores the VALUE Object (which must be an appropriate Integer for
   byte or word objects and if accessing indexed instance variables)
   into the INDEX-th instance variable of the Object pointed to by
   OOP.  */
extern void inst_var_at_put(OOP oop, int index, OOP value) ATTRIBUTE_HIDDEN;
extern void atomic_inst_var_at_put(OOP oop, int index, OOP value);

/* Returns the INDEX-th instance variable of the Object pointed to by
   OOP.  No range checks are done in INDEX.  */
extern OOP inst_var_at(OOP oop, int index);
extern OOP atomic_inst_var_at(OOP oop, int index);

/* Returns the number of instance variables (both fixed and indexed) in OOP.  */
extern int oop_num_fields(OOP oop);

/* Fill OOPCOUNT pointers to OOPs, starting at OOPPTR,
   with OOPs for the NIL object.  */
extern void nil_fill(OOP *oopPtr, size_t oopCount);

/* Returns a new, uninitialized instance of CLASS_OOP with
   NUMINDEXFIELDS indexable fields.  Returns an OOP for a newly
   allocated instance of CLASS_OOP, with NUMINDEXFIELDS fields.  The
   object data is returned, the OOP is stored in P_OOP.  The OOP is
   adjusted to reflect any variance in size (such as a string that's
   shorter than a word boundary).  */
extern gst_object new_instance_with(OOP class_oop, size_t numIndexFields,
                                           OOP *p_oop);

/* Creates a new instance of class CLASS_OOP.  The space is allocated,
   the class and size fields of the class are filled in, and the
   instance is returned.  Its fields are NOT INITIALIZED.  CLASS_OOP
   must represent a class with no indexable fields. An OOP will be
   allocated and stored in P_OOP.  */
extern gst_object new_instance(OOP class_oop, OOP *p_oop);

/* Returns a new, initialized instance of CLASS_OOP within an
   object of size NUMBYTES.  INSTANCESPEC is used to find the
   number of fixed instance variables and initialize them to
   _gst_nil_oop.  The pointer to the object data is returned,
   the OOP is stored in P_OOP.  The OOP is not adjusted to reflect
   any variance in size (such as a string that's shorter than a word
   boundary).  */
extern gst_object instantiate_numbytes(OOP class_oop, OOP *p_oop,
                                              intptr_t instanceSpec,
                                              size_t numBytes);

/* Returns a new, initialized instance of CLASS_OOP with
   NUMINDEXFIELDS indexable fields.  If the instance contains
   pointers, they are initialized to _gst_nil_oop, else they are set
   to the SmallInteger 0.  The pointer to the object data is returned,
   the OOP is stored in P_OOP.  The OOP is adjusted to reflect any
   variance in size (such as a string that's shorter than a word
   boundary).  */
extern gst_object instantiate_with(OOP class_oop, size_t numIndexFields,
                                          OOP *p_oop);

/* Create and return a new instance of class CLASS_OOP.  CLASS_OOP
   must be a class with no indexable fields.  The named instance
   variables of the new instance are initialized to _gst_nil_oop,
   since fixed-field-only objects can only have pointers. The pointer
   to the object data is returned, the OOP is stored in P_OOP.  */
extern gst_object instantiate(OOP class_oop, OOP *p_oop);

/* Return the Character object for the Unicode value C.  */
extern OOP char_new(unsigned codePoint);

/* Answer the associated containing KEYOOP in the Dictionary (or a
   subclass having the same representation) DICTIONARYOOP.  */
extern OOP dictionary_association_at(OOP dictionaryOOP, OOP keyOOP);

/* Answer the value associated to KEYOOP in the Dictionary (or a
   subclass having the same representation) DICTIONARYOOP.  */
extern OOP dictionary_at(OOP dictionaryOOP, OOP keyOOP);

/* Creates a new Association object having the
   specified KEY and VALUE.  */
extern OOP association_new(OOP key, OOP value);

/* Creates a new VariableBinding object having the
   specified KEY and VALUE.  */
extern OOP variable_binding_new(OOP key, OOP value, OOP environment);

/* Returns an Object (an Integer for byte or word objects) containing
   the value of the INDEX-th indexed instance variable of the Object
   pointed to by OOP.  Range checks are done in INDEX and NULL is returned
   if this is the checking fails.  */
extern OOP index_oop(OOP oop, size_t index);

/* Returns an Object (an Integer for byte or word objects) containing
   the value of the INDEX-th indexed instance variable of the Object
   pointed to by OOP.  Range checks are done in INDEX and NULL is returned
   if this is the checking fails.  OBJECT and INSTANCESPEC are cached
   out of OOP and its class.  */
extern OOP index_oop_spec(OOP oop, gst_object object, size_t index,
                                 intptr_t instanceSpec);

/* Returns the number of valid object instance variables in OOP.  */
extern int num_valid_oops(OOP oop);

/* Returns whether the SCANNEDOOP points to TARGETOOP.  */
extern bool is_owner(OOP scannedOOP, OOP targetOOP);

/* Converts F to a Smalltalk FloatD, taking care of avoiding alignment
   problems.  */
extern OOP floatd_new(double f);

/* Converts F to a Smalltalk FloatE.  */
extern OOP floate_new(double f);

/* Converts F to a Smalltalk FloatQ, taking care of avoiding alignment
   problems.  */
extern OOP floatq_new(long double f);

/* Returns the address of the data stored in a CObject.  */
extern PTR cobject_value(OOP oop);

/* Sets the address of the data stored in a CObject.  */
extern void set_cobject_value(OOP oop, PTR val);

/* Return whether the address of the data stored in a CObject, offsetted
   by OFFSET bytes, is still in bounds.  */
extern bool cobject_index_check(OOP oop, intptr_t offset, intptr_t size);

/* Answer true if OOP is a SmallInteger or a LargeInteger of an
   appropriate size.  */
extern bool is_c_int_32(OOP oop);

/* Answer true if OOP is a SmallInteger or a LargeInteger of an
   appropriate size.  */
extern bool is_c_uint_32(OOP oop);

/* Converts the 32-bit int I to the appropriate SmallInteger or
   LargeInteger.  */
extern OOP from_c_int_32(int32_t i);

/* Converts the long int LNG to the appropriate SmallInteger or
   LargePositiveInteger.  */
extern OOP from_c_uint_32(uint32_t ui);

/* Converts the OOP (which must be a SmallInteger or a small enough
   LargeInteger) to a long int.  If the OOP was for an unsigned long,
   you can simply cast the result to an unsigned long.  */
extern int32_t to_c_int_32(OOP oop);

/* Answer true if OOP is a SmallInteger or a LargeInteger of an
   appropriate size.  */
extern bool is_c_int_64(OOP oop);

/* Answer true if OOP is a SmallInteger or a LargeInteger of an
   appropriate size.  */
extern bool is_c_uint_64(OOP oop);

/* Converts the 64-bit int I to the appropriate SmallInteger or
   LargeInteger.  */
extern OOP from_c_int_64(int64_t i);

/* Converts the long int LNG to the appropriate SmallInteger or
   LargePositiveInteger.  */
extern OOP from_c_uint_64(uint64_t ui);

/* Converts the OOP (which must be a SmallInteger or a small enough
   LargeInteger) to a 64-bit signed integer.  */
extern int64_t to_c_int_64(OOP oop);

/* Converts the OOP (which must be a SmallInteger or a small enough
   LargeInteger) to a 64-bit unsigned integer.  */
extern uint64_t to_c_uint_64(OOP oop);

#if (ALIGNOF_DOUBLE <= SIZEOF_OOP)
#define FLOATD_OOP_VALUE(floatOOP)                                             \
  ((OBJ_FLOATD_GET_VALUE(OOP_TO_OBJ(floatOOP))))

#else
#define FLOATD_OOP_VALUE(floatOOP) floatd_oop_value(floatOOP)

static double floatd_oop_value(OOP floatOOP);
#endif

#if (ALIGNOF_LONG_DOUBLE <= SIZEOF_OOP)
#define FLOATQ_OOP_VALUE(floatOOP) (((gst_floatq)OOP_TO_OBJ(floatOOP))->value)

#else
#define FLOATQ_OOP_VALUE(floatOOP) floatq_oop_value(floatOOP)

extern long double floatq_oop_value(OOP floatOOP);
#endif

/* Creates the Symbols that the VM knows about, and initializes
   the globals in the Smalltalk dictionary.  */

#endif /* GST_DICT_H */
