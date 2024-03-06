
#include "object.h"

const size_t C_OBJ_HEADER_SIZE_WORDS = sizeof(gst_object_header_s) / sizeof(ObjectPtr);

std::atomic<uintptr_t> gst_object_header_s::OBJECT_IDENTITY(0);

static_assert(sizeof(struct object_s) == 0x10, "Be carrefull with padding needed by IS_OOP_ADDR");
static_assert(C_OBJ_HEADER_SIZE_WORDS == 3, "Be carrefull when adding new fields in the header take care of context copy and allocation!");

#include "doctest.h"

TEST_CASE("object is an integer") {
  CHECK(!object_s::isInteger(reinterpret_cast<ObjectPtr>(36280)));
  CHECK(object_s::isInteger(reinterpret_cast<ObjectPtr>(36281)));
}

TEST_CASE("object is an object") {
  CHECK(object_s::isObject(reinterpret_cast<ObjectPtr>(36280)));
  CHECK(!object_s::isObject(reinterpret_cast<ObjectPtr>(36281)));
}

TEST_CASE("objects are integers") {
  CHECK(!object_s::areIntegers(reinterpret_cast<ObjectPtr>(36280),
                               reinterpret_cast<ObjectPtr>(36282)));

  CHECK(!object_s::areIntegers(reinterpret_cast<ObjectPtr>(36281),
                               reinterpret_cast<ObjectPtr>(36282)));

  CHECK(object_s::areIntegers(reinterpret_cast<ObjectPtr>(36281),
                              reinterpret_cast<ObjectPtr>(36283)));
}

