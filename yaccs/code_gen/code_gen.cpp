#include "yaccs/code_gen/code_gen.hpp"
#include <cassert>
#include <cstddef>


CodeGen::CodeGen()
{

}

void CodeGen::push_header()
{
    header_ss_ << "OpCapability Shader\n";
    header_ss_ << "OpMemoryModel Logical GLSL450\n";
}

void CodeGen::push_dtype(DType dt, id_t id)
{
    switch (dt) {
    case DT_FLOAT:
        type_def_ss_ << "%" << id << " = OpTypeFloat 32\n";
        break;
    case DT_FLOAT16:
        type_def_ss_ << "%" << id << " = OpTypeFloat 16\n";
        break;
    case DT_INT32:
        type_def_ss_ << "%" << id << " = OpTypeInt 32 1\n";
        break;
    default: assert(false && "Unsupported type");
    }
}

void CodeGen::push_array_dtype(const ArrTypeDef& arr)
{
    type_def_ss_ << "%" << arr.id << " = OpTypeArray %" << arr.dtype << " %" << arr.length_id << "\n";
}

void CodeGen::push_struct_dtype(const StructTypeDef& sd)
{
    type_def_ss_ << "%" << sd.id << " = OpTypeStruct ";
    for (size_t i = 0; i < sd.num_fields; ++i) {
        type_def_ss_ << "%" << sd.fields[i];
        if (i != sd.num_fields - 1) type_def_ss_ << " ";
    }
    type_def_ss_ << "\n";
}

void CodeGen::push_void_type(id_t id)
{
    type_def_ss_ << "%" << id << " = OpTypeVoid\n";
}

void CodeGen::push_function_type(const FunctionTypeDef& ft)
{
    type_def_ss_ << "%" << ft.id << " = OpTypeFunction %" << ft.return_type_id << "\n";
}

void CodeGen::push_const_composite(const ConstCompositeDef& ccd)
{
    const_def_ss_ << "%" << ccd.id << " = OpConstantComposite %" << ccd.type_id;
    for (auto it : ccd.elem_ids) {
        const_def_ss_ << " %" << it;
    }
    const_def_ss_ << "\n";
}

void CodeGen::push_function(const FunctionHeaderDef& fh)
{
    fn_def_ss_ << "\n";
    fn_def_ss_ << "%" << fh.id << " = OpFunction " << "%" << fh.return_type_id
        << " None %" << fh.function_type_id << "\n";
}

void CodeGen::push_label(id_t id)
{
    fn_def_ss_ << "%" << id << " = OpLabel\n";
}

void CodeGen::push_return()
{
    fn_def_ss_ << "\tOpReturn\n";
}

void CodeGen::push_function_end()
{
    fn_def_ss_ << "\tOpFunctionEnd\n";
}

void CodeGen::push_decorate_set_binding(const DecorateSetBindingDef& deco)
{
    assert(deco.binding >= 0 && deco.set >= 0 && "Bad decoration");
    decorate_ss_ << "OpDecorate %" << deco.target << " DescriptorSet " << deco.set << "\n";
    decorate_ss_ << "OpDecorate %" << deco.target << " Binding " << deco.binding << "\n";
}

void CodeGen::assemble(std::ofstream& ofs)
{
    ofs << header_ss_.str();
    ofs << entry_def_ss_.str();
    ofs << decorate_ss_.str();
    ofs << type_def_ss_.str();
    ofs << input_def_ss_.str();
    ofs << output_def_ss_.str();
    ofs << const_def_ss_.str();
    ofs << fn_def_ss_.str();
}
