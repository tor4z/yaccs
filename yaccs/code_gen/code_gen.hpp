#ifndef YACCS_CODE_GEN_H_
#define YACCS_CODE_GEN_H_

#include "yaccs/dtype.hpp"
#include "yaccs/code_gen/def.hpp"
#include <fstream>
#include <sstream>

struct CodeGen
{
    CodeGen();
    void assemble(std::ofstream& ofs);

    void push_header();
    void push_dtype(DType dt, id_t id);
    void push_array_dtype(const ArrTypeDef& arr);
    void push_struct_dtype(const StructTypeDef& sd);
    template<typename T>
    void push_const_dtype(const DTypeConstDef<T>& dconst);
private:
    std::stringstream header_ss_;
    std::stringstream entry_def_ss_;
    std::stringstream decorate_ss_;
    std::stringstream type_def_ss_;
    std::stringstream input_def_ss_;
    std::stringstream output_def_ss_;
    std::stringstream const_def_ss_;
    std::stringstream fn_def_ss_;
}; // class CodeGen

template<typename T>
void CodeGen::push_const_dtype(const DTypeConstDef<T>& dconst)
{
    const_def_ss_ << "%" << dconst.id << " = OpConstant %" << dconst.dtype_id << " " << dconst.value << "\n";
}

#endif // YACCS_CODE_GEN_H_
