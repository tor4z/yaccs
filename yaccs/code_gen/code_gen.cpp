#include "yaccs/code_gen/code_gen.hpp"
#include "yaccs/baker/def.hpp"
#include "yaccs/baker/layer1/utils.hpp"
#include "yaccs/dtype.hpp"
#include <cassert>
#include <cstddef>


CodeGen::CodeGen() {}

void CodeGen::assemble(std::ofstream& ofs)
{
    ofs << header_ss_.str();
    ofs << ext_import_ss_.str();
    ofs << entry_def_ss_.str();
    ofs << decorate_ss_.str();
    ofs << type_const_def_ss_.str();
    ofs << fn_def_ss_.str();
}

void CodeGen::push_header()
{
    header_ss_ << "OpCapability Shader\n";
}

void CodeGen::push_ext_import(const ExtImportDef& eid)
{
    ext_import_ss_ << "%" << eid.id << " = OpExtInstImport \"" << eid.ext_name << "\"\n";
}

void CodeGen::push_entry(const EntryDef& ed)
{
    entry_def_ss_ << "OpMemoryModel Logical GLSL450\n";
    entry_def_ss_ << "OpEntryPoint GLCompute %" << ed.main_id << " \"main\"";
    for (auto it: ed.input_ids) {
        entry_def_ss_ << " %" << it;
    }
    entry_def_ss_ << "\n";

    entry_def_ss_ << "OpExecutionMode %" << ed.main_id << " LocalSize "
        << ed.local_size_x << " " << ed.local_size_y << " " << ed.local_size_z << "\n";
}

void CodeGen::push_struct_decorate(const DecorateStructDef& dsd)
{
    if (dsd.deco != DECO_NONE) {
        decorate_ss_ <<  "OpDecorate %" << dsd.struct_type_id << " " << as_string(dsd.deco) << "\n";
    }

    for (const auto& it : dsd.member_deco) {
        decorate_ss_ << "OpMemberDecorate %" << dsd.struct_type_id << " " << it.field << " Offset " << it.offset << "\n";
    }
}

void CodeGen::push_array_decorate(const DecorateArrayDef& dad)
{
    decorate_ss_ << "OpDecorate %" << dad.array_type_id << " ArrayStride 4\n";
}

void CodeGen::push_builtin_decorate(const DecorateBuiltInDef& built_in)
{
    decorate_ss_ << "OpDecorate %" << built_in.var_id << " BuiltIn " << as_string(built_in.built_in) << "\n";
}

void CodeGen::push_dtype(DType dt, id_t id)
{
    switch (dt) {
    case DT_FLOAT:
        type_const_def_ss_ << "%" << id << " = OpTypeFloat 32\n";
        break;
    case DT_FLOAT16:
        type_const_def_ss_ << "%" << id << " = OpTypeFloat 16\n";
        break;
    case DT_INT32:
        type_const_def_ss_ << "%" << id << " = OpTypeInt 32 1\n";
        break;
    case DT_UINT32:
        type_const_def_ss_ << "%" << id << " = OpTypeInt 32 0\n";
        break;
    case DT_BOOL:
        type_const_def_ss_ << "%" << id << " = OpTypeBool\n";
        break;
    default: assert(false && "Unsupported type");
    }
}

void CodeGen::push_array_dtype(const ArrTypeDef& arr)
{
    type_const_def_ss_ << "%" << arr.id << " = OpTypeArray %" << arr.dtype << " %" << arr.length_id << "\n";
}

void CodeGen::push_struct_dtype(const StructTypeDef& sd)
{
    type_const_def_ss_ << "%" << sd.id << " = OpTypeStruct ";
    for (size_t i = 0; i < sd.num_fields; ++i) {
        type_const_def_ss_ << "%" << sd.fields[i];
        if (i != sd.num_fields - 1) type_const_def_ss_ << " ";
    }
    type_const_def_ss_ << "\n";
}

void CodeGen::push_void_type(id_t id)
{
    type_const_def_ss_ << "%" << id << " = OpTypeVoid\n";
}

void CodeGen::push_function_type(const FunctionTypeDef& ft)
{
    type_const_def_ss_ << "%" << ft.id << " = OpTypeFunction %" << ft.return_type_id << "\n";
}

void CodeGen::push_const_composite(const ConstCompositeDef& ccd)
{
    type_const_def_ss_ << "%" << ccd.id << " = OpConstantComposite %" << ccd.type_id;
    for (auto it : ccd.elem_ids) {
        type_const_def_ss_ << " %" << it;
    }
    type_const_def_ss_ << "\n";
}

void CodeGen::push_type_pointer(const TypePointerDef& tp)
{
    type_const_def_ss_ << "%" << tp.id << " = OpTypePointer "
        << as_string(tp.storage_class) << " %" << tp.type_id << "\n";
}

void CodeGen::push_variable(const VarDef& var)
{
    auto& ss{var.storage_class == SC_FUNCTION ? this_fn_.var_def_ss : type_const_def_ss_};
    if (var.storage_class == SC_FUNCTION) {
        ss << "\t";
    }

    ss << "%" << var.id << " = OpVariable %" << var.type_pointer_id << " " << as_string(var.storage_class);
    if (var.initializer_id == 0) {
        ss << "\n";
    } else {
        ss << " %" << var.initializer_id << "\n";
    }
}

void CodeGen::push_decorate_set_binding(const DecorateSetBindingDef& deco)
{
    assert(deco.binding >= 0 && deco.set >= 0 && "Bad decoration");
    decorate_ss_ << "OpDecorate %" << deco.target << " DescriptorSet " << deco.set << "\n";
    decorate_ss_ << "OpDecorate %" << deco.target << " Binding " << deco.binding << "\n";
}

void CodeGen::push_vector_dtype(const VectorDef& vd)
{
    type_const_def_ss_ << "%" << vd.id << " = OpTypeVector %" << vd.component_type_id << " " << vd.count << "\n";
}
