#include "ops.hpp"
#include "program.hpp"
#include <cstdint>
#include <mutex>


uint32_t alloc_id()
{
    static uint32_t id_cnt{0};
    static std::mutex locker;

    std::lock_guard<std::mutex> guard{locker};
    auto id{id_cnt++};
    return id;
}


Op::Op(bool has_ret)
    : id(alloc_id())
    , has_ret(has_ret)
{}
