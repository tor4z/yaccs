#include "yaccs/code_gen/code_gen.hpp"
#include "yaccs/code_gen/def.hpp"
#include "yaccs/code_gen/utils.hpp"
#include "yaccs/dtype.hpp"
#include <cassert>
#include <cstddef>


CodeGen::CodeGen() {}

void CodeGen::push_header()
{
    header_ss_ << "OpCapability Shader\n";
    header_ss_ << "OpMemoryModel Logical GLSL450\n";
}

void CodeGen::push_entry(const EntryDef& ed)
{
    entry_def_ss_ << "OpEntryPoint GLCompute %" << ed.main_id << " \"main\"";
    for (auto it: ed.input_ids) {
        entry_def_ss_ << " %" << it;
    }
    entry_def_ss_ << "\n";

    entry_def_ss_ << "OpExecutionMode %" << ed.main_id << " LocalSize "
        << ed.local_size_x << " " << ed.local_size_y << " " << ed.local_size_z << "\n";
}

void CodeGen::push_struct_decorate(const DecorateStructDef& dsd)
{
    if (dsd.deco != DECO_NONE) {
        decorate_ss_ <<  "OpDecorate %" << dsd.struct_type_id << " " << as_string(dsd.deco) << "\n";
    }

    for (const auto& it : dsd.member_deco) {
        decorate_ss_ << "OpMemberDecorate %" << dsd.struct_type_id << " " << it.field << " Offset " << it.offset << "\n";
    }
}

void CodeGen::push_array_decorate(const DecorateArrayDef& dad)
{
    decorate_ss_ << "OpDecorate %" << dad.array_type_id << " ArrayStride 16\n";
}

void CodeGen::push_builtin_decorate(const DecorateBuiltInDef& built_in)
{
    decorate_ss_ << "OpDecorate %" << built_in.var_id << " BuiltIn " << as_string(built_in.built_in) << "\n";
}

void CodeGen::push_dtype(DType dt, id_t id)
{
    switch (dt) {
    case DT_FLOAT:
        type_const_def_ss_ << "%" << id << " = OpTypeFloat 32\n";
        break;
    case DT_FLOAT16:
        type_const_def_ss_ << "%" << id << " = OpTypeFloat 16\n";
        break;
    case DT_INT32:
        type_const_def_ss_ << "%" << id << " = OpTypeInt 32 1\n";
        break;
    case DT_UINT32:
        type_const_def_ss_ << "%" << id << " = OpTypeInt 32 0\n";
        break;
    case DT_BOOL:
        type_const_def_ss_ << "%" << id << " = OpTypeBool\n";
        break;
    default: assert(false && "Unsupported type");
    }
}

void CodeGen::push_array_dtype(const ArrTypeDef& arr)
{
    type_const_def_ss_ << "%" << arr.id << " = OpTypeArray %" << arr.dtype << " %" << arr.length_id << "\n";
}

void CodeGen::push_struct_dtype(const StructTypeDef& sd)
{
    type_const_def_ss_ << "%" << sd.id << " = OpTypeStruct ";
    for (size_t i = 0; i < sd.num_fields; ++i) {
        type_const_def_ss_ << "%" << sd.fields[i];
        if (i != sd.num_fields - 1) type_const_def_ss_ << " ";
    }
    type_const_def_ss_ << "\n";
}

void CodeGen::push_void_type(id_t id)
{
    type_const_def_ss_ << "%" << id << " = OpTypeVoid\n";
}

void CodeGen::push_function_type(const FunctionTypeDef& ft)
{
    type_const_def_ss_ << "%" << ft.id << " = OpTypeFunction %" << ft.return_type_id << "\n";
}

void CodeGen::push_const_composite(const ConstCompositeDef& ccd)
{
    type_const_def_ss_ << "%" << ccd.id << " = OpConstantComposite %" << ccd.type_id;
    for (auto it : ccd.elem_ids) {
        type_const_def_ss_ << " %" << it;
    }
    type_const_def_ss_ << "\n";
}

void CodeGen::push_type_pointer(const TypePointerDef& tp)
{
    type_const_def_ss_ << "%" << tp.id << " = OpTypePointer "
        << as_string(tp.storage_class) << " %" << tp.type_id << "\n";
}

