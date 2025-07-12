#ifndef YACCS_TENSOR_H_
#define YACCS_TENSOR_H_

#include "yaccs/dtype.hpp"
#include <string>
#include <vector>

#define MAX_TENSOR_DIMS 6


struct TensorType
{
    int shape[MAX_TENSOR_DIMS];
    std::string name;
    DType dtype;
    int dims;
}; // struct TensorType

struct Tensor
{
    TensorType tt;
    std::vector<char> data;
}; // struct Tensor

#endif // YACCS_TENSOR_H_
