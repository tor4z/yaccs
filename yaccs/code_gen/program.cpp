#include "yaccs/code_gen/program.hpp"
#include "yaccs/code_gen/def.hpp"
#include "yaccs/code_gen/utils.hpp"
#include "yaccs/dtype.hpp"
#include <unordered_map>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <fstream>
#include <utility>
#include <iostream>
#include <vector>


Program::Program()
{
    code_gen_.push_header();
}

void Program::set_name(const std::string& name) { name_  = name; }

void Program::add_input(const TensorType& tensor_type)
{
    DecorateSetBindingDef deco;
    auto type_id{add_tensor_type(tensor_type)};
    auto var_id{add_var(type_id)};

    deco.binding = 0;
    deco.set = 0;
    deco.target = var_id;
    code_gen_.push_decorate_set_binding(deco);

}

void Program::add_output(const TensorType& tensor_type)
{
    return;
    DecorateSetBindingDef deco;
    auto type_id{add_tensor_type(tensor_type)};
    auto var_id{add_var(type_id)};

    deco.binding = 0;
    deco.set = 1;
    deco.target = var_id;
    code_gen_.push_decorate_set_binding(deco);
}

void Program::dump_ir()
{
    std::ofstream ofs{name_, std::ios::out};
    code_gen_.assemble(ofs);
    ofs.close();
}

id_t Program::add_dtype(DType dtype)
{
    static std::unordered_map<DType, id_t> defs_;
    if (defs_.find(dtype) == defs_.end()) {
        const auto id{alloc_id()};
        defs_.insert(std::make_pair(dtype, id));
        code_gen_.push_dtype(dtype, id);
    }
    return defs_.at(dtype);
}

id_t Program::add_type_pointer(id_t type_id)
{
    static std::unordered_map<id_t, TypePointerDef> dfs;

    if (dfs.find(type_id) != dfs.end()) {
        return dfs.at(type_id).id;
    }

    TypePointerDef tpd;
    tpd.type_id = type_id;
    tpd.id  = alloc_id();
    dfs.insert(std::make_pair(type_id, tpd));
    code_gen_.push_type_pointer(tpd);

    return tpd.id;
}

id_t Program::add_var(id_t type_id)
{
    VarDef vd;
    vd.id = alloc_id();
    vd.type_pointer_id = add_type_pointer(type_id);
    code_gen_.push_variable(vd);
    return vd.id;
}

id_t Program::add_struct_dtype(const std::vector<id_t>& dtypes)
{
    static std::vector<StructTypeDef> defs;

    auto struct_match{[&dtypes] (const StructTypeDef& std) -> bool {
        if (std.num_fields != dtypes.size()) return false;
        for (int i = 0; i < std.num_fields; ++i) {
            if (std.fields[i] != dtypes.at(i)) return false;
        }
        return true;
    }};

    for (const auto& it : defs) {
        if (struct_match(it)) {
            return it.id;
        }
    }

    StructTypeDef std{.id = alloc_id(), .num_fields = dtypes.size()};
    std.fields.resize(std.num_fields);
    memcpy(std.fields.data(), dtypes.data(), sizeof(std.fields[0]) * std.num_fields);
    defs.push_back(std);
    code_gen_.push_struct_dtype(std);
    return std.id;
}

id_t Program::add_array_dtype(id_t dtype, int length)
{
    static std::vector<ArrTypeDef> defs;

    for (const auto& it : defs) {
        if (it.dtype == dtype && it.length == length) {
            return it.id;
        }
    }

    id_t length_id{add_const(DT_INT32, length)};
    ArrTypeDef arr{.length = length, .dtype = dtype, .length_id = length_id, .id = alloc_id()};
    defs.push_back(arr);
    code_gen_.push_array_dtype(arr);
    return arr.id;
}

id_t Program::add_tensor_type(const TensorType& tensor_type)
{
    /*
     * {
     *     int dims;
     *     int shape[0];
     *     int shape[1];
     *     int shape[...];
     *     DType data[0];
     *     DType data[1];
     *     DType data[...];
     * }
     */

    int num_elems{1};
    auto type_id{add_dtype(tensor_type.dtype)}; // define type dtype
    auto dt_int{add_dtype(DT_INT32)};           // define int type
    std::vector<id_t> struct_ids;
    struct_ids.push_back(dt_int);

    for (int i = 0; i < tensor_type.dims; ++i) {
        struct_ids.push_back(dt_int);
        num_elems *= tensor_type.shape[i];
    }
    for (int i = 0; i < num_elems; ++i) {
        struct_ids.push_back(type_id);
    }
    auto struct_type_id{add_struct_dtype(struct_ids)};

    return struct_type_id;
}

