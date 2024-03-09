#include "alloc_new.h"

#include <cstring>
#include <iostream>
#include <ostream>
#include <set>
#include <vector>

static uintptr_t *src_buffer = nullptr;
static uintptr_t *dst_buffer = nullptr;

static uintptr_t *src_buffer_limit = nullptr;
static uintptr_t *dst_buffer_limit = nullptr;

static uintptr_t *current_buffer_position = nullptr;

void initialize_new_generation_buffer(std::size_t size) {
  assert(size > 0);

  src_buffer = new uintptr_t[size];
  src_buffer_limit = src_buffer + size;
  current_buffer_position = src_buffer;

  dst_buffer = new uintptr_t[size];
  dst_buffer_limit = dst_buffer + size;
}

void free_new_generation_buffer() {
  delete[] src_buffer;
  delete[] dst_buffer;

  src_buffer = nullptr;
  dst_buffer = nullptr;

  src_buffer_limit = nullptr;
  dst_buffer_limit = nullptr;

  current_buffer_position = nullptr;
}

std::optional<ObjectDataPtr> alloc_object_data_new_gen(ObjectPtr object, std::size_t slots, std::size_t indexedSlots, ObjectShape shape) {
  assert((shape == SHAPE_EMTPY && indexedSlots == 0) || (shape != SHAPE_EMTPY));
  assert(object != nullptr);
  assert(slots < 0b10000);
  assert(indexedSlots < 0b1000000000000000000000000);

  std::size_t objectDataSize = slots + (sizeof(gst_object_header_s) / sizeof(void *));

  if (current_buffer_position + objectDataSize > src_buffer_limit) {
    return std::nullopt;
  }

  ObjectDataPtr data = new(current_buffer_position) object_data_s;
  current_buffer_position+=objectDataSize;

  object->flags.generation = NEW_GENERATION;
  object->flags.slots = slots;
  object->object = data;

  return {data};
}

void copy_garbage_collector(uintptr_t *from_buffer, uintptr_t *dest_buffer, std::set<ObjectPtr> &intergenerational_pointers, std::vector<ObjectPtr> &queue) {
  assert(intergenerational_pointers.size() == queue.size());

  uintptr_t *dest_buffer_it = dest_buffer;

  while (!queue.empty()) {
    ObjectPtr object = queue.back();
    queue.pop_back();

    {
      ObjectPtr klass = object->object->objClass;
      const ObjectGeneration generation = klass->getGeneration();

      if (generation == NEW_GENERATION) {
        const size_t klass_size = klass->getSlots();
        std::memcpy(klass->object, dest_buffer_it, klass_size);
        klass->object = reinterpret_cast<ObjectDataPtr>(dest_buffer_it);
        klass->setGeneration(NEW_GENERATION_TENURED);
        dest_buffer_it+=klass_size;

        auto res = intergenerational_pointers.insert(klass);
        if (res.second) {
          queue.push_back(klass);
        }
      } else if (generation == NEW_GENERATION_TENURED) {
        // TENURE TO OLD GENERATION
        std::abort();
      }

    }

    for (size_t i = 0; i < object->getSlots(); i++) {
      ObjectPtr nested = object->object->data[i];
      const ObjectGeneration generation = nested->getGeneration();

      if (generation == NEW_GENERATION) {
        const size_t object_size = nested->getSlots();
        std::memcpy(nested->object, dest_buffer_it, object_size);
        object->object = reinterpret_cast<ObjectDataPtr>(dest_buffer_it);
        nested->setGeneration(NEW_GENERATION_TENURED);
        dest_buffer_it+=object_size;

        auto res = intergenerational_pointers.insert(nested);
        if (res.second) {
          queue.push_back(nested);
        }
      } else if (generation == NEW_GENERATION_TENURED) {
        // TENURE TO OLD GENERATION
        std::abort();
      }

    }
  }

  uintptr_t *tmp_buffer = src_buffer;
  src_buffer = dst_buffer;
  dst_buffer = tmp_buffer;

  uintptr_t *tmp_buffer_limit = src_buffer_limit;
  src_buffer_limit = dst_buffer_limit;
  dst_buffer_limit = tmp_buffer_limit;

  current_buffer_position = dest_buffer_it;
}

