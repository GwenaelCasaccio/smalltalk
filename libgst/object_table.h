#ifndef GST_OBJECT_TABLE_HPP
#define GST_OBJECT_TABLE_HPP

#include "object.h"

#include <array>
#include <cstdlib>
#include <functional>
#include <optional>

template<std::size_t N = 1024 * 1024> class ObjectTable {

 public:
  ObjectTable();

  std::optional< std::reference_wrapper<object_s> > alloc() {
    if (nextObjectToBeAllocatedIT == table.end()) {
      return std::nullopt;
    }

    object_s &allocatedObject = *nextObjectToBeAllocatedIT;
    allocatedObject.setAllocatedFlag(allocatedFlag);

    numberAllocatedObject++;

    findFirstFreeObject();

    return std::ref(allocatedObject);
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
    return numberAllocatedObject >= gcAllocatedBarrier;
  }

  void displaySomeStats();

  typename std::array<object_s, N>::iterator begin() {
    return table.begin();
  }

  typename std::array<object_s, N>::iterator end() {
    return table.end();
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

  std::array<object_s, N> table;
  typename std::array<object_s, N>::iterator nextObjectToBeAllocatedIT;
  size_t numberAllocatedObject;
  size_t gcAllocatedBarrier;
  bool allocatedFlag;
};

#endif // GST_OBJECT_TABLE_HPP
