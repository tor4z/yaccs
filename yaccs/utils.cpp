#include "yaccs/utils.hpp"
#include "yaccs/dtype.hpp"
#include <cassert>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>

std::string as_identifier(const std::string& name)
{
    std::string id{name};
    for (size_t i = 0; i < id.size(); ++i) {
        char c{id.at(i)};
        if (c == '/' || c == '.') {
            id.at(i) = '_';
        }
    }
    return id;
}

void tensor_type_from_onnx(const onnx::TypeProto_Tensor& onnx_tensor, TensorType& tensor_type,
    const std::unordered_map<std::string, int>& dynamic_axes)
{
    tensor_type.dims = onnx_tensor.shape().dim_size();
    tensor_type.dtype = static_cast<DType>(onnx_tensor.elem_type());
    tensor_type.row_major = true;
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

void tensor_from_onnx(const onnx::TensorProto& pb_tensor, Tensor* tensor)
{
    tensor->tt.name = pb_tensor.name();
    tensor->tt.dtype = static_cast<DType>(pb_tensor.data_type());
    tensor->tt.dims = pb_tensor.dims().size();
    tensor->tt.row_major = true;
    for (int i = 0; i < tensor->tt.dims; ++i) {
        tensor->tt.shape[i] = pb_tensor.dims().at(i);
    }
    tensor->data.resize(pb_tensor.raw_data().size());
    memcpy(tensor->data.data(), pb_tensor.raw_data().data(), tensor->data.size());
}

void gemm_from_onnx(const onnx::NodeProto& node, const onnx::GraphProto& graph, OpGemm& gemm)
{
    assert(node.op_type().compare("Gemm") == 0 && "Not matched operator for Gemm");

    gemm.name = node.name();
    gemm.op_type = node.op_type();

    // Setup default attribue. ref: Gemm specification
    gemm.alpha = 1.0f;
    gemm.beta = 1.0f;
    gemm.trans_a = 0;
    gemm.trans_b = 0;
    for (const auto& attr : node.attribute()) {
        if (attr.name().compare("alpha") == 0) {
            gemm.alpha = attr.f();
        } else if (attr.name().compare("beta") == 0) {
            gemm.beta = attr.f();
        } else if (attr.name().compare("transA") == 0) {
            gemm.trans_a = attr.i();
        } else if (attr.name().compare("transB") == 0) {
            gemm.trans_b = attr.i();
        } else {
            assert(false && "Unrecognized attribute for Gemm");
        }
    }

    const int num_inputs{3};
    int idx{0};
    Tensor* tensors[num_inputs]{&gemm.A, &gemm.B, &gemm.C};

    for (const auto& input: node.input()) {
        tensors[idx]->tt.name = input;
        for (const auto& it : graph.initializer()) {
            if (input.compare(it.name()) == 0) {
                tensor_from_onnx(it, tensors[idx]);
                break;
            }
        }
        ++idx;
    }

    assert(node.output().size() == 1 && "Bad num of output for Gemm");
    gemm.Y.tt.name = node.output().at(0);
    gemm.Y.tt.dtype = gemm.B.tt.dtype;
    gemm.Y.tt.dims = gemm.B.tt.dims;
    gemm.Y.tt.shape[0] = gemm.B.tt.shape[0];
    if (gemm.trans_b) {
        gemm.Y.tt.shape[1] = gemm.B.tt.shape[1];
    } else {
        gemm.Y.tt.shape[1] = gemm.B.tt.shape[0];
    }
}

void relu_from_onnx(const onnx::NodeProto& node, OpRelu& relu)
{
    assert(node.op_type().compare("Relu") == 0 && "Not matched operator for Relu");
    assert(node.input().size() == 1 && "Bad num of input for Relu");
    assert(node.output().size() == 1 && "Bad num of output for Relu");

    relu.name = node.name();
    relu.op_type = node.op_type();
    relu.X.tt.name = node.input().at(0);
    relu.Y.tt.name = node.output().at(0);
}

std::string extract_filename(const std::string& path)
{
    auto last_slash_pos{path.rfind("/")};

    if (last_slash_pos == path.back()) {
        last_slash_pos = 0;
    } else {
        ++last_slash_pos;
    }

    auto filename_ext{path.substr(last_slash_pos, path.back())};
    auto last_dot_pos{filename_ext.rfind(".")};

    if (last_dot_pos == path.back()) {
        return filename_ext;
    } else {
        return filename_ext.substr(0, last_dot_pos);
    }
}

bool invoke_spirv_as(const std::string& spvasm, const std::string& out_file)
{
    auto child_pid{fork()};
    if (child_pid == 0) {
        // child process
        execlp("spirv-as", "spirv-as", "-o", out_file.c_str(), spvasm.c_str(), NULL);
    } else {
        // parent process
        int status{0};
        waitpid(child_pid, &status, 0);
    }
    return true;
}

bool remove_file(const std::string& filename)
{
    remove(filename.c_str());
    return true;
}
