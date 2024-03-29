/******************************** -*- C -*- ****************************
 *
 *	Dictionary Support Module.
 *
 *
 ***********************************************************************/

/***********************************************************************
 *
 * Copyright
 *1988,89,90,91,92,94,95,99,2000,2001,2002,2003,2005,2006,2007,2008,2009 Free
 *Software Foundation, Inc. Written by Steve Byrne.
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

#include "gstpriv.h"
#include <assert.h>

/* Primary class variables.  These variables hold the class objects for
   most of the builtin classes in the system */
OOP _gst_abstract_namespace_class = NULL;
OOP _gst_array_class = NULL;
OOP _gst_arrayed_collection_class = NULL;
OOP _gst_association_class = NULL;
OOP _gst_behavior_class = NULL;
OOP _gst_binding_dictionary_class = NULL;
OOP _gst_block_closure_class = NULL;
OOP _gst_block_context_class = NULL;
OOP _gst_boolean_class = NULL;
OOP _gst_byte_array_class = NULL;
OOP _gst_c_callable_class = NULL;
OOP _gst_c_callback_descriptor_class = NULL;
OOP _gst_c_func_descriptor_class = NULL;
OOP _gst_c_object_class = NULL;
OOP _gst_c_type_class = NULL;
OOP _gst_callin_process_class = NULL;
OOP _gst_char_class = NULL;
OOP _gst_character_array_class = NULL;
OOP _gst_class_class = NULL;
OOP _gst_class_description_class = NULL;
OOP _gst_collection_class = NULL;
OOP _gst_compiled_block_class = NULL;
OOP _gst_compiled_code_class = NULL;
OOP _gst_compiled_method_class = NULL;
OOP _gst_context_part_class = NULL;
OOP _gst_continuation_class = NULL;
OOP _gst_deferred_variable_binding_class = NULL;
OOP _gst_dictionary_class = NULL;
OOP _gst_message_lookup_class = NULL;
OOP _gst_false_class = NULL;
OOP _gst_file_descriptor_class = NULL;
OOP _gst_file_segment_class = NULL;
OOP _gst_file_stream_class = NULL;
OOP _gst_float_class = NULL;
OOP _gst_floatd_class = NULL;
OOP _gst_floate_class = NULL;
OOP _gst_floatq_class = NULL;
OOP _gst_fraction_class = NULL;
OOP _gst_homed_association_class = NULL;
OOP _gst_identity_dictionary_class = NULL;
OOP _gst_integer_class = NULL;
OOP _gst_interval_class = NULL;
OOP _gst_large_integer_class = NULL;
OOP _gst_large_negative_integer_class = NULL;
OOP _gst_large_positive_integer_class = NULL;
OOP _gst_large_zero_integer_class = NULL;
OOP _gst_lookup_key_class = NULL;
OOP _gst_message_class = NULL;
OOP _gst_metaclass_class = NULL;
OOP _gst_method_context_class = NULL;
OOP _gst_method_dictionary_class = NULL;
OOP _gst_method_info_class = NULL;
OOP _gst_namespace_class = NULL;
OOP _gst_number_class = NULL;
OOP _gst_object_class = NULL;
OOP _gst_object_memory_class = NULL;
OOP _gst_process_class = NULL;
OOP _gst_root_namespace_class = NULL;
OOP _gst_semaphore_class = NULL;
OOP gst_small_integer_class = NULL;
OOP _gst_smalltalk_dictionary = NULL;
OOP _gst_string_class = NULL;
OOP _gst_sym_link_class = NULL;
OOP _gst_symbol_class = NULL;
OOP _gst_system_dictionary_class = NULL;
OOP _gst_true_class = NULL;
OOP _gst_undefined_object_class = NULL;
OOP _gst_unicode_character_class = NULL;
OOP _gst_unicode_string_class = NULL;
OOP _gst_variable_binding_class = NULL;
OOP _gst_processor_oop[100] = {NULL};
OOP _gst_key_hash_oop = NULL;
OOP _gst_debug_information_class = NULL;
OOP _gst_key_hash_class = NULL;

/* this must be big enough that the Smalltalk dictionary does not have to
   grow between the time gst_dictionary is loaded and the time the kernel is
   initialized.  Otherwise some of the methods needed to grow the dictionary
   might not be defined yet!!  */
#define INITIAL_SMALLTALK_SIZE 512

typedef struct class_definition {
  OOP *classVar;
  const char *name;
} class_definition;

/* The class definition structure.  From this structure, the initial
   set of Smalltalk classes are defined.  */

#define GST_PROCESSOR_SCHEDULER_NUMBER_INSTANCE_VARIABLES 9

static const class_definition class_info[] = {
    {&_gst_object_class, "Object"},

    {&_gst_object_memory_class, "ObjectMemory"},

    {&_gst_message_class, "Message"},

    {&_gst_message_lookup_class, "MessageLookup"},

    {&_gst_char_class, "Character"},

    {&_gst_unicode_character_class, "UnicodeCharacter"},

    {&_gst_float_class, "Float"},

    {&_gst_floatd_class, "FloatD"},

    {&_gst_floate_class, "FloatE"},

    {&_gst_floatq_class, "FloatQ"},

    {&_gst_integer_class, "Integer"},

    {&gst_small_integer_class, "SmallInteger"},

    {&_gst_large_integer_class, "LargeInteger"},

    {&_gst_large_positive_integer_class, "LargePositiveInteger"},

    {&_gst_large_zero_integer_class, "LargeZeroInteger"},

    {&_gst_large_negative_integer_class, "LargeNegativeInteger"},

    {&_gst_lookup_key_class, "LookupKey"},

    {&_gst_deferred_variable_binding_class, "DeferredVariableBinding"},

    {&_gst_association_class, "Association"},

    {&_gst_variable_binding_class, "VariableBinding"},

    {&_gst_process_class, "Process"},

    {&_gst_callin_process_class, "CallinProcess"},

    {&_gst_sym_link_class, "SymLink"},

    {&_gst_semaphore_class, "Semaphore"},

    {&_gst_array_class, "Array"},

    {&_gst_string_class, "String"},

    {&_gst_unicode_string_class, "UnicodeString"},

    {&_gst_symbol_class, "Symbol"},

    {&_gst_byte_array_class, "ByteArray"},

    {&_gst_compiled_block_class, "CompiledBlock"},

    {&_gst_compiled_method_class, "CompiledMethod"},

    {&_gst_interval_class, "Interval"},

    {&_gst_dictionary_class, "Dictionary"},

    {&_gst_identity_dictionary_class, "IdentityDictionary"},

    {&_gst_method_dictionary_class, "MethodDictionary"},

    {&_gst_binding_dictionary_class, "BindingDictionary"},

    {&_gst_abstract_namespace_class, "AbstractNamespace"},

    {&_gst_root_namespace_class, "RootNamespace"},

    {&_gst_namespace_class, "Namespace"},

    {&_gst_system_dictionary_class, "SystemDictionary"},

    {&_gst_file_descriptor_class, "FileDescriptor"},

    {&_gst_file_stream_class, "FileStream"},

    {&_gst_undefined_object_class, "UndefinedObject"},

    {&_gst_boolean_class, "Boolean"},

    {&_gst_false_class, "False"},

    {&_gst_true_class, "True"},

    {&_gst_behavior_class, "Behavior"},

    {&_gst_class_description_class, "ClassDescription"},

    {&_gst_class_class, "Class"},

    {&_gst_metaclass_class, "Metaclass"},

    {&_gst_context_part_class, "ContextPart"},

    {&_gst_method_context_class, "MethodContext"},

    {&_gst_block_context_class, "BlockContext"},

    {&_gst_continuation_class, "Continuation"},

    {&_gst_block_closure_class, "BlockClosure"},

    {&_gst_c_object_class, "CObject"},

    {&_gst_c_type_class, "CType"},

    {&_gst_c_callable_class, "CCallable"},

    {&_gst_c_callback_descriptor_class, "CCallbackDescriptor"},

    {&_gst_method_info_class, "MethodInfo"},

    {&_gst_file_segment_class, "FileSegment"},

    {&_gst_debug_information_class, "DebugInformation"},

    {&_gst_key_hash_class, "KeyHash"},
};


/* Called when a dictionary becomes full, this routine replaces the
   dictionary instance that DICTIONARYOOP is pointing to with a new,
   larger dictionary, and returns this new dictionary (the object
   pointer, not the OOP).  */
static gst_object grow_dictionary(OOP dictionaryOOP);

/* Called when an IdentityDictionary becomes full, this routine
   replaces the IdentityDictionary instance that IDENTITYDICTIONARYOOP
   is pointing to with a new, larger dictionary, and returns this new
   dictionary (the object pointer, not the OOP).  */
static gst_object grow_identity_dictionary(OOP identityDictionaryOOP);

/* Look for the index at which KEYOOP resides in IDENTITYDICTIONARYOOP
   or, if not found, find a nil slot which can be replaced by that
   key.  */
static size_t identity_dictionary_find_key_or_nil(OOP identityDictionaryOOP,
                                                  OOP keyOOP);

/* assume the value is an integer already or key does not exist, increase the
   value by inc or set the value to inc */
static int _gst_identity_dictionary_at_inc(OOP identityDictionaryOOP,
                                           OOP keyOOP, int inc);

/* This fills MAP so that it associates primitive numbers in the saved
   image to primitive numbers in this VM.  */
static void prepare_primitive_numbers_table(void);

/* Add a global named GLOBALNAME and give it the value GLOBALVALUE.
   Return GLOBALVALUE.  */
static OOP add_smalltalk(const char *globalName, OOP globalValue);

/* Adds to Smalltalk a global named FILEOBJECTNAME which is a
   FileStream referring to file descriptor FD.  */
static void add_file_stream_object(int fd, int access,
                                   const char *fileObjectName);

/* Creates the CSymbols pool dictionary, which gives access from
   Smalltalk to some definitions in float.h and config.h.  */
static void init_c_symbols(void);

signed char _gst_log2_sizes[32] = {0,
                                   -1,
                                   0,
                                   -1,
                                   0,
                                   -1,
                                   1,
                                   -1,
                                   1,
                                   -1,
                                   2,
                                   -1,
                                   2,
                                   -1,
                                   2,
                                   -1,
                                   3,
                                   -1,
                                   3,
                                   -1,
                                   3,
                                   -1,
                                   2,
                                   -1,
                                   -1,
                                   -1,
                                   -1,
                                   -1,
                                   -1,
                                   -1,
                                   sizeof(long) == 4 ? 2 : 3,
                                   -1};

static OOP add_smalltalk(const char *globalName, OOP globalValue) {
  NAMESPACE_AT_PUT(_gst_smalltalk_dictionary, _gst_intern_string(globalName),
                   globalValue);

  return globalValue;
}

