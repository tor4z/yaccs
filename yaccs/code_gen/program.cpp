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
    deco.binding = 0;
    deco.set = 0;
    add_tensor_type(tensor_type, deco);
}

void Program::add_output(const TensorType& tensor_type)
{
    DecorateSetBindingDef deco;
    deco.binding = 0;
    deco.set = 1;
    add_tensor_type(tensor_type, deco);
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

id_t Program::add_struct_dtype(const std::vector<id_t>& dtypes)
{
    assert(dtypes.size() <= MAX_STRUCT_FIELDS && "struct containting too much fields");
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
    memcpy(std.fields, dtypes.data(), sizeof(std.fields[0]) * std.num_fields);
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
    DecorateSetBindingDef deco;
    deco.set = -1;
    deco.binding = -1;
    return add_tensor_type(tensor_type, deco);
}

id_t Program::add_tensor_type(const TensorType& tensor_type, DecorateSetBindingDef deco)
{
    /*
     * {
     *     int dims;
     *     int shape[];
     *     DType data[];
     * }
     */

    auto type_id{add_dtype(tensor_type.dtype)}; // define type dtype
    auto dt_int{add_dtype(DT_INT32)};           // define int type
    auto int_arr{add_array_dtype(dt_int, shape_to_dsize(tensor_type.dims, tensor_type.shape))}; // define int array for shape
    auto dt_arr{add_array_dtype(type_id, shape_to_dsize(tensor_type.dims, tensor_type.shape))}; // define dtype array for data
    auto struct_type_id{add_struct_dtype({dt_int /* dims */, int_arr /* shape */, dt_arr /* data */})};
    
    if (deco.binding >= 0 && deco.set >= 0) {
        deco.target = struct_type_id;
        code_gen_.push_decorate_set_binding(deco);
    }

    return struct_type_id;
}

id_t Program::add_array(id_t arr_type, const std::vector<id_t>& elem_ids)
{
    static std::vector<ConstCompositeDef> dfs{};

    ConstCompositeDef ad;
    ad.type_id = arr_type;
    ad.elem_ids = elem_ids;

    auto arr_matched{[&ad] (const ConstCompositeDef& target) -> bool {
        if (target.type_id != ad.type_id) return false;
        return target.elem_ids == ad.elem_ids;
    }};
        
    for (const auto& it : dfs) {
        if (arr_matched(it)) {
            return it.id;
        }
    }

    ad.id = alloc_id();
    dfs.push_back(ad);
    code_gen_.push_const_composite(ad);
    return ad.id;
}

id_t Program::add_const_tensor(const Tensor& tensor)
{
    static std::vector<ConstCompositeDef> dfs;

    int num_elems{1};
    id_t dims_id;
    std::vector<id_t> shape_elem_ids;
    std::vector<id_t> data_elem_ids;
    auto tensor_type_id{add_tensor_type(tensor.tt)};

    // setup dims
    dims_id = add_const(DT_INT32, tensor.tt.dims);

    // setup shape
    for (int i = 0; i < tensor.tt.dims; ++i) {
        num_elems *= tensor.tt.shape[i];
        shape_elem_ids.push_back(add_const(DT_INT32, tensor.tt.shape[i]));
    }

    auto shape_type_id{add_array_dtype(DT_INT32, shape_elem_ids.size())};
    auto shape_id{add_array(shape_type_id, shape_elem_ids)};

    // setup data
    for (int i = 0; i < num_elems; ++i) {
        const auto const_id{add_raw_const(tensor.tt.dtype, i, tensor.data.data())};
        data_elem_ids.push_back(const_id);
    }
    auto data_type_id{add_array_dtype(tensor.tt.dtype, data_elem_ids.size())};
    auto data_id{add_array(data_type_id, data_elem_ids)};

    ConstCompositeDef sd;
    sd.type_id = tensor_type_id;
    sd.elem_ids = {dims_id, shape_id, data_id};

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