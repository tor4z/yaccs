#ifndef YACCS_DEF_H_
#define YACCS_DEF_H_

#include <cstddef>
#include <cstdint>

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

template<typename T>
struct DTypeConstDef
{
    T value;
    id_t dtype_id;
    id_t id;
}; // struct DtypeConstDef

#endif // YACCS_DEF_H_
