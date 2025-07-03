#ifndef YACCS_DEF_H_
#define YACCS_DEF_H_

#include <cstdint>


using id_t = uint32_t;


enum class DType
{
    INVALID = 0,
    FN,
    VOID,
    FLOAT,
    VEC,
    INT,
    STRUCT,
}; // enum class DType


enum class StorageClass
{
    UNIFORM_CONSTANT = 0,
    INPUT,
    UNIFORM,
    OUTPUT,
    WORKGROUP,
    CROSS_WORKGROUP,
    PRIVATE,
    FUNCTION,
    GENERIC,
    PUSH_CONSTANT,
    ATOMIC_COUNTER,
    IMAGE,
    STORAGE_BUFFER,
    TILE_IMAGE_EXT,
    NODE_PAYLOAD_AMDX,
}; // enum class StorageClass


struct Decoration
{
    enum Type {
        RELAXED_PRECISION = 0,
        SPECID,
        BLOCK,
        BUFFER_BLOCK,
        ROW_MAJOR,
        COL_MAJOR,
        ARRAY_STRIDE,
        MATRIX_STRIDE,
        GLSL_SHARED,
        GLSL_PACKED,
        CPACKED,
        BUILTIN,
        NO_PERSPECTIVE,
        FLAT,
        PATCH,
        CENTROID,
        SAMPLE,
        INVARIANT,
        RESTRICT,
        ALIASED,
        VOLATILE,
        CONSTANT,
        COHERENT,
        NON_WRITABLE,
        NON_READABLE,
        UNIFORM,
        UNIFORMID,
        SATURATED_CONVERSION,
        STREAM,
        LOCATION,
        COMPONENT,
        INDEX,
        BINDING,
        DESCRIPTOR_SET,
        OFFSET,
        XFB_BUFFER,
        XFB_STRIDE,
    }; // enum Type

    Decoration() {}
    Decoration(Type t, uint32_t p) : type(t), param(p) {}
    Type type;
    uint32_t param;
}; // Decoration


#endif // YACCS_DEF_H_
