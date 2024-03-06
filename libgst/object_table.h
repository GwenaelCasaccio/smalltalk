#ifndef GST_OBJECT_TABLE_HPP
#define GST_OBJECT_TABLE_HPP

#include "object.h"

#include <array>
#include <cstdlib>

class ObjectTable {

 public:
  ObjectTable();

  object_s &alloc() {
    assert(nextObjectToBeAllocatedIT != table.end());

    object_s &allocatedObject = *nextObjectToBeAllocatedIT;

    numberAllocatedObject++;

    findFirstFreeObject();

    return allocatedObject;
  }

  void mark(object_s &object) {
    object.setAllocatedFlag(allocatedFlag);
  }

  void sweep(object_s &object) {
    object.setAllocatedFlag(allocatedFlag);
  }

  void resetAllocatedFlag() {
    // TODO
  }

  bool shouldLaunchGC() {
    return numberAllocatedObject == gcAllocatedBarrier;
  }

 private:
  bool findFirstFreeObject() {
    while (nextObjectToBeAllocatedIT != table.end()) {
      if ((*nextObjectToBeAllocatedIT).getAllocatedFlag() != allocatedFlag) {
        return true;
      }

      nextObjectToBeAllocatedIT++;
    }

    return false;
  }

  std::array<object_s, 1024 * 1024> table;
  std::array<object_s, 1024 * 1024>::iterator nextObjectToBeAllocatedIT;
  size_t numberAllocatedObject;
  size_t gcAllocatedBarrier;
  uintptr_t allocatedFlag;
};

#endif // GST_OBJECT_TABLE_HPP
