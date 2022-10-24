#ifndef GST_OBJECT_POINTER_H
#define GST_OBJECT_POINTER_H

extern intptr_t _gst_object_identity;

#define OBJ_ARRAY_AT(obj, n) (((OOP *)((gst_object)obj)->data)[(n)-1])
#define OBJ_STRING_AT(obj, n) (((char *)((gst_object)obj)->data)[(n)-1])

#define OBJ_SIZE(obj) ((obj)->objSize)

#define OBJ_SET_SIZE(obj, valueOOP)                                            \
  do {                                                                         \
    (obj)->objSize = (valueOOP);                                               \
  } while (0)

#define OBJ_IDENTITY(obj) ((obj)->objIdentity)

#define OBJ_SET_IDENTITY(obj, valueOOP)                                        \
  do {                                                                         \
    (obj)->objIdentity = (valueOOP);                                           \
  } while (0)

#define OBJ_UPDATE_IDENTITY(obj)                                               \
  do {                                                                         \
    if (TO_INT((obj)->objIdentity) == 0) {                                     \
      _gst_object_identity++;                                                  \
      (obj)->objIdentity = FROM_INT(_gst_object_identity);                     \
    }                                                                          \
  } while (0)

#define OBJ_CLASS(obj) ((obj)->objClass)

#define OBJ_SET_CLASS(obj, valueOOP)                                           \
  do {                                                                         \
    (obj)->objClass = (valueOOP);                                              \
  } while (0)

/* METHOD CONTEXT */

#define OBJ_METHOD_CONTEXT_PARENT_CONTEXT(obj) ((obj)->data[0])
#define OBJ_METHOD_CONTEXT_SET_PARENT_CONTEXT(obj, valueOOP)                   \
  do {                                                                         \
    (obj)->data[0] = (valueOOP);                                               \
  } while (0)

#define OBJ_METHOD_CONTEXT_NATIVE_IP(obj) ((obj)->data[1])
#define OBJ_METHOD_CONTEXT_SET_NATIVE_IP(obj, valueOOP)                        \
  do {                                                                         \
    (obj)->data[1] = (valueOOP);                                               \
  } while (0)

#define OBJ_METHOD_CONTEXT_IP_OFFSET(obj) ((obj)->data[2])
#define OBJ_METHOD_CONTEXT_SET_IP_OFFSET(obj, valueOOP)                        \
  do {                                                                         \
    (obj)->data[2] = (valueOOP);                                               \
  } while (0)

#define OBJ_METHOD_CONTEXT_SP_OFFSET(obj) ((obj)->data[3])
#define OBJ_METHOD_CONTEXT_SET_SP_OFFSET(obj, valueOOP)                        \
  do {                                                                         \
    (obj)->data[3] = (valueOOP);                                               \
  } while (0)

#define OBJ_METHOD_CONTEXT_RECEIVER(obj) ((obj)->data[4])
#define OBJ_METHOD_CONTEXT_SET_RECEIVER(obj, valueOOP)                         \
  do {                                                                         \
    (obj)->data[4] = (valueOOP);                                               \
  } while (0)

#define OBJ_METHOD_CONTEXT_METHOD(obj) ((obj)->data[5])
#define OBJ_METHOD_CONTEXT_SET_METHOD(obj, valueOOP)                           \
  do {                                                                         \
    (obj)->data[5] = (valueOOP);                                               \
  } while (0)

#define OBJ_METHOD_CONTEXT_FLAGS(obj) ((obj)->data[6])
#define OBJ_METHOD_CONTEXT_SET_FLAGS(obj, valueOOP)                            \
  do {                                                                         \
    (obj)->data[6] = (valueOOP);                                               \
  } while (0)

#define OBJ_METHOD_CONTEXT_CONTEXT_STACK(obj) (&(obj)->data[7])
#define OBJ_METHOD_CONTEXT_CONTEXT_STACK_AT_PUT(obj, idx, valueOOP)            \
  do {                                                                         \
    (obj)->data[7 + (idx)] = (valueOOP);                                       \
  } while (0)

/* BLOCK CONTEXT */

#define OBJ_BLOCK_CONTEXT_PARENT_CONTEXT(obj) ((obj)->data[0])
#define OBJ_BLOCK_CONTEXT_SET_PARENT_CONTEXT(obj, valueOOP)                    \
  do {                                                                         \
    (obj)->data[0] = (valueOOP);                                               \
  } while (0)

#define OBJ_BLOCK_CONTEXT_NATIVE_IP(obj) ((obj)->data[1])
#define OBJ_BLOCK_CONTEXT_SET_NATIVE_IP(obj, valueOOP)                         \
  do {                                                                         \
    (obj)->data[1] = (valueOOP);                                               \
  } while (0)