static OOP relocate_path_oop(const char *s) {
  OOP resultOOP;
  char *path = _gst_relocate_path(s);
  if (path) {
    resultOOP = _gst_string_new(path);
  } else {
    resultOOP = _gst_nil_oop;
  }

  free(path);
  return resultOOP;
}

void init_runtime_objects(void) {
  add_smalltalk("UserFileBasePath", _gst_string_new(_gst_user_file_base_path));

  add_smalltalk("SystemKernelPath", relocate_path_oop(KERNEL_PATH));
  add_smalltalk("ModulePath", relocate_path_oop(MODULE_PATH));
  add_smalltalk("LibexecPath", relocate_path_oop(LIBEXEC_PATH));
  add_smalltalk("Prefix", relocate_path_oop(PREFIX));
  add_smalltalk("ExecPrefix", relocate_path_oop(EXEC_PREFIX));
  add_smalltalk("ImageFilePath", _gst_string_new(_gst_image_file_path));
  add_smalltalk("ExecutableFileName", _gst_string_new(_gst_executable_path));
  add_smalltalk("ImageFileName", _gst_string_new(_gst_binary_image_name));
  add_smalltalk("OutputVerbosity", FROM_INT(_gst_verbosity));
  add_smalltalk("RegressionTesting",
                _gst_regression_testing ? _gst_true_oop : _gst_false_oop);

#ifdef WORDS_BIGENDIAN
  add_smalltalk("Bigendian", _gst_true_oop);
#else
  add_smalltalk("Bigendian", _gst_false_oop);
#endif

  add_file_stream_object(0, O_RDONLY, "stdin");
  add_file_stream_object(1, O_WRONLY, "stdout");
  add_file_stream_object(2, O_WRONLY, "stderr");

  init_c_symbols();

  NAMESPACE_AT_PUT(_gst_smalltalk_dictionary, _gst_sip_hash_key_symbol,
                   _gst_key_hash_oop);

  /* Add the root among the roots :-) to the root set */
  _gst_register_oop(_gst_smalltalk_dictionary);
}

void init_c_symbols() {
  OOP cSymbolsOOP =
      dictionary_at(_gst_smalltalk_dictionary, _gst_intern_string("CSymbols"));

  NAMESPACE_AT_PUT(cSymbolsOOP, _gst_intern_string("HostSystem"),
                   _gst_string_new(HOST_SYSTEM));

  NAMESPACE_AT_PUT(cSymbolsOOP, _gst_intern_string("CIntSize"),
                   FROM_INT(sizeof(int)));
  NAMESPACE_AT_PUT(cSymbolsOOP, _gst_intern_string("CShortSize"),
                   FROM_INT(sizeof(short)));
  NAMESPACE_AT_PUT(cSymbolsOOP, _gst_intern_string("CLongSize"),
                   FROM_INT(sizeof(long)));
  NAMESPACE_AT_PUT(cSymbolsOOP, _gst_intern_string("CFloatSize"),
                   FROM_INT(sizeof(float)));
  NAMESPACE_AT_PUT(cSymbolsOOP, _gst_intern_string("CDoubleSize"),
                   FROM_INT(sizeof(double)));
  NAMESPACE_AT_PUT(cSymbolsOOP, _gst_intern_string("CLongDoubleSize"),
                   FROM_INT(sizeof(long double)));
  NAMESPACE_AT_PUT(cSymbolsOOP, _gst_intern_string("CPtrSize"),
                   FROM_INT(sizeof(PTR)));

#ifndef INFINITY
#define INFINITY LDBL_MAX * 2
#endif
#ifndef NAN
#define NAN (0.0 / 0.0)
#endif

#if defined WIN32 && !defined __CYGWIN__
  NAMESPACE_AT_PUT(cSymbolsOOP, _gst_intern_string("PathSeparator"),
                   CHAR_OOP_AT('\\'));
#else
  NAMESPACE_AT_PUT(cSymbolsOOP, _gst_intern_string("PathSeparator"),
                   CHAR_OOP_AT('/'));
#endif

  NAMESPACE_AT_PUT(cSymbolsOOP, _gst_intern_string("CDoubleMin"),
                   floatd_new(DBL_MIN));
  NAMESPACE_AT_PUT(cSymbolsOOP, _gst_intern_string("CDoubleMax"),
                   floatd_new(DBL_MAX));
  NAMESPACE_AT_PUT(cSymbolsOOP, _gst_intern_string("CDoublePInf"),
                   floatd_new((double)INFINITY));
  NAMESPACE_AT_PUT(cSymbolsOOP, _gst_intern_string("CDoubleNInf"),
                   floatd_new((double)-INFINITY));
  NAMESPACE_AT_PUT(cSymbolsOOP, _gst_intern_string("CDoubleNaN"),
                   floatd_new((double)NAN));
  NAMESPACE_AT_PUT(cSymbolsOOP, _gst_intern_string("CDoubleDigits"),
                   FROM_INT(ceil(DBL_MANT_DIG * 0.301029995663981)));
  NAMESPACE_AT_PUT(cSymbolsOOP, _gst_intern_string("CDoubleBinaryDigits"),
                   FROM_INT(DBL_MANT_DIG));
  NAMESPACE_AT_PUT(cSymbolsOOP, _gst_intern_string("CDoubleMinExp"),
                   FROM_INT((DBL_MIN_EXP)));
  NAMESPACE_AT_PUT(cSymbolsOOP, _gst_intern_string("CDoubleMaxExp"),
                   FROM_INT(DBL_MAX_EXP));
  NAMESPACE_AT_PUT(cSymbolsOOP, _gst_intern_string("CDoubleAlignment"),
                   FROM_INT(ALIGNOF_DOUBLE));
  NAMESPACE_AT_PUT(cSymbolsOOP, _gst_intern_string("CLongLongAlignment"),
                   FROM_INT(ALIGNOF_LONG_LONG));

  NAMESPACE_AT_PUT(cSymbolsOOP, _gst_intern_string("CFloatPInf"),
                   floate_new((float)INFINITY));
  NAMESPACE_AT_PUT(cSymbolsOOP, _gst_intern_string("CFloatNInf"),
                   floate_new((float)-INFINITY));
  NAMESPACE_AT_PUT(cSymbolsOOP, _gst_intern_string("CFloatNaN"),
                   floate_new((float)NAN));
  NAMESPACE_AT_PUT(cSymbolsOOP, _gst_intern_string("CFloatMin"),
                   floate_new(FLT_MIN));
  NAMESPACE_AT_PUT(cSymbolsOOP, _gst_intern_string("CFloatMax"),
                   floate_new(FLT_MAX));
  NAMESPACE_AT_PUT(cSymbolsOOP, _gst_intern_string("CFloatDigits"),
                   FROM_INT(ceil(FLT_MANT_DIG * 0.301029995663981)));
  NAMESPACE_AT_PUT(cSymbolsOOP, _gst_intern_string("CFloatBinaryDigits"),
                   FROM_INT(FLT_MANT_DIG));
  NAMESPACE_AT_PUT(cSymbolsOOP, _gst_intern_string("CFloatMinExp"),
                   FROM_INT(FLT_MIN_EXP));
  NAMESPACE_AT_PUT(cSymbolsOOP, _gst_intern_string("CFloatMaxExp"),
                   FROM_INT(FLT_MAX_EXP));
  NAMESPACE_AT_PUT(cSymbolsOOP, _gst_intern_string("CFloatAlignment"),
                   FROM_INT(sizeof(float)));

  NAMESPACE_AT_PUT(cSymbolsOOP, _gst_intern_string("CLongDoublePInf"),
                   floatq_new((long double)INFINITY));
  NAMESPACE_AT_PUT(cSymbolsOOP, _gst_intern_string("CLongDoubleNInf"),
                   floatq_new((long double)-INFINITY));
  NAMESPACE_AT_PUT(cSymbolsOOP, _gst_intern_string("CLongDoubleNaN"),
                   floatq_new((long double)NAN));
  NAMESPACE_AT_PUT(cSymbolsOOP, _gst_intern_string("CLongDoubleMin"),
                   floatq_new(LDBL_MIN));
  NAMESPACE_AT_PUT(cSymbolsOOP, _gst_intern_string("CLongDoubleMax"),
                   floatq_new(LDBL_MAX));
  NAMESPACE_AT_PUT(cSymbolsOOP, _gst_intern_string("CLongDoubleDigits"),
                   FROM_INT(ceil(LDBL_MANT_DIG * 0.301029995663981)));
  NAMESPACE_AT_PUT(cSymbolsOOP, _gst_intern_string("CLongDoubleBinaryDigits"),
                   FROM_INT(LDBL_MANT_DIG));
  NAMESPACE_AT_PUT(cSymbolsOOP, _gst_intern_string("CLongDoubleMinExp"),
                   FROM_INT(LDBL_MIN_EXP));
  NAMESPACE_AT_PUT(cSymbolsOOP, _gst_intern_string("CLongDoubleMaxExp"),
                   FROM_INT(LDBL_MAX_EXP));
  NAMESPACE_AT_PUT(cSymbolsOOP, _gst_intern_string("CLongDoubleAlignment"),
                   FROM_INT(ALIGNOF_LONG_DOUBLE));
}

void add_file_stream_object(int fd, int access, const char *fileObjectName) {
  OOP fileStreamOOP;
  OOP keyOOP;

  keyOOP = _gst_intern_string(fileObjectName);
  fileStreamOOP = dictionary_at(_gst_smalltalk_dictionary, keyOOP);
  if (IS_NIL(fileStreamOOP)) {
    instantiate(_gst_file_stream_class, &fileStreamOOP);
  }

  _gst_set_file_stream_file(fileStreamOOP, fd, _gst_string_new(fileObjectName),
                            _gst_is_pipe(fd) ? _gst_true_oop : _gst_false_oop,
                            access, true);

  add_smalltalk(fileObjectName, fileStreamOOP);
}

bool _gst_init_dictionary_on_image_load(bool prim_table_matches) {
  const class_definition *ci;

  _gst_nil_oop = OOP_AT(NIL_OOP_INDEX);
  _gst_true_oop = OOP_AT(TRUE_OOP_INDEX);
  _gst_false_oop = OOP_AT(FALSE_OOP_INDEX);
  _gst_smalltalk_dictionary = OOP_AT(SMALLTALK_OOP_INDEX);
  _gst_processor_oop[0] = OOP_AT(PROCESSOR_OOP_INDEX);
  _gst_symbol_table = OOP_AT(SYM_TABLE_OOP_INDEX);
  _gst_key_hash_oop = OOP_AT(KEY_HASH_OOP_INDEX);

  if (IS_NIL(_gst_processor_oop[0]) || IS_NIL(_gst_symbol_table) ||
      IS_NIL(_gst_smalltalk_dictionary)) {
    return (false);
  }

  _gst_restore_symbols();

  for (ci = class_info; ci < class_info + sizeof(class_info) / sizeof(class_definition); ci++) {
    *ci->classVar = dictionary_at(_gst_smalltalk_dictionary, _gst_intern_string(ci->name));
    if (IS_NIL(*ci->classVar)) {
      return false;
    }
  }

  _gst_current_namespace =
      dictionary_at(_gst_class_variable_dictionary(_gst_namespace_class),
                    _gst_intern_string("Current"));

  /* Important: this is called *after* _gst_init_symbols
      fills in _gst_vm_primitives_symbol! */
  if (prim_table_matches) {
    memcpy(_gst_primitive_table, _gst_default_primitive_table,
           sizeof(_gst_primitive_table));
  } else {
    prepare_primitive_numbers_table();
  }

  init_runtime_objects();
  return (true);
}

