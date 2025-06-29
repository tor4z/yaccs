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
    Variable(Program* prog, uint32_t type_ptr);
    ~Variable();
private:
    friend class Program;
    Program* prog_;
    OpVariable* op_var_;

    virtual void dump_spirv(std::ostream& os) const override;
}; // struct Variable


struct Function: public ProgCompo
{
    Variable* new_var();
    void as_entry();
private:
    friend class Program;
    OpFunction* op_fn_;
    OpLabel* op_lab_;
    OpReturn* op_ret_;
    OpFunctionEnd* op_fend_;
    Program* prog_;

    uint32_t ret_type_;
    std::vector<uint32_t> params_;

    virtual void dump_spirv(std::ostream& os) const override;
    explicit Function(Program* prog, uint32_t ret_type, const std::vector<uint32_t>& params);
}; // class Function


struct Program: public ProgCompo
{
    Program();
    ~Program();
    Function* new_fn(uint32_t ret_type, const std::vector<uint32_t>& params);
    Variable* new_var(uint32_t type, StorageClass sc);
    template<typename ...Args>
    uint32_t get_type(DType dt, Args&& ...args);
private:
    friend class Function;
    friend class Variable;
    friend std::ostream& operator<<(std::ostream&, const Program&); 

    virtual void dump_spirv(std::ostream& os) const override;
    void prologue();
    void set_entry_id(uint32_t id);
    uint32_t get_type_ptr_id(uint32_t type_id, StorageClass sc);
    OpTypePointer* get_type_ptr(uint32_t id);

    OpCapability* capa_;
    OpMemoryModel* mem_mode_;
    OpEntryPoint* entry_point_;
    std::vector<Function*> fns_;
    std::vector<Variable*> vars_;
    std::vector<OpTypeBase*> types_;
    std::vector<OpTypePointer*> type_ptrs_;
}; // class Program


std::ostream& operator<<(std::ostream& os, const Program& prag);


template<typename ...Args>
uint32_t Program::get_type(DType dt, Args&& ...args)
{
    switch (dt) {
    case DType::VOID: return OpTypeCreator<DType::VOID>(types_).get_type()->id; 
    case DType::FLOAT: return OpTypeCreator<DType::FLOAT>(types_).get_type(args...)->id; 
    case DType::FN: return OpTypeCreator<DType::FN>(types_).get_type(args...)->id; 
    default:
        assert("Not implement!");
    }
    return 0;
}

#endif // YACCS_PROGRAM_H_
