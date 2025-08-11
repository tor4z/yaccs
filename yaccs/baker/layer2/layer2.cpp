#include "yaccs/baker/layer2/layer2.hpp"


Layer2::Layer2()
{
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
    def.inc_amount_id = layer1_.add_const(DT_UINT32, 1);
    def.bool_type_id = layer1_.add_dtype(DT_BOOL);
    def.i_type_id = layer1_.add_dtype(DT_UINT32);
    def.i_type_ptr_id = layer1_.add_type_pointer(def.i_type_id, SC_FUNCTION);
    def.i_var_id = layer1_.add_var(def.i_type_id, SC_FUNCTION, layer1_.add_const(DT_UINT32, 0));
    def.cmp_op = CO_LT;
    code_gen_.push_snippet_begin_for(def);
}

void Layer2::end_for(ForLoopDef& def)
{
    code_gen_.push_snippet_end_for(def);
}

void Layer2::begin_if(IfDef& def, id_t op1_id, CmpOp cmp_op, id_t op2_id)
{
    def.cmp_op1_id = op1_id;
    def.cmp_op2_id = op2_id;
    def.cmp_op = cmp_op;
    def.bool_type_id = layer1_.add_dtype(DT_BOOL);
    def.body_label_id = alloc_id();
    def.next_label_id = alloc_id();

    code_gen_.push_snippet_begin_if(def);
}

void Layer2::end_if(IfDef& def)
{
    code_gen_.push_snippet_end_if(def);
}
