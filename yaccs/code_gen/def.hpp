#ifndef YACCS_DEF_H_
#define YACCS_DEF_H_

#include "yaccs/dtype.hpp"
#include "yaccs/tensor.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

using id_t = uint32_t;

enum Decoration
{
    DECO_RELAXED_PRECISION = 1,
    DECO_SPECID,
    DECO_BLOCK,
    DECO_BUFFER_BLOCK,
    DECO_NONE,
}; // enum Decoration

enum Scope : uint32_t
{
    SCOPE_CROSSDEVICE = 0,
    SCOPE_DEVICE,
    SCOPE_WORKGROUP,
    SCOPE_SUBGROUP,
    SCOPE_INVOCATION,
    SCOPE_QUEUEFAMILY,
    SCOPE_SHADERCALLKHR,
}; // enum Scope

enum MemSemantic : uint32_t
{
    MS_NONE = 0x0,
    MS_ACQUIRE = 0x2,
    MS_RELEASE = 0x4,
    MS_ACQUIRE_RELEASE = 0x8,
    MS_SEQUENTIALLY_CONSISTENT = 0x10,
    MS_UNIFORM_MEMORY = 0x40,
    MS_SUBGROUP_MEMORY = 0x80,
    MS_WORKGROUP_MEMORY = 0x100,
    MS_CROSS_WORKGROUP_MEMORY = 0x200,
    MS_ATOMIC_COUNTER_MEMORY = 0x400,
    MS_IMAGE_MEMORY = 0x800,
    MS_OUTPUT_MEMORY = 0x1000,
    MS_MAKE_AVAILABLE = 0x2000,
    MS_MAKE_VISIBLE = 0x4000,
    MS_VOLATILE = 0x8000,
}; // enum MemSemantic


enum StorageClass
{
    SC_UNIFORM_CONSTANT = 0,
    SC_INPUT,
    SC_UNIFORM,
    SC_OUTPUT,
    SC_WORKGROUP,
    SC_CROSS_WORKGROUP,
    SC_PRIVATE,
    SC_FUNCTION,
    SC_GENERIC,
    SC_PUSH_CONSTANT,
    SC_ATOMICCOUNTER,
    SC_IMAGE,
    SC_STORAGE_BUFFER,
    SC_TILE_IMAGE_EXT,
    SC_TILE_ATTACHMENT_QCOM,
    SC_NODE_PAYLOAD_AMDX,
    SC_CALLABLE_DATA_KHR,
    SC_INCOMING_CALLABLE_DATA_KHR,
    SC_RAY_PAYLOAD_KHR,
    SC_HIT_ATTRIBUTE_KHR,
    SC_INCOMING_RAYPAYLOAD_KHR,
    SC_SHADER_RECORD_BUFFER_KHR,
    SC_PHYSICAL_STORAGE_BUFFER,
    SC_HIT_OBJECT_ATTRIBUTE_NV,
    SC_TASK_PAYLOAD_WORKGROUP_EXT,
    SC_CODE_SECTION_INTEL,
    SC_DEVICE_ONLY_INTEL,
    SC_HOST_ONLY_INTEL,
    SC_NONE,
}; // enum StorageClass

struct ArrTypeDef {
    uint32_t length;
    id_t dtype;
    id_t length_id;
    id_t id;
}; // struct ArrTypeDef

struct StructTypeDef {
    std::vector<id_t> fields;
    id_t id;
    size_t num_fields;
}; // struct StructTypeDef

struct ConstCompositeDef {
    std::vector<id_t> elem_ids;
    id_t type_id;
    id_t id;
}; // struct ArrTypeDef

template<typename T>
struct DTypeConstDef
{
    T value;
    id_t dtype_id;
    id_t id;
}; // struct DtypeConstDef

struct FunctionTypeDef
{
    id_t return_type_id;
    id_t id;
}; // struct FuncTypeDef

struct FunctionHeaderDef
{
    id_t return_type_id;
    id_t function_type_id;
    id_t id;
}; // struct FunctionHeaderDef

struct VarDef
{
    id_t id;
    id_t type_pointer_id;
    StorageClass storage_class;
}; // struct VarDef

struct TypePointerDef
{
    id_t id;
    id_t type_id;
    StorageClass storage_class;
}; // struct TypePointerDef

struct EntryDef
{
    id_t main_id;
    std::vector<id_t> input_ids;
    int local_size_x;
    int local_size_y;
    int local_size_z;
}; // struct EntryDef


struct DecorateSetBindingDef
{
    id_t target;
    int binding;
    int set;
}; // struct SetBindingDef

struct DecorateArrayDef
{
    id_t array_type_id;
}; // struct DecorateArrayDef

struct DecorateStructDef
{
    struct FieldOffset
    {
        uint32_t field;
        uint32_t offset;
    }; // struct FieldOffset

    std::vector<FieldOffset> member_deco;
    id_t struct_type_id;
    Decoration deco;
}; // struct DecorateStructDef

struct FunctionCallDef
{
    id_t id;
    id_t func_id;
    id_t return_type_id;
}; // struct FunctionCallDef

struct ControlBarrierDef
{
    id_t exe_scope_id;
    id_t mem_scope_id;
    id_t mem_semantics_id;
}; // struct ControlBarrierDef

struct TensorMeta
{
    std::string name;
    int shape[MAX_TENSOR_DIMS];
    int dims;
    DType dtype;
    id_t tensor_id;
    id_t dtype_id;
    id_t dtype_pointer_id;
}; // struct TensorMeta

#endif // YACCS_DEF_H_
