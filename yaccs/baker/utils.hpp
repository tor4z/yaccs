#ifndef YACCS_BAKER_UTILS_H_
#define YACCS_BAKER_UTILS_H_

#include "yaccs/baker/def.hpp"
#include "yaccs/tensor.hpp"
#include <cstdint>

id_t alloc_id();
uint32_t shape_to_dsize(int dims, Shape shape);

#endif // YACCS_BAKER_UTILS_H_