std::optional<ObjectDataPtr> alloc_object_data_old_gen(ObjectPtr object, std::size_t slots, std::size_t indexedSlots, ObjectShape shape) {
  assert((shape == SHAPE_EMTPY && indexedSlots == 0) || (shape != SHAPE_EMTPY));
  assert(object != nullptr);
  assert(slots < 0b10000);
  assert(indexedSlots < 0b1000000000000000000000000);

  object->flags.generation = OLD_GENERATION;
  object->flags.slots = slots;
  // object->object = data;

  return {};
}

std::optional<ObjectDataPtr> alloc_object_data_static_gen(ObjectPtr object, std::size_t slots, std::size_t indexedSlots, ObjectShape shape) {
  assert((shape == SHAPE_EMTPY && indexedSlots == 0) || (shape != SHAPE_EMTPY));
  assert(object != nullptr);
  assert(slots < 0b10000);
  assert(indexedSlots < 0b1000000000000000000000000);

  object->flags.generation = FIXED_GENERATION;
  object->flags.slots = slots;
  // object->object = data;

  return {};
}

#include "doctest.h"

TEST_CASE("initialize new generation") {
  CHECK(src_buffer == nullptr);
  CHECK(dst_buffer == nullptr);
  CHECK(src_buffer_limit == nullptr);
  CHECK(dst_buffer_limit == nullptr);
  CHECK(current_buffer_position == nullptr);

  initialize_new_generation_buffer(10);

  CHECK(src_buffer != nullptr);
  CHECK(dst_buffer != nullptr);
  CHECK(src_buffer_limit > src_buffer);
  CHECK(dst_buffer_limit > dst_buffer);
  CHECK(src_buffer == current_buffer_position);

  free_new_generation_buffer();

  CHECK(src_buffer == nullptr);
  CHECK(dst_buffer == nullptr);
  CHECK(src_buffer_limit == nullptr);
  CHECK(dst_buffer_limit == nullptr);
  CHECK(current_buffer_position == nullptr);
}

TEST_CASE("new generation allocation") {
  initialize_new_generation_buffer(10);

  object_s object;
  std::optional<ObjectDataPtr> optObjectData = alloc_object_data_new_gen(&object, 5, 0, SHAPE_EMTPY);

  CHECK(optObjectData.has_value());

  ObjectDataPtr object_data = optObjectData.value();

  CHECK(reinterpret_cast<uintptr_t>(object_data) == reinterpret_cast<uintptr_t>(src_buffer));
  CHECK(object.getAllocatedFlag() == 0);
  CHECK(object.getGeneration() == NEW_GENERATION);
  CHECK(object.getSlots() == 5);
  CHECK(object.getIndexedSlots() == 0);
  CHECK(object.getShape() == SHAPE_EMTPY);
  CHECK(object.object == object_data);

  free_new_generation_buffer();
}

TEST_CASE("new generation limit allocation") {
  initialize_new_generation_buffer(10);

  object_s object;
  std::optional<ObjectDataPtr> optObjectData = alloc_object_data_new_gen(&object, 7, 0, SHAPE_EMTPY);

  CHECK(optObjectData.has_value());

  ObjectDataPtr object_data = optObjectData.value();

  CHECK(reinterpret_cast<uintptr_t>(object_data) == reinterpret_cast<uintptr_t>(src_buffer));
  CHECK(object.getAllocatedFlag() == 0);

  free_new_generation_buffer();
}

TEST_CASE("new generation too big allocation") {
  initialize_new_generation_buffer(10);

  object_s object;
  std::optional<ObjectDataPtr> optObjectData = alloc_object_data_new_gen(&object, 10, 0, SHAPE_EMTPY);

  uintptr_t *copy_src_buffer = src_buffer;

  CHECK(!optObjectData.has_value());

  CHECK(copy_src_buffer == src_buffer);

  free_new_generation_buffer();
}

