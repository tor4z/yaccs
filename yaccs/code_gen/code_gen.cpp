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
