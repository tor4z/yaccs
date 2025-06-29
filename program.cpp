#include "program.hpp"
#include "code_gen.hpp"
#include "ops.hpp"
#include <sstream>
#include <cassert>
#include <string>
#include <utility>


Function::Function(Program* prog)
    : prog_(prog)
    , op_fn_(new OpFunction())
    , op_fend_(new OpFunctionEnd())
{
    set_ret_type(DType::VOID); // default return type
}


Variable* Function::new_var()
{
    return nullptr;
}


void Function::as_entry()
{
    prog_->set_entry_id(op_fn_->id);   
    set_ret_type(DType::VOID);
}


void Function::set_ret_type(DType dt)
{
    op_ret_type_ = prog_->get_type(dt);
}


void Function::dump_spirv(std::ostream& os) const
{
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


Function* Program::new_fn()
{
    auto fn{new Function(this)};
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
