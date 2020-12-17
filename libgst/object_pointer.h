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

#define OBJ_BLOCK_CONTEXT_OUTER_CONTEXT(obj) ((obj)->data[6])

#define OBJ_BLOCK_CONTEXT_SET_OUTER_CONTEXT(obj)                               \
  do {                                                                         \
    (obj)->data[6] = (valueOOP);                                               \
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
#define OBJ_DICTIONARY_SET_TALLY(obj, valueOOP) do { ((obj))->data[0] = (valueOOP); } while(0)

#define OBJ_BINDING_DICTIONARY_GET_TALLY(obj) ((obj))->data[0]
#define OBJ_BINDING_DICTIONARY_SET_TALLY(obj, valueOOP) do { ((obj))->data[0] = (valueOOP); } while(0)

#define OBJ_BINDING_DICTIONARY_GET_ENVIRONMENT(obj) ((obj))->data[1]
#define OBJ_BINDING_DICTIONARY_SET_ENVIRONMENT(obj, valueOOP) do { ((obj))->data[1] = (valueOOP); } while(0)

#define OBJ_BINDING_DICTIONARY_AT_ASSOC(obj, index) ((obj))->data[2 + (index)]
#define OBJ_BINDING_DICTIONARY_AT_PUT_ASSOC(obj, index, valueOOP) do { ((obj))->data[2 + (index)] = (valueOOP); } while(0)

#endif /* GST_OBJECT_POINTER_H */