#define OBJ_BLOCK_CONTEXT_IP_OFFSET(obj) ((obj)->data[2])
#define OBJ_BLOCK_CONTEXT_SET_IP_OFFSET(obj, valueOOP)                         \
  do {                                                                         \
    (obj)->data[2] = (valueOOP);                                               \
  } while (0)

#define OBJ_BLOCK_CONTEXT_SP_OFFSET(obj) ((obj)->data[3])
#define OBJ_BLOK_CONTEXT_SET_SP_OFFSET(obj, valueOOP)                          \
  do {                                                                         \
    (obj)->data[3] = (valueOOP);                                               \
  } while (0)

#define OBJ_BLOCK_CONTEXT_RECEIVER(obj) ((obj)->data[4])
#define OBJ_BLOCK_CONTEXT_SET_RECEIVER(obj, valueOOP)                          \
  do {                                                                         \
    (obj)->data[4] = (valueOOP);                                               \
  } while (0)

#define OBJ_BLOCK_CONTEXT_METHOD(obj) ((obj)->data[5])
#define OBJ_BLOCK_CONTEXT_SET_METHOD(obj, valueOOP)                            \
  do {                                                                         \
    (obj)->data[5] = (valueOOP);                                               \
  } while (0)

#define OBJ_BLOCK_CONTEXT_GET_OUTER_CONTEXT(obj) ((obj)->data[6])
#define OBJ_BLOCK_CONTEXT_SET_OUTER_CONTEXT(obj, valueOOP)                     \
  do {                                                                         \
    (obj)->data[6] = (valueOOP);                                               \
  } while (0)

#define OBJ_BLOCK_CONTEXT_AT_STACK(obj, index) ((obj)->data[7 + (index)])
#define OBJ_BLOCK_CONTEXT_AT_PUT_STACK(obj, index, valueOOP)                   \
  do {                                                                         \
    (obj)->data[7 + (index)] = (valueOOP);                                     \
  } while (0)

#define OBJ_CONTINUATION_GET_STACK(obj) ((obj)->data[0])

#define OBJ_CONTINUATION_SET_STACK(obj, valueOOP)                              \
  do {                                                                         \
    (obj)->data[0] = (valueOOP);                                               \
  } while (0)

#define OBJ_SEMAPHORE_GET_FIRST_LINK(obj) ((obj))->data[0]
#define OBJ_SEMAPHORE_SET_FIRST_LINK(obj, valueOOP)                            \
  do {                                                                         \
    ((obj))->data[0] = (valueOOP);                                             \
  } while (0)

#define OBJ_SEMAPHORE_GET_LAST_LINK(obj) ((obj))->data[1]
#define OBJ_SEMAPHORE_SET_LAST_LINK(obj, valueOOP)                             \
  do {                                                                         \
    ((obj))->data[1] = (valueOOP);                                             \
  } while (0)

#define OBJ_SEMAPHORE_GET_SIGNALS(obj) atomic_load((_Atomic OOP*) &((obj)->data[2]))
#define OBJ_SEMAPHORE_SET_SIGNALS(obj, valueOOP)                               \
  do {                                                                         \
    atomic_store((_Atomic OOP*) &((obj)->data[2]), (valueOOP));                \
  } while (0)

#define OBJ_SEMAPHORE_GET_NAME(obj) ((obj))->data[3]

#define OBJ_SEMAPHORE_GET_LOCK_THREAD_ID(obj) ((obj))->data[4]
#define OBJ_SEMAPHORE_SET_LOCK_THREAD_ID(obj, valueOOP)                        \
  do {                                                                         \
    ((obj))->data[4] = (valueOOP);                                             \
  } while (0)

#define OBJ_DICTIONARY_GET_TALLY(obj) ((obj))->data[0]
#define OBJ_DICTIONARY_SET_TALLY(obj, valueOOP)                                \
  do {                                                                         \
    ((obj))->data[0] = (valueOOP);                                             \
  } while (0)

#define OBJ_BINDING_DICTIONARY_GET_TALLY(obj) ((obj))->data[0]
#define OBJ_BINDING_DICTIONARY_SET_TALLY(obj, valueOOP)                        \
  do {                                                                         \
    ((obj))->data[0] = (valueOOP);                                             \
  } while (0)

#define OBJ_BINDING_DICTIONARY_GET_ENVIRONMENT(obj) ((obj))->data[1]
#define OBJ_BINDING_DICTIONARY_SET_ENVIRONMENT(obj, valueOOP)                  \
  do {                                                                         \
    ((obj))->data[1] = (valueOOP);                                             \
  } while (0)

#define OBJ_BINDING_DICTIONARY_AT_ASSOC(obj, index) ((obj))->data[2 + (index)]
#define OBJ_BINDING_DICTIONARY_AT_PUT_ASSOC(obj, index, valueOOP)              \
  do {                                                                         \
    ((obj))->data[2 + (index)] = (valueOOP);                                   \
  } while (0)

