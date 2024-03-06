#ifndef GST_OBJECT_H
#define GST_OBJECT_H

#include <cassert>
#include <cstdint>
#include <cstddef>
#include <atomic>

typedef struct object_s *ObjectPtr;

typedef struct object_data_s *ObjectDataPtr;

struct object_s
{
  struct object_flags_s {
    uintptr_t allocated: 1;
    uintptr_t : 63;
  };

  ObjectDataPtr object;
  object_flags_s flags;

  void setAllocatedFlag(bool flag) {
    flags.allocated = flag;
  }

  bool getAllocatedFlag() {
    return flags.allocated;
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
