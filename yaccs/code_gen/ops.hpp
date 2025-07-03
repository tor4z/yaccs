#ifndef YACCSS_OPS_H_
#define YACCSS_OPS_H_

#include "base.hpp"
#include "def.hpp"
#include <string_view>
#include <cstdint>
#include <string>
#include <vector>


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


template<typename T>
struct OpConstant: public Op
{
    OpConstant() : Op(Op::CONSTANT, true) {}
    uint32_t type_id;
    T value;    /// TODO: support other type
}; // struct OpConstant


struct OpConstantComposite: public Op
{
    OpConstantComposite() : Op(Op::CONSTANT_COMPOSITE, true) {}
    uint32_t type_id;
    std::vector<uint32_t> comps;
}; // struct OpConstantComposite

#endif // YACCSS_OPS_H_