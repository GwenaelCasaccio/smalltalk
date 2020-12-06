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

#endif /* GST_OBJECT_POINTER_H */
