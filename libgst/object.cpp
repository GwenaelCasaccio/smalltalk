
#include "object.h"

const size_t C_OBJ_HEADER_SIZE_WORDS = sizeof(gst_object_header_s) / sizeof(ObjectPtr);

std::atomic<uintptr_t> gst_object_header_s::OBJECT_IDENTITY(0);

static_assert(sizeof(struct object_s) == 0x10, "Be carrefull with padding needed by IS_OOP_ADDR");
static_assert(C_OBJ_HEADER_SIZE_WORDS == 3, "Be carrefull when adding new fields in the header take care of context copy and allocation!");

#include "doctest.h"

TEST_CASE("testing object") {
  CHECK(1 == 3628800);
}

