#ifndef YACCS_TENSOR_H_
#define YACCS_TENSOR_H_

#include "yaccs/dtype.hpp"
#include <string>

#define MAX_TENSOR_DIMS 6


struct TensorType
{
    int shape[MAX_TENSOR_DIMS];
    std::string name;
    DType dtype;
    int dims;
}; // struct TensorType

#endif // YACCS_TENSOR_H_
