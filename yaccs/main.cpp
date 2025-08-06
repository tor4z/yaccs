#include "code_gen/program.hpp"
#include "yaccs/utils.hpp"
#include "yaccs/ops.hpp"
#include <cassert>
#include <endian.h>
#include <fstream>
#include <iostream>
#include <onnx.pb.h>
#include <string>
#include <unordered_map>



int main(int argc, char** argv)
{
    onnx::ModelProto model;
    std::ifstream ifs{"../examples/01/model.onnx", std::ios::in};
    model.ParseFromIstream(&ifs);
    ifs.close();


    std::unordered_map<std::string, int> input_dynamic_axes {
        {"batch_size", 1}
    };
    std::unordered_map<std::string, int> output_dynamic_axes {
        {"batch_size", 1}
    };

    Program program;
    program.set_name("a.spvasm");

    for (const auto& it : model.graph().input()) {
        if (it.type().has_tensor_type()) {
            TensorType tt{};
            tt.name = it.name();
            tensor_type_from_onnx(it.type().tensor_type(), tt, input_dynamic_axes);
            program.add_input(tt);
        }
    }

    for (const auto& it : model.graph().output()) {
        if (it.type().has_tensor_type()) {
            TensorType tt{};
            tt.name = it.name();
            tensor_type_from_onnx(it.type().tensor_type(), tt, output_dynamic_axes);
            program.add_output(tt);
        }
    }

    for (const auto& node: model.graph().node()) {
        if (node.op_type().compare("Gemm") == 0) {
            OpGemm gemm;
            gemm_from_onnx(node, model.graph(), gemm);
            std::cout << gemm.B << "\n";
            std::cout << gemm.B.transpose() << "\n";
            std::cout << "===\n";
            program.add_gemm(gemm);
        } else if (node.op_type().compare("Relu") == 0) {
            OpRelu relu;
            relu_from_onnx(node, relu);
            program.add_relu(relu);
        } else {
            assert(false && "Not supportted operator");
        }
    }

    program.set_main();
    program.dump_ir();
    return 0;
}