void prepare_primitive_numbers_table() {
  int i;
  OOP primitivesDictionaryOOP;

  primitivesDictionaryOOP =
      dictionary_at(_gst_smalltalk_dictionary, _gst_vm_primitives_symbol);

  for (i = 0; i < NUM_PRIMITIVES; i++) {
    _gst_set_primitive_attributes(i, NULL);
  }

  for (i = 0; i < NUM_PRIMITIVES; i++) {
    prim_table_entry *pte = _gst_get_primitive_attributes(i);
    OOP symbolOOP, valueOOP;
    int old_index;

    if (!pte->name) {
      continue;
    }

    symbolOOP = _gst_intern_string(pte->name);
    valueOOP = dictionary_at(primitivesDictionaryOOP, symbolOOP);

    /* Do nothing if the primitive is unknown to the image.  */
    if (IS_NIL(valueOOP)) {
      continue;
    }

    old_index = TO_INT(valueOOP);
    _gst_set_primitive_attributes(old_index, pte);
  }
}

OOP _gst_get_class_symbol(OOP class_oop) {
  gst_object class;

  class = OOP_TO_OBJ(class_oop);
  return OBJ_CLASS_GET_NAME(class);
  /* this is the case when we have a metaclass, ??? I don't think that
     this is right, but I don't know what else to do here */
}

OOP _gst_find_class(OOP classNameOOP) {
  return (dictionary_at(_gst_smalltalk_dictionary, classNameOOP));
}

OOP _gst_valid_class_method_dictionary(OOP class_oop) {
  gst_object class;

  /* ??? check for non-class objects */
  class = OOP_TO_OBJ(class_oop);
  if (IS_NIL(OBJ_BEHAVIOR_GET_METHOD_DICTIONARY(class))) {
    OOP methodDictionaryOOP;
    methodDictionaryOOP =
        _gst_method_dictionary_new(32);
    class = OOP_TO_OBJ(class_oop);
    OBJ_BEHAVIOR_SET_METHOD_DICTIONARY(class, methodDictionaryOOP);
  }

  return OBJ_BEHAVIOR_GET_METHOD_DICTIONARY(class);
}

OOP _gst_find_class_method(OOP class_oop, OOP selector) {
  gst_object class = OOP_TO_OBJ(class_oop);
  OOP method_dictionary_oop = OBJ_BEHAVIOR_GET_METHOD_DICTIONARY(class);
  if (IS_NIL(method_dictionary_oop)) {
    return _gst_nil_oop;
  }

  const ssize_t index = _gst_method_dictionary_find_key(method_dictionary_oop, selector);
  if (index == -1) {
    return _gst_nil_oop;
  }

  OOP valuesOOP = OBJ_METHOD_DICTIONARY_GET_VALUES(OOP_TO_OBJ(method_dictionary_oop));

  return ARRAY_AT(valuesOOP, index);
}

OOP _gst_class_variable_dictionary(OOP class_oop) {
  gst_object class;

  /* ??? check for non-class objects */
  class = OOP_TO_OBJ(class_oop);
  return OBJ_CLASS_GET_CLASS_VARIABLES(class);
}

OOP _gst_instance_variable_array(OOP class_oop) {
  gst_object class;

  /* ??? check for non-class objects */
  class = OOP_TO_OBJ(class_oop);
  return OBJ_BEHAVIOR_GET_INSTANCE_VARIABLES(class);
}

OOP _gst_shared_pool_dictionary(OOP class_oop) {
  gst_object class;

  /* ??? check for non-class objects */
  class = OOP_TO_OBJ(class_oop);
  return OBJ_CLASS_GET_SHARED_POOLS(class);
}

OOP _gst_namespace_association_at(OOP poolOOP, OOP symbol) {
  OOP assocOOP;
  gst_object pool;

  if (is_a_kind_of(OOP_CLASS(poolOOP), _gst_class_class)) {
    poolOOP = _gst_class_variable_dictionary(poolOOP);
  }

  for (;;) {
    if (!is_a_kind_of(OOP_CLASS(poolOOP), _gst_dictionary_class)) {
      return (_gst_nil_oop);
    }

    assocOOP = dictionary_association_at(poolOOP, symbol);
    if (!IS_NIL(assocOOP)) {
      return (assocOOP);
    }

    /* Try to find a super-namespace */
    if (!is_a_kind_of(OOP_CLASS(poolOOP), _gst_abstract_namespace_class)) {
      return (_gst_nil_oop);
    }

    pool = OOP_TO_OBJ(poolOOP);
    poolOOP = OBJ_NAMESPACE_GET_SUPER_SPACE(pool);
  }
}

OOP _gst_namespace_at(OOP poolOOP, OOP symbol) {
  OOP assocOOP = _gst_namespace_association_at(poolOOP, symbol);
  if (IS_NIL(assocOOP)) {
    return assocOOP;
  } else {
    return OBJ_ASSOCIATION_GET_VALUE(OOP_TO_OBJ(assocOOP));
  }
}

size_t new_num_fields(size_t oldNumFields) {
  /* Find a power of two that is larger than oldNumFields */

  int n = 1;

  /* Already a power of two? duplicate the size */
  if COMMON ((oldNumFields & (oldNumFields - 1)) == 0)
    return oldNumFields * 2;

  /* Find the next power of two by setting all bits to the right of
     the leftmost 1 bit to 1, and then incrementing.  */
  for (; oldNumFields & (oldNumFields + 1); n <<= 1) {
    oldNumFields |= oldNumFields >> n;
  }

  return oldNumFields + 1;
}

static int find_key_or_nil(OOP dictionaryOOP, OOP keyOOP) {
  size_t count, numFields, numFixedFields;
  intptr_t index;
  gst_object dictionary;
  OOP associationOOP;
  gst_object association;

  dictionary = OOP_TO_OBJ(dictionaryOOP);
  numFixedFields = OOP_FIXED_FIELDS(dictionaryOOP);
  numFields = NUM_WORDS(dictionary) - numFixedFields;
  OBJ_UPDATE_IDENTITY(OOP_TO_OBJ(keyOOP));
  index = scramble(TO_INT(OBJ_IDENTITY(OOP_TO_OBJ(keyOOP))));
  count = numFields;

  for (; count; count--) {
    index &= numFields - 1;
    associationOOP = dictionary->data[numFixedFields + index];
    if COMMON (IS_NIL(associationOOP))
      return (index);

    association = OOP_TO_OBJ(associationOOP);

    if (OBJ_ASSOCIATION_GET_KEY(association) == keyOOP) {
      return (index);
    }

    /* linear reprobe -- it is simple and guaranteed */
    index++;
  }

  _gst_errorf("Error - searching dictionary for nil, but it is full!\n");

  abort();
}

gst_object grow_dictionary(OOP oldDictionaryOOP) {
  gst_object oldDictionary, dictionary;
  size_t oldNumFields, numFields, i, index, numFixedFields;
  OOP associationOOP;
  gst_object association;
  OOP dictionaryOOP;

  oldDictionary = OOP_TO_OBJ(oldDictionaryOOP);
  numFixedFields = OOP_FIXED_FIELDS(oldDictionaryOOP);
  oldNumFields = NUM_WORDS(oldDictionary) - numFixedFields;

  numFields = new_num_fields(oldNumFields);

  /* no need to use the incubator here.  We are instantiating just one
     object, the new dictionary itself */

  dictionary =
      instantiate_with(OOP_CLASS(oldDictionaryOOP), numFields, &dictionaryOOP);
  memcpy(dictionary->data, oldDictionary->data, sizeof(PTR) * numFixedFields);
  oldDictionary = OOP_TO_OBJ(oldDictionaryOOP);

  /* rehash all associations from old dictionary into new one */
  for (i = 0; i < oldNumFields; i++) {
    associationOOP = oldDictionary->data[numFixedFields + i];
    if COMMON (!IS_NIL(associationOOP)) {
      association = OOP_TO_OBJ(associationOOP);
      index =
          find_key_or_nil(dictionaryOOP, OBJ_ASSOCIATION_GET_KEY(association));
      dictionary->data[numFixedFields + index] = associationOOP;
    }
  }

  _gst_swap_objects(dictionaryOOP, oldDictionaryOOP);
  return (OOP_TO_OBJ(oldDictionaryOOP));
}

gst_object grow_identity_dictionary(OOP oldIdentityDictionaryOOP) {
  gst_object oldIdentityDictionary, identityDictionary;
  gst_object oldIdentDict, identDict;
  OOP key, identityDictionaryOOP;
  size_t oldNumFields, numFields, numFixedFields, i, index;

  oldIdentityDictionary = OOP_TO_OBJ(oldIdentityDictionaryOOP);
  numFixedFields = OOP_FIXED_FIELDS(oldIdentityDictionaryOOP);
  oldNumFields = (NUM_WORDS(oldIdentityDictionary) - numFixedFields) / 2;

  numFields = new_num_fields(oldNumFields);

  identityDictionary = instantiate_with(OOP_CLASS(oldIdentityDictionaryOOP),
                                        numFields * 2, &identityDictionaryOOP);
  oldIdentityDictionary = OOP_TO_OBJ(oldIdentityDictionaryOOP);
  oldIdentDict = oldIdentityDictionary;
  identDict = identityDictionary;
  OBJ_IDENTITY_DICTIONARY_SET_TALLY(
      identDict, INCR_INT(OBJ_IDENTITY_DICTIONARY_GET_TALLY(oldIdentDict)));

  /* rehash all associations from old dictionary into new one */
  for (i = 0; i < oldNumFields; i++) {
    key = oldIdentityDictionary->data[i * 2 + numFixedFields];
    if COMMON (!IS_NIL(key)) {
      index = identity_dictionary_find_key_or_nil(identityDictionaryOOP, key);
      identityDictionary->data[index - 1 + numFixedFields] = key;
      identityDictionary->data[index + numFixedFields] =
          oldIdentityDictionary->data[i * 2 + 1 + numFixedFields];
    }
  }

  _gst_swap_objects(identityDictionaryOOP, oldIdentityDictionaryOOP);
  return (OOP_TO_OBJ(oldIdentityDictionaryOOP));
}

