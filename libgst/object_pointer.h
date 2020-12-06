#ifndef GST_OBJECT_POINTER_H
#define GST_OBJECT_POINTER_H

#define OBJ_INDEXED_WORD(obj, n)   ( ((long *) ((obj) + 1))[(n)-1] )
#define OBJ_INDEXED_BYTE(obj, n)   ( ((char *) ((obj) + 1))[(n)-1] )
#define OBJ_INDEXED_OBJECT(obj, n)    ( ((OOP *) ((obj) + 1))[(n)-1] )
#define OBJ_ARRAY_AT(obj, n)   ( ((OOP *) ((gst_object) obj)->data)[(n)-1] )
#define OBJ_STRING_AT(obj, n)  ( ((char *) ((gst_object) obj)->data)[(n)-1] )

#define OBJ_CLASS(obj) ((obj)->objClass)

#define OBJ_SET_CLASS(obj, valueOOP) do { \
  (obj)->objClass = (valueOOP);           \
} while (0)

/* METHOD CONTEXT */

#define OBJ_METHOD_CONTEXT_PARENT_CONTEXT(obj) ((obj)->data[0])
#define OBJ_METHOD_CONTEXT_NATIVE_IP(obj) ((obj)->data[1])
#define OBJ_METHOD_CONTEXT_IP_OFFSET(obj) ((obj)->data[2])
#define OBJ_METHOD_CONTEXT_SP_OFFSET(obj) ((obj)->data[3])
#define OBJ_METHOD_CONTEXT_RECEIVER(obj) ((obj)->data[4])
#define OBJ_METHOD_CONTEXT_METHOD(obj) ((obj)->data[5])
#define OBJ_METHOD_CONTEXT_FLAGS(obj) ((obj)->data[6])
#define OBJ_METHOD_CONTEXT_CONTEXT_STACK(obj) (&(obj)->data[7])

#endif /* GST_OBJECT_POINTER_H */