void CodeGen::push_variable(const VarDef& var)
{
    if (var.storage_class == SC_FUNCTION) {
        fn_def_ss_ << "\t%" << var.id << " = OpVariable %" << var.type_pointer_id
            << " " << as_string(var.storage_class) << "\n";
    } else {
        type_const_def_ss_ << "%" << var.id << " = OpVariable %" << var.type_pointer_id
            << " " << as_string(var.storage_class) << "\n";
    }
}

void CodeGen::push_function(const FunctionHeaderDef& fh)
{
    fn_def_ss_ << "\n";
    fn_def_ss_ << "%" << fh.id << " = OpFunction " << "%" << fh.return_type_id
        << " None %" << fh.function_type_id << "\n";
}

void CodeGen::push_label(id_t id)
{
    fn_def_ss_ << "\t%" << id << " = OpLabel\n";
}

void CodeGen::push_return()
{
    fn_def_ss_ << "\tOpReturn\n";
}

void CodeGen::push_function_end()
{
    fn_def_ss_ << "\tOpFunctionEnd\n";
}

void CodeGen::push_function_call(const FunctionCallDef& fcd)
{
    fn_def_ss_ << "\t%" << fcd.id << " = OpFunctionCall %" << fcd.return_type_id << " %" << fcd.func_id << "\n";
}

void CodeGen::push_control_barrier(const ControlBarrierDef& cbd)
{
    fn_def_ss_ << "\t\tOpControlBarrier %"
        << cbd.exe_scope_id << " %" << cbd.mem_scope_id << " %" << cbd.mem_semantics_id << "\n";
}

void CodeGen::push_decorate_set_binding(const DecorateSetBindingDef& deco)
{
    assert(deco.binding >= 0 && deco.set >= 0 && "Bad decoration");
    decorate_ss_ << "OpDecorate %" << deco.target << " DescriptorSet " << deco.set << "\n";
    decorate_ss_ << "OpDecorate %" << deco.target << " Binding " << deco.binding << "\n";
}

void CodeGen::push_vector_dtype(const VectorDef& vd)
{
    type_const_def_ss_ << "%" << vd.id << " = OpTypeVector %" << vd.component_type_id << " " << vd.count << "\n";
}

void CodeGen::push_binary_operation(const BinaryOpDef& bod)
{
    fn_def_ss_ << "%" << bod.result_id << " = " << as_string(bod.bo) << " %" << bod.type_id
        << " %" << bod.op1_id << " %" << bod.op2_id << "\n";
}

void CodeGen::push_load(const LoadDef& ld)
{
    fn_def_ss_ << "\t%" << ld.id << " = OpLoad %" << ld.type_id << " %" << ld.pointer << "\n";
}

void CodeGen::push_store(const StoreDef& sd)
{
    fn_def_ss_ << "\tOpStore %" << sd.pointer << " %" << sd.object << "\n";
}

void CodeGen::push_access_chain(const AccessChainDef& acd)
{
    fn_def_ss_ << "\t%" << acd.id << " = OpAccessChain %" << acd.type_id << " %" << acd.base_id;
    for (auto it: acd.index_ids) {
        fn_def_ss_ << " %" << it;
    }
    fn_def_ss_ << "\n";
}

void CodeGen::push_snippet_invo_bound_check(const InvocationBoundCheckDef& def)
{
    fn_def_ss_ << "\t%" << def.condition_id << " = OpUGreaterThan %" << def.bool_type_id
        << " %" << def.invo_comp_id << " %" << def.tensor_shape_comp_id << "\n";
    fn_def_ss_ << "\t\tOpSelectionMerge %" << def.label_id_next << " None\n";
    fn_def_ss_ << "\t\tOpBranchConditional %" << def.condition_id << " %"
        << def.label_id_ret << " %" << def.label_id_next << "\n";
    fn_def_ss_ << "\t%" << def.label_id_ret << " = OpLabel\n";
    fn_def_ss_ << "\t\tOpReturn\n";
    fn_def_ss_ << "\t%" << def.label_id_next << " = OpLabel\n";
}

void CodeGen::assemble(std::ofstream& ofs)
{
    ofs << header_ss_.str();
    ofs << entry_def_ss_.str();
    ofs << decorate_ss_.str();
    ofs << type_const_def_ss_.str();
    ofs << fn_def_ss_.str();
}
