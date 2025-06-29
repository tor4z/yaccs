#include "program.hpp"
#include "code_gen.hpp"
#include "ops.hpp"
#include <sstream>
#include <cassert>
#include <string>
#include <utility>


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
    op_fn_->type_id = op_fn_type->id;
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
    for (auto f : functions_) {
        delete f;
    }

    types_.clear();
    functions_.clear();
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

    for (auto fn : functions_) {
        fn->dump_spirv(os);
    }
}


Function* Program::new_fn(uint32_t ret_type, const std::vector<uint32_t>& params)
{
    auto fn{new Function(this, ret_type, params)};
    functions_.push_back(fn);
    return fn;
}


Variable* Program::new_var()
{
    return nullptr;
}


void Program::set_entry_id(uint32_t id)
{
    entry_point_->set_entry_id(id);
}


std::ostream& operator<<(std::ostream& os, const Program& prag)
{
    prag.dump_spirv(os);
    return os;
}
