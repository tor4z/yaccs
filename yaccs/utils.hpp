#ifndef YACCS_UTILS_H_
#define YACCS_UTILS_H_

#include "yaccs/tensor.hpp"
#include "yaccs/ops.hpp"
#include <onnx.pb.h>

#define DEF_SINGLETON(classname)                                                \
public:                                                                         \
    static inline classname* instance()                                         \
    {                                                                           \
        static classname *instance_ = nullptr;                                  \
        static std::once_flag flag;                                             \
        if (!instance_) {                                                       \
            std::call_once(flag, [&](){                                         \
                instance_ = new (std::nothrow) classname();                     \
            });                                                                 \
        }                                                                       \
        return instance_;                                                       \
    }                                                                           \
private:                                                                        \
    classname(const classname&) = delete;                                       \
    classname& operator=(const classname&) = delete;                            \
    classname(const classname&&) = delete;                                      \
    classname& operator=(const classname&&) = delete;


void tensor_type_from_onnx(const onnx::TypeProto_Tensor& onnx_tensor, TensorType& tensor_type,
    const std::unordered_map<std::string, int>& dynamic_axes);

void gemm_from_onnx(const onnx::NodeProto& node, const onnx::GraphProto& graph, OpGemm& gemm);
void relu_from_onnx(const onnx::NodeProto& node, OpRelu& relu);

std::string extract_filename(const std::string& path);
bool invoke_spirv_as(const std::string& spvasm, const std::string& out_file);
bool remove_file(const std::string& filename);

#endif // YACCS_UTILS_H_
