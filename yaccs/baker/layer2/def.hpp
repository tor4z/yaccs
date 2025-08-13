#ifndef YACCS_BAKER_LAYER2_DEF_H_
#define YACCS_BAKER_LAYER2_DEF_H_

#include "yaccs/baker/def.hpp"
#include "yaccs/baker/layer1/def.hpp"

struct FunctionDef
{
    id_t id;
}; // struct FunctionDef

struct ForLoopDef
{
    id_t i_var_id;
    id_t i_type_ptr_id;
    id_t i_boundary_id;
    id_t inc_amount_id;
    
    id_t init_label_id;
    id_t loop_body_label_id;
    id_t cond_label_id;
    id_t i_inc_label_id;
    id_t loop_exit_label_id;
    
    id_t i_type_id;
    id_t bool_type_id;
    id_t cmp_id;
    CmpOp cmp_op;
}; // struct ForLoopDef

struct IfDef
{
    id_t cmp_op1_id;
    id_t cmp_op2_id;
    id_t bool_type_id;
    id_t body_label_id;
    id_t next_label_id;
    CmpOp cmp_op;
}; // IfDef

#endif // YACCS_BAKER_LAYER2_DEF_H_
