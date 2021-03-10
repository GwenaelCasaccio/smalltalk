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

_Static_assert(sizeof(struct oop_s) == 0x10, "Be carrefull with padding needed by IS_OOP_ADDR");

/* Convert an OOP (indirect pointer to an object) to the real object
   data.  */
#define OOP_TO_OBJ(oop) \
  ((oop)->object)

/* Retrieve the class for the object pointed to by OOP.  OOP must be
   a real pointer, not a SmallInteger.  */
#define OOP_CLASS(oop) \
  (OBJ_CLASS (OOP_TO_OBJ((oop))))

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

typedef struct forward_object_allocator_s {
  size_t free_oops;
} forward_object_allocator_t;

/* Initialize an OOP table of SIZE bytes, trying at the given address if
   possible.  Initially, all the OOPs are on the free list so that's
   just how we initialize them.  We do as much initialization as we can,
   but we're called before classses are defined, so things that have
   definite classes must wait until the classes are defined.  */
extern void _gst_init_oop_table(PTR address, size_t size);

/* Allocates a table for OOPs of SIZE bytes, and store pointers to the
   builtin OOPs into _gst_nil_oop et al.  */
extern void _gst_alloc_oop_table(size_t size);

/* Grow the OOP table to NEWSIZE pointers and initialize the newly
   created pointers.  */
extern mst_Boolean _gst_realloc_oop_table(size_t newSize);

/* Dump the entire contents of the OOP table.  Mainly for debugging
   purposes.  */
extern void _gst_dump_oop_table();

extern void _gst_dump_owners(OOP oop);

#endif /* GST_FORWARD_OBJECT_H */
