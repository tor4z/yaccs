#ifndef YACCS_PROGRAM_H_
#define YACCS_PROGRAM_H_

#include "yaccs/code_gen/utils.hpp"
#include "yaccs/code_gen/code_gen.hpp"
#include "yaccs/tensor.hpp"
#include "yaccs/ops.hpp"
#include <string>
#include <vector>


struct Program
{
    Program();
    void set_name(const std::string& name);
    void set_main();
    void add_input(const TensorType& tensor_type);
    void add_output(const TensorType& tensor_type);
    void add_gemm(const OpGemm& gemm);
    void add_relu(const OpRelu& relu);
    void dump_ir();
private:
    std::string name_;
    CodeGen code_gen_;

    id_t add_function_prologue(id_t return_type_id);
    void add_function_epilogue();
    id_t add_void_type();
    id_t add_label();
    id_t add_function_type(id_t return_type_id);
    id_t add_struct_dtype(const std::vector<id_t>& dtypes);
    id_t add_array_dtype(id_t dtype, int length);
    id_t add_const_array(id_t arr_type, const std::vector<id_t>& elem_ids);
    id_t add_dtype(DType dtype);
    id_t add_const_tensor(const Tensor& tensor);
    id_t add_tensor_type(const TensorType& tensor_type);
    id_t add_type_pointer(id_t type_id);
    id_t add_var(id_t type_id);
    id_t add_raw_const(DType dtype, int elem_idx, const char* data);
    template<typename T>
    id_t add_const(DType dtype, T value);
}; // class Program

template<typename T>
id_t Program::add_const(DType dtype, T value)
{
    static std::vector<DTypeConstDef<T>> defs{};

    for (auto& it: defs) {
        if (std::abs(it.value - value) < 1.0e-6) {
            return it.id;
        }
    }

    DTypeConstDef<T> dconst {.value = value, .dtype_id = add_dtype(dtype), .id = alloc_id()};
    code_gen_.push_const_dtype(dconst);
    defs.push_back(dconst);
    return dconst.id;
}

#endif // YACCS_PROGRAM_H_
