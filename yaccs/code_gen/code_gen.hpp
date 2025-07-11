#ifndef YACCS_CODE_GEN_H_
#define YACCS_CODE_GEN_H_

#include "yaccs/dtype.hpp"
#include <fstream>
#include <sstream>

struct CodeGen
{
    CodeGen();
    void push_header();
    void push_type(DType dt, id_t id);
    void assemble(std::ofstream& ofs);
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

#endif // YACCS_CODE_GEN_H_
