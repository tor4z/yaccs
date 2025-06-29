#include "code_gen.hpp"
#include "ops.hpp"
#include <cassert>


template<DType T>
void dt_gen(std::ostream& ss, const OpType<T>* op) {}

template<>
void dt_gen<DType::VOID>(std::ostream& ss, const OpType<DType::VOID>* op)
{
    ss << "%" << op->id << " = " << op->name() << "\n";
}

template<>
void dt_gen<DType::FN>(std::ostream& ss, const OpType<DType::FN>* op)
{
    ss << "%" << op->id << " = " << op->name() << "\n";
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
    ss << op->name() << " GLCompute %" << op->entry_id_ << " \"main\"" << "\n";
}


void CodeGen::gen(std::ostream& ss, const OpFunction* op)
{
    ss << op->name() << "\n";
}


void CodeGen::gen(std::ostream& ss, const OpLabel* op)
{
    ss << op->name() << "\n";
}


void CodeGen::gen(std::ostream& ss, const OpFunctionEnd* op)
{
    ss << op->name() << "\n";
}


void CodeGen::gen(std::ostream& ss, const OpTypeBase* op)
{
    switch (op->dt) {
    case DType::FN:     return dt_gen<DType::FN>(ss, reinterpret_cast<const OpType<DType::FN>*>(op));
    case DType::VOID:   return dt_gen<DType::VOID>(ss, reinterpret_cast<const OpType<DType::VOID>*>(op));
    default:            assert("TODO!");
    }
}
