#ifndef YACCS_PROGRAM_H_
#define YACCS_PROGRAM_H_

#include "yaccs/code_gen/def.hpp"
#include "yaccs/code_gen/utils.hpp"
#include "yaccs/code_gen/code_gen.hpp"
#include "yaccs/tensor.hpp"
#include "yaccs/ops.hpp"
#include <string>
#include <unordered_map>
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
    std::vector<id_t> layers_;  // layers in order
    std::unordered_map<id_t, FunctionHeaderDef> global_funcs_;
    std::unordered_map<std::string, TensorMeta> global_tensors_;
    std::string name_;
    CodeGen code_gen_;

    id_t add_function_prologue(id_t return_type_id);
    void add_function_epilogue();
    id_t add_void_type();
    id_t add_label();
    id_t add_function_type(id_t return_type_id);
    id_t add_struct_dtype(const std::vector<id_t>& dtypes, bool reuse=true);
    id_t add_array_dtype(id_t dtype, uint32_t length, StorageClass sc, bool reuse=true);
    id_t add_const_array(id_t arr_type, const std::vector<id_t>& elem_ids);
    id_t add_dtype(DType dtype);
    id_t add_const_tensor(const Tensor& tensor);
    id_t add_shared_tensor(const Tensor& tensor);
    id_t add_tensor_type(const TensorType& tensor_type, StorageClass sc, bool reuse=true);
    id_t add_type_pointer(id_t type_id, StorageClass sc);
    id_t add_var(id_t type_id, StorageClass sc);
    id_t add_raw_const(DType dtype, int elem_idx, const char* data);
    template<typename T>
    id_t add_const(DType dtype, T value);
    id_t add_function_call(id_t id);
    void add_control_barrier(Scope exe_scope, Scope mem_scope, MemSemantic mem_semantics);

    FunctionHeaderDef& find_function_def(id_t id);
}; // class Program

template<typename T>
id_t Program::add_const(DType dtype, T value)
{
    static std::vector<DTypeConstDef<T>> defs{};

    for (auto& it: defs) {
        if (value_eq(it.value, value)) {
            return it.id;
        }
    }

    DTypeConstDef<T> dconst {.value = value, .dtype_id = add_dtype(dtype), .id = alloc_id()};
    code_gen_.push_const_dtype(dconst);
    defs.push_back(dconst);
    return dconst.id;
}

#endif // YACCS_PROGRAM_H_
