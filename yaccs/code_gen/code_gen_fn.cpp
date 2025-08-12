#include "yaccs/code_gen/code_gen.hpp"
#include "yaccs/baker/def.hpp"
#include "yaccs/baker/layer1/utils.hpp"
#include "yaccs/baker/utils.hpp"
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

void CodeGen::push_snippet_begin_if(const IfDef& def)
{
    auto condition_id{alloc_id()};

    this_fn_.body_ss << "\t%" << condition_id << " = OpUGreaterThan %" << def.bool_type_id
        << " %" << def.cmp_op1_id << " %" << def.cmp_op2_id << "\n";
    this_fn_.body_ss << "\t\tOpSelectionMerge %" << def.next_label_id << " None\n";
    this_fn_.body_ss << "\t\tOpBranchConditional %" << condition_id << " %"
        << def.body_label_id << " %" << def.next_label_id << "\n";
    this_fn_.body_ss << "\t%" << def.body_label_id << " = OpLabel\n";
}

void CodeGen::push_snippet_end_if(const IfDef& def)
{
    this_fn_.body_ss << "\t%" << def.next_label_id << " = OpLabel\n";
}

void CodeGen::push_snippet_begin_for(const ForLoopDef& for_def)
{
    auto i_id{alloc_id()};

    this_fn_.body_ss << "\t\tOpBranch %" << for_def.init_label_id << "\n";
    this_fn_.body_ss << "\t%" << for_def.init_label_id << " = OpLabel\n";
    this_fn_.body_ss << "\t\tOpLoopMerge %" << for_def.loop_exit_label_id << " %" << for_def.i_inc_label_id << " None\n";
    this_fn_.body_ss << "\t\tOpBranch %" << for_def.cond_label_id << "\n";
    // cmp
    this_fn_.body_ss << "\t%" << for_def.cond_label_id << " = OpLabel\n";
    this_fn_.body_ss << "\t%" << i_id << " = OpLoad %" << for_def.i_type_id << " %" << for_def.i_var_id << "\n";
    this_fn_.body_ss << "\t%" << for_def.cmp_id << " = " << as_string(for_def.cmp_op)
        << " %" << for_def.bool_type_id << " %" << i_id << " %" << for_def.i_boundary_id << "\n";
    this_fn_.body_ss << "\t\tOpBranchConditional %" << for_def.cmp_id << " %" << for_def.loop_body_label_id << " %" << for_def.loop_exit_label_id << "\n";
    this_fn_.body_ss << "\t%" << for_def.loop_body_label_id << " = OpLabel\n";
}


void CodeGen::push_snippet_end_for(const ForLoopDef& for_def)
{
    auto i_id{alloc_id()};
    auto i_inc_id{alloc_id()};

    this_fn_.body_ss << "\t\tOpBranch %" << for_def.i_inc_label_id << "\n";
    this_fn_.body_ss << "\t%" << for_def.i_inc_label_id << " = OpLabel\n";
    this_fn_.body_ss << "\t%" << i_id << " = OpLoad %" << for_def.i_type_id << " %" << for_def.i_var_id << "\n";
    this_fn_.body_ss << "\t%" << i_inc_id << " = OpIAdd %" << for_def.i_type_id << " %" << i_id << " %" << for_def.inc_amount_id << "\n";
    this_fn_.body_ss << "\t\tOpStore %" << for_def.i_var_id << " %" << i_inc_id << "\n";
    this_fn_.body_ss << "\t\tOpBranch %" << for_def.init_label_id <<"\n";
    this_fn_.body_ss << "\t%" << for_def.loop_exit_label_id << " = OpLabel\n";
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
