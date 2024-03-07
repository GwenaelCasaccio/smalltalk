#ifndef GST_OBJECT_H
#define GST_OBJECT_H

#include <cassert>
#include <cstdint>
#include <cstddef>
#include <atomic>

enum ObjectShape : std::uint8_t {
  SHAPE_EMTPY,
  SHAPE_OBJECT,
  SHAPE_UINT_8,
  SHAPE_INT_8,
  SHAPE_UINT_16,
  SHAPE_INT_16,
  SHAPE_UINT_32,
  SHAPE_INT_32,
  SHAPE_UINT_64,
  SHAPE_INT_64,
  SHAPE_FLOAT,
  SHAPE_DOUBLE,
  SHAPE_LONG_DOUBLE,
};

typedef struct object_s *ObjectPtr;

typedef struct object_data_s *ObjectDataPtr;

struct object_s
{
  struct object_flags_s {
    uintptr_t allocated: 1  = 0;
    ObjectShape shape: 4    = SHAPE_EMTPY;
    uintptr_t empty: 59     = 0;
  };

  ObjectDataPtr object     = nullptr;
  object_flags_s flags;

  void setAllocatedFlag(bool flag) {
    flags.allocated = flag;
  }

  bool getAllocatedFlag() {
    return flags.allocated;
  }

  void setShape(ObjectShape shape) {
    flags.shape = shape;
  }

  ObjectShape getShape() {
    return flags.shape;
  }

  static bool isInteger(const ObjectPtr object) {
    return (reinterpret_cast<intptr_t>(object) & 1);
  }

  static bool areIntegers(const ObjectPtr object1, const ObjectPtr object2) {
    return (reinterpret_cast<intptr_t>(object1) & reinterpret_cast<intptr_t>(object2) & 1);
  }

  static bool isObject(const ObjectPtr object) {
    return !isInteger(object);
  }
};

struct gst_object_header_s
{
  static std::atomic<uintptr_t> OBJECT_IDENTITY;

  static ObjectPtr allocateObjectIdentity() {
    return reinterpret_cast<ObjectPtr>(OBJECT_IDENTITY+=2);
  }

  static uintptr_t initializeObjectIdentity(uintptr_t objectIdentity) {
    assert(objectIdentity & 1);

    OBJECT_IDENTITY = objectIdentity;
  }

  ObjectPtr objSize;
  ObjectPtr objIdentity;
  ObjectPtr objClass;
};

extern "C" const size_t C_OBJ_HEADER_SIZE_WORDS;

struct object_data_s : gst_object_header_s
{
  ObjectPtr data[0];
};

#endif // GST_OBJECT_H