size_t identity_dictionary_find_key_or_nil(OOP identityDictionaryOOP,
                                           OOP keyOOP) {
  gst_object identityDictionary;
  size_t index, count, numFields, numFixedFields;

  identityDictionary = OOP_TO_OBJ(identityDictionaryOOP);
  numFixedFields = OOP_FIXED_FIELDS(identityDictionaryOOP);

  numFields = NUM_WORDS(identityDictionary) - numFixedFields;
  OBJ_UPDATE_IDENTITY(OOP_TO_OBJ(keyOOP));
  index = scramble(TO_INT(OBJ_IDENTITY(OOP_TO_OBJ(keyOOP)))) * 2;
  count = numFields / 2;
  /* printf ("%d %d %O\n", count, index & numFields - 1, keyOOP); */
  while (count--) {
    index &= numFields - 1;

    if COMMON (IS_NIL(identityDictionary->data[index + numFixedFields]))
      return (index + 1);

    if COMMON (identityDictionary->data[index + numFixedFields] == keyOOP)
      return (index + 1);

    /* linear reprobe -- it is simple and guaranteed */
    index += 2;
  }

  _gst_errorf(
      "Error - searching IdentityDictionary for nil, but it is full!\n");

  abort();
}

OOP _gst_identity_dictionary_new(OOP classOOP, int size) {
  gst_object identityDictionary;
  OOP identityDictionaryOOP;

  size = new_num_fields(size);

  identityDictionary =
      instantiate_with(classOOP, size * 2, &identityDictionaryOOP);

  OBJ_IDENTITY_DICTIONARY_SET_TALLY(identityDictionary, FROM_INT(0));
  return (identityDictionaryOOP);
}

OOP _gst_identity_dictionary_at_put(OOP identityDictionaryOOP, OOP keyOOP,
                                    OOP valueOOP) {
  gst_object identityDictionary;
  gst_object identDict;
  intptr_t index;
  OOP oldValueOOP;
  size_t numFixedFields;

  identityDictionary = OOP_TO_OBJ(identityDictionaryOOP);
  numFixedFields = OOP_FIXED_FIELDS(identityDictionaryOOP);

  /* Never make dictionaries too full! For simplicity, we do this even
     if the key is present in the dictionary (because it will most
     likely resolve some collisions and make things faster).  */

  identDict = identityDictionary;
  if UNCOMMON (TO_INT(OBJ_IDENTITY_DICTIONARY_GET_TALLY(identDict)) >=
               TO_INT(OBJ_SIZE(identDict)) * 3 / 8)
    identityDictionary = grow_identity_dictionary(identityDictionaryOOP);

  index = identity_dictionary_find_key_or_nil(identityDictionaryOOP, keyOOP);

  if COMMON (IS_NIL(identityDictionary->data[index - 1 + numFixedFields])) {
    identDict = identityDictionary;
    OBJ_IDENTITY_DICTIONARY_SET_TALLY(
        identDict, INCR_INT(OBJ_IDENTITY_DICTIONARY_GET_TALLY(identDict)));
  }

  identityDictionary->data[index - 1 + numFixedFields] = keyOOP;
  oldValueOOP = identityDictionary->data[index + numFixedFields];
  identityDictionary->data[index + numFixedFields] = valueOOP;

  return (oldValueOOP);
}
OOP _gst_identity_dictionary_at(OOP identityDictionaryOOP, OOP keyOOP) {
  gst_object identityDictionary;
  intptr_t index;
  size_t numFixedFields;

  identityDictionary = OOP_TO_OBJ(identityDictionaryOOP);
  numFixedFields = OOP_FIXED_FIELDS(identityDictionaryOOP);

  index = identity_dictionary_find_key_or_nil(identityDictionaryOOP, keyOOP);

  return identityDictionary->data[index + numFixedFields];
}

OOP _gst_dictionary_new(int size) {
  gst_object dictionary;
  OOP dictionaryOOP;

  size = new_num_fields(size);
  dictionary = instantiate_with(_gst_dictionary_class, size, &dictionaryOOP);

  OBJ_DICTIONARY_SET_TALLY(dictionary, FROM_INT(0));

  return (dictionaryOOP);
}

OOP _gst_binding_dictionary_new(int size, OOP environmentOOP) {
  gst_object dictionary;
  OOP dictionaryOOP;

  size = new_num_fields(size);
  dictionary =
      instantiate_with(_gst_binding_dictionary_class, size, &dictionaryOOP);

  OBJ_BINDING_DICTIONARY_SET_TALLY(dictionary, FROM_INT(0));
  OBJ_BINDING_DICTIONARY_SET_ENVIRONMENT(dictionary, environmentOOP);

  return (dictionaryOOP);
}

OOP _gst_dictionary_add(OOP dictionaryOOP, OOP associationOOP) {
  intptr_t index;
  gst_object association;
  gst_object dictionary;
  gst_object dict;
  OOP value;
  inc_ptr incPtr; /* I'm not sure clients are protecting
                     association OOP */

  incPtr = INC_SAVE_POINTER();
  INC_ADD_OOP(associationOOP);

  association = OOP_TO_OBJ(associationOOP);
  dictionary = OOP_TO_OBJ(dictionaryOOP);
  dict = dictionary;
  if UNCOMMON (TO_INT(OBJ_DICTIONARY_GET_TALLY(dict)) >=
               TO_INT(OBJ_SIZE(dict)) * 3 / 4) {
    dictionary = grow_dictionary(dictionaryOOP);
    dict = dictionary;
  }

  index = find_key_or_nil(dictionaryOOP, OBJ_ASSOCIATION_GET_KEY(association));
  index += OOP_FIXED_FIELDS(dictionaryOOP);
  if COMMON (IS_NIL(dictionary->data[index])) {
    OBJ_DICTIONARY_SET_TALLY(dict, INCR_INT(OBJ_DICTIONARY_GET_TALLY(dict)));
    dictionary->data[index] = associationOOP;
  } else {
    value = OBJ_ASSOCIATION_GET_VALUE(OOP_TO_OBJ(associationOOP));
    associationOOP = dictionary->data[index];
    OBJ_ASSOCIATION_SET_VALUE(OOP_TO_OBJ(associationOOP), value);
  }

  INC_RESTORE_POINTER(incPtr);
  return (associationOOP);
}

OOP _gst_object_copy(OOP oop) {
  gst_object old, new;
  OOP newOOP;
  size_t numFields;

  if UNCOMMON (IS_INT(oop))
    return (oop);

  numFields = NUM_INDEXABLE_FIELDS(oop);

  new = instantiate_with(OOP_CLASS(oop), numFields, &newOOP);
  old = OOP_TO_OBJ(oop);
  memcpy(new, old, SIZE_TO_BYTES(TO_INT(OBJ_SIZE(old))));

  OOP_SET_FLAGS(newOOP,
                OOP_GET_FLAGS(newOOP) | (OOP_GET_FLAGS(oop) & F_CONTEXT));
  return (newOOP);
}

OOP _gst_new_string(size_t len) {
  OOP stringOOP;

  new_instance_with(_gst_string_class, len, &stringOOP);
  return (stringOOP);
}

OOP _gst_string_new(const char *s) {
  gst_object string;
  size_t len;
  OOP stringOOP;

  if (s) {
    len = strlen(s);
    string = new_instance_with(_gst_string_class, len, &stringOOP);

    memcpy(OBJ_STRING_GET_CHARS(string), s, len);
  } else {
    string = new_instance_with(_gst_string_class, 0, &stringOOP);
  }
  return (stringOOP);
}

OOP _gst_unicode_string_new(const wchar_t *s) {
  gst_object string;
  size_t len;
  OOP stringOOP;

  if (s) {
    len = wcslen(s);
    string = new_instance_with(_gst_unicode_string_class, len, &stringOOP);

    if (sizeof(wchar_t) == sizeof(OBJ_UNICODE_STRING_GET_CHARS(string)[0])) {
      memcpy(OBJ_UNICODE_STRING_GET_CHARS(string), s, len * sizeof(wchar_t));
    } else {
      for (size_t i = 0; i < len; i++) {
        OBJ_UNICODE_STRING_SET_CHARS(string, i, *s++);
      }
    }
  } else {
    string = new_instance_with(_gst_unicode_string_class, 0, &stringOOP);
  }

  return (stringOOP);
}

OOP _gst_counted_string_new(const char *s, size_t len) {
  gst_object string;
  OOP stringOOP;

  string = new_instance_with(_gst_string_class, len, &stringOOP);

  if (len) {
    memcpy(OBJ_STRING_GET_CHARS(string), s, len);
  }

  return (stringOOP);
}

void _gst_set_oopstring(OOP stringOOP, const char *s) {
  OOP newStringOOP;

  newStringOOP = _gst_string_new(s);
  _gst_swap_objects(stringOOP, newStringOOP);
}

void _gst_set_oop_unicode_string(OOP unicodeStringOOP, const wchar_t *s) {
  OOP newStringOOP;

  newStringOOP = _gst_unicode_string_new(s);
  _gst_swap_objects(unicodeStringOOP, newStringOOP);
}

char *_gst_to_cstring(OOP stringOOP) {
  char *result;
  size_t len;
  gst_object string;

  string = OOP_TO_OBJ(stringOOP);
  len = oop_num_fields(stringOOP);
  result = (char *)xmalloc(len + 1);
  memcpy(result, OBJ_STRING_GET_CHARS(string), len);
  result[len] = '\0';

  return (result);
}

wchar_t *_gst_to_wide_cstring(OOP stringOOP) {
  wchar_t *result, *p;
  size_t len;
  gst_object string;
  size_t i;

  string = OOP_TO_OBJ(stringOOP);
  len = oop_num_fields(stringOOP);
  result = (wchar_t *)xmalloc(len + 1);
  if (sizeof(wchar_t) == 4) {
    memcpy(result, OBJ_UNICODE_STRING_GET_CHARS(string), len * sizeof(wchar_t));
  } else {
    for (p = result, i = 0; i < len; i++) {
      *p++ = OBJ_UNICODE_STRING_GET_CHARS(string)[i];
    }
  }
  result[len] = '\0';

  return (result);
}

OOP _gst_byte_array_new(const gst_uchar *bytes, size_t len) {
  gst_object byteArray;
  OOP byteArrayOOP;

  byteArray = new_instance_with(_gst_byte_array_class, len, &byteArrayOOP);

  memcpy(byteArray->data, bytes, len);
  return (byteArrayOOP);
}

gst_uchar *_gst_to_byte_array(OOP byteArrayOOP) {
  gst_uchar *result;
  size_t len;
  gst_object byteArray;

  byteArray = OOP_TO_OBJ(byteArrayOOP);
  len = oop_num_fields(byteArrayOOP);
  result = (gst_uchar *)xmalloc(len);
  memcpy(result, byteArray->data, len);

  return (result);
}

