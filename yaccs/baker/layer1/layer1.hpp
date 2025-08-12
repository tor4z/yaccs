#ifndef YACCS_BAKER_LAYER1_H_
#define YACCS_BAKER_LAYER1_H_

#include "yaccs/baker/def.hpp"
#include "yaccs/baker/layer1/exts/exts.hpp"
#include "yaccs/code_gen/code_gen.hpp"
#include "yaccs/baker/utils.hpp"
#include "yaccs/dtype.hpp"
#include <cstdint>
#include <unordered_map>
#include <utility>
#include <vector>


struct Layer1
{
    Layer1();
    // function component
    void add_function_epilogue();
    id_t add_function_prologue(id_t return_type_id);
    void add_return();
    id_t add_label();
    id_t add_function_call(id_t id);
    id_t load_var(id_t dtype_id, id_t pointer);
    void store_var(id_t pointer, id_t object);
    id_t access_chain_indices(id_t func_id, id_t type_id, id_t base_id, const std::vector<uint32_t>& indices);
    id_t access_chain(id_t func_id, id_t type_id, id_t base_id, const std::vector<id_t>& indices);
    void add_control_barrier(Scope exe_scope, Scope mem_scope, MemSemantic mem_semantics);
    id_t access_invocation_index(id_t func_id, uint32_t index);
    id_t global_invocation_id();

    // type def
    id_t add_void_type();
    id_t add_function_type(id_t return_type_id);
    id_t add_struct_dtype(const std::vector<id_t>& dtypes, bool reuse=true);
    id_t add_vector_dtype(id_t component_type_id, int count);
    id_t add_array_dtype(id_t dtype, uint32_t length, StorageClass sc, bool reuse=true);
    id_t add_dtype(DType dtype);

    void set_entry(id_t main_id);
    void add_binding(id_t var_id, int binding, int set);
    void add_struct_decorate(id_t type_id, Decoration deco, StorageClass sc,
        const std::vector<std::pair<uint32_t, uint32_t>>& member_deco);

    // variable & const
    template<typename T>
    id_t add_const(DType dtype, T value);
    id_t add_type_pointer(id_t type_id, StorageClass sc);
    id_t add_const_array(id_t arr_type, const std::vector<id_t>& elem_ids);
    id_t add_const_struct(id_t struct_id, const std::vector<id_t>& elem_ids);
    id_t add_var(id_t type_id, StorageClass sc, id_t initializer = 0);

    // arithmatic
    id_t binary_op(BinaryOperator bo, id_t func_id, id_t type_id, id_t op1_id, id_t op2_id);

    ext::Ext* std450() { return &std450_; }
    CodeGen* code_gen() { return &code_gen_; }
    void push_entry_listed_id(id_t id);
    FunctionHeaderDef& find_function_def(id_t id);
private:
    std::vector<id_t> entry_listed_ids_;
    std::unordered_map<id_t, FunctionHeaderDef> global_funcs_;
    CodeGen code_gen_;
    ext::Ext std450_;
}; // struct Layer1

template<typename T>
id_t Layer1::add_const(DType dtype, T value)
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

#endif // YACCS_BAKER_LAYER1_H_
