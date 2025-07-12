#ifndef YACCS_UTILS_H_
#define YACCS_UTILS_H_

#include "yaccs/tensor.hpp"
#include <onnx.pb.h>

void tensor_type_from_onnx(const onnx::TypeProto_Tensor& onnx_tensor, TensorType& tensor_type,
    const std::unordered_map<std::string, int>& dynamic_axes);

#endif // YACCS_UTILS_H_
