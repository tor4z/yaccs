#ifndef YACCS_PROGRAM_H_
#define YACCS_PROGRAM_H_

#include "yaccs/tensor.hpp"
#include "yaccs/code_gen/code_gen.hpp"
#include <string>


struct Program
{
    Program();
    void set_name(const std::string& name);
    void add_input(const TensorType& tensor);
    void add_output(const TensorType& tensor);
    void dump_ir();
private:
    std::string name_;
    CodeGen code_gen_;

    id_t add_type(DType dt);
}; // class Program

#endif // YACCS_PROGRAM_H_
