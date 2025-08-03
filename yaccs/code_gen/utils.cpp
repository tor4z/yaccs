#include "yaccs/code_gen/utils.hpp"
#include "yaccs/code_gen/def.hpp"
#include <cassert>

StorageClass storage_class_for_accessment(StorageClass sc)
{
    switch (sc) {
    case SC_GLOBAL_CONST:   return SC_FUNCTION;
    default:                return sc;
    }
}

const std::string& as_string(StorageClass sc)
{
    static const std::string uniform_constant{"UniformConstant"};
    static const std::string input{"Input"};
    static const std::string uniform{"Uniform"};
    static const std::string output{"Output"};
    static const std::string workgroup{"Workgroup"};
    static const std::string cross_workgroup{"CrossWorkgroup"};
    static const std::string sc_private{"Private"};
    static const std::string function{"Function"};
    static const std::string generic{"Generic"};
    static const std::string push_constant{"PushConstant"};
    static const std::string atomiccounter{"AtomicCounter"};
    static const std::string image{"Image"};
    static const std::string storage_buffer{"Storage_Buffer"};
    static const std::string tile_image_ext{"TileImageEXT"};
    static const std::string tile_attachment_qcom{"TileAttachmentQCOM"};
    static const std::string node_payload_amdx{"NodePayloadAMDX"};
    static const std::string callable_data_khr{"CallableDataKHR"};
    static const std::string incoming_callable_data_khr{"IncomingCallableDataKHR"};
    static const std::string ray_payload_khr{"RayPayloadKHR"};
    static const std::string hit_attribute_khr{"HitAttributeKHR"};
    static const std::string incoming_raypayload_khr{"IncomingRayPayloadKHR"};
    static const std::string shader_record_buffer_khr{"ShaderRecordBufferKHR"};
    static const std::string physical_storage_buffer{"PhysicalStorageBuffer"};
    static const std::string hit_object_attribute_nv{"HitObjectAttributeNV"};
    static const std::string task_payload_workgroup_ext{"TaskPayloadWorkgroupEXT"};
    static const std::string code_section_intel{"CodeSectionINTEL"};
    static const std::string device_only_intel{"DeviceOnlyINTEL"};
    static const std::string host_only_intel{"HostOnlyINTEL"};

    switch (sc) {
    case SC_UNIFORM_CONSTANT:           return uniform_constant;
    case SC_INPUT:                      return input;
    case SC_UNIFORM:                    return uniform;
    case SC_OUTPUT:                     return output;
    case SC_WORKGROUP:                  return workgroup;
    case SC_CROSS_WORKGROUP:            return cross_workgroup;
    case SC_PRIVATE:                    return sc_private;
    case SC_FUNCTION:                   return function;
    case SC_GENERIC:                    return generic;
    case SC_PUSH_CONSTANT:              return push_constant;
    case SC_ATOMICCOUNTER:              return atomiccounter;
    case SC_IMAGE:                      return image;
    case SC_STORAGE_BUFFER:             return storage_buffer;
    case SC_TILE_IMAGE_EXT:             return tile_image_ext;
    case SC_TILE_ATTACHMENT_QCOM:       return tile_attachment_qcom;
    case SC_NODE_PAYLOAD_AMDX:          return node_payload_amdx;
    case SC_CALLABLE_DATA_KHR:          return callable_data_khr;
    case SC_INCOMING_CALLABLE_DATA_KHR: return incoming_callable_data_khr;
    case SC_RAY_PAYLOAD_KHR:            return ray_payload_khr;
    case SC_HIT_ATTRIBUTE_KHR:          return hit_attribute_khr;
    case SC_INCOMING_RAYPAYLOAD_KHR:    return incoming_raypayload_khr;
    case SC_SHADER_RECORD_BUFFER_KHR:   return shader_record_buffer_khr;
    case SC_PHYSICAL_STORAGE_BUFFER:    return physical_storage_buffer;
    case SC_HIT_OBJECT_ATTRIBUTE_NV:    return hit_object_attribute_nv;
    case SC_TASK_PAYLOAD_WORKGROUP_EXT: return task_payload_workgroup_ext;
    case SC_CODE_SECTION_INTEL:         return code_section_intel;
    case SC_DEVICE_ONLY_INTEL:          return device_only_intel;
    case SC_HOST_ONLY_INTEL:            return host_only_intel;
    default:                            assert(false && "Unreachable");
    }

    return uniform_constant; // unreachable, return something to suppress compiler warning
}

const std::string& as_string(Decoration deco)
{
    static const std::string relaxed_precision{"RelaxedPrecision"};
    static const std::string specid{"SpecId"};
    static const std::string block{"Block"};   
    static const std::string buffer_block{"BufferBlock"};   

    switch (deco) {
    case DECO_RELAXED_PRECISION:    return relaxed_precision;
    case DECO_SPECID:               return specid;
    case DECO_BLOCK:                return block;
    case DECO_BUFFER_BLOCK:         return buffer_block;
    default:                        assert(false && "Unreachable");
    }

    return relaxed_precision; // unreachable, return something to suppress compiler warning
}

const std::string& as_string(BuiltIn built_in)
{
    static const std::string workgroup_size{"WorkgroupSize"};
    static const std::string global_invocation_id{"GlobalInvocationId"};

    switch (built_in) {
    case BI_WORKGROUP_SIZE:         return workgroup_size;
    case BI_GLOBAL_INVOCATION_ID:   return global_invocation_id;
    default:                        assert(false && "Not implemented");
    }

    return workgroup_size;  // return something to suppress compiler warning
}

const std::string& as_string(BinaryOperator bo)
{
    static const std::string iadd{"OpIAdd"};
    static const std::string imul{"OpIMul"};

    switch (bo) {
    case BO_IADD:   return iadd;
    case BO_IMUL:   return imul;
    default:        assert(false && "Not implemented");
    }

    return iadd;  // return something to suppress compiler warning
}