#define OBJ_NAMESPACE_GET_TALLY(obj) ((obj))->data[0]
#define OBJ_NAMESPACE_SET_TALLY(obj, valueOOP)                                 \
  do {                                                                         \
    ((obj))->data[0] = (valueOOP);                                             \
  } while (0)

#define OBJ_NAMESPACE_GET_SUPER_SPACE(obj) ((obj))->data[1]
#define OBJ_NAMESPACE_SET_SUPER_SPACE(obj, valueOOP)                           \
  do {                                                                         \
    ((obj))->data[1] = (valueOOP);                                             \
  } while (0)

#define OBJ_NAMESPACE_GET_NAME(obj) ((obj))->data[2]
#define OBJ_NAMESPACE_SET_NAME(obj, valueOOP)                                  \
  do {                                                                         \
    ((obj))->data[2] = (valueOOP);                                             \
  } while (0)

#define OBJ_NAMESPACE_GET_SUBSPACES(obj) ((obj))->data[3]
#define OBJ_NAMESPACE_SET_SUBSPACES(obj, valueOOP)                             \
  do {                                                                         \
    ((obj))->data[3] = (valueOOP);                                             \
  } while (0)

#define OBJ_NAMESPACE_GET_SHARED_POOLS(obj) ((obj))->data[4]
#define OBJ_NAMESPACE_SET_SHARED_POOLS(obj, valueOOP)                          \
  do {                                                                         \
    ((obj))->data[4] = (valueOOP);                                             \
  } while (0)

#define OBJ_NAMESPACE_ASSOC(obj) &((obj))->data[5]
#define OBJ_NAMESPACE_AT_ASSOC(obj, index) ((obj))->data[5 + (index)]
#define OBJ_NAMESPACE_AT_PUT_ASSOC(obj, index, valueOOP)                       \
  do {                                                                         \
    ((obj))->data[5 + (index)] = (valueOOP);                                   \
  } while (0)

/* ASSOCIATION */

#define OBJ_ASSOCIATION_GET_KEY(obj) ((obj))->data[0]
#define OBJ_ASSOCIATION_SET_KEY(obj, valueOOP)                                 \
  do {                                                                         \
    ((obj))->data[0] = (valueOOP);                                             \
  } while (0)

#define OBJ_ASSOCIATION_GET_VALUE(obj) ((obj))->data[1]
#define OBJ_ASSOCIATION_SET_VALUE(obj, valueOOP)                               \
  do {                                                                         \
    ((obj))->data[1] = (valueOOP);                                             \
  } while (0)

/* VARIABLE BINDING */

#define OBJ_VARIABLE_BINDING_GET_KEY(obj) ((obj))->data[0]
#define OBJ_VARIABLE_BINDING_SET_KEY(obj, valueOOP)                            \
  do {                                                                         \
    ((obj))->data[0] = (valueOOP);                                             \
  } while (0)

#define OBJ_VARIABLE_BINDING_GET_VALUE(obj) ((obj))->data[1]
#define OBJ_VARIABLE_BINDING_SET_VALUE(obj, valueOOP)                          \
  do {                                                                         \
    ((obj))->data[1] = (valueOOP);                                             \
  } while (0)

#define OBJ_VARIABLE_BINDING_GET_ENVIRONMENT(obj) ((obj))->data[2]
#define OBJ_VARIABLE_BINDING_SET_ENVIRONMENT(obj, valueOOP)                    \
  do {                                                                         \
    ((obj))->data[2] = (valueOOP);                                             \
  } while (0)

/* DEFERRED VARIABLE BINDING */

#define OBJ_DEFERRED_VARIABLE_BINDING_GET_KEY(obj) ((obj))->data[0]
#define OBJ_DEFERRED_VARIABLE_BINDING_SET_KEY(obj, valueOOP)                   \
  do {                                                                         \
    ((obj))->data[0] = (valueOOP);                                             \
  } while (0)

#define OBJ_DEFERRED_VARIABLE_BINDING_GET_CLASS(obj) ((obj))->data[1]
#define OBJ_DEFERRED_VARIABLE_BINDING_SET_CLASS(obj, valueOOP)                 \
  do {                                                                         \
    ((obj))->data[1] = (valueOOP);                                             \
  } while (0)

#define OBJ_DEFERRED_VARIABLE_BINDING_GET_DEFAULT_DICTIONARY(obj)              \
  ((obj))->data[2]
#define OBJ_DEFERRED_VARIABLE_BINDING_SET_DEFAULT_DICTIONARY(obj, valueOOP)    \
  do {                                                                         \
    ((obj))->data[2] = (valueOOP);                                             \
  } while (0)

#define OBJ_DEFERRED_VARIABLE_BINDING_GET_ASSOCIATION(obj) ((obj))->data[3]
#define OBJ_DEFERRED_VARIABLE_BINDING_SET_ASSOCIATION(obj, valueOOP)           \
  do {                                                                         \
    ((obj))->data[3] = (valueOOP);                                             \
  } while (0)

