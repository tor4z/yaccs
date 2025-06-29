#ifndef YACCS_TYPES_H_
#define YACCS_TYPES_H_

#include "ops.hpp"
#include <cassert>
#include <cstdint>
#include <utility>
#include <vector>


struct OpTypeBase: public Op
{
    explicit OpTypeBase(DType t) : Op(true), dt(t) {}
    virtual std::string_view name() const = 0;
    const DType dt;
    uint32_t ret_type_id;
}; // strut OpTypeBase


template<DType T>
struct OpType: public OpTypeBase
{
    OpType() : OpTypeBase(DType::INVALID) {}
    virtual std::string_view name() const override { assert("Try to use invalid type."); }
}; // struct OpType


template<>
struct OpType<DType::VOID>: public OpTypeBase
{
    OpType() : OpTypeBase(DType::VOID) {}
    virtual std::string_view name() const override { return "OpTypeVoid"; }
}; // struct OpType


template<>
struct OpType<DType::FN>: public OpTypeBase
{
    OpType() : OpTypeBase(DType::FN) {}
    virtual std::string_view name() const override { return "OpTypeFunction"; }
}; // struct OpType


template<DType DT>
struct OpTypeCreator
{
    explicit OpTypeCreator(std::vector<OpTypeBase*>& types) : types_(types) {}
    OpTypeBase* get_type();
    OpTypeBase* get_type(uint32_t id);
private:
    std::vector<OpTypeBase*>& types_;
}; // struct OpTypeCreator


template<DType DT>
OpTypeBase* OpTypeCreator<DT>::get_type()
{
    for (auto t : types_) {
        if (t->dt == DT) {
            return t;
        }
    }

    auto t{new OpType<DT>()};
    types_.push_back(t);
    return t;
}


template<DType DT>
OpTypeBase* OpTypeCreator<DT>::get_type(uint32_t id)
{
    for (auto t : types_) {
        if (t->dt == DT && id == t->ret_type_id) {
            return t;
        }
    }

    auto t{new OpType<DT>()};
    t.ret_type_id = id;
    types_.push_back(t);
    return t;
}

#endif // YACCS_TYPES_H_
