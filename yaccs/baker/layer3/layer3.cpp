#include "yaccs/baker/layer3/layer3.hpp"
#include "yaccs/baker/def.hpp"
#include "yaccs/baker/layer1/layer1.hpp"
#include "yaccs/baker/layer1/utils.hpp"
#include "yaccs/baker/layer2/def.hpp"
#include "yaccs/baker/layer2/layer2.hpp"
#include "yaccs/dtype.hpp"
#include "yaccs/tensor.hpp"
#include <utility>
#include <vector>


Layer3::Layer3()
    : layer1_(new Layer1)
    , layer2_(new Layer2(layer1_))
{
}

void Layer3::set_name(const std::string& name)
{
    name_  = name;
}

void Layer3::set_main()
{
    FunctionDef fdef;
    layer2_->begin_function(fdef, T_VOID);
        Scope exe_scope{SCOPE_WORKGROUP};
        Scope mem_scope{SCOPE_WORKGROUP};
        MemSemantic mem_semantics{MS_WORKGROUP_MEMORY | MS_ACQUIRE_RELEASE};
        for (size_t i = 0; i < layers_.size(); ++i) {
            auto layer{layers_.at(i)};
            layer1_->add_function_call(layer);
            if (i < layers_.size() - 1) {
                layer1_->add_control_barrier(exe_scope, mem_scope, mem_semantics);
            }
        }
    layer2_->end_function(fdef);

    layer1_->set_entry(fdef.id);
}

void Layer3::add_input(const TensorType& tensor_type)
{
    auto storage_class{SC_STORAGE_BUFFER};
    auto type_id{add_tensor_type(tensor_type, storage_class, false)};
    auto input_tensor_type{layer1_->add_struct_dtype({type_id}, false)};
    auto var_id{layer1_->add_var(input_tensor_type, storage_class)};

    layer1_->add_struct_decorate(input_tensor_type, DECO_BLOCK, storage_class, {{0, 0}});
    layer1_->add_binding(var_id, 0, 0);

    TensorMeta tm;
    tm.name = tensor_type.name;
    tm.dtype = tensor_type.dtype;
    tm.storage_class = storage_class;
    tm.id = var_id;
    tm.dtype_id = layer1_->add_dtype(tensor_type.dtype);
    tm.dtype_pointer_id = layer1_->add_type_pointer(tm.dtype_id, storage_class);
    global_tensors_.insert(std::make_pair(tensor_type.name, tm));
    layer1_->push_entry_listed_id(var_id);
}

void Layer3::add_output(const TensorType& tensor_type)
{
    auto storage_class{SC_STORAGE_BUFFER};
    auto type_id{add_tensor_type(tensor_type, storage_class, false)};
    auto output_tensor_type{layer1_->add_struct_dtype({type_id}, false)};
    auto var_id{layer1_->add_var(output_tensor_type, storage_class)};

    layer1_->add_struct_decorate(output_tensor_type, DECO_BLOCK, storage_class, {{0, 0}});
    layer1_->add_binding(var_id, 0, 1);

    TensorMeta tm;
    tm.name = tensor_type.name;
    tm.dtype = tensor_type.dtype;
    tm.storage_class = storage_class;
    tm.id = var_id;
    tm.dtype_id = layer1_->add_dtype(tensor_type.dtype);
    tm.dtype_pointer_id = layer1_->add_type_pointer(tm.dtype_id, storage_class);
    global_tensors_.insert(std::make_pair(tensor_type.name, tm));
    layer1_->push_entry_listed_id(var_id);
}

void Layer3::dump_ir()
{
    std::ofstream ofs{name_, std::ios::out};
    layer1_->code_gen()->assemble(ofs);
    ofs.close();
}

