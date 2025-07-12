#include "yaccs/utils.hpp"

void tensor_type_from_onnx(const onnx::TypeProto_Tensor& onnx_tensor, TensorType& tensor_type,
    const std::unordered_map<std::string, int>& dynamic_axes)
{
    tensor_type.dims = onnx_tensor.shape().dim_size();
    tensor_type.dtype = static_cast<DType>(onnx_tensor.elem_type());
    for (int i = 0; i < tensor_type.dims; ++i) {
        const auto& dim{onnx_tensor.shape().dim().Get(i)};
        if (!dim.dim_param().empty()) {
            assert(dynamic_axes.find(dim.dim_param()) != dynamic_axes.end() && "Input param not defined");
            tensor_type.shape[i] = dynamic_axes.at(dim.dim_param());
        } else {
            tensor_type.shape[i] = dim.dim_value();
        }
    }
}
