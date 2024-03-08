#include "alloc_new.h"

#include <iostream>
#include <ostream>

static uintptr_t *buffer_a = nullptr;
static uintptr_t *buffer_b = nullptr;
static uintptr_t *buffer_limit_a = nullptr;
static uintptr_t *buffer_limit_b = nullptr;
static uintptr_t *current_buffer_position = nullptr;

void initialize_new_generation_buffer(std::size_t size) {
  buffer_a = new uintptr_t[size];
  buffer_b = new uintptr_t[size];

  buffer_limit_a = buffer_a + size;
  buffer_limit_b = buffer_b + size;

  current_buffer_position = buffer_a;
}

void free_new_generation_buffer() {
  delete[] buffer_a;
  delete[] buffer_b;

  buffer_a = nullptr;
  buffer_b = nullptr;
  buffer_limit_a = nullptr;
  buffer_limit_b = nullptr;
  current_buffer_position = nullptr;
}

std::optional<ObjectDataPtr> alloc_object_data_new_gen(ObjectPtr object, std::size_t slots, std::size_t indexedSlots, ObjectShape shape) {
  assert((shape == SHAPE_EMTPY && indexedSlots == 0) || (shape != SHAPE_EMTPY));
  assert(object != nullptr);
  assert(slots < 0b10000);
  assert(indexedSlots < 0b1000000000000000000000000);

  std::size_t objectDataSize = slots + (sizeof(gst_object_header_s) / sizeof(void *));

  if (current_buffer_position + objectDataSize > buffer_limit_a) {
    return std::nullopt;
  }

  ObjectDataPtr data = new(current_buffer_position) object_data_s;
  current_buffer_position+=objectDataSize;

  object->flags.generation = NEW_GENERATION;
  object->flags.slots = slots;

  return {data};
}

std::optional<ObjectDataPtr> alloc_object_data_old_gen(ObjectPtr object, std::size_t slots, std::size_t indexedSlots, ObjectShape shape) {
  assert((shape == SHAPE_EMTPY && indexedSlots == 0) || (shape != SHAPE_EMTPY));
  assert(object != nullptr);
  assert(slots < 0b10000);
  assert(indexedSlots < 0b1000000000000000000000000);

  object->flags.generation = OLD_GENERATION;
  object->flags.slots = slots;

  return {};
}

std::optional<ObjectDataPtr> alloc_object_data_static_gen(ObjectPtr object, std::size_t slots, std::size_t indexedSlots, ObjectShape shape) {
  assert((shape == SHAPE_EMTPY && indexedSlots == 0) || (shape != SHAPE_EMTPY));
  assert(object != nullptr);
  assert(slots < 0b10000);
  assert(indexedSlots < 0b1000000000000000000000000);

  object->flags.generation = FIXED_GENERATION;
  object->flags.slots = slots;

  return {};
}

#include "doctest.h"

TEST_CASE("initialize new generation") {
  CHECK(buffer_a == nullptr);
  CHECK(buffer_b == nullptr);
  CHECK(buffer_limit_a == nullptr);
  CHECK(buffer_limit_b == nullptr);
  CHECK(current_buffer_position == nullptr);

  initialize_new_generation_buffer(10);

  CHECK(buffer_a != nullptr);
  CHECK(buffer_b != nullptr);
  CHECK(buffer_limit_a > buffer_a);
  CHECK(buffer_limit_b > buffer_b);
  CHECK(buffer_a == current_buffer_position);

  free_new_generation_buffer();

  CHECK(buffer_a == nullptr);
  CHECK(buffer_b == nullptr);
  CHECK(buffer_limit_a == nullptr);
  CHECK(buffer_limit_b == nullptr);
  CHECK(current_buffer_position == nullptr);
}

TEST_CASE("new generation allocation") {
  initialize_new_generation_buffer(10);

  object_s object;
  std::optional<ObjectDataPtr> optObjectData = alloc_object_data_new_gen(&object, 5, 0, SHAPE_EMTPY);

  CHECK(optObjectData.has_value());

  ObjectDataPtr object_data = optObjectData.value();

  CHECK(reinterpret_cast<uintptr_t>(object_data) == reinterpret_cast<uintptr_t>(buffer_a));
  CHECK(object.getAllocatedFlag() == 0);
  CHECK(object.getGeneration() == NEW_GENERATION);
  CHECK(object.getSlots() == 5);
  CHECK(object.getIndexedSlots() == 0);
  CHECK(object.getShape() == SHAPE_EMTPY);

  free_new_generation_buffer();
}

TEST_CASE("new generation limit allocation") {
  initialize_new_generation_buffer(10);

  object_s object;
  std::optional<ObjectDataPtr> optObjectData = alloc_object_data_new_gen(&object, 7, 0, SHAPE_EMTPY);

  CHECK(optObjectData.has_value());

  ObjectDataPtr object_data = optObjectData.value();

  CHECK(reinterpret_cast<uintptr_t>(object_data) == reinterpret_cast<uintptr_t>(buffer_a));
  CHECK(object.getAllocatedFlag() == 0);

  free_new_generation_buffer();
}

TEST_CASE("new generation too big allocation") {
  initialize_new_generation_buffer(10);

  object_s object;
  std::optional<ObjectDataPtr> optObjectData = alloc_object_data_new_gen(&object, 500, 0, SHAPE_EMTPY);

  uintptr_t *copy_buffer_a = buffer_a;

  CHECK(!optObjectData.has_value());

  CHECK(copy_buffer_a == buffer_a);

  free_new_generation_buffer();
}

