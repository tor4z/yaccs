#ifndef YACCS_PROGRAM_H_
#define YACCS_PROGRAM_H_

#include <cassert>
#include <cstdint>
#include <unordered_map>
#include <utility>
#include <vector>
#include "code_gen.hpp"
#include "ops.hpp"
#include "types.hpp"
#include "utils.hpp"

struct Program;
template<typename T>
struct ConstantComposite;

struct Variable: public trait::Dumpable
{
    Variable(Program* prog, uint32_t type_ptr);
    virtual ~Variable() override;

    void decorate(Decoration dec);
    uint32_t id();
    uint32_t type();
private:
    friend class Program;
    Program* prog_;
    OpVariable* op_var_;

    virtual void dump(std::ostream& os) const override;
}; // struct Variable


struct ConstValue : public trait::Dumpable
{
    virtual uint32_t id() = 0;
    virtual uint32_t type() = 0;
}; // struct ConstValue


template<typename T>
struct Constant: public ConstValue
{
    Constant(Program* prog, uint32_t type, T value);
    virtual ~Constant() override;

    virtual uint32_t id() override;
    virtual uint32_t type() override;
private:
    friend class Program;
    friend class ConstantComposite<T>;
    Program* prog_;
    OpConstant<T>* op_const_;

    virtual void dump(std::ostream& os) const override;
}; // struct Constant


template<typename T>
struct ConstantComposite: public ConstValue
{
    ConstantComposite(Program* prog, uint32_t type, const std::vector<T>& values);
    virtual ~ConstantComposite() override;

    virtual uint32_t id() override;
    virtual uint32_t type() override;
private:
    friend class Program;
    Program* prog_;
    OpConstantComposite* op_const_comp_;
    std::vector<Constant<T>*> consts_;

    virtual void dump(std::ostream& os) const override;
}; // struct ConstantComposite


struct Function: public trait::Dumpable
{
    virtual ~Function() override;
    Variable* new_var();
    void as_entry();
    void op_assign(Variable* to, Variable* from);
    void op_assign(Variable* to, ConstValue* from);
private:
    friend class Program;
    OpFunction* op_fn_;
    OpLabel* op_lab_;
    OpReturn* op_ret_;
    OpFunctionEnd* op_fend_;
    Program* prog_;

    uint32_t ret_type_;
    std::vector<uint32_t> params_;
    std::vector<Op*> ops_;

    uint32_t load(Variable* var);
    virtual void dump(std::ostream& os) const override;
    explicit Function(Program* prog, uint32_t ret_type, const std::vector<uint32_t>& params);
}; // class Function


struct Program: public trait::Dumpable
{
    Program();
    virtual ~Program() override;
    Function* new_fn(uint32_t ret_type, const std::vector<uint32_t>& params);
    Variable* new_var(uint32_t type, StorageClass sc);

    template<typename T>
    Constant<T>* new_const(uint32_t type, T value);
    template<typename T>
    ConstantComposite<T>* new_const(uint32_t type, std::vector<T>&& value);
    template<typename ...Args>
    uint32_t get_type(DType dt, Args&& ...args);

    void prologue();
    void set_entry_id(uint32_t id, const std::vector<uint32_t>& params);
    Type* get_op_type(uint32_t type_id);
    uint32_t get_type_ptr_id(uint32_t type_id, StorageClass sc);
    uint32_t add_decoration(uint32_t var_id, const Decoration& dec);
    OpTypePointer* get_type_ptr(uint32_t id);

    virtual void dump(std::ostream& os) const override;
private:
    friend std::ostream& operator<<(std::ostream&, const Program&); 

    OpCapability* capa_;
    OpMemoryModel* mem_mode_;
    OpEntryPoint* entry_point_;

    std::vector<Dumpable*> const_comps_;
    std::vector<Dumpable*> consts_;
    std::vector<Function*> fns_;
    std::vector<Variable*> vars_;
    std::vector<OpDecorate*> decs_;
    std::vector<Type*> types_;
    std::vector<OpTypePointer*> type_ptrs_;
}; // class Program


std::ostream& operator<<(std::ostream& os, const Program& prag);


template<typename ...Args>
uint32_t Program::get_type(DType dt, Args&& ...args)
{
#define CREATE_GET_TYPE_ID(DT) \
    case DT: return OpTypeCreator<DT>(types_).get_type(args...)->id

    switch (dt) {
    CREATE_GET_TYPE_ID(DType::VOID);
    CREATE_GET_TYPE_ID(DType::FLOAT);
    CREATE_GET_TYPE_ID(DType::VEC);
    CREATE_GET_TYPE_ID(DType::FN);
    CREATE_GET_TYPE_ID(DType::STRUCT);
    default: NOT_IMPLEMENTED("get_type");
    }
    return 0;
#undef CREATE_GET_TYPE_ID
}


template<typename T>
Constant<T>* Program::new_const(uint32_t type, T value)
{
    auto v{new Constant<T>(this, type, value)};
    consts_.push_back(v);
    return v;
}


template<typename T>
ConstantComposite<T>* Program::new_const(uint32_t type, std::vector<T>&& value)
{
    auto v{new ConstantComposite<T>(this, type, value)};
    const_comps_.push_back(v);
    return v;
}


template <typename T>
Constant<T>::Constant(Program* prog, uint32_t type, T value)
    : prog_(prog)
{
    op_const_ = new OpConstant<T>();
    op_const_->type_id = type;
    op_const_->value = value;
}


template <typename T>
Constant<T>::~Constant()
{
    delete op_const_;
}


template <typename T>
uint32_t Constant<T>::id()
{
    return op_const_->id;
}


template <typename T>
uint32_t Constant<T>::type()
{
    return op_const_->type_id;
}


template <typename T>
void Constant<T>::dump(std::ostream& os) const
{
    CodeGen::gen(os, op_const_);
}


template <typename T>
ConstantComposite<T>::ConstantComposite(Program* prog, uint32_t type, const std::vector<T>& values)
    : prog_(prog)
{
    op_const_comp_ = new OpConstantComposite();
    op_const_comp_->type_id = type;
    auto comp_type{prog_->get_op_type(type)};

    for (auto v : values) {
        auto c{new Constant<T>(prog, comp_type->type()->comp_type, v)};
        op_const_comp_->comps.push_back(c->id());
        consts_.push_back(c);
    }
}


template <typename T>
ConstantComposite<T>::~ConstantComposite()
{
    delete op_const_comp_;
    for (auto c : consts_) {
        delete c;
    }
    consts_.clear();
}


template <typename T>
uint32_t ConstantComposite<T>::id()
{
    return op_const_comp_->id();
}


template <typename T>
uint32_t ConstantComposite<T>::type()
{
    return op_const_comp_->type_id;
}


template <typename T>
void ConstantComposite<T>::dump(std::ostream& os) const
{
    for (auto c : consts_) {
        c->dump(os);
    }
    CodeGen::gen(os, op_const_comp_);
}

#endif // YACCS_PROGRAM_H_
