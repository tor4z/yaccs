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
    void push_entry(const EntryDef& ed);
    void push_struct_decorate(const DecorateStructDef& dsd);
    void push_array_decorate(const DecorateArrayDef& dad);
    void push_builtin_decorate(const DecorateBuiltInDef& built_in);
    void push_label(id_t id);
    void push_return();
    void push_function_end();
    void push_dtype(DType dt, id_t id);
    void push_function(const FunctionHeaderDef& fh);
    void push_array_dtype(const ArrTypeDef& arr);
    void push_struct_dtype(const StructTypeDef& sd);
    void push_void_type(id_t id);
    void push_function_type(const FunctionTypeDef& ft);
    void push_decorate_set_binding(const DecorateSetBindingDef& deco);
    void push_const_composite(const ConstCompositeDef& ccd);
    void push_variable(const VarDef& var);
    void push_type_pointer(const TypePointerDef& tp);
    void push_function_call(const FunctionCallDef& fcd);
    void push_control_barrier(const ControlBarrierDef& cbd);
    void push_vector_dtype(const VectorDef& vd);
    void push_load(const LoadDef& ld);
    void push_store(const StoreDef& sd);
    void push_access_chain(const AccessChainDef& acd);
    template<typename T>
    void push_const_dtype(const DTypeConstDef<T>& dconst);
private:
    std::stringstream header_ss_;
    std::stringstream entry_def_ss_;
    std::stringstream decorate_ss_;
    std::stringstream type_const_def_ss_;
    std::stringstream fn_def_ss_;
}; // class CodeGen

template<typename T>
void CodeGen::push_const_dtype(const DTypeConstDef<T>& dconst)
{
    type_const_def_ss_ << "%" << dconst.id << " = OpConstant %" << dconst.dtype_id << " " << dconst.value << "\n";
}

#endif // YACCS_CODE_GEN_H_