#define OBJ_DEFERRED_VARIABLE_BINDING_GET_PATH(obj) ((obj))->data[4]
#define OBJ_DEFERRED_VARIABLE_BINDING_SET_PATH(obj, valueOOP)                  \
  do {                                                                         \
    ((obj))->data[4] = (valueOOP);                                             \
  } while (0)

/* MESSAGE */

#define OBJ_MESSAGE_GET_SELECTOR(obj) ((obj))->data[0]
#define OBJ_MESSAGE_SET_SELECTOR(obj, valueOOP)                                \
  do {                                                                         \
    ((obj))->data[0] = (valueOOP);                                             \
  } while (0)

#define OBJ_MESSAGE_GET_ARGS(obj) ((obj))->data[1]
#define OBJ_MESSAGE_SET_ARGS(obj, valueOOP)                                    \
  do {                                                                         \
    ((obj))->data[1] = (valueOOP);                                             \
  } while (0)

/* MESSAGE LOOKUP */
#define OBJ_MESSAGE_LOOKUP_GET_SELECTOR(obj) ((obj))->data[0]
#define OBJ_MESSAGE_LOOKUP_SET_SELECTOR(obj, valueOOP)                         \
  do {                                                                         \
    ((obj))->data[0] = (valueOOP);                                             \
  } while (0)

#define OBJ_MESSAGE_LOOKUP_GET_ARGS(obj) ((obj))->data[1]
#define OBJ_MESSAGE_LOOKUP_SET_ARGS(obj, valueOOP)                             \
  do {                                                                         \
    ((obj))->data[1] = (valueOOP);                                             \
  } while (0)

#define OBJ_MESSAGE_LOOKUP_GET_STARTING_CLASS(obj) ((obj))->data[2]
#define OBJ_MESSAGE_LOOKUP_SET_STARTING_CLASS(obj, valueOOP)                   \
  do {                                                                         \
    ((obj))->data[2] = (valueOOP);                                             \
  } while (0)

/* IDENTITY DICTIONARY */

#define OBJ_IDENTITY_DICTIONARY_GET_TALLY(obj) ((obj))->data[0]
#define OBJ_IDENTITY_DICTIONARY_SET_TALLY(obj, valueOOP)                       \
  do {                                                                         \
    ((obj))->data[0] = (valueOOP);                                             \
  } while (0)

/* METHOD DICTIONARY */

#define OBJ_METHOD_DICTIONARY_GET_TALLY(obj) ((obj))->data[0]
#define OBJ_METHOD_DICTIONARY_SET_TALLY(obj, valueOOP)                         \
  do {                                                                         \
    ((obj))->data[0] = (valueOOP);                                             \
  } while (0)

#define OBJ_METHOD_DICTIONARY_GET_KEYS(obj) ((obj))->data[1]
#define OBJ_METHOD_DICTIONARY_SET_KEYS(obj, valueOOP)                          \
  do {                                                                         \
    ((obj))->data[1] = (valueOOP);                                             \
  } while (0)

#define OBJ_METHOD_DICTIONARY_GET_VALUES(obj) ((obj))->data[2]
#define OBJ_METHOD_DICTIONARY_SET_VALUES(obj, valueOOP)                        \
  do {                                                                         \
    ((obj))->data[2] = (valueOOP);                                             \
  } while (0)

/* BEHAVIOR */
#define OBJ_BEHAVIOR_GET_SUPER_CLASS(obj) ((obj))->data[0]
#define OBJ_BEHAVIOR_SET_SUPER_CLASS(obj, valueOOP)                            \
  do {                                                                         \
    ((obj))->data[0] = (valueOOP);                                             \
  } while (0)

#define OBJ_BEHAVIOR_GET_METHOD_DICTIONARY(obj) ((obj))->data[1]
#define OBJ_BEHAVIOR_SET_METHOD_DICTIONARY(obj, valueOOP)                      \
  do {                                                                         \
    ((obj))->data[1] = (valueOOP);                                             \
  } while (0)

#define OBJ_BEHAVIOR_GET_INSTANCE_SPEC(obj) ((obj))->data[2]
#define OBJ_BEHAVIOR_SET_INSTANCE_SPEC(obj, valueOOP)                          \
  do {                                                                         \
    ((obj))->data[2] = (valueOOP);                                             \
  } while (0)

#define OBJ_BEHAVIOR_GET_SUB_CLASSES(obj) ((obj))->data[3]
#define OBJ_BEHAVIOR_SET_SUB_CLASSES(obj, valueOOP)                            \
  do {                                                                         \
    ((obj))->data[3] = (valueOOP);                                             \
  } while (0)