id_t Layer3::add_tensor_type(const TensorType& tt, StorageClass sc, bool reuse)
{
    /*
     * {
     *     int dims;
     *     int shape[MAX_TENSOR_DIMS];
     *     DType data[num_elems];
     * }
     */

    const auto num_elems{tt.num_elems()};
    const auto dtype_id{layer1_->add_dtype(tt.dtype)};    // define type dtype
    const auto uint_id{layer1_->add_dtype(DT_UINT32)};    // define uint type
    const auto shape_id{layer1_->add_array_dtype(uint_id, tt.dims, sc, reuse)};
    const auto data_id{layer1_->add_array_dtype(dtype_id, num_elems, sc, reuse)};

    uint32_t offset{0};
    uint32_t field_idx{0};
    std::vector<std::pair<uint32_t, uint32_t>> member_deco;
    std::vector<id_t> struct_ids;

    member_deco.push_back(std::make_pair(field_idx++, offset)); offset += 4;
    struct_ids.push_back(uint_id);   // dims
    member_deco.push_back(std::make_pair(field_idx++, offset)); offset += 4 * tt.dims; 
    struct_ids.push_back(shape_id); // shape
    member_deco.push_back(std::make_pair(field_idx++, offset));
    struct_ids.push_back(data_id);  // data

    auto tensor_type_id{layer1_->add_struct_dtype(struct_ids, reuse)};
    layer1_->add_struct_decorate(tensor_type_id, DECO_NONE, sc, member_deco);

    return tensor_type_id;
}

id_t Layer3::add_const_tensor(const Tensor& tensor)
{
    std::vector<id_t> elem_ids;
    auto tensor_type_id{add_tensor_type(tensor.tt, SC_NONE)};

    // setup dims
    auto dims_type_id{layer1_->add_dtype(DT_UINT32)};
    elem_ids.push_back(layer1_->add_const(DT_UINT32, tensor.tt.dims));

    // setup shape
    int num_elems{1};
    std::vector<id_t> shape(tensor.tt.dims);
    for (int i = 0; i < tensor.tt.dims; ++i) {
        num_elems *= i < tensor.tt.dims ? tensor.tt.shape[i] : 1;
        shape.at(i) = layer1_->add_const(DT_UINT32, i < tensor.tt.dims ? tensor.tt.shape[i] : 0);
    }
    auto uint_id{layer1_->add_dtype(DT_UINT32)};
    auto shape_type_id{layer1_->add_array_dtype(uint_id, tensor.tt.dims, SC_GLOBAL_CONST)};
    elem_ids.push_back(layer1_->add_const_array(shape_type_id, shape));

    // setup data
    std::vector<id_t> data(num_elems);
    for (int i = 0; i < num_elems; ++i) {
        data.at(i) = add_const_tensor_element(tensor.tt.dtype, i, tensor);
    }
    auto dtype_id{layer1_->add_dtype(tensor.tt.dtype)};
    auto data_type_id{layer1_->add_array_dtype(dtype_id, num_elems, SC_GLOBAL_CONST)};
    elem_ids.push_back(layer1_->add_const_array(data_type_id, data));

    // create const tensor
    auto const_tensor_id{layer1_->add_const_struct(tensor_type_id, elem_ids)};

    TensorMeta tm;
    tm.name = tensor.tt.name;
    tm.dtype = tensor.tt.dtype;
    tm.id = const_tensor_id;
    tm.dims_id = elem_ids.at(0);
    tm.shape_id = elem_ids.at(1);
    tm.data_id = elem_ids.at(2);
    tm.dims_type_id = dims_type_id;
    tm.shape_type_id = shape_type_id;
    tm.data_type_id = data_type_id;
    tm.storage_class = SC_GLOBAL_CONST;
    tm.dtype_id = layer1_->add_dtype(tensor.tt.dtype);
    global_tensors_.insert(std::make_pair(tensor.tt.name, tm));

    return const_tensor_id;
}

id_t Layer3::add_shared_tensor(const Tensor& tensor)
{
    const auto storage_class{SC_WORKGROUP};
    const auto tensor_type_id{add_tensor_type(tensor.tt, storage_class)};
    const auto var_id{layer1_->add_var(tensor_type_id, storage_class)};

    TensorMeta tm;
    tm.name = tensor.tt.name;
    tm.dtype = tensor.tt.dtype;
    tm.id = var_id;
    tm.storage_class = storage_class;
    tm.dtype_id = layer1_->add_dtype(tensor.tt.dtype);
    global_tensors_.insert(std::make_pair(tensor.tt.name, tm));
    layer1_->push_entry_listed_id(var_id);

    return var_id;
}

