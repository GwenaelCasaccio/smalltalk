#include "gstpriv.h"

/* Called when an MethodDictionary becomes full, this routine
   replaces the MethodDictionary instance that MethodDictionary OOP
   is pointing to with a new, larger dictionary, and returns this new
   dictionary (the object pointer, not the OOP).  */
static gst_object grow_method_dictionary(OOP methodDictionaryOOP);

/* Look for the index at which keyOOP resides in MethodDictionaryOOP
   and answer it or, if not found, return the firt Nil index.
   In case of error (too much iterations) it will abort */
static size_t method_dictionary_find_key_or_nil(OOP methodDictionaryOOP,
                                                OOP keyOOP);

OOP _gst_method_dictionary_new(size_t wanted_size) {
  const size_t size = new_num_fields(wanted_size);

  OOP methodDictionaryOOP;
  gst_object methodDictionary = instantiate(_gst_method_dictionary_class, &methodDictionaryOOP);

  OBJ_METHOD_DICTIONARY_SET_TALLY(methodDictionary, FROM_INT(0));

  OOP keysOOP;
  instantiate_with(_gst_array_class, size, &keysOOP);
  OBJ_METHOD_DICTIONARY_SET_KEYS(methodDictionary, keysOOP);

  OOP valuesOOP;
  instantiate_with(_gst_array_class, size, &valuesOOP);
  OBJ_METHOD_DICTIONARY_SET_VALUES(methodDictionary, valuesOOP);

  return methodDictionaryOOP;
}

OOP _gst_method_dictionary_at_put(OOP methodDictionaryOOP,
                                  OOP keyOOP,
                                  OOP valueOOP) {
  gst_object methodDictionary = OOP_TO_OBJ(methodDictionaryOOP);
  OOP keysOOP = OBJ_METHOD_DICTIONARY_GET_KEYS(methodDictionary);
  gst_object keys = OOP_TO_OBJ(keysOOP);
  OOP valuesOOP = OBJ_METHOD_DICTIONARY_GET_VALUES(methodDictionary);

  if (UNCOMMON (TO_INT(OBJ_IDENTITY_DICTIONARY_GET_TALLY(methodDictionary)) >= TO_INT(OBJ_SIZE(keys)) * 3 / 8)) {
    methodDictionary = grow_method_dictionary(methodDictionaryOOP);

    methodDictionary = OOP_TO_OBJ(methodDictionaryOOP);
    keysOOP = OBJ_METHOD_DICTIONARY_GET_KEYS(methodDictionary);
    keys = OOP_TO_OBJ(keysOOP);
    valuesOOP = OBJ_METHOD_DICTIONARY_GET_VALUES(methodDictionary);
  }

  const size_t index = method_dictionary_find_key_or_nil(methodDictionaryOOP, keyOOP);

  if (COMMON (IS_NIL(ARRAY_AT(keysOOP, index)))) {
    OBJ_METHOD_DICTIONARY_SET_TALLY(methodDictionary, INCR_INT(OBJ_METHOD_DICTIONARY_GET_TALLY(methodDictionary)));
  }

  ARRAY_AT_PUT(keysOOP, index, keyOOP);
  OOP oldValueOOP = ARRAY_AT(valuesOOP, index);
  ARRAY_AT_PUT(valuesOOP, index, valueOOP);

  return oldValueOOP;
}

gst_object grow_method_dictionary(OOP oldMethodDictionaryOOP) {
  gst_object oldMethodDictionary = OOP_TO_OBJ(oldMethodDictionaryOOP);
  const OOP oldKeysOOP = OBJ_METHOD_DICTIONARY_GET_KEYS(oldMethodDictionary);
  const size_t oldNumFields = NUM_WORDS(OOP_TO_OBJ(oldKeysOOP));
  const OOP oldValuesOOP = OBJ_METHOD_DICTIONARY_GET_VALUES(oldMethodDictionary);

  const size_t numFields = new_num_fields(oldNumFields);

  inc_ptr incPtr = INC_SAVE_POINTER();

  OOP methodDictionaryOOP;
  gst_object methodDictionary = instantiate(_gst_method_dictionary_class, &methodDictionaryOOP);
  INC_ADD_OOP(methodDictionaryOOP);

  OOP keysOOP;
  instantiate_with(_gst_array_class, numFields, &keysOOP);
  INC_ADD_OOP(keysOOP);

  OOP valuesOOP;
  instantiate_with(_gst_array_class, numFields, &valuesOOP);
  INC_ADD_OOP(valuesOOP);

  OBJ_METHOD_DICTIONARY_SET_TALLY(
      methodDictionary,
      OBJ_METHOD_DICTIONARY_GET_TALLY(oldMethodDictionary));
  OBJ_METHOD_DICTIONARY_SET_KEYS(methodDictionary, keysOOP);
  OBJ_METHOD_DICTIONARY_SET_VALUES(methodDictionary, valuesOOP);

  /* rehash all associations from old dictionary into new one */
  for (size_t i = 1; i <= oldNumFields; i++) {
    OOP keyOOP = ARRAY_AT(oldKeysOOP, i);
    if (COMMON (!IS_NIL(keyOOP))) {
      const size_t index = method_dictionary_find_key_or_nil(methodDictionaryOOP, keyOOP);
      ARRAY_AT_PUT(keysOOP, index, keyOOP);
      ARRAY_AT_PUT(valuesOOP, index, ARRAY_AT(oldValuesOOP, i));
    }
  }

  _gst_swap_objects(methodDictionaryOOP, oldMethodDictionaryOOP);

  INC_RESTORE_POINTER(incPtr);

  return OOP_TO_OBJ(oldMethodDictionaryOOP);
}

ssize_t _gst_method_dictionary_find_key(OOP methodDictionaryOOP, OOP keyOOP) {
  gst_object methodDictionary = OOP_TO_OBJ(methodDictionaryOOP);
  OOP keysOOP = OBJ_METHOD_DICTIONARY_GET_KEYS(methodDictionary);
  gst_object keys = OOP_TO_OBJ(keysOOP);

  const size_t numFields = NUM_WORDS(keys);

  OBJ_UPDATE_IDENTITY(OOP_TO_OBJ(keyOOP));

  for (size_t index = 1; index <= numFields; index++) {
    if (COMMON(ARRAY_AT(keysOOP, index) == keyOOP))
      return index;
  }

  return -1;
}

size_t method_dictionary_find_key_or_nil(OOP methodDictionaryOOP, OOP keyOOP) {
  gst_object methodDictionary = OOP_TO_OBJ(methodDictionaryOOP);
  OOP keysOOP = OBJ_METHOD_DICTIONARY_GET_KEYS(methodDictionary);
  gst_object keys = OOP_TO_OBJ(keysOOP);
  const size_t numFields = NUM_WORDS(keys);

  OBJ_UPDATE_IDENTITY(OOP_TO_OBJ(keyOOP));

  const size_t index = (scramble(TO_INT(OBJ_IDENTITY(OOP_TO_OBJ(keyOOP))))& (numFields - 1)) + 1;

  size_t i = index;
  size_t count = numFields;

  while (count--) {
    if (COMMON(IS_NIL(ARRAY_AT(keysOOP, i)))) {
      return i;
    }

    if (COMMON(ARRAY_AT(keysOOP, i) == keyOOP)) {
      return i;
    }

    if (i == numFields) {
      i = 1;
    }
    else {
      i++;
    }

  }

  _gst_errorf("Error - searching MethodDictionary for nil, but it is full!\n");
  abort();
}

