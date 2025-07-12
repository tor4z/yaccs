#include <cassert>
#include <cstdint>
#include <endian.h>
#include <fstream>
#include <iostream>
#include <onnx.pb.h>
#include <string>
#include <unordered_map>
#include "code_gen/program.hpp"


int main(int argc, char** argv)
{
    onnx::ModelProto model;
    std::ifstream ifs{"../examples/01/model.onnx", std::ios::in};
    model.ParseFromIstream(&ifs);
    ifs.close();


    std::unordered_map<std::string, int> input_params {
        {"batch_size", 1}
    };
    Program program;
    program.set_name("a.spvasm");

    for (const auto& it : model.graph().input()) {
        if (it.type().has_tensor_type()) {
            TensorType tt{};
            const auto& onnx_tensor{it.type().tensor_type()};
            tt.dims = onnx_tensor.shape().dim_size();
            tt.dtype = static_cast<DType>(onnx_tensor.elem_type());
            for (int i = 0; i < tt.dims; ++i) {
                const auto& dim{onnx_tensor.shape().dim().Get(i)};
                if (!dim.dim_param().empty()) {
                    assert(input_params.find(dim.dim_param()) != input_params.end() && "Input param not defined");
                    tt.shape[i] = input_params.at(dim.dim_param());
                } else {
                    tt.shape[i] = dim.dim_value();
                }
            }
            program.add_input(tt);
        }
    }

    for (const auto& it : model.graph().output()) {
        if (it.type().has_tensor_type()) {
            TensorType tt{};
            tt.dtype = static_cast<DType>(it.type().tensor_type().elem_type());
            program.add_output(tt);
        }
    }

    program.dump_ir();

    std::cout << "========================================\n";
    std::cout << "name: " << model.graph().name() << "\n"; 
    std::cout << "=======\n";

    std::cout << "input size: " << model.graph().input().size() << "\n";
    for (const auto& it : model.graph().input()) {
        std::cout << "\t" << it.name() <<"\n"; 
        if (it.type().has_tensor_type()) {
            std::cout << "\tDType: " << it.type().tensor_type().elem_type() << "\n";
            std::cout << "\tTensor Dims[";
            for (const auto& dim : it.type().tensor_type().shape().dim())
                std::cout << dim.dim_param() << ":" << dim.dim_value() <<" "; 
            std::cout << "]\n";
        }
    }
    std::cout << "output size: " << model.graph().output().size() << "\n";
    for (const auto& it : model.graph().output()) {
        std::cout << "\t" << it.name() <<"\n"; 
    }

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
