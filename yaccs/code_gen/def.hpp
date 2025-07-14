#ifndef YACCS_DEF_H_
#define YACCS_DEF_H_

#include <cstddef>
#include <cstdint>
#include <vector>

using id_t = uint32_t;

enum Decoration
{
    DECO_RELAXEDPRECISION = 1,
    DECO_SPECID,
    DECO_BLOCK,
}; // enum Decoration


struct ArrTypeDef {
    int length;
    id_t dtype;
    id_t length_id;
    id_t id;
}; // struct ArrTypeDef

struct StructTypeDef {
    std::vector<id_t> fields;
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

struct VarDef
{
    id_t id;
    id_t type_pointer_id;
}; // struct VarDef

struct TypePointerDef
{
    id_t id;
    id_t type_id;
}; // struct TypePointerDef

struct EntryDef
{
    id_t main_id;
    std::vector<id_t> input_ids;
}; // struct EntryDef


struct DecorateSetBindingDef
{
    id_t target;
    int binding;
    int set;
}; // struct SetBindingDef

struct DecorateStructDef
{
    struct FieldOffset
    {
        uint32_t field;
        uint32_t offset;
    }; // struct FieldOffset

    std::vector<FieldOffset> member_deco;
    id_t struct_type_id;
    Decoration deco;
}; // struct DecorateStructDef

#endif // YACCS_DEF_H_
