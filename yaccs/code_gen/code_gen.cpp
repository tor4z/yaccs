#include "yaccs/code_gen/code_gen.hpp"
#include <cassert>


CodeGen::CodeGen()
{

}

void CodeGen::push_header()
{
    header_ss_ << "OpCapability Shader\n";
    header_ss_ << "OpMemoryModel Logical GLSL450\n";
}

void CodeGen::push_type(DType dt, id_t id)
{
    switch (dt) {
    case DT_FLOAT:
        type_def_ss_ << "%" << id << " = OpTypeFloat 32\n";
        break;
    case DT_FLOAT16:
        type_def_ss_ << "%" << id << " = OpTypeFloat 16\n";
        break;
    default: assert(false && "Unsupported type");
    }
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
