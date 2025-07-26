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
    static id_t cnt{1};

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

#endif // YACCS_CODE_GEN_UTILS_H_
