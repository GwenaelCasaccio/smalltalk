#ifndef GST_OBJECT_POINTER_H
#define GST_OBJECT_POINTER_H

#define OBJ_ARRAY_AT(obj, n) (((OOP *)((gst_object)obj)->data)[(n)-1])
#define OBJ_STRING_AT(obj, n) (((char *)((gst_object)obj)->data)[(n)-1])

#define OBJ_SIZE(obj) ((obj)->objSize)

#define OBJ_SET_SIZE(obj, valueOOP)                                            \
  do {                                                                         \
    (obj)->objSize = (valueOOP);                                               \
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

#define OBJ_SEMAPHORE_GET_SIGNALS(obj) ((obj))->data[2]
#define OBJ_SEMAPHORE_SET_SIGNALS(obj, valueOOP)                               \
  do {                                                                         \
    ((obj))->data[2] = (valueOOP);                                             \
  } while (0)

#define OBJ_SEMAPHORE_GET_NAME(obj) ((obj))->data[3]

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
#define OBJ_DEFERRED_VARIABLE_BINDING_SET_KEY(obj, valueOOP)  \
  do {                                                        \
    ((obj))->data[0] = (valueOOP);                            \
  } while (0)

#define OBJ_DEFERRED_VARIABLE_BINDING_GET_CLASS(obj) ((obj))->data[1]
#define OBJ_DEFERRED_VARIABLE_BINDING_SET_CLASS(obj, valueOOP)  \
  do {                                                        \
    ((obj))->data[1] = (valueOOP);                            \
  } while (0)

#define OBJ_DEFERRED_VARIABLE_BINDING_GET_DEFAULT_DICTIONARY(obj) ((obj))->data[2]
#define OBJ_DEFERRED_VARIABLE_BINDING_SET_DEFAULT_DICTIONARY(obj, valueOOP) \
  do {                                                                  \
    ((obj))->data[2] = (valueOOP);                                      \
  } while (0)

#define OBJ_DEFERRED_VARIABLE_BINDING_GET_ASSOCIATION(obj) ((obj))->data[3]
#define OBJ_DEFERRED_VARIABLE_BINDING_SET_ASSOCIATION(obj, valueOOP)    \
  do {                                                                  \
    ((obj))->data[3] = (valueOOP);                                      \
  } while (0)

#define OBJ_DEFERRED_VARIABLE_BINDING_GET_PATH(obj) ((obj))->data[4]
#define OBJ_DEFERRED_VARIABLE_BINDING_SET_PATH(obj, valueOOP)         \
  do {                                                                \
    ((obj))->data[4] = (valueOOP);                                    \
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
#define OBJ_COBJECT_SET_TYPE(obj, valueOOP) \
  do {                                            \
    ((obj))->data[0] = (valueOOP);                \
  } while (0)

#define OBJ_COBJECT_GET_STORAGE(obj) ((obj))->data[1]
#define OBJ_COBJECT_SET_STORAGE(obj, valueOOP)     \
  do {                                          \
    ((obj))->data[1] = (valueOOP);              \
  } while (0)

/* Answer the offset component of the a CObject, COBJ (*not* an OOP,
   but an object pointer).  */
#define COBJECT_OFFSET_OBJ(cObj)                      \
  (((uintptr_t *)cObj)[TO_INT(OBJ_SIZE((cObj))) - 1])

/* Sets to VALUE the offset component of the CObject, COBJ (*not* an
   OOP, but an object pointer).  */
#define SET_COBJECT_OFFSET_OBJ(cObj, value)                             \
  (((uintptr_t *)cObj)[TO_INT(OBJ_SIZE((cObj))) - 1] = (uintptr_t)(value))


/* CTYPE */

#define OBJ_CTYPE_GET_COBJECT_TYPE(obj) ((obj))->data[0]
#define OBJ_CTYPE_SET_COBJECT_TYPE(obj, valueOOP)         \
  do {                                                    \
    ((obj))->data[0] = (valueOOP);                        \
  } while (0)

/* FLOATD */
#define OBJ_FLOATD_GET_VALUE(obj) ((double *)((obj))->data)[0]
#define OBJ_FLOATD_SET_VALUE(obj, valueOOP)          \
  do {                                               \
    ((double *)((obj))->data)[0] = (valueOOP);        \
  } while (0)

#endif /* GST_OBJECT_POINTER_H */
