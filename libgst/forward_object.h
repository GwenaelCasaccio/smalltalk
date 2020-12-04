#ifndef GST_FORWARD_OBJECT_H
#define GST_FORWARD_OBJECT_H

#include <stdint.h>

/* An indirect pointer to object data.  */
typedef struct oop_s *OOP;

/* A direct pointer to the object data.  */
typedef struct object_s *gst_object, *mst_Object;

/* The contents of an indirect pointer to object data.  */
struct oop_s
{
  gst_object object;
  uintptr_t flags;
};

/* Convert an OOP (indirect pointer to an object) to the real object
   data.  */
#define OOP_TO_OBJ(oop) \
  ((oop)->object)

/* Retrieve the class for the object pointed to by OOP.  OOP must be
   a real pointer, not a SmallInteger.  */
#define OOP_CLASS(oop) \
  (OOP_TO_OBJ(oop)->objClass)

/* Set the indirect object pointer OOP to point to OBJ.  */
#define OOP_SET_OBJECT(oop, obj) do {				\
  (oop)->object = (gst_object) (obj);				\
} while(0)

#define OOP_GET_FLAGS(oop) \
  ((oop)->flags)

#define OOP_SET_FLAGS(oop, value) do {	\
  (oop)->flags = (value);               \
} while(0)

#define OOP_NEXT(oop) \
  (oop)++

#define OOP_PREV(oop) \
  (oop)--

#endif /* GST_FORWARD_OBJECT_H */
