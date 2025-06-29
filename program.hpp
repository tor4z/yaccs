#ifndef YACCS_PROGRAM_H_
#define YACCS_PROGRAM_H_

#include <cassert>
#include <cstdint>
#include <ostream>
#include <unordered_map>
#include <utility>
#include <vector>
#include "ops.hpp"
#include "types.hpp"

struct Program;


struct ProgCompo
{
    virtual void dump_spirv(std::ostream& os) const = 0;
}; // struct ProgCompo


struct Variable: public ProgCompo
{
private:
    Program* prog_;
    virtual void dump_spirv(std::ostream& os) const override;
}; // struct Variable


struct Function: public ProgCompo
{
    Variable* new_var();
    void as_entry();
    void set_ret_type(DType dt);
private:
    friend class Program;
    OpFunction* op_fn_;
    OpLabel* op_lab_;
    OpReturn* op_ret_;
    OpFunctionEnd* op_fend_;

    OpTypeBase* op_ret_type_;
    Program* prog_;

    virtual void dump_spirv(std::ostream& os) const override;
    explicit Function(Program* prog);
}; // class Function


struct Program: public ProgCompo
{
    Program();
    ~Program();
    Function* new_fn();
    Variable* new_var();
    template<typename ...Args>
    OpTypeBase* get_type(DType dt, Args&& ...args);
private:
    friend class Function;
    friend std::ostream& operator<<(std::ostream&, const Program&); 
    virtual void dump_spirv(std::ostream& os) const override;
    void prologue();
    void set_entry_id(uint32_t id);

    OpCapability* capa_;
    OpMemoryModel* mem_mode_;
    OpEntryPoint* entry_point_;
    std::vector<Function*> functions_;
    std::vector<OpTypeBase*> types_;
}; // class Program


std::ostream& operator<<(std::ostream& os, const Program& prag);


template<typename ...Args>
OpTypeBase* Program::get_type(DType dt, Args&& ...args)
{
    switch (dt) {
    case DType::VOID: return OpTypeCreator<DType::VOID>(types_).get_type(); 
    case DType::FN: return OpTypeCreator<DType::FN>(types_).get_type(args...); 
    default:
        assert("Not implement!");
    }
    return nullptr;
}

#endif // YACCS_PROGRAM_H_
