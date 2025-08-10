#include "yaccs/code_gen/program.hpp"
#include "yaccs/utils.hpp"
#include "yaccs/ops.hpp"
#include <onnx.pb.h>
#include <unordered_map>
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <string>

#define FLAGS_IMPLEMENTATION
#include "flags.hpp"


int main(int argc, char** argv)
{
    Flags::parse(argc, argv)
        ->with_arg<std::string>("output", 'o', "a.spv", "The output spv filename")
        ->with_opt("S", 'S', "Compile only, not assemble")
        ->set_help("Yaccs compiler");

    if (Flags::raw_params().empty()) {
        std::cerr << "yaccs: fatal error: no input file\n"
            << "compilation terminated\n";
        exit(1);
    }

    std::string onnx_filename{Flags::raw_params().at(0)};
    std::string apvasm_filename{extract_filename(onnx_filename) + ".spvasm"};
    std::string apv_filename{Flags::arg<std::string>("output")};
    std::cout << "Compiling " << onnx_filename << "\n";

    onnx::ModelProto model;
    std::ifstream ifs{onnx_filename, std::ios::in};
    model.ParseFromIstream(&ifs);
    ifs.close();

    std::unordered_map<std::string, int> input_dynamic_axes {
        {"batch_size", 1}
    };
    std::unordered_map<std::string, int> output_dynamic_axes {
        {"batch_size", 1}
    };

    Program program;
    program.set_name(apvasm_filename);

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

    if (!Flags::opt("S")) {
        invoke_spirv_as(apvasm_filename, apv_filename);
        remove_file(apvasm_filename);
    }
    return 0;
}
