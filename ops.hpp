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

#endif // YACCSS_OPS_H_