id_t Layer3::add_const_tensor_element(DType dtype, int elem_idx, const Tensor& tensor)
{
    switch (dtype) {
        case DT_FLOAT: return layer1_->add_const(dtype, tensor.at<DT_FLOAT>(elem_idx));
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

    assert(false && "Unreachable");
    return 0;
}

id_t Layer3::access_tensor_dims(id_t func_id, const TensorMeta& tm)
{
    std::vector<uint32_t> access_indices{};
    if (tm.storage_class == SC_UNIFORM || tm.storage_class == SC_STORAGE_BUFFER) {
        access_indices = {0, 0};
    } else if (tm.storage_class == SC_GLOBAL_CONST) {
        return tm.dims_id;
    } else {
        access_indices = {0};
    }

    auto dims_type_id = layer1_->add_dtype(DT_UINT32);
    auto dims_type_ptr_id = layer1_->add_type_pointer(dims_type_id, storage_class_for_accessment(tm.storage_class));
    auto dims_ptr_id = layer1_->access_chain_indices(func_id, dims_type_ptr_id, tm.id, access_indices);
    return layer1_->load_var(dims_type_id, dims_ptr_id);
}

id_t Layer3::access_tensor_shape_index(id_t func_id, const TensorMeta& tm, uint32_t index)
{
    static std::vector<AccessTensorShapeEelementDef> dfs;
    AccessTensorShapeEelementDef def;

    for (const auto& it: dfs) {
        if (it.func_id == func_id && it.tensor_id == tm.id && it.index == index) {
            return it.id;
        }
    }

    def.tensor_id = tm.id;
    def.base_id = def.tensor_id;
    def.func_id = func_id;
    def.index = index;

    std::vector<uint32_t> access_indices{};
    if (tm.storage_class == SC_UNIFORM || tm.storage_class == SC_STORAGE_BUFFER) {
        access_indices = {0, 1, index};
    } else if (tm.storage_class == SC_GLOBAL_CONST) {
        def.base_id = layer1_->add_var(tm.shape_type_id, SC_FUNCTION, tm.shape_id);
        access_indices = {index};
    } else {
        access_indices = {1, index};
    }

    def.shape_comp_type_id = layer1_->add_dtype(DT_UINT32);
    def.shape_comp_type_ptr_id = layer1_->add_type_pointer(def.shape_comp_type_id, storage_class_for_accessment(tm.storage_class));
    def.shape_comp_ptr_id = layer1_->access_chain_indices(func_id, def.shape_comp_type_ptr_id, def.base_id, access_indices);
    def.id = layer1_->load_var(def.shape_comp_type_id, def.shape_comp_ptr_id);

    dfs.push_back(def);
    return def.id;
}

void Layer3::invocation_boundary_check(id_t func_id, const TensorMeta& tm, uint32_t index)
{
    IfDef if_def;
    auto cmp_op1_id{layer1_->access_invocation_index(func_id, index)};
    auto cmp_op2_id{access_tensor_shape_index(func_id, tm, index)};

    layer2_->begin_if(if_def, cmp_op1_id, CO_GT, cmp_op2_id);
        layer1_->add_return();
    layer2_->end_if(if_def);
}

id_t Layer3::load_tensor_element(id_t func_id, const TensorMeta& tm, id_t index_id)
{
    std::vector<id_t> access_index_ids{};
    auto data_index_id{layer1_->add_const(DT_UINT32, 2)};
    auto tensor_index_id{layer1_->add_const(DT_UINT32, 0)}; // for uniform input

    id_t base_id{tm.id};
    if (tm.storage_class == SC_UNIFORM || tm.storage_class == SC_STORAGE_BUFFER) {
        access_index_ids = {tensor_index_id, data_index_id, index_id};
    } else if (tm.storage_class == SC_GLOBAL_CONST) {
        base_id = layer1_->add_var(tm.data_type_id, SC_FUNCTION, tm.data_id);
        access_index_ids = {index_id};
    } else {
        access_index_ids = {data_index_id, index_id};
    }

    auto tensor_dtype_id{layer1_->add_dtype(tm.dtype)};
    auto tensor_dtype_ptr_id{layer1_->add_type_pointer(tensor_dtype_id, storage_class_for_accessment(tm.storage_class))};
    auto ptr{layer1_->access_chain(func_id, tensor_dtype_ptr_id, base_id, access_index_ids)};
    return layer1_->load_var(tensor_dtype_id, ptr);
}

id_t Layer3::load_tensor_element(id_t func_id, const TensorMeta& tm, id_t i, id_t step, id_t j)
{
    auto type_id{layer1_->add_dtype(DT_UINT32)};
    auto i_mul_step{layer1_->binary_op(BO_IMUL, func_id, type_id, i, step)};
    id_t index_id{layer1_->binary_op(BO_IADD, func_id, type_id, i_mul_step, j)};
    return load_tensor_element(func_id, tm, index_id);
}

void Layer3::store_tensor_element(id_t func_id, const TensorMeta& tm, id_t index_id, id_t object_id)
{
    auto data_index_id{layer1_->add_const(DT_UINT32, 2)};
    auto tensor_dtype_id{layer1_->add_dtype(tm.dtype)};
    auto tensor_dtype_ptr_id{layer1_->add_type_pointer(tensor_dtype_id, tm.storage_class)};
    auto tensor_index_id{layer1_->add_const(DT_UINT32, 0)}; // for uniform input

    std::vector<id_t> access_index_ids{};
    if (tm.storage_class == SC_UNIFORM || tm.storage_class == SC_STORAGE_BUFFER) {
        access_index_ids = {tensor_index_id, data_index_id, index_id};
    } else {
        access_index_ids = {data_index_id, index_id};
    }


    auto ptr{layer1_->access_chain(func_id, tensor_dtype_ptr_id, tm.id, access_index_ids)};
    layer1_->store_var(ptr, object_id);
}

void Layer3::store_tensor_element(id_t func_id, const TensorMeta& tm, id_t i, id_t step, id_t j, id_t object_id)
{
    auto type_id{layer1_->add_dtype(DT_UINT32)};
    auto i_mul_step{layer1_->binary_op(BO_IMUL, func_id, type_id, i, step)};
    id_t index_id{layer1_->binary_op(BO_IADD, func_id, type_id, i_mul_step, j)};
    store_tensor_element(func_id, tm, index_id, object_id);
}

void Layer3::store_tensor_shape_element(id_t func_id, const TensorMeta& tm, uint32_t index, id_t object_id)
{
    auto shape_base_index_id{layer1_->add_const(DT_UINT32, 1)};
    auto shape_index_id{layer1_->add_const(DT_UINT32, index)};
    auto shape_type_id{layer1_->add_dtype(DT_UINT32)};
    auto shape_type_ptr_id{layer1_->add_type_pointer(shape_type_id, tm.storage_class)};

    std::vector<id_t> access_index_ids{};
    if (tm.storage_class == SC_UNIFORM || tm.storage_class == SC_STORAGE_BUFFER) {
        auto tensor_index_id{layer1_->add_const(DT_UINT32, 0)}; // for uniform input
        access_index_ids = {tensor_index_id, shape_base_index_id, shape_index_id};
    } else {
        access_index_ids = {shape_base_index_id, shape_index_id};
    }

    auto ptr{layer1_->access_chain(func_id, shape_type_ptr_id, tm.id, access_index_ids)};
    layer1_->store_var(ptr, object_id);
}

void Layer3::store_tensor_dims(id_t func_id, const TensorMeta& tm, id_t object_id)
{
    std::vector<uint32_t> access_indices{};
    if (tm.storage_class == SC_UNIFORM || tm.storage_class == SC_STORAGE_BUFFER) {
        access_indices = {0, 0};
    } else {
        access_indices = {0};
    }

    auto dims_type_id{layer1_->add_dtype(DT_UINT32)};
    auto dims_type_ptr_id{layer1_->add_type_pointer(dims_type_id, tm.storage_class)};
    auto ptr{layer1_->access_chain_indices(func_id, dims_type_ptr_id, tm.id, access_indices)};
    layer1_->store_var(ptr, object_id);
}