void _gst_set_oop_bytes(OOP byteArrayOOP, gst_uchar *bytes) {
  gst_object byteArray;
  size_t len;

  len = oop_num_fields(byteArrayOOP);
  byteArray = OOP_TO_OBJ(byteArrayOOP);
  memcpy(byteArray->data, bytes, len);
}

OOP _gst_message_new_args(OOP selectorOOP, OOP argsArray) {
  gst_object message;
  OOP messageOOP;

  message = new_instance(_gst_message_class, &messageOOP);

  OBJ_MESSAGE_SET_SELECTOR(message, selectorOOP);
  OBJ_MESSAGE_SET_ARGS(message, argsArray);

  return (messageOOP);
}

OOP _gst_c_object_new_base(OOP baseOOP, uintptr_t cObjOfs, OOP typeOOP,
                           OOP defaultClassOOP) {
  gst_object cObject;
  gst_object cType;
  OOP cObjectOOP;
  OOP classOOP;

  if (!IS_NIL(typeOOP)) {
    cType = OOP_TO_OBJ(typeOOP);
    classOOP = OBJ_ASSOCIATION_GET_VALUE(
        OOP_TO_OBJ(OBJ_CTYPE_GET_COBJECT_TYPE(cType)));
  } else {
    classOOP = defaultClassOOP;
  }

  cObject = instantiate_with(classOOP, 1, &cObjectOOP);
  OBJ_COBJECT_SET_TYPE(cObject, typeOOP);
  OBJ_COBJECT_SET_STORAGE(cObject, baseOOP);
  SET_COBJECT_OFFSET_OBJ(cObject, cObjOfs);

  return (cObjectOOP);
}

void _gst_free_cobject(OOP cObjOOP) {
  gst_object cObject;

  cObject = OOP_TO_OBJ(cObjOOP);
  if (!IS_NIL(OBJ_COBJECT_GET_STORAGE(cObject))) {
    OBJ_COBJECT_SET_STORAGE(cObject, _gst_nil_oop);
  } else {
    xfree((PTR)COBJECT_OFFSET_OBJ(cObject));
  }

  /* make it not point to falsely valid storage */
  SET_COBJECT_OFFSET_OBJ(cObject, NULL);
}

void _gst_set_file_stream_file(OOP fileStreamOOP, int fd, OOP fileNameOOP,
                               OOP isPipe, int access, bool buffered) {
  gst_object fileStream;

  fileStream = OOP_TO_OBJ(fileStreamOOP);

  switch (access & O_ACCMODE) {
  case O_RDONLY:
    OBJ_FILE_STREAM_SET_ACCESS(fileStream, FROM_INT(1));
    break;
  case O_WRONLY:
    OBJ_FILE_STREAM_SET_ACCESS(fileStream, FROM_INT(2));
    break;
  case O_RDWR:
    OBJ_FILE_STREAM_SET_ACCESS(fileStream, FROM_INT(3));
    break;
  }

  if (buffered) {
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    OBJ_FILE_STREAM_SET_COLLECTION(
        fileStream, _gst_counted_string_new(buffer, sizeof(buffer)));
    OBJ_FILE_STREAM_SET_PTR(fileStream, FROM_INT(1));
    OBJ_FILE_STREAM_SET_END_PTR(fileStream, FROM_INT(0));
    OBJ_FILE_STREAM_SET_WRITE_PTR(fileStream, _gst_nil_oop);
    OBJ_FILE_STREAM_SET_WRITE_END(fileStream, _gst_nil_oop);
  }

  OBJ_FILE_STREAM_SET_FD(fileStream, FROM_INT(fd));
  OBJ_FILE_STREAM_SET_FILE(fileStream, fileNameOOP);
  OBJ_FILE_STREAM_SET_IS_PIPE(fileStream, isPipe);
}

/* Profiling callback.  The profiler use a simple data structure
   to store the cost and the call graph, which is a 2 level
   IdentityDictionary. First level keys are the CompiledMethod or
   CompiledBlock, and the second level key is the CompiledMethod or
   CompiledBlock that it calls. Values are the number of calls made. There
   is a special key "true" in the second level whose corresponding value
   is the accumulative cost for this method.  */

void _gst_record_profile(OOP oldMethod, OOP newMethod, int ipOffset) {
  OOP profile;
  inc_ptr incPtr;

  /* Protect oldMethod from GC here to avoid complicating the fast path
     in interp-bc.inl.  */
  incPtr = INC_SAVE_POINTER();
  INC_ADD_OOP(oldMethod);

  profile = _gst_identity_dictionary_at(_gst_raw_profile, oldMethod);
  if UNCOMMON (IS_NIL(profile)) {
    profile = _gst_identity_dictionary_new(_gst_identity_dictionary_class, 6);
    _gst_identity_dictionary_at_put(_gst_raw_profile, oldMethod, profile);
  }

  _gst_identity_dictionary_at_inc(profile, _gst_true_oop,
                                  _gst_bytecode_counter -
                                      _gst_saved_bytecode_counter);
  _gst_saved_bytecode_counter = _gst_bytecode_counter;

  /* if ipOffset is 0 then it is a callin and not a return, so we also record
     the call.  */
  if (ipOffset == 0) {
    _gst_identity_dictionary_at_inc(profile, newMethod, 1);
  }

  INC_RESTORE_POINTER(incPtr);
}

/* Assume the value for KEYOOP is an integer already or the key does not exist;
   increase the value by INC or set it to INC if it does not exist.  */
int _gst_identity_dictionary_at_inc(OOP identityDictionaryOOP, OOP keyOOP,
                                    int inc) {
  gst_object identityDictionary;
  gst_object identDict;
  intptr_t index;
  int oldValue;
  size_t numFixedFields;

  identityDictionary = OOP_TO_OBJ(identityDictionaryOOP);
  numFixedFields = OOP_FIXED_FIELDS(identityDictionaryOOP);

  /* Never make dictionaries too full! For simplicity, we do this even
     if the key is present in the dictionary (because it will most
     likely resolve some collisions and make things faster).  */

  identDict = identityDictionary;
  if UNCOMMON (TO_INT(OBJ_IDENTITY_DICTIONARY_GET_TALLY(identDict)) >=
               TO_INT(OBJ_SIZE(identDict)) * 3 / 8)
    identityDictionary = grow_identity_dictionary(identityDictionaryOOP);
  index = identity_dictionary_find_key_or_nil(identityDictionaryOOP, keyOOP);

  if UNCOMMON (IS_NIL(identityDictionary->data[index - 1 + numFixedFields])) {
    identDict = identityDictionary;
    OBJ_IDENTITY_DICTIONARY_SET_TALLY(
        identDict, INCR_INT(OBJ_IDENTITY_DICTIONARY_GET_TALLY(identDict)));
    oldValue = 0;
  } else
    oldValue = TO_INT(identityDictionary->data[index + numFixedFields]);

  identityDictionary->data[index - 1 + numFixedFields] = keyOOP;
  identityDictionary->data[index + numFixedFields] = FROM_INT(inc + oldValue);

  return (oldValue);
}

OOP floate_new(double f) {
  gst_object floatObject;
  OOP floatOOP;

  floatObject = new_instance_with(_gst_floate_class, sizeof(float),
                                              &floatOOP);

  OBJ_FLOATE_SET_VALUE(floatObject, f);
  MAKE_OOP_READONLY(floatOOP, true);
  return (floatOOP);
}

#if (ALIGNOF_DOUBLE > SIZEOF_OOP)
double floatd_oop_value(OOP floatOOP)
{
  gst_object obj;
  double d;

  /* we may not be aligned properly...fetch things out the hard way */
  obj = OOP_TO_OBJ(floatOOP);
  memcpy(&d, obj->data, sizeof(double));
  return (d);
}
#endif

OOP floatd_new(double f) {
  OOP floatOOP;
#if (ALIGNOF_DOUBLE <= SIZEOF_OOP)
  gst_object floatObject;

  floatObject = new_instance_with(_gst_floatd_class, sizeof(double),
                                  &floatOOP);

  OBJ_FLOATD_SET_VALUE(floatObject, f);
#else
  gst_object obj;

  obj = new_instance_with(_gst_floatd_class, sizeof(double), &floatOOP);

  memcpy(&obj->data, &f, sizeof(double));
#endif

  MAKE_OOP_READONLY(floatOOP, true);
  return (floatOOP);
}

#if (ALIGNOF_LONG_DOUBLE > SIZEOF_OOP)
long double floatq_oop_value(OOP floatOOP)
{
  gst_object obj;
  long double d;

  /* we may not be aligned properly...fetch things out the hard way */
  obj = OOP_TO_OBJ(floatOOP);
  memcpy(&d, obj->data, sizeof(long double));
  return (d);
}
#endif

OOP floatq_new(long double f) {
  OOP floatOOP;
  gst_object obj = new_instance_with(_gst_floatq_class, 16, &floatOOP);

#if defined __i386__ || defined __x86_64__
  /* Two bytes (six on x86-64) of 80-bit long doubles are unused.  */
  memcpy(&obj->data, &f, 10);
  memset(((char *)obj->data) + 10, 0, 6);
#else
  memcpy(&obj->data, &f, sizeof(long double));
  memset(((char *)obj->data) + sizeof(long double), 0,
         16 - sizeof(long double));
#endif

  MAKE_OOP_READONLY(floatOOP, true);
  return (floatOOP);
}

OOP char_new(unsigned codePoint) {
  gst_object charObject;
  OOP charOOP;

  if (codePoint <= 127)
    return CHAR_OOP_AT(codePoint);
  if UNCOMMON (codePoint > 0x10FFFF)
    codePoint = 0xFFFD;

  charObject = new_instance(_gst_unicode_character_class, &charOOP);

  OBJ_CHAR_SET_CODE_POINTS(charObject, FROM_INT(codePoint));
  MAKE_OOP_READONLY(charOOP, true);
  return (charOOP);
}

uintptr_t scramble(uintptr_t x) {
#if SIZEOF_OOP == 8
  x ^= (x >> 31) | (x << 33);
#endif
  x ^= (x << 10) | (x >> 22);
  x ^= (x << 6) | (x >> 26);
  x ^= (x << 16) | (x >> 16);

  return x & MAX_ST_INT;
}

bool is_a_kind_of(OOP testedOOP, OOP class_oop) {
  do {
    if (testedOOP == class_oop)
      return (true);
    testedOOP = SUPERCLASS(testedOOP);
  } while (!IS_NIL(testedOOP));

  return (false);
}