#define OBJ_BEHAVIOR_GET_INSTANCE_VARIABLES(obj) ((obj))->data[4]
#define OBJ_BEHAVIOR_SET_INSTANCE_VARIABLES(obj, valueOOP)                     \
  do {                                                                         \
    ((obj))->data[4] = (valueOOP);                                             \
  } while (0)

/* CLASS */

#define OBJ_CLASS_GET_NAME(obj) ((obj))->data[5]
#define OBJ_CLASS_SET_NAME(obj, valueOOP)                                      \
  do {                                                                         \
    ((obj))->data[5] = (valueOOP);                                             \
  } while (0)

#define OBJ_CLASS_GET_COMMENT(obj) ((obj))->data[6]
#define OBJ_CLASS_SET_COMMENT(obj, valueOOP)                                   \
  do {                                                                         \
    ((obj))->data[6] = (valueOOP);                                             \
  } while (0)

#define OBJ_CLASS_GET_CATEGORY(obj) ((obj))->data[7]
#define OBJ_CLASS_SET_CATEGORY(obj, valueOOP)                                  \
  do {                                                                         \
    ((obj))->data[7] = (valueOOP);                                             \
  } while (0)

#define OBJ_CLASS_GET_ENVIRONMENT(obj) ((obj))->data[8]
#define OBJ_CLASS_SET_ENVIRONMENT(obj, valueOOP)                               \
  do {                                                                         \
    ((obj))->data[8] = (valueOOP);                                             \
  } while (0)

#define OBJ_CLASS_GET_CLASS_VARIABLES(obj) ((obj))->data[9]
#define OBJ_CLASS_SET_CLASS_VARIABLES(obj, valueOOP)                           \
  do {                                                                         \
    ((obj))->data[9] = (valueOOP);                                             \
  } while (0)

#define OBJ_CLASS_GET_SHARED_POOLS(obj) ((obj))->data[10]
#define OBJ_CLASS_SET_SHARED_POOLS(obj, valueOOP)                              \
  do {                                                                         \
    ((obj))->data[10] = (valueOOP);                                            \
  } while (0)

#define OBJ_CLASS_GET_PRAGMA_HANDLERS(obj) ((obj))->data[11]
#define OBJ_CLASS_SET_PRAGMA_HANDLERS(obj, valueOOP)                           \
  do {                                                                         \
    ((obj))->data[11] = (valueOOP);                                            \
  } while (0)

/* META CLASS */

#define OBJ_META_CLASS_GET_INSTANCE_CLASS(obj) ((obj))->data[5]
#define OBJ_META_CLASS_SET_INSTANCE_CLASS(obj, valueOOP)                       \
  do {                                                                         \
    ((obj))->data[5] = (valueOOP);                                             \
  } while (0)

/* COBJECT */
#define OBJ_COBJECT_GET_TYPE(obj) ((obj))->data[0]
#define OBJ_COBJECT_SET_TYPE(obj, valueOOP)                                    \
  do {                                                                         \
    ((obj))->data[0] = (valueOOP);                                             \
  } while (0)

#define OBJ_COBJECT_GET_STORAGE(obj) ((obj))->data[1]
#define OBJ_COBJECT_SET_STORAGE(obj, valueOOP)                                 \
  do {                                                                         \
    ((obj))->data[1] = (valueOOP);                                             \
  } while (0)

/* Answer the offset component of the a CObject, COBJ (*not* an OOP,
   but an object pointer).  */
#define COBJECT_OFFSET_OBJ(cObj)                                               \
  (((uintptr_t *)cObj)[TO_INT(OBJ_SIZE((cObj))) - 1])

/* Sets to VALUE the offset component of the CObject, COBJ (*not* an
   OOP, but an object pointer).  */
#define SET_COBJECT_OFFSET_OBJ(cObj, value)                                    \
  (((uintptr_t *)cObj)[TO_INT(OBJ_SIZE((cObj))) - 1] = (uintptr_t)(value))

/* CTYPE */

#define OBJ_CTYPE_GET_COBJECT_TYPE(obj) ((obj))->data[0]
#define OBJ_CTYPE_SET_COBJECT_TYPE(obj, valueOOP)                              \
  do {                                                                         \
    ((obj))->data[0] = (valueOOP);                                             \
  } while (0)

/* FLOATD */
#define OBJ_FLOATD_GET_VALUE(obj) ((double *)((obj))->data)[0]
#define OBJ_FLOATD_SET_VALUE(obj, valueOOP)                                    \
  do {                                                                         \
    ((double *)((obj))->data)[0] = (valueOOP);                                 \
  } while (0)

/* FLOATE */
#define OBJ_FLOATE_GET_VALUE(obj) ((float *)((obj))->data)[0]
#define OBJ_FLOATE_SET_VALUE(obj, valueOOP)                                    \
  do {                                                                         \
    ((float *)((obj))->data)[0] = (valueOOP);                                  \
  } while (0)

