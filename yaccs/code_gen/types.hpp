#ifndef YACCS_TYPES_H_
#define YACCS_TYPES_H_

#include "base.hpp"
#include "def.hpp"
#include <cassert>
#include <cstdint>
#include <iostream>
#include <utility>
#include <vector>


struct OpTypeBase: public Op
{
    explicit OpTypeBase(Op::Type op_type, DType t) : Op(op_type, true), dt(t) {}

    const DType dt;
    id_t ret_type_id;
    uint32_t width;
    uint32_t comp_type;
    uint32_t comp_count;
    std::vector<id_t> param_type_ids;
}; // strut OpTypeBase


struct OpTypePointer: public Op
{
    explicit OpTypePointer(uint32_t type_id) : Op(Op::TYPE_POINTER, true), type_id(type_id) {}
    id_t type_id;
    StorageClass storage_class;
}; // strut OpTypePointer


template<DType T>
struct OpType: public OpTypeBase
{
    OpType() : OpTypeBase(Op::INVALID, DType::INVALID) {}
}; // struct OpType


template<>
struct OpType<DType::VOID>: public OpTypeBase
{
    OpType() : OpTypeBase(Op::TYPE_VOID, DType::VOID) {}
}; // struct OpType


template<>
struct OpType<DType::FN>: public OpTypeBase
{
    OpType() : OpTypeBase(Op::TYPE_FUNCTION, DType::FN) {}
}; // struct OpType


template<>
struct OpType<DType::FLOAT>: public OpTypeBase
{
    OpType() : OpTypeBase(Op::TYPE_FLOAT, DType::FLOAT) {}
}; // struct OpType


template<>
struct OpType<DType::VEC>: public OpTypeBase
{
    OpType() : OpTypeBase(Op::TYPE_VECTOR, DType::VEC) {}
}; // struct OpType


template<>
struct OpType<DType::STRUCT>: public OpTypeBase
{
    OpType() : OpTypeBase(Op::TYPE_STRUCT, DType::STRUCT) {}
}; // struct OpType


struct Type : public trait::Dumpable, public trait::Decoratable, public trait::Identable
{
    virtual ~Type() override { delete type_; }
    virtual id_t id() override { return type_->id(); }
    virtual void decorate(Decoration dec) override;
    virtual void dump(std::ostream& os) const override;
    OpTypeBase*& type() { return type_; }
private:
    OpTypeBase* type_;
}; // struct Type


template<DType DT>
struct OpTypeCreator
{
    explicit OpTypeCreator(std::vector<Type*>& types) : types_(types) {}
    Type* get_type()
    {
        for (auto t : types_) {
            if (t->type()->dt == DT) {
                return t;
            }
        }

        auto t{new OpType<DT>()};
        types_.push_back(t);
        return t;    
    }
    Type* get_type(uint32_t) { return nullptr; }
    Type* get_type(uint32_t, uint32_t) { return nullptr; }
    Type* get_type(const std::vector<uint32_t>&) { return nullptr; }
private:
    std::vector<Type*>& types_;
}; // struct OpTypeCreator


template<>
struct OpTypeCreator<DType::FLOAT>
{
    explicit OpTypeCreator(std::vector<Type*>& types) : types_(types) {}
    Type* get_type(uint32_t width)
    {
        for (auto t : types_) {
            if (t->type()->dt == DType::FLOAT && t->type()->width == width) {
                return t;
            }
        }

        auto t{new Type()};
        t->type() = new OpType<DType::FLOAT>();
        t->type()->width = width;
        types_.push_back(t);
        return t;
    }
    Type* get_type() { return nullptr; }
    Type* get_type(uint32_t, uint32_t) { return nullptr; }
    Type* get_type(const std::vector<uint32_t>&) { return nullptr; }
private:
    std::vector<Type*>& types_;
}; // struct OpTypeCreator


template<>
struct OpTypeCreator<DType::VEC>
{
    explicit OpTypeCreator(std::vector<Type*>& types) : types_(types) {}
    Type* get_type(uint32_t comp_type, uint32_t comp_count)
    {
        for (auto t : types_) {
            if (t->type()->dt == DType::VEC && t->type()->comp_type == comp_type &&
                t->type()->comp_count == comp_count) {
                return t;
            }
        }


        auto t{new Type()};
        t->type() = new OpType<DType::VEC>();
        t->type()->comp_type = comp_type;
        t->type()->comp_count = comp_count;
        types_.push_back(t);
        return t;
    }
    Type* get_type() { return nullptr; }
    Type* get_type(uint32_t) { return nullptr; }
    Type* get_type(const std::vector<uint32_t>&) { return nullptr; }
private:
    std::vector<Type*>& types_;
}; // struct OpTypeCreator


template<>
struct OpTypeCreator<DType::STRUCT>
{
    explicit OpTypeCreator(std::vector<Type*>& types) : types_(types) {}
    Type* get_type(const std::vector<uint32_t>& types)
    {
        for (auto t : types_) {
            if (t->type()->dt == DType::STRUCT && t->type()->param_type_ids == types) {
                return t;
            }
        }

        auto t{new Type()};
        t->type() = new OpType<DType::STRUCT>();
        t->type()->param_type_ids = types;
        types_.push_back(t);
        return t;
    }
    Type* get_type() { return nullptr; }
    Type* get_type(uint32_t u32) { return nullptr; }
    Type* get_type(uint32_t comp_type, uint32_t comp_count) { return nullptr; }
private:
    std::vector<Type*>& types_;
}; // struct OpTypeCreator


template<>
struct OpTypeCreator<DType::FN>
{
    explicit OpTypeCreator(std::vector<Type*>& types) : types_(types) {}
    Type* get_type(uint32_t ret_type_id)
    {
        for (auto t : types_) {
            if (t->type()->dt == DType::FN && t->type()->ret_type_id == ret_type_id) {
                return t;
            }
        }

        auto t{new Type()};
        t->type() = new OpType<DType::FN>();
        t->type()->ret_type_id = ret_type_id;
        types_.push_back(t);
        return t;    
    }
    Type* get_type() { return nullptr; }
    Type* get_type(uint32_t, uint32_t) { return nullptr; }
    Type* get_type(const std::vector<uint32_t>&) { return nullptr; }
private:
    std::vector<Type*>& types_;
}; // struct OpTypeCreator

#endif // YACCS_TYPES_H_
