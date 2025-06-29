#ifndef YACCSS_OPS_H_
#define YACCSS_OPS_H_

#include <cstdint>
#include <string>
#include <string_view>

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
    explicit Op(bool has_ret);
    virtual ~Op() = default;
    virtual std::string_view name() const = 0;
    const uint32_t id;
    const bool has_ret;
private:
}; // strut Op


struct OpCapability: public Op
{
    OpCapability() : Op(false) {}
    virtual std::string_view name() const override { return "OpCapability"; }
}; // struct OpCapability


struct OpMemoryModel: public Op
{
    OpMemoryModel() : Op(false) {}
    virtual std::string_view name() const override { return "OpMemoryModel"; }
}; // struct OpMemoryModel


struct OpEntryPoint: public Op
{
    OpEntryPoint() : Op(false) {}
    virtual std::string_view name() const override { return "OpEntryPoint"; }
    void set_entry_id(uint32_t entry_id) { entry_id_ = entry_id; }
private:
    friend class CodeGen;
    uint32_t entry_id_;
}; // struct OpEntryPoint


struct OpDecorate: public Op
{
    OpDecorate() : Op(false) {}
    virtual std::string_view name() const override { return "OpDecorate"; }
    uint32_t target_id;
    Decoration dec;
}; // struct OpDecorate


struct OpVariable: public Op
{
    OpVariable() : Op(true) {}
    virtual std::string_view name() const override { return "OpVariable"; }
    uint32_t type_ptr_id;
    StorageClass storage_class;
}; // struct OpVariable


struct OpFunction: public Op
{
    OpFunction() : Op(true) {}
    virtual std::string_view name() const override { return "OpFunction"; }
    uint32_t type_id;
    uint32_t ret_type_id;
}; // struct OpFunction


struct OpLabel: public Op
{
    OpLabel() : Op(true) {}
    virtual std::string_view name() const override { return "OpLabel"; }
}; // struct OpLabel


struct OpReturn: public Op
{
    OpReturn() : Op(false) {}
    virtual std::string_view name() const override { return "OpReturn"; }
}; // struct OpReturn


struct OpFunctionEnd: public Op
{
    OpFunctionEnd() : Op(false) {}
    virtual std::string_view name() const override { return "OpFunctionEnd"; }
}; // struct OpFunctionEnd


const std::string& as_string(Decoration::Type dec);

#endif // YACCSS_OPS_H_