/* FLOATQ */
#define OBJ_FLOATQ_GET_VALUE(obj) ((long double *)((obj))->data)[0]
#define OBJ_FLOATQ_SET_VALUE(obj, valueOOP)                                    \
  do {                                                                         \
    ((long double *)((obj))->data)[0] = (valueOOP);                            \
  } while (0)

/* STRING */
#define OBJ_STRING_GET_CHARS(obj) ((char *)((obj))->data)
#define OBJ_STRING_SET_CHARS(obj, valueOOP)                                    \
  do {                                                                         \
    ((char *)((obj))->data)[0] = (valueOOP);                                   \
  } while (0)

/* Answer a pointer to the first character of STRINGOOP.  */
#define STRING_OOP_CHARS(stringOOP)                                            \
  ((gst_uchar *)(OBJ_STRING_GET_CHARS(OOP_TO_OBJ(stringOOP))))

/* UNICODE STRING */
#define OBJ_UNICODE_STRING_GET_CHARS(obj) ((uint32_t *)((obj))->data)
#define OBJ_UNICODE_STRING_SET_CHARS(obj, i, valueOOP)                         \
  do {                                                                         \
    ((uint32_t *)((obj))->data)[(i)] = (valueOOP);                             \
  } while (0)

/* CHAR */
#define OBJ_CHAR_GET_CODE_POINTS(obj) (((obj))->data)[0]
#define OBJ_CHAR_SET_CODE_POINTS(obj, valueOOP)                                \
  do {                                                                         \
    (((obj))->data)[0] = (valueOOP);                                           \
  } while (0)

/* BYTE ARRAY */
#define OBJ_BYTE_ARRAY_GET_BYTES(obj, i) ((gst_uchar *)((obj))->data)[(i)]
#define OBJ_BYTE_ARRAY_SET_BYTES(obj, i, valueOOP)                             \
  do {                                                                         \
    ((gst_uchar *)((obj))->data)[(i)] = (valueOOP);                            \
  } while (0)

/* FILE STREAM */
#define OBJ_FILE_STREAM_GET_ACCESS(obj) (((obj))->data)[0]
#define OBJ_FILE_STREAM_SET_ACCESS(obj, valueOOP)                              \
  do {                                                                         \
    (((obj))->data)[0] = (valueOOP);                                           \
  } while (0)

#define OBJ_FILE_STREAM_GET_FD(obj) (((obj))->data)[1]
#define OBJ_FILE_STREAM_SET_FD(obj, valueOOP)                                  \
  do {                                                                         \
    (((obj))->data)[1] = (valueOOP);                                           \
  } while (0)

#define OBJ_FILE_STREAM_GET_FILE(obj) (((obj))->data)[2]
#define OBJ_FILE_STREAM_SET_FILE(obj, valueOOP)                                \
  do {                                                                         \
    (((obj))->data)[2] = (valueOOP);                                           \
  } while (0)

#define OBJ_FILE_STREAM_GET_IS_PIPE(obj) (((obj))->data)[3]
#define OBJ_FILE_STREAM_SET_IS_PIPE(obj, valueOOP)                             \
  do {                                                                         \
    (((obj))->data)[3] = (valueOOP);                                           \
  } while (0)

#define OBJ_FILE_STREAM_GET_AT_END(obj) (((obj))->data)[4]
#define OBJ_FILE_STREAM_SET_AT_END(obj, valueOOP)                              \
  do {                                                                         \
    (((obj))->data)[4] = (valueOOP);                                           \
  } while (0)

#define OBJ_FILE_STREAM_GET_PEEK(obj) (((obj))->data)[5]
#define OBJ_FILE_STREAM_SET_PEEK(obj, valueOOP)                                \
  do {                                                                         \
    (((obj))->data)[5] = (valueOOP);                                           \
  } while (0)

#define OBJ_FILE_STREAM_GET_COLLECTION(obj) (((obj))->data)[6]
#define OBJ_FILE_STREAM_SET_COLLECTION(obj, valueOOP)                          \
  do {                                                                         \
    (((obj))->data)[6] = (valueOOP);                                           \
  } while (0)

#define OBJ_FILE_STREAM_GET_PTR(obj) (((obj))->data)[7]
#define OBJ_FILE_STREAM_SET_PTR(obj, valueOOP)                                 \
  do {                                                                         \
    (((obj))->data)[7] = (valueOOP);                                           \
  } while (0)

#define OBJ_FILE_STREAM_GET_END_PTR(obj) (((obj))->data)[8]
#define OBJ_FILE_STREAM_SET_END_PTR(obj, valueOOP)                             \
  do {                                                                         \
    (((obj))->data)[8] = (valueOOP);                                           \
  } while (0)

#define OBJ_FILE_STREAM_GET_WRITE_PTR(obj) (((obj))->data)[9]
#define OBJ_FILE_STREAM_SET_WRITE_PTR(obj, valueOOP)                           \
  do {                                                                         \
    (((obj))->data)[9] = (valueOOP);                                           \
  } while (0)

