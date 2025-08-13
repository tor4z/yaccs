#ifndef YACCS_ONNX_PARSER_H_
#define YACCS_ONNX_PARSER_H_

#include "yaccs/tensor.hpp"
#include "yaccs/onnx/ops.hpp"
#include <onnx.pb.h>


void tensor_type_from_onnx(const onnx::TypeProto_Tensor& onnx_tensor, TensorType& tensor_type,
    const std::unordered_map<std::string, int>& dynamic_axes);

void gemm_from_onnx(const onnx::NodeProto& node, const onnx::GraphProto& graph, OpGemm& gemm);
void relu_from_onnx(const onnx::NodeProto& node, OpRelu& relu);

#endif // YACCS_ONNX_PARSER_H_