void nil_fill(OOP *oopPtr, size_t oopCount) {
  REGISTER(3, OOP nilObj);

  nilObj = _gst_nil_oop;
  while (oopCount >= 8) {
    oopPtr[0] = oopPtr[1] = oopPtr[2] = oopPtr[3] = oopPtr[4] = oopPtr[5] =
        oopPtr[6] = oopPtr[7] = nilObj;
    oopPtr += 8;
    oopCount -= 8;
  }

  if (oopCount & 4) {
    oopPtr[0] = oopPtr[1] = oopPtr[2] = oopPtr[3] = nilObj;
    oopPtr += 4;
  }

  if (oopCount & 2) {
    oopPtr[0] = oopPtr[1] = nilObj;
    oopPtr += 2;
  }

  if (oopCount & 1)
    oopPtr[0] = nilObj;
}

gst_object new_instance_with(OOP class_oop, size_t numIndexFields, OOP *p_oop) {
  size_t numBytes, alignedBytes;
  intptr_t instanceSpec;
  gst_object p_instance;

  instanceSpec = CLASS_INSTANCE_SPEC(class_oop);
  numBytes = sizeof(gst_object_header) +
             SIZE_TO_BYTES(instanceSpec >> ISP_NUMFIXEDFIELDS) +
             (numIndexFields << _gst_log2_sizes[instanceSpec & ISP_SHAPE]);

  alignedBytes = ROUNDED_BYTES(numBytes);
  p_instance = _gst_alloc_obj(alignedBytes, p_oop);
  INIT_UNALIGNED_OBJECT(*p_oop, alignedBytes - numBytes);

  OBJ_SET_CLASS(p_instance, class_oop);

  return p_instance;
}

gst_object new_instance(OOP class_oop, OOP *p_oop) {
  size_t numBytes;
  intptr_t instanceSpec;
  gst_object p_instance;

  instanceSpec = CLASS_INSTANCE_SPEC(class_oop);
  numBytes = sizeof(gst_object_header) +
             SIZE_TO_BYTES(instanceSpec >> ISP_NUMFIXEDFIELDS);

  p_instance = _gst_alloc_obj(numBytes, p_oop);
  OBJ_SET_CLASS(p_instance, class_oop);

  return p_instance;
}

gst_object instantiate_numbytes(OOP class_oop, OOP *p_oop,
                                intptr_t instanceSpec, size_t numBytes) {
  gst_object p_instance;
  int n;
  OOP src, *dest;

  p_instance = _gst_alloc_obj(numBytes, p_oop);
  OBJ_SET_CLASS(p_instance, class_oop);

  n = instanceSpec >> ISP_NUMFIXEDFIELDS;
  if UNCOMMON (n == 0) {
    return p_instance;
    }

  src = _gst_nil_oop;
  dest = p_instance->data;
  dest[0] = src;
  if UNCOMMON (n == 1) {
    return p_instance;
    }

  dest[1] = src;
  if UNCOMMON (n == 2) {
      return p_instance;
    }

  dest[2] = src;
  if UNCOMMON (n == 3) {
      return p_instance;
    }

  dest += 3;
  n -= 3;
  do {
    *(dest++) = src;
  } while (--n > 0);

  return p_instance;
}

gst_object instantiate_with(OOP class_oop, size_t numIndexFields, OOP *p_oop) {
  size_t numBytes, indexedBytes, alignedBytes;
  intptr_t instanceSpec;
  gst_object p_instance;

  instanceSpec = CLASS_INSTANCE_SPEC(class_oop);
#ifndef OPTIMIZE
  if (!(instanceSpec & ISP_ISINDEXABLE) && numIndexFields != 0)
    _gst_errorf(
        "class without indexed instance variables passed to instantiate_with");
#endif

  indexedBytes = numIndexFields << _gst_log2_sizes[instanceSpec & ISP_SHAPE];
  numBytes = sizeof(gst_object_header) +
             SIZE_TO_BYTES(instanceSpec >> ISP_NUMFIXEDFIELDS) + indexedBytes;

  if COMMON ((instanceSpec & ISP_INDEXEDVARS) == GST_ISP_POINTER) {
    p_instance = _gst_alloc_obj(numBytes, p_oop);
    OBJ_SET_CLASS(p_instance, class_oop);
    nil_fill(p_instance->data,
             (instanceSpec >> ISP_NUMFIXEDFIELDS) + numIndexFields);
  } else {
    alignedBytes = ROUNDED_BYTES(numBytes);
    p_instance =
        instantiate_numbytes(class_oop, p_oop, instanceSpec, alignedBytes);
    INIT_UNALIGNED_OBJECT(*p_oop, alignedBytes - numBytes);
    memset(&p_instance->data[instanceSpec >> ISP_NUMFIXEDFIELDS], 0,
           indexedBytes);
  }

  return p_instance;
}

gst_object instantiate(OOP class_oop, OOP *p_oop) {
  size_t numBytes;
  intptr_t instanceSpec;
  gst_object p_instance;

  instanceSpec = CLASS_INSTANCE_SPEC(class_oop);
  numBytes = sizeof(gst_object_header) +
             SIZE_TO_BYTES(instanceSpec >> ISP_NUMFIXEDFIELDS);
  p_instance = instantiate_numbytes(class_oop, p_oop, instanceSpec, numBytes);

  return p_instance;
}

OOP dictionary_association_at(OOP dictionaryOOP, OOP keyOOP) {
  gst_object dictionary;
  size_t index, count, numFields, numFixedFields;
  OOP associationOOP;
  gst_object association;

  if UNCOMMON (IS_NIL(dictionaryOOP))
    return (_gst_nil_oop);

  dictionary = OOP_TO_OBJ(dictionaryOOP);
  numFixedFields = OOP_FIXED_FIELDS(dictionaryOOP);
  numFields = NUM_WORDS(dictionary) - numFixedFields;
  OBJ_UPDATE_IDENTITY(OOP_TO_OBJ(keyOOP));
  index = scramble(TO_INT(OBJ_IDENTITY(OOP_TO_OBJ(keyOOP))));
  count = numFields;

  while (count--) {
    index &= numFields - 1;
    associationOOP = dictionary->data[numFixedFields + index];
    if COMMON (IS_NIL(associationOOP))
      return (_gst_nil_oop);

    association = OOP_TO_OBJ(associationOOP);

    if COMMON (OBJ_ASSOCIATION_GET_KEY(association) == keyOOP)
      return (associationOOP);

    /* linear reprobe -- it is simple and guaranteed */
    index++;
  }

  _gst_errorf("Error - searching Dictionary for nil, but it is full!\n");
  abort();
}

OOP dictionary_at(OOP dictionaryOOP, OOP keyOOP) {
  OOP assocOOP;

  assocOOP = dictionary_association_at(dictionaryOOP, keyOOP);

  if UNCOMMON (IS_NIL(assocOOP))
    return (_gst_nil_oop);
  else
    return (OBJ_ASSOCIATION_GET_VALUE(OOP_TO_OBJ(assocOOP)));
}

OOP association_new(OOP key, OOP value) {
  gst_object association;
  OOP associationOOP;

  association = new_instance(_gst_association_class, &associationOOP);

  OBJ_ASSOCIATION_SET_KEY(association, key);
  OBJ_ASSOCIATION_SET_VALUE(association, value);

  return (associationOOP);
}

OOP variable_binding_new(OOP key, OOP value, OOP environment) {
  gst_object binding;
  OOP bindingOOP;

  binding = new_instance(_gst_variable_binding_class, &bindingOOP);

  OBJ_VARIABLE_BINDING_SET_KEY(binding, key);
  OBJ_VARIABLE_BINDING_SET_VALUE(binding, value);
  OBJ_VARIABLE_BINDING_SET_ENVIRONMENT(binding, environment);

  return (bindingOOP);
}

int oop_num_fields(OOP oop) {
  gst_object object;
  intptr_t instanceSpec;
  size_t words, dataBytes, fixed;

  object = OOP_TO_OBJ(oop);
  words = NUM_WORDS(object);

  if COMMON (!(OOP_GET_FLAGS(oop) & F_BYTE))
    return words;

  instanceSpec = GET_INSTANCE_SPEC(object);
  fixed = instanceSpec >> ISP_NUMFIXEDFIELDS;
  words -= fixed;
  dataBytes = SIZE_TO_BYTES(words) - (OOP_GET_FLAGS(oop) & EMPTY_BYTES);
  return fixed + (dataBytes >> _gst_log2_sizes[instanceSpec & ISP_SHAPE]);
}

int num_valid_oops(const OOP oop) {
  const gst_object object = OOP_TO_OBJ(oop);

  if UNCOMMON (OOP_GET_FLAGS(oop) & F_CONTEXT) {
    const intptr_t methodSP = TO_INT(OBJ_METHOD_CONTEXT_SP_OFFSET(object));
    return OBJ_METHOD_CONTEXT_CONTEXT_STACK(object) + methodSP + 1 -
           object->data;
  } else
    return NUM_OOPS(object);
}

/* Returns whether the SCANNEDOOP points to TARGETOOP.  */
bool is_owner(OOP scannedOOP, OOP targetOOP) {
  gst_object object;
  OOP *scanPtr;
  int n;

  object = OOP_TO_OBJ(scannedOOP);
  if UNCOMMON (OBJ_CLASS(object) == targetOOP)
    return true;

  n = num_valid_oops(scannedOOP);

  /* Peel a couple of iterations for optimization.  */
  if (n--) {
    scanPtr = object->data;
    if UNCOMMON (*scanPtr++ == targetOOP)
      return true;

    if (n--)
      do
        if UNCOMMON (*scanPtr++ == targetOOP)
          return true;
      while (n--);
  }

  return false;
}

OOP index_oop(OOP oop, size_t index) {
  gst_object object = OOP_TO_OBJ(oop);
  intptr_t instanceSpec = GET_INSTANCE_SPEC(object);
  return index_oop_spec(oop, object, index, instanceSpec);
}