#define OBJ_FILE_STREAM_GET_WRITE_END(obj) (((obj))->data)[10]
#define OBJ_FILE_STREAM_SET_WRITE_END(obj, valueOOP)                           \
  do {                                                                         \
    (((obj))->data)[10] = (valueOOP);                                          \
  } while (0)

/* PROCESS */
#define OBJ_PROCESS_GET_NEXT_LINK(obj) (((obj))->data)[0]
#define OBJ_PROCESS_SET_NEXT_LINK(obj, valueOOP)                               \
  do {                                                                         \
    (((obj))->data)[0] = (valueOOP);                                           \
  } while (0)

#define OBJ_PROCESS_GET_SUSPENDED_CONTEXT(obj) (((obj))->data)[1]
#define OBJ_PROCESS_SET_SUSPENDED_CONTEXT(obj, valueOOP)                       \
  do {                                                                         \
    (((obj))->data)[1] = (valueOOP);                                           \
  } while (0)

#define OBJ_PROCESS_GET_PRIORITY(obj) (((obj))->data)[2]
#define OBJ_PROCESS_SET_PRIORITY(obj, valueOOP)                                \
  do {                                                                         \
    (((obj))->data)[2] = (valueOOP);                                           \
  } while (0)

#define OBJ_PROCESS_GET_MY_LIST(obj) (((obj))->data)[3]
#define OBJ_PROCESS_SET_MY_LIST(obj, valueOOP)                                 \
  do {                                                                         \
    (((obj))->data)[3] = (valueOOP);                                           \
  } while (0)

#define OBJ_PROCESS_GET_NAME(obj) (((obj))->data)[4]
#define OBJ_PROCESS_SET_NAME(obj, valueOOP)                                    \
  do {                                                                         \
    (((obj))->data)[4] = (valueOOP);                                           \
  } while (0)

#define OBJ_PROCESS_GET_UNWIND_POINTS(obj) (((obj))->data)[5]
#define OBJ_PROCESS_SET_UNWIND_POINTS(obj, valueOOP)                           \
  do {                                                                         \
    (((obj))->data)[5] = (valueOOP);                                           \
  } while (0)

#define OBJ_PROCESS_GET_INTERRUPTS(obj) (((obj))->data)[6]
#define OBJ_PROCESS_SET_INTERRUPTS(obj, valueOOP)                              \
  do {                                                                         \
    (((obj))->data)[6] = (valueOOP);                                           \
  } while (0)

#define OBJ_PROCESS_GET_INTERRUPT_LOCK(obj) (((obj))->data)[7]
#define OBJ_PROCESS_SET_INTERRUPT_LOCK(obj, valueOOP)                          \
  do {                                                                         \
    (((obj))->data)[7] = (valueOOP);                                           \
  } while (0)

#define OBJ_PROCESS_GET_PROCESSOR_SCHEDULER(obj) (((obj))->data)[8]
#define OBJ_PROCESS_SET_PROCESSOR_SCHEDULER(obj, valueOOP)                     \
  do {                                                                         \
    (((obj))->data)[8] = (valueOOP);                                           \
  } while (0)

/* CALLIN PROCESS */
#define OBJ_CALLIN_PROCESS_GET_RETURNED_VALUE(obj) (((obj))->data)[9]
#define OBJ_CALLIN_PROCESS_SET_RETURNED_VALUE(obj, valueOOP)                   \
  do {                                                                         \
    (((obj))->data)[9] = (valueOOP);                                           \
  } while (0)

/* PROCESSOR SCHEDULER */
#define OBJ_PROCESSOR_SCHEDULER_GET_PROCESS_LISTS(obj) (((obj))->data)[0]
#define OBJ_PROCESSOR_SCHEDULER_SET_PROCESS_LISTS(obj, valueOOP)               \
  do {                                                                         \
    (((obj))->data)[0] = (valueOOP);                                           \
  } while (0)

#define OBJ_PROCESSOR_SCHEDULER_GET_ACTIVE_PROCESS(obj) (((obj))->data)[1]
#define OBJ_PROCESSOR_SCHEDULER_SET_ACTIVE_PROCESS(obj, valueOOP)              \
  do {                                                                         \
    (((obj))->data)[1] = (valueOOP);                                           \
  } while (0)

#define OBJ_PROCESSOR_SCHEDULER_GET_IDLE_TASKS(obj) (((obj))->data)[2]
#define OBJ_PROCESSOR_SCHEDULER_SET_IDLE_TASKS(obj, valueOOP)                  \
  do {                                                                         \
    (((obj))->data)[2] = (valueOOP);                                           \
  } while (0)

