#ifndef YACCS_BAKER_LAYER1_UTILS_H_
#define YACCS_BAKER_LAYER1_UTILS_H_

#include "yaccs/baker/layer1/def.hpp"
#include <cassert>
#include <cstdlib>
#include <limits>

template<typename T>
inline bool value_eq(T a, T b)
{
    return a == b;
}

template<>
inline bool value_eq<float>(float a, float b)
{
    return std::abs(a - b) < std::numeric_limits<float>::epsilon();
}

template<>
inline bool value_eq<double>(double a, double b)
{
    return std::abs(a - b) < std::numeric_limits<double>::epsilon();
}

inline bool should_decorate(StorageClass sc)
{
    switch (sc) {
    case SC_UNIFORM:            return true;
    case SC_STORAGE_BUFFER:     return true;
    default:                    return false;
    }
}

StorageClass storage_class_for_accessment(StorageClass sc);

const std::string& as_string(StorageClass sc);
const std::string& as_string(Decoration deco);
const std::string& as_string(BuiltIn built_in);
const std::string& as_string(BinaryOperator bo);
const std::string& as_string(CmpOp cmp_op);

#endif // YACCS_BAKER_LAYER1_UTILS_H_