OOP index_oop_spec(OOP oop, gst_object object, size_t index,
                   intptr_t instanceSpec) {
  size_t maxIndex, maxByte, base;
  char *src;

  if UNCOMMON (index < 1)
    return (NULL);

#define DO_INDEX_OOP(type, dest)                                               \
  /* Find the number of bytes in the object.  */                               \
  maxByte = NUM_WORDS(object) * sizeof(PTR);                                   \
  if (sizeof(type) <= sizeof(PTR))                                             \
    maxByte -= (OOP_GET_FLAGS(oop) & EMPTY_BYTES);                             \
                                                                               \
  base = (instanceSpec >> ISP_NUMFIXEDFIELDS) * sizeof(PTR);                   \
  index = base + index * sizeof(type);                                         \
                                                                               \
  /* Check that we're on bounds.  */                                           \
  base += sizeof(type);                                                        \
  if UNCOMMON (index - base > maxByte - base)                                  \
    return (NULL);                                                             \
                                                                               \
  index -= sizeof(type);                                                       \
                                                                               \
  /* Use a cast if unaligned accesses are supported, else memcpy.  */          \
  src = ((char *)object->data) + index;                                        \
  if (sizeof(type) <= sizeof(PTR))                                             \
    (dest) = *(type *)src;                                                     \
  else                                                                         \
    memcpy((char *)&(dest), src, sizeof(type));

  switch (instanceSpec & ISP_INDEXEDVARS) {
  case GST_ISP_SCHAR: {
    int8_t i;
    DO_INDEX_OOP(int8_t, i);
    return FROM_INT(i);
  }

  case GST_ISP_UCHAR: {
    uint8_t i;
    DO_INDEX_OOP(uint8_t, i);
    return FROM_INT(i);
  }

  case GST_ISP_CHARACTER: {
    uint8_t i;
    DO_INDEX_OOP(uint8_t, i);
    return CHAR_OOP_AT(i);
  }

  case GST_ISP_SHORT: {
    uint16_t i;
    DO_INDEX_OOP(int16_t, i);
    return FROM_INT(i);
  }

  case GST_ISP_USHORT: {
    uint16_t i;
    DO_INDEX_OOP(uint16_t, i);
    return FROM_INT(i);
  }

  case GST_ISP_INT: {
    uint32_t i;
    DO_INDEX_OOP(int32_t, i);
    return from_c_int_32(i);
  }

  case GST_ISP_UINT: {
    uint32_t i;
    DO_INDEX_OOP(uint32_t, i);
    return from_c_uint_32(i);
  }

  case GST_ISP_FLOAT: {
    float f;
    DO_INDEX_OOP(float, f);
    return floate_new(f);
  }

  case GST_ISP_INT64: {
    uint64_t i;
    DO_INDEX_OOP(int64_t, i);
    return from_c_int_64(i);
  }

  case GST_ISP_UINT64: {
    uint64_t i;
    DO_INDEX_OOP(uint64_t, i);
    return from_c_uint_64(i);
  }

  case GST_ISP_DOUBLE: {
    double d;
    DO_INDEX_OOP(double, d);
    return floatd_new(d);
  }

  case GST_ISP_UTF32: {
    uint32_t i;
    DO_INDEX_OOP(uint32_t, i);
    return char_new(i);
  }

  case GST_ISP_POINTER:
    maxIndex = NUM_WORDS(object);
    base = instanceSpec >> ISP_NUMFIXEDFIELDS;
    index += base;
    base++;
    if UNCOMMON (index - base > maxIndex - base)
      return (NULL);

    return (object->data[index - 1]);
  }
#undef DO_INDEX_OOP

  return (NULL);
}

bool index_oop_put(OOP oop, size_t index, OOP value) {
  gst_object object = OOP_TO_OBJ(oop);
  intptr_t instanceSpec = GET_INSTANCE_SPEC(object);
  return index_oop_put_spec(oop, object, index, value, instanceSpec);
}

bool index_oop_put_spec(OOP oop, gst_object object, size_t index,
                               OOP value, intptr_t instanceSpec) {
  size_t maxIndex, base;

  if UNCOMMON (index < 1)
    return (false);

#define DO_INDEX_OOP_PUT(type, cond, src)                                      \
  if COMMON (cond) {                                                           \
    /* Find the number of bytes in the object.  */                             \
    size_t maxByte = NUM_WORDS(object) * sizeof(PTR);                          \
    if (sizeof(type) <= sizeof(PTR))                                           \
      maxByte -= (OOP_GET_FLAGS(oop) & EMPTY_BYTES);                           \
                                                                               \
    base = (instanceSpec >> ISP_NUMFIXEDFIELDS) * sizeof(PTR);                 \
    index = base + index * sizeof(type);                                       \
                                                                               \
    /* Check that we're on bounds.  */                                         \
    base += sizeof(type);                                                      \
    if UNCOMMON (index - base > maxByte - base)                                \
      return (false);                                                          \
                                                                               \
    index -= sizeof(type);                                                     \
                                                                               \
    /* Use a cast if unaligned accesses are ok, else memcpy.  */               \
    if (sizeof(type) <= sizeof(PTR)) {                                         \
      type *destAddr = (type *)(((char *)object->data) + index);               \
      *destAddr = (type)(src);                                                 \
    } else {                                                                   \
      char *destAddr = ((char *)object->data) + index;                         \
      type src_ = (type)(src);                                                 \
      memcpy(destAddr, (char *)&src_, sizeof(type));                           \
    }                                                                          \
    return (true);                                                             \
  }

  switch (instanceSpec & ISP_INDEXEDVARS) {
  case GST_ISP_SCHAR: {
    DO_INDEX_OOP_PUT(
        int8_t, IS_INT(value) && TO_INT(value) >= -128 && TO_INT(value) <= 127,
        TO_INT(value));
    return (false);
  }

  case GST_ISP_UCHAR: {
    DO_INDEX_OOP_PUT(
        uint8_t, IS_INT(value) && TO_INT(value) >= 0 && TO_INT(value) <= 255,
        TO_INT(value));
    return (false);
  }

  case GST_ISP_CHARACTER: {
    DO_INDEX_OOP_PUT(uint8_t,
                     !IS_INT(value) && OOP_CLASS(value) == _gst_char_class,
                     CHAR_OOP_VALUE(value));
    return (false);
  }

  case GST_ISP_SHORT: {
    DO_INDEX_OOP_PUT(uint16_t,
                     IS_INT(value) && TO_INT(value) >= -32768 &&
                         TO_INT(value) <= 32767,
                     TO_INT(value));
    return (false);
  }

  case GST_ISP_USHORT: {
    DO_INDEX_OOP_PUT(
        uint16_t, IS_INT(value) && TO_INT(value) >= 0 && TO_INT(value) <= 65535,
        TO_INT(value));
    return (false);
  }

  case GST_ISP_INT: {
    DO_INDEX_OOP_PUT(int32_t, is_c_int_32(value), to_c_int_32(value));
    return (false);
  }

  case GST_ISP_UINT: {
    DO_INDEX_OOP_PUT(uint32_t, is_c_uint_32(value), to_c_int_32(value));
    return (false);
  }

  case GST_ISP_FLOAT: {
    DO_INDEX_OOP_PUT(float, IS_INT(value), TO_INT(value));
    DO_INDEX_OOP_PUT(float, OOP_CLASS(value) == _gst_floate_class,
                     FLOATE_OOP_VALUE(value));
    DO_INDEX_OOP_PUT(float, OOP_CLASS(value) == _gst_floatd_class,
                     FLOATD_OOP_VALUE(value));
    DO_INDEX_OOP_PUT(float, OOP_CLASS(value) == _gst_floatq_class,
                     FLOATQ_OOP_VALUE(value));
    return (false);
  }

  case GST_ISP_INT64: {
    DO_INDEX_OOP_PUT(int64_t, is_c_int_64(value), to_c_int_64(value));
    return (false);
  }

  case GST_ISP_UINT64: {
    DO_INDEX_OOP_PUT(uint64_t, is_c_uint_64(value), to_c_uint_64(value));
    return (false);
  }

  case GST_ISP_DOUBLE: {
    DO_INDEX_OOP_PUT(double, IS_INT(value), TO_INT(value));
    DO_INDEX_OOP_PUT(double, OOP_CLASS(value) == _gst_floatd_class,
                     FLOATD_OOP_VALUE(value));
    DO_INDEX_OOP_PUT(double, OOP_CLASS(value) == _gst_floate_class,
                     FLOATE_OOP_VALUE(value));
    DO_INDEX_OOP_PUT(double, OOP_CLASS(value) == _gst_floatq_class,
                     FLOATQ_OOP_VALUE(value));
    return (false);
  }

  case GST_ISP_UTF32: {
    DO_INDEX_OOP_PUT(uint32_t,
                     !IS_INT(value) &&
                         (OOP_CLASS(value) == _gst_unicode_character_class ||
                          (OOP_CLASS(value) == _gst_char_class &&
                           CHAR_OOP_VALUE(value) <= 127)),
                     CHAR_OOP_VALUE(value));
    return (false);
  }

  case GST_ISP_POINTER:
    maxIndex = NUM_WORDS(object);
    base = instanceSpec >> ISP_NUMFIXEDFIELDS;
    index += base;
    base++;
    if UNCOMMON (index - base > maxIndex - base)
      return (false);

    object->data[index - 1] = value;
    return (true);
  }
#undef DO_INDEX_OOP_PUT

  return (false);
}

OOP inst_var_at(OOP oop, int index) {
  gst_object object;

  object = OOP_TO_OBJ(oop);
  return (object->data[index - 1]);
}

void inst_var_at_put(OOP oop, int index, OOP value) {
  gst_object object;

  object = OOP_TO_OBJ(oop);
  object->data[index - 1] = value;
}

OOP atomic_inst_var_at(OOP oop, int index) {
  gst_object object;

  object = OOP_TO_OBJ(oop);
  return (atomic_load((_Atomic OOP*) &object->data[index - 1]));
}

void atomic_inst_var_at_put(OOP oop, int index, OOP value) {
  gst_object object;

  object = OOP_TO_OBJ(oop);
  object->data[index - 1] = value;
}

bool is_c_int_32(OOP oop) {
  gst_object ba;

  if COMMON (IS_INT(oop))
#if SIZEOF_OOP == 4
    return (true);
#else
    return (TO_INT(oop) >= INT_MIN && TO_INT(oop) < INT_MAX);
#endif

  ba = OOP_TO_OBJ(oop);
  if (COMMON(OBJ_CLASS(ba) == _gst_large_positive_integer_class) ||
      OBJ_CLASS(ba) == _gst_large_negative_integer_class)
    return (NUM_INDEXABLE_FIELDS(oop) == 4);

  return (false);
}

bool is_c_uint_32(OOP oop) {
  gst_object ba;

  if COMMON (IS_INT(oop))
#if SIZEOF_OOP == 4
    return (TO_INT(oop) >= 0);
#else
    return (TO_INT(oop) >= 0 && TO_INT(oop) < UINT_MAX);
#endif

  ba = OOP_TO_OBJ(oop);
  if COMMON (OBJ_CLASS(ba) == _gst_large_positive_integer_class) {
    switch (NUM_INDEXABLE_FIELDS(oop)) {
    case 4:
      return (true);
    case 5:
      return (OBJ_BYTE_ARRAY_GET_BYTES(ba, 4) == 0);
    }
  }

  return (false);
}

int32_t to_c_int_32(OOP oop) {
  gst_object ba;

  if COMMON (IS_INT(oop))
    return (TO_INT(oop));

  ba = OOP_TO_OBJ(oop);
  return ((int32_t)(
      (((uint32_t)OBJ_BYTE_ARRAY_GET_BYTES(ba, 3)) << 24) + (((uint32_t)OBJ_BYTE_ARRAY_GET_BYTES(ba, 2)) << 16) +
      (((uint32_t)OBJ_BYTE_ARRAY_GET_BYTES(ba, 1)) << 8) + ((uint32_t)OBJ_BYTE_ARRAY_GET_BYTES(ba, 0))));
}

