#ifndef YACCS_CODE_GEN_UTILS_H_
#define YACCS_CODE_GEN_UTILS_H_

#include "yaccs/code_gen/def.hpp"
#include <cassert>
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

#endif // YACCS_CODE_GEN_UTILS_H_
