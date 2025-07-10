#include <cstdint>
#include <endian.h>
#include <fstream>
#include <iostream>
#include <onnx.pb.h>


int main(int argc, char** argv)
{
    onnx::ModelProto model;
    std::ifstream ifs{"../examples/01/model.onnx", std::ios::in};
    model.ParseFromIstream(&ifs);
    ifs.close();

    std::cout << "name: " << model.graph().name() << "\n";
    std::cout << "=======\n";

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

    std::cout << "input size: " << model.graph().input().size() << "\n";
    for (const auto& it : model.graph().input()) {
        std::cout << "\t" << it.name() <<"\n"; 
        std::cout << "\t" << it.GetTypeName() <<"\n"; 
        std::cout << "\t" << it.type().GetTypeName() <<"\n"; 
    }
    std::cout << "output size: " << model.graph().output().size() << "\n";
    for (const auto& it : model.graph().output()) {
        std::cout << "\t" << it.name() <<"\n"; 
    }

    return 0;
}
