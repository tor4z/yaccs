#include "code_gen/program.hpp"
#include "yaccs/utils.hpp"
#include "yaccs/ops.hpp"
#include <cassert>
#include <cstdint>
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
            tensor_type_from_onnx(it.type().tensor_type(), tt, input_dynamic_axes);
            program.add_input(tt);
        }
    }

    for (const auto& it : model.graph().output()) {
        if (it.type().has_tensor_type()) {
            TensorType tt{};
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

    for (const auto& node: model.graph().node()) {
        std::cout << node.name() << "\n";
        std::cout << "OP Type: " << node.op_type() << "\n";
        std::cout << "Attribute name: ";
        for (const auto& attr: node.attribute()) {
            std::cout << attr.name() << ", ";
        }
        std::cout << "\n";
        std::cout << "Attribute type: ";
        for (const auto& attr: node.attribute()) {
            std::cout << attr.type() << "-" << attr.f() << "-" << attr.i() << "-hasG-" << attr.has_g() << ", ";
        }
        std::cout << "\n";

        std::cout << "Input: ";
        for (const auto& input : node.input()) {
            std::cout << input << ", ";
        }

        std::cout << "\nOutput: ";
        for (const auto& output : node.output()) {
            std::cout << output << ", ";
        }
        std::cout << "\n";
    }

    std::cout << "=======\n";
    for (const auto& it : model.graph().initializer()) {
        std::cout << "name: " << it.name() << "\n";
        std::cout << "dt: " << it.data_type() << "\n";
        int num_elems{1};
        std::cout << "dims: [";
        for (auto d : it.dims()) {
            std::cout << d << " ";
            num_elems *= d;
        }
        std::cout << "]\n";
        // for (int i = 0; i < num_elems; ++i) {
        //     float v;
        //     auto raw{le32toh(*reinterpret_cast<const uint32_t*>(it.raw_data().data() + i * 4))};
        //     v =  *reinterpret_cast<float*>(&raw);
        //     std:: cout << v << " ";
        // }
        // std::cout << "\n";
    }

    std::cout << "--------------------------------\n";
    return 0;
}
