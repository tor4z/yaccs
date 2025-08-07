#ifndef YACCS_UTILS_H_
#define YACCS_UTILS_H_

#include "yaccs/tensor.hpp"
#include "yaccs/ops.hpp"
#include <onnx.pb.h>


void tensor_type_from_onnx(const onnx::TypeProto_Tensor& onnx_tensor, TensorType& tensor_type,
    const std::unordered_map<std::string, int>& dynamic_axes);

void gemm_from_onnx(const onnx::NodeProto& node, const onnx::GraphProto& graph, OpGemm& gemm);
void relu_from_onnx(const onnx::NodeProto& node, OpRelu& relu);
std::string extract_filename(const std::string& path);

#endif // YACCS_UTILS_H_
