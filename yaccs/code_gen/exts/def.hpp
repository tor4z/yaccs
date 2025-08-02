#ifndef YACCS_EXTS_DEF_H_
#define YACCS_EXTS_DEF_H_

#include "yaccs/code_gen/def.hpp"
namespace ext {

enum BinaryOperator
{
    BO_FMAX = 0,
}; // enum BinaryOperator

struct BinaryOpDef
{
    id_t result_id;
    id_t func_id;
    id_t type_id;
    id_t op1_id;
    id_t op2_id;
    id_t ext_id;
    BinaryOperator bo;
}; // struct BinaryOpDef

} // namespace ext

#endif // YACCS_EXTS_DEF_H_
