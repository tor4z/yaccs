#ifndef YACCS_PROGRAM_H_
#define YACCS_PROGRAM_H_

#include "yaccs/code_gen/def.hpp"
#include "yaccs/code_gen/exts/exts.hpp"
#include "yaccs/code_gen/utils.hpp"
#include "yaccs/code_gen/code_gen.hpp"
#include "yaccs/dtype.hpp"
#include "yaccs/tensor.hpp"
#include "yaccs/ops.hpp"
#include <cstdint>
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
    friend struct ext::Ext;

    std::vector<id_t> layers_;  // layers in order
    std::unordered_map<id_t, FunctionHeaderDef> global_funcs_;
    std::unordered_map<std::string, TensorMeta> global_tensors_;
    std::string name_;
    CodeGen code_gen_;
    ext::Ext std_450_;

    id_t add_function_prologue(id_t return_type_id);
    void add_function_epilogue();
    id_t add_void_type();
    id_t add_label();
    id_t add_function_type(id_t return_type_id);
    id_t add_struct_dtype(const std::vector<id_t>& dtypes, bool reuse=true);
    id_t add_vector_dtype(id_t component_type_id, int count);
    id_t add_array_dtype(id_t dtype, uint32_t length, StorageClass sc, bool reuse=true);
    id_t add_const_array(id_t arr_type, const std::vector<id_t>& elem_ids);
    id_t add_dtype(DType dtype);
    id_t add_const_tensor(const Tensor& tensor);
    id_t add_shared_tensor(const Tensor& tensor);
    id_t add_tensor_type(const TensorType& tensor_type, StorageClass sc, bool reuse=true);
    id_t add_type_pointer(id_t type_id, StorageClass sc);
    id_t add_var(id_t type_id, StorageClass sc, id_t initializer = 0);
    id_t add_raw_const(DType dtype, int elem_idx, const Tensor& tensor);
    id_t add_function_call(id_t id);
    id_t global_invocation_id();
    id_t load_var(id_t dtype_id, id_t pointer);
    void store_var(id_t pointer, id_t object);
    id_t access_chain_indices(id_t func_id, id_t type_id, id_t base_id, const std::vector<uint32_t>& indices);
    id_t access_chain(id_t func_id, id_t type_id, id_t base_id, const std::vector<id_t>& indices);
    void add_control_barrier(Scope exe_scope, Scope mem_scope, MemSemantic mem_semantics);

    id_t binary_op(BinaryOperator bo, id_t func_id, id_t type_id, id_t op1_id, id_t op2_id);

    id_t access_invocation_index(id_t func_id, uint32_t index);

    void invocation_boundary_check(id_t func_id, const TensorMeta& tm, uint32_t index);
    id_t access_tensor_dims(id_t func_id, const TensorMeta& tm);
    id_t access_tensor_shape_index(id_t func_id, const TensorMeta& tm, uint32_t index);
    id_t load_tensor_element(id_t func_id, const TensorMeta& tm, id_t index_id);
    id_t load_tensor_element(id_t func_id, const TensorMeta& tm, id_t i, id_t step, id_t j);
    void store_tensor_element(id_t func_id, const TensorMeta& tm, id_t index_id, id_t object_id);
    void store_tensor_element(id_t func_id, const TensorMeta& tm, id_t i, id_t step, id_t j, id_t object_id);
    void store_tensor_shape_element(id_t func_id, const TensorMeta& tm, uint32_t index, id_t object_id);
    void store_tensor_dims(id_t func_id, const TensorMeta& tm, id_t object_id);

    void for_loop_init(ForLoopDef& def);
    void for_loop_begin(const ForLoopDef& def);
    void for_loop_end(const ForLoopDef& def);

    template<typename T>
    id_t add_const(DType dtype, T value);

    FunctionHeaderDef& find_function_def(id_t id);
}; // class Program

template<typename T>
id_t Program::add_const(DType dtype, T value)
{
    static std::vector<DTypeConstDef<T>> defs{};

    auto dtype_id{add_dtype(dtype)};
    for (auto& it: defs) {
        if (it.dtype_id == dtype_id && value_eq(it.value, value)) {
            return it.id;
        }
    }

    DTypeConstDef<T> dconst {.value = value, .dtype_id = dtype_id, .id = alloc_id()};
    code_gen_.push_const_dtype(dconst);
    defs.push_back(dconst);
    return dconst.id;
}

#endif // YACCS_PROGRAM_H_
