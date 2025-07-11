#ifndef YACCS_PROGRAM_H_
#define YACCS_PROGRAM_H_

#include "yaccs/tensor.hpp"
#include "yaccs/code_gen/code_gen.hpp"
#include <string>
#include <sys/types.h>
#include <vector>


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

    id_t add_struct_type(const std::vector<id_t>& dtypes);
    id_t add_array_type(id_t dtype, int length);
    id_t add_dtype(DType dtype);
}; // class Program

#endif // YACCS_PROGRAM_H_
