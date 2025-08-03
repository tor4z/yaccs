#include "yaccs/code_gen/code_gen.hpp"
#include "yaccs/code_gen/def.hpp"
#include "yaccs/code_gen/utils.hpp"
#include <cassert>

void CodeGen::push_function(const FunctionHeaderDef& fh)
{
    this_fn_.prologue_ss << "\n";
    this_fn_.prologue_ss << "%" << fh.id << " = OpFunction " << "%" << fh.return_type_id
        << " None %" << fh.function_type_id << "\n";
    this_fn_.prologue_ss << "\t%" << fh.open_label_id << " = OpLabel\n";
}

void CodeGen::push_function_end()
{
    this_fn_.epilogue_ss << "\t\tOpReturn\n";
    this_fn_.epilogue_ss << "\t\tOpFunctionEnd\n";

    fn_def_ss_ << this_fn_.prologue_ss.str();
    fn_def_ss_ << this_fn_.var_def_ss.str();
    fn_def_ss_ << this_fn_.body_ss.str();
    fn_def_ss_ << this_fn_.epilogue_ss.str();
    this_fn_.clear();
}

void CodeGen::push_return()
{
    this_fn_.body_ss << "\t\tOpReturn\n";
}

void CodeGen::push_label(id_t id)
{
    this_fn_.body_ss << "\t%" << id << " = OpLabel\n";
}

void CodeGen::push_function_call(const FunctionCallDef& fcd)
{
    this_fn_.body_ss << "\t%" << fcd.id << " = OpFunctionCall %" << fcd.return_type_id << " %" << fcd.func_id << "\n";
}

void CodeGen::push_control_barrier(const ControlBarrierDef& cbd)
{
    this_fn_.body_ss << "\t\tOpControlBarrier %"
        << cbd.exe_scope_id << " %" << cbd.mem_scope_id << " %" << cbd.mem_semantics_id << "\n";
}

void CodeGen::push_binary_operation(const BinaryOpDef& bod)
{
    this_fn_.body_ss << "\t%" << bod.result_id << " = " << as_string(bod.bo) << " %" << bod.type_id
        << " %" << bod.op1_id << " %" << bod.op2_id << "\n";
}

void CodeGen::push_load(const LoadDef& ld)
{
    this_fn_.body_ss << "\t%" << ld.id << " = OpLoad %" << ld.type_id << " %" << ld.pointer << "\n";
}

void CodeGen::push_store(const StoreDef& sd)
{
    this_fn_.body_ss << "\t\tOpStore %" << sd.pointer << " %" << sd.object << "\n";
}

void CodeGen::push_access_chain(const AccessChainDef& acd)
{
    this_fn_.body_ss << "\t%" << acd.id << " = OpAccessChain %" << acd.type_id << " %" << acd.base_id;
    for (auto it: acd.index_ids) {
        this_fn_.body_ss << " %" << it;
    }
    this_fn_.body_ss << "\n";
}

void CodeGen::push_snippet_invo_bound_check(const InvocationBoundCheckDef& def)
{
    this_fn_.body_ss << "\t%" << def.condition_id << " = OpUGreaterThan %" << def.bool_type_id
        << " %" << def.invo_comp_id << " %" << def.tensor_shape_comp_id << "\n";
    this_fn_.body_ss << "\t\tOpSelectionMerge %" << def.label_id_next << " None\n";
    this_fn_.body_ss << "\t\tOpBranchConditional %" << def.condition_id << " %"
        << def.label_id_ret << " %" << def.label_id_next << "\n";
    this_fn_.body_ss << "\t%" << def.label_id_ret << " = OpLabel\n";
    this_fn_.body_ss << "\t\tOpReturn\n";
    this_fn_.body_ss << "\t%" << def.label_id_next << " = OpLabel\n";
}

void CodeGen::FnCodeGen::clear()
{
    prologue_ss.str("");
    var_def_ss.str("");
    body_ss.str("");
    epilogue_ss.str("");

    prologue_ss.clear();
    var_def_ss.clear();
    body_ss.clear();
    epilogue_ss.clear();
}
