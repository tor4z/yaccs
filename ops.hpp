#ifndef YACCSS_OPS_H_
#define YACCSS_OPS_H_

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

struct Program;
struct CodeGen;

enum class DType
{
    INVALID = 0,
    FN,
    VOID,
    FLOAT,
    INT,
}; // enum class DType


enum class StorageClass
{
    UNIFORM_CONSTANT = 0,
    INPUT,
    UNIFORM,
    OUTPUT,
    WORKGROUP,
    CROSS_WORKGROUP,
    PRIVATE,
    FUNCTION,
    GENERIC,
    PUSH_CONSTANT,
    ATOMIC_COUNTER,
    IMAGE,
    STORAGE_BUFFER,
    TILE_IMAGE_EXT,
    NODE_PAYLOAD_AMDX,
}; // enum class StorageClass


struct Decoration
{
    enum Type {
        RELAXED_PRECISION = 0,
        SPECID,
        BLOCK,
        BUFFER_BLOCK,
        ROW_MAJOR,
        COL_MAJOR,
        ARRAY_STRIDE,
        MATRIX_STRIDE,
        GLSL_SHARED,
        GLSL_PACKED,
        CPACKED,
        BUILTIN,
        NO_PERSPECTIVE,
        FLAT,
        PATCH,
        CENTROID,
        SAMPLE,
        INVARIANT,
        RESTRICT,
        ALIASED,
        VOLATILE,
        CONSTANT,
        COHERENT,
        NON_WRITABLE,
        NON_READABLE,
        UNIFORM,
        UNIFORMID,
        SATURATED_CONVERSION,
        STREAM,
        LOCATION,
        COMPONENT,
        INDEX,
        BINDING,
        DESCRIPTOR_SET,
        OFFSET,
        XFB_BUFFER,
        XFB_STRIDE,
    }; // enum Type

    Decoration() {}
    Decoration(Type t, uint32_t p) : type(t), param(p) {}
    Type type;
    uint32_t param;
}; // Decoration

struct Op
{
    enum Type {
        INVALID = 0,
        CAPABILITY,
        MEMORY_MODEL,
        ENTRY_POINT,
        DECORATE,
        VARIABLE,
        FUNCTION,
        LOAD,
        STORE,
        LABEL,
        RETURN,
        FUNCTION_END,
        TYPE_POINTER,
        TYPE_VOID,
        TYPE_FUNCTION,
        TYPE_FLOAT,
    }; // enum Type

    Op(Type op_type, bool has_ret);
    virtual ~Op() = default;
    const uint32_t id;
    const bool has_ret;
    const Type type;
}; // strut Op


struct OpCapability: public Op
{
    OpCapability() : Op(Op::CAPABILITY, false) {}
}; // struct OpCapability


struct OpMemoryModel: public Op
{
    OpMemoryModel() : Op(Op::MEMORY_MODEL, false) {}
}; // struct OpMemoryModel


struct OpEntryPoint: public Op
{
    OpEntryPoint() : Op(Op::ENTRY_POINT, false) {}
    uint32_t entry_id;
    std::vector<uint32_t> params;
}; // struct OpEntryPoint


struct OpDecorate: public Op
{
    OpDecorate() : Op(Op::DECORATE, false) {}
    uint32_t target_id;
    Decoration dec;
}; // struct OpDecorate


struct OpVariable: public Op
{
    OpVariable() : Op(Op::VARIABLE, true) {}
    uint32_t type_ptr_id;
    StorageClass storage_class;
}; // struct OpVariable


struct OpFunction: public Op
{
    OpFunction() : Op(Op::FUNCTION, true) {}
    uint32_t type_id;
    uint32_t ret_type_id;
}; // struct OpFunction


struct OpLoad: public Op
{
    OpLoad() : Op(Op::LOAD, true) {}
    uint32_t type_id;
    uint32_t var_id;
}; // struct OpLoad


struct OpStore: public Op
{
    OpStore() : Op(Op::STORE, false) {}
    uint32_t src_id;
    uint32_t dst_id;
}; // struct OpStore


struct OpLabel: public Op
{
    OpLabel() : Op(Op::LABEL, true) {}
}; // struct OpLabel


struct OpReturn: public Op
{
    OpReturn() : Op(Op::RETURN, false) {}
}; // struct OpReturn


struct OpFunctionEnd: public Op
{
    OpFunctionEnd() : Op(Op::FUNCTION_END, false) {}
}; // struct OpFunctionEnd


const std::string& as_string(Decoration::Type dec);
const std::string& as_string(Op::Type op_type);

#endif // YACCSS_OPS_H_