#ifndef YACCS_CODE_GEN_UTILS_H_
#define YACCS_CODE_GEN_UTILS_H_

#include "yaccs/code_gen/def.hpp"
#include <cassert>
#include <cstdlib>
#include <limits>
#include <mutex>

inline id_t alloc_id()
{
    static std::mutex locker;
    static id_t cnt{1}; // mark 0 as invalid id

    std::lock_guard<std::mutex> guard(locker);
    id_t result{cnt};
    ++cnt;
    assert(cnt <= std::numeric_limits<id_t>::max());
    return result;
}

inline int shape_to_dsize(int dims, const int shape[])
{
    int data_size{1};
    for (int i = 0; i < dims; ++i) {
        data_size *= shape[i];
    }
    return data_size;
}

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

#endif // YACCS_CODE_GEN_UTILS_H_
