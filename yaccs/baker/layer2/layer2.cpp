#include "yaccs/baker/layer2/layer2.hpp"
#include "yaccs/baker/def.hpp"
#include <cassert>


Layer2::Layer2(Layer1* layer1)
    : layer1_(layer1)
{
}

void Layer2::begin_function(FunctionDef& def, Type type)
{
    id_t return_type_id{};
    switch (type) {
    case T_VOID:    return_type_id = layer1_->add_void_type();
    default:        assert(false && "Not supported function return type");
    }

    def.id = layer1_->add_function_prologue(return_type_id);
}

void Layer2::end_function(FunctionDef& def)
{
    layer1_->add_function_epilogue();
}

void Layer2::begin_for(ForLoopDef& def)
{
    // init
    def.init_label_id = alloc_id();
    def.cond_label_id = alloc_id();
    def.loop_exit_label_id = alloc_id();
    def.loop_body_label_id = alloc_id();
    def.i_inc_label_id = alloc_id();
    def.cmp_id = alloc_id();
    def.inc_amount_id = layer1_->add_const(DT_UINT32, 1);
    def.bool_type_id = layer1_->add_dtype(DT_BOOL);
    def.i_type_id = layer1_->add_dtype(DT_UINT32);
    def.i_type_ptr_id = layer1_->add_type_pointer(def.i_type_id, SC_FUNCTION);
    def.i_var_id = layer1_->add_var(def.i_type_id, SC_FUNCTION, layer1_->add_const(DT_UINT32, 0));
    def.cmp_op = CO_LT;
    layer1_->code_gen()->push_snippet_begin_for(def);
}

void Layer2::end_for(ForLoopDef& def)
{
    layer1_->code_gen()->push_snippet_end_for(def);
}

void Layer2::begin_if(IfDef& def, id_t op1_id, CmpOp cmp_op, id_t op2_id)
{
    def.cmp_op1_id = op1_id;
    def.cmp_op2_id = op2_id;
    def.cmp_op = cmp_op;
    def.bool_type_id = layer1_->add_dtype(DT_BOOL);
    def.body_label_id = alloc_id();
    def.next_label_id = alloc_id();

    layer1_->code_gen()->push_snippet_begin_if(def);
}

void Layer2::end_if(IfDef& def)
{
    layer1_->code_gen()->push_snippet_end_if(def);
}
