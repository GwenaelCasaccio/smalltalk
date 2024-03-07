#ifndef GST_ALLOC_NEW_H
#define GST_ALLOC_NEW_H

#include "object.h"

#include <cstdint>
#include <optional>

std::optional<ObjectDataPtr> alloc_object_data_new_gen(std::size_t slots, std::size_t anonymousSlots, ObjectShape shape);

std::optional<ObjectDataPtr> alloc_object_data_old_gen(std::size_t slots, std::size_t anonymousSlots, ObjectShape shape);

std::optional<ObjectDataPtr> alloc_object_data_static_gen(std::size_t slots, std::size_t anonymousSlots, ObjectShape shape);

#endif // GST_ALLOC_NEW_H

