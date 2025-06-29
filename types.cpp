#include "types.hpp"
#include <cstdint>
#include <string>


const std::string& as_string(StorageClass sc)
{
    static const std::string invalid{"Invalid"};
    static const std::string uniform_constant{"UniformConstant"};
    static const std::string input{"Input"};
    static const std::string uniform{"Uniform"};
    static const std::string output{"Output"};
    static const std::string workgroup{"Workgroup"};
    static const std::string cross_workgroup{"CrossWorkgroup"};
    static const std::string private_sc{"Private"};
    static const std::string function{"Function"};
    static const std::string generic{"Generic"};
    static const std::string push_constant{"PushConstant"};
    static const std::string atomic_counter{"AtomicCounter"};
    static const std::string image{"Image"};
    static const std::string storage_buffer{"StorageBuffer"};
    static const std::string tile_image_ext{"TileImageEXT"};
    static const std::string node_payload_amdx{"NodePayloadAMDX"};

    switch (sc) {
        case StorageClass::UNIFORM_CONSTANT:    return uniform_constant;
        case StorageClass::INPUT:               return input;
        case StorageClass::UNIFORM:             return uniform;
        case StorageClass::OUTPUT:              return output;
        case StorageClass::WORKGROUP:           return workgroup;
        case StorageClass::CROSS_WORKGROUP:     return cross_workgroup;
        case StorageClass::PRIVATE:             return private_sc;
        case StorageClass::FUNCTION:            return function;
        case StorageClass::GENERIC:             return generic;
        case StorageClass::PUSH_CONSTANT:       return push_constant;
        case StorageClass::ATOMIC_COUNTER:      return atomic_counter;
        case StorageClass::IMAGE:               return image;
        case StorageClass::STORAGE_BUFFER:      return storage_buffer;
        case StorageClass::TILE_IMAGE_EXT:      return tile_image_ext;
        case StorageClass::NODE_PAYLOAD_AMDX:   return node_payload_amdx;
    }
    return invalid;
}
