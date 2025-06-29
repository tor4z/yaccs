#ifndef YACCS_TYPES_H_
#define YACCS_TYPES_H_

#include "ops.hpp"
#include <cassert>
#include <cstdint>
#include <iostream>
#include <utility>
#include <vector>


struct OpTypeBase: public Op
{
    explicit OpTypeBase(DType t) : Op(true), dt(t) {}
    virtual std::string_view name() const = 0;

    const DType dt;
    uint32_t ret_type_id;
    uint32_t width;
    std::vector<uint32_t> param_type_ids;
}; // strut OpTypeBase


struct OpTypePointer: public Op
{
    explicit OpTypePointer(uint32_t type_id) : Op(true), type_id(type_id) {}
    std::string_view name() const { return "OpTypePointer"; };
    uint32_t type_id;
    StorageClass storage_class;
}; // strut OpTypePointer


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


template<>
struct OpType<DType::FLOAT>: public OpTypeBase
{
    OpType() : OpTypeBase(DType::FLOAT) {}
    virtual std::string_view name() const override { return "OpTypeFloat"; }
}; // struct OpType


template<DType DT>
struct OpTypeCreator
{
    explicit OpTypeCreator(std::vector<OpTypeBase*>& types) : types_(types) {}
    OpTypeBase* get_type()
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
    OpTypeBase* get_type(uint32_t u32) { return nullptr; }
    OpTypeBase* get_type(uint32_t u32, const std::vector<uint32_t>& u32s) { return nullptr; }
private:
    std::vector<OpTypeBase*>& types_;
}; // struct OpTypeCreator


template<>
struct OpTypeCreator<DType::FLOAT>
{
    explicit OpTypeCreator(std::vector<OpTypeBase*>& types) : types_(types) {}
    OpTypeBase* get_type() { return nullptr; }
    OpTypeBase* get_type(uint32_t u32)
    {
        for (auto t : types_) {
            if (t->dt == DType::FLOAT && t->width == u32) {
                return t;
            }
        }

        auto t{new OpType<DType::FLOAT>()};
        t->width = u32;
        types_.push_back(t);
        return t;
    }
    OpTypeBase* get_type(uint32_t u32, const std::vector<uint32_t>& u32s) { return nullptr; }
private:
    std::vector<OpTypeBase*>& types_;
}; // struct OpTypeCreator


template<>
struct OpTypeCreator<DType::FN>
{
    explicit OpTypeCreator(std::vector<OpTypeBase*>& types) : types_(types) {}
    OpTypeBase* get_type() { return nullptr; }
    OpTypeBase* get_type(uint32_t u32)
    {
        for (auto t : types_) {
            if (t->dt == DType::FN && t->ret_type_id == u32) {
                return t;
            }
        }

        auto t{new OpType<DType::FN>()};
        t->ret_type_id = u32;
        types_.push_back(t);
        return t;    
    }
    OpTypeBase* get_type(uint32_t u32, const std::vector<uint32_t>& u32s) { return nullptr; }
private:
    std::vector<OpTypeBase*>& types_;
}; // struct OpTypeCreator


const std::string& as_string(StorageClass sc);


#endif // YACCS_TYPES_H_
