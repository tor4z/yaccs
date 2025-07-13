#ifndef YACCS_DEF_H_
#define YACCS_DEF_H_

#include <cstddef>
#include <cstdint>
#include <vector>

#define MAX_STRUCT_FIELDS 16

using id_t = uint32_t;

struct ArrTypeDef {
    int length;
    id_t dtype;
    id_t length_id;
    id_t id;
}; // struct ArrTypeDef

struct StructTypeDef {
    id_t fields[MAX_STRUCT_FIELDS];
    id_t id;
    size_t num_fields;
}; // struct StructTypeDef

struct ConstCompositeDef {
    std::vector<id_t> elem_ids;
    id_t type_id;
    id_t id;
}; // struct ArrTypeDef

template<typename T>
struct DTypeConstDef
{
    T value;
    id_t dtype_id;
    id_t id;
}; // struct DtypeConstDef

struct FunctionTypeDef
{
    id_t return_type_id;
    id_t id;
}; // struct FuncTypeDef

struct FunctionHeaderDef
{
    id_t return_type_id;
    id_t function_type_id;
    id_t id;
}; // struct FunctionHeaderDef

struct DecorateSetBindingDef
{
    id_t target;
    int binding;
    int set;
}; // struct SetBindingDef

#endif // YACCS_DEF_H_
