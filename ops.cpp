#include "ops.hpp"
#include "program.hpp"
#include <cstdint>
#include <mutex>
#include <string>


uint32_t alloc_id()
{
    static uint32_t id_cnt{0};
    static std::mutex locker;

    std::lock_guard<std::mutex> guard{locker};
    auto id{id_cnt++};
    return id;
}


Op::Op(Type op_type, bool has_ret)
    : id(alloc_id())
    , has_ret(has_ret)
    , type(op_type)
{}


const std::string& as_string(Decoration::Type dec)
{
    static const std::string invalid{"Invalid"};
    static const std::string relaxed_precision{"RelaxedPrecision"};
    static const std::string specid{"Specid"};
    static const std::string block{"Block"};
    static const std::string buffer_block{"BufferBlock"};
    static const std::string row_major{"RowMajor"};
    static const std::string col_major{"ColMajor"};
    static const std::string array_stride{"ArrayStride"};
    static const std::string matrix_stride{"MatrixStride"};
    static const std::string glsl_shared{"GLSLShared"};
    static const std::string glsl_packed{"GLSLPacked"};
    static const std::string cpacked{"Cpacked"};
    static const std::string builtin{"Builtin"};
    static const std::string no_perspective{"NoPerspective"};
    static const std::string flat{"Flat"};
    static const std::string patch{"Patch"};
    static const std::string centroid{"Centroid"};
    static const std::string sample{"Sample"};
    static const std::string invariant{"Invariant"};
    static const std::string restrict{"Restrict"};
    static const std::string aliased{"Aliased"};
    static const std::string volatile_dec{"Volatile"};
    static const std::string constant{"Constant"};
    static const std::string coherent{"Coherent"};
    static const std::string non_writable{"NonWritable"};
    static const std::string non_readable{"NonReadable"};
    static const std::string uniform{"Uniform"};
    static const std::string uniformid{"Uniformid"};
    static const std::string saturated_conversion{"SaturatedConversion"};
    static const std::string stream{"Stream"};
    static const std::string location{"Location"};
    static const std::string component{"Component"};
    static const std::string index{"Index"};
    static const std::string binding{"Binding"};
    static const std::string descriptor_set{"DescriptorSet"};
    static const std::string offset{"Offset"};
    static const std::string xfb_buffer{"XfbBuffer"};
    static const std::string xfb_stride{"XfbStride"};

    switch (dec) {
    case Decoration::RELAXED_PRECISION: return relaxed_precision;
    case Decoration::SPECID: return specid;
    case Decoration::BLOCK: return block;
    case Decoration::BUFFER_BLOCK: return buffer_block;
    case Decoration::ROW_MAJOR: return row_major;
    case Decoration::COL_MAJOR: return col_major;
    case Decoration::ARRAY_STRIDE: return array_stride;
    case Decoration::MATRIX_STRIDE: return matrix_stride;
    case Decoration::GLSL_SHARED: return glsl_shared;
    case Decoration::GLSL_PACKED: return glsl_packed;
    case Decoration::CPACKED: return cpacked;
    case Decoration::BUILTIN: return builtin;
    case Decoration::NO_PERSPECTIVE: return no_perspective;
    case Decoration::FLAT: return flat;
    case Decoration::PATCH: return patch;
    case Decoration::CENTROID: return centroid;
    case Decoration::SAMPLE: return sample;
    case Decoration::INVARIANT: return invariant;
    case Decoration::RESTRICT: return restrict;
    case Decoration::ALIASED: return aliased;
    case Decoration::VOLATILE: return volatile_dec;
    case Decoration::CONSTANT: return constant;
    case Decoration::COHERENT: return coherent;
    case Decoration::NON_WRITABLE: return non_writable;
    case Decoration::NON_READABLE: return non_readable;
    case Decoration::UNIFORM: return uniform;
    case Decoration::UNIFORMID: return uniformid;
    case Decoration::SATURATED_CONVERSION: return saturated_conversion;
    case Decoration::STREAM: return stream;
    case Decoration::LOCATION: return location;
    case Decoration::COMPONENT: return component;
    case Decoration::INDEX: return index;
    case Decoration::BINDING: return binding;
    case Decoration::DESCRIPTOR_SET: return descriptor_set;
    case Decoration::OFFSET: return offset;
    case Decoration::XFB_BUFFER: return xfb_buffer;
    case Decoration::XFB_STRIDE: return xfb_stride;
    }
    return invalid;
}


const std::string& as_string(Op::Type op_type)
{
    static const std::string op_invalid{"OpInvalid"};
    static const std::string op_capability{"OpCapability"};
    static const std::string op_memory_model{"OpMemoryModel"};
    static const std::string op_entry_point{"OpEntryPoint"};
    static const std::string op_decorate{"OpDecorate"};
    static const std::string op_variable{"OpVariable"};
    static const std::string op_function{"OpFunction"};
    static const std::string op_load{"OpLoad"};
    static const std::string op_store{"OpStore"};
    static const std::string op_label{"OpLabel"};
    static const std::string op_return{"OpReturn"};
    static const std::string op_function_end{"OpFunctionEnd"};
    static const std::string op_type_pointer{"OpTypePointer"};
    static const std::string op_type_void{"OpTypeVoid"};
    static const std::string op_type_function{"OpTypeFunction"};
    static const std::string op_type_float{"OpTypeFloat"};

    switch (op_type) {
    case Op::INVALID: return op_invalid;
    case Op::CAPABILITY: return op_capability;
    case Op::MEMORY_MODEL: return op_memory_model;
    case Op::ENTRY_POINT: return op_entry_point;
    case Op::DECORATE: return op_decorate;
    case Op::VARIABLE: return op_variable;
    case Op::FUNCTION: return op_function;
    case Op::LOAD: return op_load;
    case Op::STORE: return op_store;
    case Op::LABEL: return op_label;
    case Op::RETURN: return op_return;
    case Op::FUNCTION_END: return op_function_end;
    case Op::TYPE_POINTER: return op_type_pointer;
    case Op::TYPE_VOID: return op_type_void;
    case Op::TYPE_FUNCTION: return op_type_function;
    case Op::TYPE_FLOAT: return op_type_float;
    }
    return op_invalid;
}
