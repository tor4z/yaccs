#include "yaccs/baker/utils.hpp"
#include <mutex>


id_t alloc_id()
{
    static std::mutex locker;
    static id_t cnt{1}; // mark 0 as invalid id

    std::lock_guard<std::mutex> guard(locker);
    id_t result{cnt};
    ++cnt;
    assert(cnt <= std::numeric_limits<id_t>::max());
    return result;
}


uint32_t shape_to_dsize(int dims, Shape shape)
{
    int data_size{1};
    for (int i = 0; i < dims; ++i) {
        data_size *= shape[i];
    }
    return data_size;
}
