#include "program.hpp"
#include "code_gen.hpp"
#include "ops.hpp"
#include <sstream>
#include <cassert>
#include <string>
#include <utility>


Variable::Variable(Program* prog, uint32_t type_ptr)
    : prog_(prog)
{
    auto tp{prog_->get_type_ptr(type_ptr)};
    op_var_ = new OpVariable();
    op_var_->type_ptr_id = type_ptr;
    op_var_->storage_class = tp->storage_class;
}


Variable::~Variable()
{
    delete op_var_;
}


void Variable::dump_spirv(std::ostream& os) const
{
    CodeGen::gen(os, op_var_);
}



Function::Function(Program* prog, uint32_t ret_type, const std::vector<uint32_t>& params)
    : prog_(prog)
    , op_fn_(new OpFunction())
    , op_lab_(new OpLabel())
    , op_ret_(new OpReturn())
    , op_fend_(new OpFunctionEnd())
    , ret_type_(ret_type)
    , params_(params)
{
    auto op_fn_type{prog_->get_type(DType::FN, ret_type)};
    op_fn_->type_id = op_fn_type;
    op_fn_->ret_type_id = ret_type;
}


Variable* Function::new_var()
{
    return nullptr;
}


void Function::as_entry()
{
    // TODO return void
    prog_->set_entry_id(op_fn_->id);
}


void Function::dump_spirv(std::ostream& os) const
{
    CodeGen::gen(os, op_fn_);
    CodeGen::gen(os, op_lab_);
    CodeGen::gen(os, op_ret_);
    CodeGen::gen(os, op_fend_);
}


Program::Program()
{
    prologue();
}


Program::~Program()
{
    delete capa_;
    delete mem_mode_;
    delete entry_point_;
    for (auto t : types_) {
        delete t;
    }
    
    for (auto f : fns_) {
        delete f;
    }

    for (auto tp: type_ptrs_) {
        delete tp;
    }

    for (auto v: vars_) {
        delete v;
    }

    type_ptrs_.clear();
    types_.clear();
    fns_.clear();
    vars_.clear();
}


void Program::prologue()
{
    capa_ = new OpCapability();
    mem_mode_ = new OpMemoryModel();
    entry_point_ = new OpEntryPoint();
}


void Program::dump_spirv(std::ostream& os) const
{
    CodeGen::gen(os, capa_);
    CodeGen::gen(os, mem_mode_);
    CodeGen::gen(os, entry_point_);

    for (auto t: types_) {
        CodeGen::gen(os, t);
    }

    for (auto tp: type_ptrs_) {
        CodeGen::gen(os, tp);
    }

    for (auto v: vars_) {
        v->dump_spirv(os);
    }

    for (auto fn : fns_) {
        fn->dump_spirv(os);
    }
}


Function* Program::new_fn(uint32_t ret_type, const std::vector<uint32_t>& params)
{
    auto fn{new Function(this, ret_type, params)};
    fns_.push_back(fn);
    return fn;
}


Variable* Program::new_var(uint32_t type, StorageClass sc)
{
    auto tp{get_type_ptr_id(type, sc)};
    auto v{new Variable(this, tp)};
    vars_.push_back(v);
    return v;
}


void Program::set_entry_id(uint32_t id)
{
    entry_point_->set_entry_id(id);
}


uint32_t Program::get_type_ptr_id(uint32_t type_id, StorageClass sc)
{
    for (auto tp : type_ptrs_) {
        if (tp->type_id == type_id && tp->storage_class == sc) {
            return tp->id;
        }
    }

    // create new type pointer
    auto tp{new OpTypePointer(type_id)};
    tp->storage_class = sc;
    type_ptrs_.push_back(tp);
    return tp->id;
}


OpTypePointer* Program::get_type_ptr(uint32_t id)
{
    for (auto tp : type_ptrs_) {
        if (tp->id == id) {
            return tp;
        }
    }

    return nullptr;
}


std::ostream& operator<<(std::ostream& os, const Program& prag)
{
    prag.dump_spirv(os);
    return os;
}