#define OBJ_PROCESSOR_SCHEDULER_GET_PROCESS_TIME_SLICE(obj) (((obj))->data)[3]
#define OBJ_PROCESSOR_SCHEDULER_SET_PROCESS_TIME_SLICE(obj, valueOOP)          \
  do {                                                                         \
    (((obj))->data)[3] = (valueOOP);                                           \
  } while (0)

#define OBJ_PROCESSOR_SCHEDULER_GET_GC_SEMAPHORE(obj) (((obj))->data)[4]
#define OBJ_PROCESSOR_SCHEDULER_SET_GC_SEMAPHORE(obj, valueOOP)                \
  do {                                                                         \
    (((obj))->data)[4] = (valueOOP);                                           \
  } while (0)

#define OBJ_PROCESSOR_SCHEDULER_GET_GC_ARRAY(obj) (((obj))->data)[5]
#define OBJ_PROCESSOR_SCHEDULER_SET_GC_ARRAY(obj, valueOOP)                    \
  do {                                                                         \
    (((obj))->data)[5] = (valueOOP);                                           \
  } while (0)

#define OBJ_PROCESSOR_SCHEDULER_GET_EVENT_SEMAPHORE(obj) (((obj))->data)[6]
#define OBJ_PROCESSOR_SCHEDULER_SET_EVENT_SEMAPHORE(obj, valueOOP)             \
  do {                                                                         \
    (((obj))->data)[6] = (valueOOP);                                           \
  } while (0)

#define OBJ_PROCESSOR_SCHEDULER_GET_VM_THREAD_ID(obj) (((obj))->data)[7]
#define OBJ_PROCESSOR_SCHEDULER_SET_VM_THREAD_ID(obj, valueOOP)                \
  do {                                                                         \
    (((obj))->data)[7] = (valueOOP);                                           \
  } while (0)

#define OBJ_PROCESSOR_SCHEDULER_GET_LOCK_THREAD_ID(obj) (((obj))->data)[8]
#define OBJ_PROCESSOR_SCHEDULER_SET_LOCK_THREAD_ID(obj, valueOOP)              \
  do {                                                                         \
    (((obj))->data)[8] = (valueOOP);                                           \
  } while (0)

#define OBJ_SIP_HASH_GET_KEY_0(obj) ((obj)->data)[0]
#define OBJ_SIP_HASH_SET_KEY_0(obj, valueOOP)                           \
  do {                                                                  \
    (((obj))->data)[0] = (valueOOP);                                    \
  } while (0)

#define OBJ_SIP_HASH_GET_KEY_1(obj) ((obj)->data)[1]
#define OBJ_SIP_HASH_SET_KEY_1(obj, valueOOP)                           \
  do {                                                                  \
    (((obj))->data)[1] = (valueOOP);                                    \
  } while (0)

/* OSPROCESS */

#define OBJ_OS_PROCESS_GET_PROGRAM(obj) ((obj)->data)[0]
#define OBJ_OS_PROCESS_SET_PROGRAM(obj, valueOOP)                              \
  do {                                                                         \
    ((obj)->data)[0] = (valueOOP);                                             \
  } while (0)

#define OBJ_OS_PROCESS_GET_ARGUMENTS(obj) ((obj)->data)[1]
#define OBJ_OS_PROCESS_SET_ARGUMENTS(obj, valueOOP)                            \
  do {                                                                         \
    ((obj)->data)[1] = (valueOOP);                                             \
  } while (0)

#define OBJ_OS_PROCESS_GET_STDIN(obj) ((obj)->data)[2]
#define OBJ_OS_PROCESS_SET_STDIN(obj, valueOOP)                                \
  do {                                                                         \
    ((obj)->data)[2] = (valueOOP);                                             \
  } while (0)

#define OBJ_OS_PROCESS_GET_STDOUT(obj) ((obj)->data)[3]
#define OBJ_OS_PROCESS_SET_STDOUT(obj, valueOOP)                               \
  do {                                                                         \
    ((obj)->data)[3] = (valueOOP);                                             \
  } while (0)

#define OBJ_OS_PROCESS_GET_STDERR(obj) ((obj)->data)[4]
#define OBJ_OS_PROCESS_SET_STDERR(obj, valueOOP)                               \
  do {                                                                         \
    ((obj)->data)[4] = (valueOOP);                                             \
  } while (0)

#define OBJ_OS_PROCESS_GET_PID(obj) ((obj)->data)[5]
#define OBJ_OS_PROCESS_SET_PID(obj, valueOOP)                                  \
  do {                                                                         \
    ((obj)->data)[5] = (valueOOP);                                             \
  } while (0)

#define OBJ_OS_PROCESS_GET_RETURN_CODE(obj) ((obj)->data)[6]
#define OBJ_OS_PROCESS_SET_RETURN_CODE(obj, valueOOP)                          \
  do {                                                                         \
    ((obj)->data)[6] = (valueOOP);                                             \
  } while (0)

#endif /* GST_OBJECT_POINTER_H */