OOP from_c_int_32(int32_t i) {
  #if SIZEOF_OOP == 4
  gst_object ba;
  OOP oop;
  const uint32_t ui = (uint32_t)i;

  if (COMMON (i >= MIN_ST_INT && i <= MAX_ST_INT)) {
    return FROM_INT(i);
  }

  if (i < 0) {
    ba = new_instance_with(_gst_large_negative_integer_class, 4, &oop);
  } else {
    ba = new_instance_with(_gst_large_positive_integer_class, 4, &oop);
  }

  OBJ_BYTE_ARRAY_SET_BYTES(ba, 0, (gst_uchar)ui);
  OBJ_BYTE_ARRAY_SET_BYTES(ba, 1, (gst_uchar)(ui >> 8));
  OBJ_BYTE_ARRAY_SET_BYTES(ba, 2, (gst_uchar)(ui >> 16));
  OBJ_BYTE_ARRAY_SET_BYTES(ba, 3, (gst_uchar)(ui >> 24));
  return (oop);
  #else
  return FROM_INT(i);
  #endif
}

OOP from_c_uint_32(uint32_t ui) {
  #if SIZEOF_OOP == 4
  gst_object ba;
  OOP oop;

  if COMMON (ui <= MAX_ST_INT)
    return (FROM_INT(ui));

  if UNCOMMON (((intptr_t)ui) < 0) {
    ba = new_instance_with(_gst_large_positive_integer_class, 5,
                                           &oop);

    OBJ_BYTE_ARRAY_SET_BYTES(ba, 4, 0);
  } else
    ba = new_instance_with(_gst_large_positive_integer_class, 4,
                                           &oop);

  OBJ_BYTE_ARRAY_SET_BYTES(ba, 0, (gst_uchar)ui);
  OBJ_BYTE_ARRAY_SET_BYTES(ba, 1, (gst_uchar)(ui >> 8));
  OBJ_BYTE_ARRAY_SET_BYTES(ba, 2, (gst_uchar)(ui >> 16));
  OBJ_BYTE_ARRAY_SET_BYTES(ba, 3, (gst_uchar)(ui >> 24));

  return (oop);
  #else
  return FROM_INT(ui);
  #endif
}

bool is_c_int_64(OOP oop) {
  gst_object ba;

  if COMMON (IS_INT(oop))
    return (true);

  ba = OOP_TO_OBJ(oop);
  if COMMON (OBJ_CLASS(ba) == _gst_large_negative_integer_class ||
             OBJ_CLASS(ba) == _gst_large_positive_integer_class) {
    switch (NUM_INDEXABLE_FIELDS(oop)) {
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
      return (true);
    }
  }

  return (false);
}

bool is_c_uint_64(OOP oop) {
  gst_object ba;

  if COMMON (IS_INT(oop))
    return (TO_INT(oop) >= 0);

  ba = OOP_TO_OBJ(oop);
  if COMMON (OBJ_CLASS(ba) == _gst_large_positive_integer_class) {
    switch (NUM_INDEXABLE_FIELDS(oop)) {
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
      return (true);
    case 9:
      return (OBJ_BYTE_ARRAY_GET_BYTES(ba, 8) == 0);
    }
  }

  return (false);
}

uint64_t to_c_uint_64(OOP oop) {
  gst_object ba;
  uint64_t result, mask;

  if COMMON (IS_INT(oop))
    return (TO_INT(oop));

  ba = OOP_TO_OBJ(oop);
  mask = (((uint64_t)2) << (NUM_INDEXABLE_FIELDS(oop) * 8 - 1)) - 1;
  result = ((int64_t)(
      (((uint64_t)OBJ_BYTE_ARRAY_GET_BYTES(ba, 3)) << 24) + (((uint64_t)OBJ_BYTE_ARRAY_GET_BYTES(ba, 2)) << 16) +
      (((uint64_t)OBJ_BYTE_ARRAY_GET_BYTES(ba, 1)) << 8) + ((uint64_t)OBJ_BYTE_ARRAY_GET_BYTES(ba, 0))));

  if (NUM_INDEXABLE_FIELDS(oop) > 4)
    result |= mask & ((int64_t)((((uint64_t)OBJ_BYTE_ARRAY_GET_BYTES(ba, 7)) << 56) +
                                (((uint64_t)OBJ_BYTE_ARRAY_GET_BYTES(ba, 6)) << 48) +
                                (((uint64_t)OBJ_BYTE_ARRAY_GET_BYTES(ba, 5)) << 40) +
                                (((uint64_t)OBJ_BYTE_ARRAY_GET_BYTES(ba, 4)) << 32)));

  return result;
}

int64_t to_c_int_64(OOP oop) {
  gst_object ba;
  int64_t result, mask;

  if COMMON (IS_INT(oop))
    return (TO_INT(oop));

  ba = OOP_TO_OBJ(oop);
  mask = (((uint64_t)2) << (NUM_INDEXABLE_FIELDS(oop) * 8 - 1)) - 1;
  result = (OBJ_CLASS(ba) == _gst_large_negative_integer_class) ? ~mask : 0;
  result |= ((int64_t)(
      (((uint64_t)OBJ_BYTE_ARRAY_GET_BYTES(ba,3)) << 24) + (((uint64_t)OBJ_BYTE_ARRAY_GET_BYTES(ba, 2)) << 16) +
      (((uint64_t)OBJ_BYTE_ARRAY_GET_BYTES(ba,1)) << 8) + ((uint64_t)OBJ_BYTE_ARRAY_GET_BYTES(ba, 0))));

  if (NUM_INDEXABLE_FIELDS(oop) > 4)
    result |= mask & ((int64_t)((((uint64_t)OBJ_BYTE_ARRAY_GET_BYTES(ba, 7)) << 56) +
                                (((uint64_t)OBJ_BYTE_ARRAY_GET_BYTES(ba, 6)) << 48) +
                                (((uint64_t)OBJ_BYTE_ARRAY_GET_BYTES(ba, 5)) << 40) +
                                (((uint64_t)OBJ_BYTE_ARRAY_GET_BYTES(ba, 4)) << 32)));

  return result;
}

OOP from_c_int_64(int64_t i) {
  gst_object ba;
  OOP oop;
  const uint64_t ui = (uint64_t)i;

  if COMMON (i >= MIN_ST_INT && i <= MAX_ST_INT)
    return (FROM_INT(i));

  if (i < 0)
    ba = new_instance_with(_gst_large_negative_integer_class, 8,
                                           &oop);
  else
    ba = new_instance_with(_gst_large_positive_integer_class, 8,
                                           &oop);

  OBJ_BYTE_ARRAY_SET_BYTES(ba, 0, (gst_uchar)ui);
  OBJ_BYTE_ARRAY_SET_BYTES(ba, 1, (gst_uchar)(ui >> 8));
  OBJ_BYTE_ARRAY_SET_BYTES(ba, 2, (gst_uchar)(ui >> 16));
  OBJ_BYTE_ARRAY_SET_BYTES(ba, 3, (gst_uchar)(ui >> 24));
  OBJ_BYTE_ARRAY_SET_BYTES(ba, 4, (gst_uchar)(ui >> 32));
  OBJ_BYTE_ARRAY_SET_BYTES(ba, 5, (gst_uchar)(ui >> 40));
  OBJ_BYTE_ARRAY_SET_BYTES(ba, 6, (gst_uchar)(ui >> 48));
  OBJ_BYTE_ARRAY_SET_BYTES(ba, 7, (gst_uchar)(ui >> 56));

  return (oop);
}

OOP from_c_uint_64(uint64_t ui) {
  gst_object ba;
  OOP oop;

  if COMMON (ui <= MAX_ST_INT)
    return (FROM_INT(ui));

  if UNCOMMON (((int64_t)ui) < 0) {
    ba = new_instance_with(_gst_large_positive_integer_class, 9,
                                           &oop);

    OBJ_BYTE_ARRAY_SET_BYTES(ba, 8, 0);
  } else
    ba = new_instance_with(_gst_large_positive_integer_class, 8,
                                           &oop);

  OBJ_BYTE_ARRAY_SET_BYTES(ba, 0, (gst_uchar)ui);
  OBJ_BYTE_ARRAY_SET_BYTES(ba, 1, (gst_uchar)(ui >> 8));
  OBJ_BYTE_ARRAY_SET_BYTES(ba, 2, (gst_uchar)(ui >> 16));
  OBJ_BYTE_ARRAY_SET_BYTES(ba, 3, (gst_uchar)(ui >> 24));
  OBJ_BYTE_ARRAY_SET_BYTES(ba, 4, (gst_uchar)(ui >> 32));
  OBJ_BYTE_ARRAY_SET_BYTES(ba, 5, (gst_uchar)(ui >> 40));
  OBJ_BYTE_ARRAY_SET_BYTES(ba, 6, (gst_uchar)(ui >> 48));
  OBJ_BYTE_ARRAY_SET_BYTES(ba, 7, (gst_uchar)(ui >> 56));

  return (oop);
}

PTR cobject_value(OOP oop) {
  gst_object cObj = OOP_TO_OBJ(oop);
  if (IS_NIL(OBJ_COBJECT_GET_STORAGE(cObj)))
    return (PTR)COBJECT_OFFSET_OBJ(cObj);
  else {
    gst_uchar *baseAddr = (gst_uchar *)(OOP_TO_OBJ(OBJ_COBJECT_GET_STORAGE(cObj)))->data;
    return (PTR)(baseAddr + COBJECT_OFFSET_OBJ(cObj));
  }
}

/* Sets the address of the data stored in a CObject.  */
void set_cobject_value(OOP oop, PTR val) {
  gst_object cObj = OOP_TO_OBJ(oop);
  OBJ_COBJECT_SET_STORAGE(cObj, _gst_nil_oop);
  SET_COBJECT_OFFSET_OBJ(cObj, (uintptr_t)val);
}

/* Return whether the address of the data stored in a CObject, offsetted
   by OFFSET bytes, is still in bounds.  */
bool cobject_index_check(OOP oop, intptr_t offset, intptr_t size) {
  gst_object cObj = OOP_TO_OBJ(oop);
  OOP baseOOP = OBJ_COBJECT_GET_STORAGE(cObj);
  intptr_t maxOffset;
  if (IS_NIL(baseOOP))
    return true;

  offset += COBJECT_OFFSET_OBJ(cObj);
  if (offset < 0)
    return false;

  maxOffset = SIZE_TO_BYTES(NUM_WORDS(OOP_TO_OBJ(baseOOP)));
  if (OOP_GET_FLAGS(baseOOP) & F_BYTE)
    maxOffset -= (OOP_GET_FLAGS(baseOOP) & EMPTY_BYTES);

  return (offset + size - 1 < maxOffset);
}

