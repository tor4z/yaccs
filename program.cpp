#include "program.hpp"
#include "code_gen.hpp"
#include "ops.hpp"
#include "utils.hpp"
#include <cstdint>
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


void Variable::decorate(Decoration dec)
{
    prog_->add_decoration(op_var_->id, dec);
}


uint32_t Variable::id()
{
    return op_var_->id;
}


uint32_t Variable::type()
{
    return prog_->get_type_ptr(op_var_->type_ptr_id)->type_id;
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
    op_fn_->type_id = prog_->get_type(DType::FN, ret_type);
    op_fn_->ret_type_id = ret_type;
}


Function::~Function()
{
    delete op_fn_;
    delete op_lab_;
    delete op_ret_;
    delete op_fend_;

    for (auto op: ops_) {
        delete op;
    }

    ops_.clear();
}


Variable* Function::new_var()
{
    return nullptr;
}


void Function::as_entry()
{
    assert(prog_->get_op_type(op_fn_->ret_type_id)->dt == DType::VOID &&
        "Entry function should be void");
    prog_->set_entry_id(op_fn_->id, params_);
}


void Function::op_assign(Variable* from, Variable* to)
{
    auto op_load{new OpLoad()};
    auto op_store{new OpStore()};

    op_load->var_id = from->id();
    op_load->type_id = from->type();

    op_store->src_id = op_load->id;
    op_store->dst_id = to->id();

    ops_.push_back(op_load);
    ops_.push_back(op_store);
}


void Function::dump_spirv(std::ostream& os) const
{
    CodeGen::gen(os, op_fn_);
    CodeGen::gen(os, op_lab_);

    for (auto op: ops_) {
        CodeGen::gen(os, op);
    }

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

    for (auto dec : decs_) {
        delete dec;
    }

    for (auto c : consts_) {
        delete c;
    }

    for (auto cc : const_comps_) {
        delete cc;
    }

    type_ptrs_.clear();
    types_.clear();
    fns_.clear();
    vars_.clear();
    decs_.clear();
    consts_.clear();
    const_comps_.clear();
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

    for (auto d: decs_) {
        CodeGen::gen(os, d);
    }

    for (auto t: types_) {
        CodeGen::gen(os, t);
    }

    for (auto tp: type_ptrs_) {
        CodeGen::gen(os, tp);
    }

    for (auto v: vars_) {
        v->dump_spirv(os);
    }

    for (auto c: consts_) {
        c->dump_spirv(os);
    }

    for (auto cc: const_comps_) {
        cc->dump_spirv(os);
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


void Program::set_entry_id(uint32_t id, const std::vector<uint32_t>& params)
{
    entry_point_->entry_id = id;
    entry_point_->params = params;
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


OpTypeBase* Program::get_op_type(uint32_t type_id)
{
    for (auto t: types_) {
        if (t->id == type_id) {
            return t;
        }
    }
    return nullptr;
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


uint32_t Program::add_decoration(uint32_t var_id, const Decoration& dec)
{
    for (auto dec : decs_) {
        if (dec->target_id == var_id) {
            assert("Duplicate decoration");
        }
    }

    auto d{new OpDecorate()};
    d->target_id = var_id;
    d->dec = dec;
    decs_.push_back(d);
    return d->id;
}



std::ostream& operator<<(std::ostream& os, const Program& prag)
{
    prag.dump_spirv(os);
    return os;
}
