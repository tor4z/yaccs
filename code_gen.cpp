#include "code_gen.hpp"
#include "ops.hpp"
#include "types.hpp"
#include <cassert>


template<DType T>
void dt_gen(std::ostream& ss, const OpType<T>* op)
{
    assert("Not implemented");
}

template<>
void dt_gen<DType::VOID>(std::ostream& ss, const OpType<DType::VOID>* op)
{
    ss << "%" << op->id << " = " << op->name() << "\n";
}

template<>
void dt_gen<DType::FN>(std::ostream& ss, const OpType<DType::FN>* op)
{
    ss << "%" << op->id << " = "
        << op->name() << " %" << op->ret_type_id << "\n";
}

template<>
void dt_gen<DType::FLOAT>(std::ostream& ss, const OpType<DType::FLOAT>* op)
{
    ss << "%" << op->id << " = "
        << op->name() << " " << op->width << "\n";
}


void CodeGen::gen(std::ostream& ss, const OpCapability* op)
{
    ss << op->name() << " Shader" << "\n";
}


void CodeGen::gen(std::ostream& ss, const OpMemoryModel* op)
{
    ss << op->name() << " Logical GLSL450" << "\n";
}


void CodeGen::gen(std::ostream& ss, const OpEntryPoint* op)
{
    ss << op->name() << " Vertex %" << op->entry_id_ << " \"main\"" << "\n";
}


void CodeGen::gen(std::ostream& ss, const OpDecorate* op)
{
    ss << op->name() << " %" << op->target_id
        << " " << as_string(op->dec.type)
        << " " << op->dec.param << "\n";
}



void CodeGen::gen(std::ostream& ss, const OpFunction* op)
{
    ss << "%" <<op->id << " = " << op->name()
        << " %" << op->ret_type_id
        << " None"
        << " %" << op->type_id << "\n";
}


void CodeGen::gen(std::ostream& ss, const OpLabel* op)
{
    ss << "%" <<op->id << " = " << op->name() << "\n";
}


void CodeGen::gen(std::ostream& ss, const OpReturn* op)
{
    ss << op->name() << "\n";
}


void CodeGen::gen(std::ostream& ss, const OpFunctionEnd* op)
{
    ss << op->name() << "\n";
}


void CodeGen::gen(std::ostream& ss, const OpTypePointer* op)
{
    ss << "%" << op->id << " = " << op->name() << " "
        << as_string(op->storage_class)
        << " %" << op->type_id << "\n";
}


void CodeGen::gen(std::ostream& ss, const OpVariable* op)
{
    ss << "%" << op->id << " = " << op->name()
        << " %" << op->type_ptr_id << " "
        << as_string(op->storage_class) << "\n";
}


void CodeGen::gen(std::ostream& ss, const OpTypeBase* op)
{
    switch (op->dt) {
    case DType::FN:     return dt_gen<DType::FN>(ss, reinterpret_cast<const OpType<DType::FN>*>(op));
    case DType::VOID:   return dt_gen<DType::VOID>(ss, reinterpret_cast<const OpType<DType::VOID>*>(op));
    case DType::FLOAT:   return dt_gen<DType::FLOAT>(ss, reinterpret_cast<const OpType<DType::FLOAT>*>(op));
    default:            assert("TODO!");
    }
}