id_t Program::add_const_array(id_t arr_type, const std::vector<id_t>& elem_ids)
{
    static std::vector<ConstCompositeDef> dfs{};

    ConstCompositeDef ccd;
    ccd.type_id = arr_type;
    ccd.elem_ids = elem_ids;

    auto arr_matched{[&ccd] (const ConstCompositeDef& target) -> bool {
        if (target.type_id != ccd.type_id) return false;
        return target.elem_ids == ccd.elem_ids;
    }};
        
    for (const auto& it : dfs) {
        if (arr_matched(it)) {
            return it.id;
        }
    }

    ccd.id = alloc_id();
    dfs.push_back(ccd);
    code_gen_.push_const_composite(ccd);
    return ccd.id;
}

id_t Program::add_const_tensor(const Tensor& tensor)
{
    static std::vector<ConstCompositeDef> dfs;

    int num_elems{1};
    ConstCompositeDef sd;
    auto tensor_type_id{add_tensor_type(tensor.tt)};
    sd.type_id = tensor_type_id;

    // setup dims
    sd.elem_ids.push_back(add_const(DT_INT32, tensor.tt.dims));

    // setup shape
    for (int i = 0; i < tensor.tt.dims; ++i) {
        num_elems *= tensor.tt.shape[i];
        sd.elem_ids.push_back(add_const(DT_INT32, tensor.tt.shape[i]));
    }

    // setup data
    for (int i = 0; i < num_elems; ++i) {
        const auto const_id{add_raw_const(tensor.tt.dtype, i, tensor.data.data())};
        sd.elem_ids.push_back(const_id);
    }

    auto const_struct_matched{[&sd] (const ConstCompositeDef& target) -> bool {
        if (target.type_id != sd.type_id) {
            return false;
        }
        return sd.elem_ids == target.elem_ids;
    }};

    for (const auto& it : dfs) {
        if (const_struct_matched(it)) {
            return it.id;
        }
    }

    sd.id = alloc_id();
    dfs.push_back(sd);
    code_gen_.push_const_composite(sd);
    return sd.id;
}

id_t Program::add_raw_const(DType dtype, int elem_idx, const char* data)
{
    id_t id{0};
    switch (dtype) {
        case DT_FLOAT:
        {
            auto raw{le32toh(*reinterpret_cast<const uint32_t*>(data + elem_idx * DT_FLOAT_BYTES))};
            float f{*reinterpret_cast<float*>(&raw)};
            id = add_const(dtype, f);
        } break;
        case DT_UINT8:
        case DT_INT8:
        case DT_UINT16:
        case DT_INT16:
        case DT_INT32:
        case DT_INT64:
        case DT_STRING:
        case DT_BOOL:
        case DT_FLOAT16:
        case DT_DOUBLE:
        case DT_UINT32:
        case DT_UINT64:
        case DT_COMPLEX64:
        case DT_COMPLEX128:
        case DT_BFLOAT16:
        case DT_FLOAT8E4M3FN:
        case DT_FLOAT8E4M3FNUZ:
        case DT_FLOAT8E5M2:
        case DT_FLOAT8E5M2FNUZ:
        case DT_UINT4:
        case DT_INT4:
        case DT_FLOAT4E2M1:
        case DT_UNDEFINED:
        default:
            assert(false && "Unrecognized data type");
    }

    return id;
}

id_t Program::add_void_type()
{
    static bool defined{false};
    static id_t id{};
    if (defined) return id;

    id = alloc_id();
    code_gen_.push_void_type(id);
    defined = true;
    return id;
}

id_t Program::add_function_type(id_t return_type_id)
{
    static std::vector<FunctionTypeDef> defs;
    for (auto it : defs) {
        if (it.return_type_id == return_type_id) {
            return it.id;
        }
    }

    FunctionTypeDef ft{.return_type_id = return_type_id, .id = alloc_id()};
    defs.push_back(ft);
    code_gen_.push_function_type(ft);
    return ft.id;
}

id_t Program::add_label()
{
    auto id{alloc_id()};
    code_gen_.push_label(id);
    return id;
}

id_t Program::add_function_prologue(id_t return_type_id)
{
    auto fn_type_id{add_function_type(return_type_id)};
    FunctionHeaderDef fh{.return_type_id = return_type_id, .function_type_id = fn_type_id, .id = alloc_id()};

    code_gen_.push_function(fh); 
    auto label_id{add_label()};
    return fh.id;
}

void Program::add_function_epilogue()
{
    code_gen_.push_return();
    code_gen_.push_function_end();
}

void Program::add_gemm(const OpGemm& gemm)
{
    auto void_type_id{add_void_type()};
    add_function_prologue(void_type_id);

    auto a_id{add_const_tensor(gemm.A)} ;
    auto b_id{add_const_tensor(gemm.B)};

    add_function_epilogue();
}

void Program::add_relu(const OpRelu& relu)
{
    auto void_type_id{add_void_type()};
    add_function_prologue(void_type_id);
    
    add_function_epilogue();
}