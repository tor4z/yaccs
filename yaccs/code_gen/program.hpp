#ifndef YACCS_PROGRAM_H_
#define YACCS_PROGRAM_H_

#include "yaccs/code_gen/utils.hpp"
#include "yaccs/tensor.hpp"
#include "yaccs/code_gen/code_gen.hpp"
#include <string>
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

    id_t add_struct_dtype(const std::vector<id_t>& dtypes);
    id_t add_array_dtype(id_t dtype, int length);
    id_t add_dtype(DType dtype);
    template<typename T>
    id_t add_const_dtype(DType dtype, T value);
}; // class Program

template<typename T>
id_t Program::add_const_dtype(DType dtype, T value)
{
    static std::vector<DTypeConstDef<T>> defs{};

    for (auto& it: defs) {
        if (it.value == value) {
            return it.id;
        }
    }

    DTypeConstDef<T> dconst {.value = value, .dtype_id = add_dtype(dtype), .id = alloc_id()};
    code_gen_.push_const_dtype(dconst);
    defs.push_back(dconst);
    return dconst.id;
}

#endif // YACCS_PROGRAM_H_
