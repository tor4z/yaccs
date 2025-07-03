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
    ss << "%" << op->id << " = " << as_string(op->type) << "\n";
}

template<>
void dt_gen<DType::FN>(std::ostream& ss, const OpType<DType::FN>* op)
{
    ss << "%" << op->id << " = "
        << as_string(op->type) << " %" << op->ret_type_id << "\n";
}


template<>
void dt_gen<DType::FLOAT>(std::ostream& ss, const OpType<DType::FLOAT>* op)
{
    ss << "%" << op->id << " = "
        << as_string(op->type) << " " << op->width << "\n";
}


template<>
void dt_gen<DType::VEC>(std::ostream& ss, const OpType<DType::VEC>* op)
{
    ss << "%" << op->id << " = "
        << as_string(op->type) << " %" << op->comp_type
        << " " << op->comp_count << "\n";
}


template<>
void dt_gen<DType::STRUCT>(std::ostream& ss, const OpType<DType::STRUCT>* op)
{
    ss << "%" << op->id << " = " << as_string(op->type);
    for (auto t : op->param_type_ids) {
        ss << " %" << t;
    }
    ss << "\n";
}


void CodeGen::gen(std::ostream& ss, const OpCapability* op)
{
    ss << as_string(op->type) << " Shader" << "\n";
}


void CodeGen::gen(std::ostream& ss, const OpMemoryModel* op)
{
    ss << as_string(op->type) << " Logical GLSL450" << "\n";
}


void CodeGen::gen(std::ostream& ss, const OpEntryPoint* op)
{
    ss << as_string(op->type) << " Vertex %" << op->entry_id << " \"main\"";

    for (auto p : op->params) {
        ss << " %" << p; 
    }
    
    ss << "\n";
}


void CodeGen::gen(std::ostream& ss, const OpDecorate* op)
{
    ss << as_string(op->type) << " %" << op->target_id
        << " " << as_string(op->dec.type)
        << " " << op->dec.param << "\n";
}



void CodeGen::gen(std::ostream& ss, const OpFunction* op)
{
    ss << "%" <<op->id << " = " << as_string(op->type)
        << " %" << op->ret_type_id
        << " None"
        << " %" << op->type_id << "\n";
}


void CodeGen::gen(std::ostream& ss, const OpLabel* op)
{
    ss << "%" <<op->id << " = " << as_string(op->type) << "\n";
}


void CodeGen::gen(std::ostream& ss, const OpLoad* op)
{
    ss << "%" << op->id << " = " << as_string(op->type)
        << " %" << op->type_id
        << " %" << op->var_id << "\n";
}


void CodeGen::gen(std::ostream& ss, const OpStore* op)
{
    ss << as_string(op->type)
        << " %" << op->dst_id
        << " %" << op->src_id << "\n";
}


void CodeGen::gen(std::ostream& ss, const OpReturn* op)
{
    ss << as_string(op->type) << "\n";
}


void CodeGen::gen(std::ostream& ss, const OpFunctionEnd* op)
{
    ss << as_string(op->type) << "\n";
}


void CodeGen::gen(std::ostream& ss, const OpTypePointer* op)
{
    ss << "%" << op->id << " = " << as_string(op->type) << " "
        << as_string(op->storage_class)
        << " %" << op->type_id << "\n";
}


void CodeGen::gen(std::ostream& ss, const OpVariable* op)
{
    ss << "%" << op->id << " = " << as_string(op->type)
        << " %" << op->type_ptr_id << " "
        << as_string(op->storage_class) << "\n";
}


void CodeGen::gen(std::ostream& ss, const OpConstantComposite* op)
{
    ss << "%" << op->id << " = " << as_string(op->type)
        << " %" << op->type_id;
    
    for (auto c : op->comps) {
        ss << " %" << c;
    }

    ss << "\n";
}


void CodeGen::gen(std::ostream& ss, const Op* op)
{
#define GEN_TYPE(OPT, DT)\
        case OPT: return dt_gen<DT>(ss, reinterpret_cast<const OpType<DT>*>(op))

    switch (op->type) {
        case Op::CAPABILITY:    return gen(ss, reinterpret_cast<const OpCapability*>(op));
        case Op::MEMORY_MODEL:  return gen(ss, reinterpret_cast<const OpMemoryModel*>(op));
        case Op::ENTRY_POINT:   return gen(ss, reinterpret_cast<const OpEntryPoint*>(op));
        case Op::DECORATE:      return gen(ss, reinterpret_cast<const OpDecorate*>(op));
        case Op::VARIABLE:      return gen(ss, reinterpret_cast<const OpVariable*>(op));
        case Op::FUNCTION:      return gen(ss, reinterpret_cast<const OpFunction*>(op));
        case Op::LOAD:          return gen(ss, reinterpret_cast<const OpLoad*>(op));
        case Op::STORE:         return gen(ss, reinterpret_cast<const OpStore*>(op));
        case Op::LABEL:         return gen(ss, reinterpret_cast<const OpLabel*>(op));
        case Op::RETURN:        return gen(ss, reinterpret_cast<const OpReturn*>(op));
        case Op::FUNCTION_END:  return gen(ss, reinterpret_cast<const OpFunctionEnd*>(op));
        case Op::TYPE_POINTER:  return gen(ss, reinterpret_cast<const OpTypePointer*>(op));

        GEN_TYPE(Op::TYPE_FUNCTION, DType::FN);
        GEN_TYPE(Op::TYPE_VOID, DType::VOID);
        GEN_TYPE(Op::TYPE_FLOAT, DType::FLOAT);
        GEN_TYPE(Op::TYPE_VECTOR, DType::VEC);
        GEN_TYPE(Op::TYPE_STRUCT, DType::STRUCT);
        case Op::INVALID:
        default:                assert("TODO!");
    }
#undef GEN_TYPE
}
