#ifndef GST_OBJECT_DATA_ALLOC_H
#define GST_OBJECT_DATA_ALLOC_H

#include "object.h"

ObjectDataPtr alloc(size_t named_slots, Shape shape, size_t indexed_slots) {
  const size_t objectSize = named_slots + getIndexedSlotsSize(shape, indexed_slots);

  uintptr_t *buffer = uintptr_t new[objectSize];
  ObjectDataPtr object = new(buffer) object_data_s;
}

void sweep(ObjectDataPtr object) {
  uintptr_t *buffer = reinterpret_cast<uintptr_t *>(object);
  delete buffer;
}

size_t getIndexedSlotsSize(Shape shape, size_t indexed_slots) {
  const size_t byteSize = shapeToSize[shape] * indexed_slots;

  return (byteSize >> 3) + (byteSize & 0x7 != 0 ? 1 : 0);
}

#endif // GST_OBJECT_DATA_ALLOC_H
