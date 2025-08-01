#include "yaccs/code_gen/program.hpp"
#include "yaccs/code_gen/def.hpp"
#include "yaccs/code_gen/utils.hpp"
#include "yaccs/dtype.hpp"
#include "yaccs/tensor.hpp"
#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <cassert>
#include <cstring>
#include <fstream>
#include <utility>
#include <iostream>
#include <vector>


Program::Program()
{
    code_gen_.push_header();
}

void Program::set_name(const std::string& name)
{
    name_  = name;
}

void Program::set_main()
{
    EntryDef ed;
    ed.local_size_x = 4;
    ed.local_size_y = 4;
    ed.local_size_z = 1;
    ed.main_id = add_function_prologue(add_void_type());

        auto invo_id{global_invocation_id()};
        ed.input_ids.push_back(invo_id);

        Scope exe_scope{SCOPE_WORKGROUP};
        Scope mem_scope{SCOPE_WORKGROUP};
        MemSemantic mem_semantics{MS_WORKGROUP_MEMORY | MS_ACQUIRE_RELEASE};
        for (size_t i = 0; i < layers_.size(); ++i) {
            auto layer{layers_.at(i)};
            add_function_call(layer);
            if (i < layers_.size() - 1) {
                add_control_barrier(exe_scope, mem_scope, mem_semantics);
            }
        }

    add_function_epilogue();

    code_gen_.push_entry(ed);
}

void Program::add_input(const TensorType& tensor_type)
{
    DecorateSetBindingDef binding_deco;
    DecorateStructDef struct_deco;
    auto storage_type{SC_UNIFORM};

    auto type_id{add_tensor_type(tensor_type, storage_type, false)};
    auto input_tensor_type{add_struct_dtype({type_id}, false)};
    auto var_id{add_var(input_tensor_type, storage_type)};

    struct_deco.deco = DECO_BUFFER_BLOCK;
    struct_deco.struct_type_id = input_tensor_type;
    struct_deco.member_deco.push_back(DecorateStructDef::FieldOffset{.field = 0, .offset = 0});
    code_gen_.push_struct_decorate(struct_deco);

    binding_deco.binding = 0;
    binding_deco.set = 0;
    binding_deco.target = var_id;
    code_gen_.push_decorate_set_binding(binding_deco);

    TensorMeta tm;
    tm.name = tensor_type.name;
    tm.dims = tensor_type.dims;
    tm.dtype = tensor_type.dtype;
    tm.tensor_id = var_id;
    tm.dtype_id = add_dtype(tensor_type.dtype);
    tm.dtype_pointer_id = add_type_pointer(tm.dtype_id, storage_type);
    memcpy(tm.shape, tensor_type.shape, sizeof(tensor_type.shape[0]) * MAX_TENSOR_DIMS);
    global_tensors_.insert(std::make_pair(tensor_type.name, tm));
}

void Program::add_output(const TensorType& tensor_type)
{
    DecorateSetBindingDef binding_deco;
    DecorateStructDef struct_deco;
    auto storage_type{SC_UNIFORM};

    auto type_id{add_tensor_type(tensor_type, storage_type, false)};
    auto output_tensor_type{add_struct_dtype({type_id}, false)};
    auto var_id{add_var(output_tensor_type, storage_type)};

    struct_deco.deco = DECO_BUFFER_BLOCK;
    struct_deco.struct_type_id = output_tensor_type;
    struct_deco.member_deco.push_back(DecorateStructDef::FieldOffset{.field = 0, .offset = 0});
    code_gen_.push_struct_decorate(struct_deco);

    binding_deco.binding = 0;
    binding_deco.set = 1;
    binding_deco.target = var_id;
    code_gen_.push_decorate_set_binding(binding_deco);

    TensorMeta tm;
    tm.name = tensor_type.name;
    tm.dims = tensor_type.dims;
    tm.dtype = tensor_type.dtype;
    tm.tensor_id = var_id;
    tm.dtype_id = add_dtype(tensor_type.dtype);
    tm.dtype_pointer_id = add_type_pointer(tm.dtype_id, storage_type);
    memcpy(tm.shape, tensor_type.shape, sizeof(tensor_type.shape[0]) * MAX_TENSOR_DIMS);
    global_tensors_.insert(std::make_pair(tensor_type.name, tm));
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

id_t Program::add_type_pointer(id_t type_id, StorageClass sc)
{
    static std::vector<TypePointerDef> dfs;

    for (size_t i = 0; i < dfs.size(); ++i) {
        const auto& df{dfs.at(i)};
        if (df.type_id == type_id && df.storage_class == sc) {
            return df.id;
        }
    }

    TypePointerDef tpd;
    tpd.type_id = type_id;
    tpd.id  = alloc_id();
    tpd.storage_class = sc;
    dfs.push_back(tpd);
    code_gen_.push_type_pointer(tpd);

    return tpd.id;
}

id_t Program::add_var(id_t type_id, StorageClass sc)
{
    VarDef vd;
    vd.id = alloc_id();
    vd.storage_class = sc;
    vd.type_pointer_id = add_type_pointer(type_id, sc);
    code_gen_.push_variable(vd);
    return vd.id;
}

id_t Program::add_struct_dtype(const std::vector<id_t>& dtypes, bool reuse)
{
    static std::vector<StructTypeDef> defs;

    auto struct_match{[&dtypes] (const StructTypeDef& std) -> bool {
        if (std.num_fields != dtypes.size()) return false;
        for (int i = 0; i < std.num_fields; ++i) {
            if (std.fields[i] != dtypes.at(i)) return false;
        }
        return true;
    }};

    if (reuse) {
        for (const auto& it : defs) {
            if (struct_match(it)) {
                return it.id;
            }
        }
    }

    StructTypeDef std{.id = alloc_id(), .num_fields = dtypes.size()};
    std.fields.resize(std.num_fields);
    memcpy(std.fields.data(), dtypes.data(), sizeof(std.fields[0]) * std.num_fields);
    if (reuse) {
        defs.push_back(std);
    }
    code_gen_.push_struct_dtype(std);
    return std.id;
}

id_t Program::add_array_dtype(id_t dtype, uint32_t length, StorageClass sc, bool reuse)
{
    static std::vector<ArrTypeDef> defs;
    static std::vector<DecorateArrayDef> decos;

    DecorateArrayDef this_deco;
    auto already_decorate_in{[&this_deco] (const std::vector<DecorateArrayDef>& targets) -> bool {
        for (const auto& it : targets) {
            if (it.array_type_id == this_deco.array_type_id) {
                return true;
            }
        }
        return false;
    }};

    id_t array_type_id{};
    bool should_create_array_type{true};
    if (reuse) {
        for (const auto& it : defs) {
            if (it.dtype == dtype && it.length == length) {
                array_type_id = it.id;
                should_create_array_type = false;
                break;
            }
        }
    }

    if (should_create_array_type) {
        id_t length_id{add_const(DT_UINT32, length)};
        ArrTypeDef arr{.length = length, .dtype = dtype, .length_id = length_id, .id = alloc_id()};
        array_type_id = arr.id;

        if (reuse) {
            defs.push_back(arr);   
        }
        code_gen_.push_array_dtype(arr);
    }

    this_deco.array_type_id = array_type_id;
    if (should_decorate(sc) && !already_decorate_in(decos)) {
        code_gen_.push_array_decorate(this_deco);
        decos.push_back(this_deco);
    }

    return array_type_id;
}

id_t Program::add_tensor_type(const TensorType& tensor_type, StorageClass sc, bool reuse)
{
    /*
     * {
     *     int dims;
     *     int shape[MAX_TENSOR_DIMS];
     *     DType data[num_elems];
     * }
     */

    static std::vector<DecorateStructDef> decos;

    int num_elems{1};
    for (int i = 0; i < tensor_type.dims; ++i) {
        num_elems *= tensor_type.shape[i];
    }

    auto dtype_id{add_dtype(tensor_type.dtype)};    // define type dtype
    auto uint_id{add_dtype(DT_UINT32)};               // define uint type
    auto shape_id{add_array_dtype(uint_id, MAX_TENSOR_DIMS, sc, reuse)};
    auto data_id{add_array_dtype(dtype_id, num_elems, sc, reuse)};

    uint32_t offset{0};
    uint32_t field_idx{0};
    DecorateStructDef dsd;

    std::vector<id_t> struct_ids;
    dsd.member_deco.push_back({.field=field_idx++, .offset = offset}); offset += 16;
    struct_ids.push_back(uint_id);   // dims
    dsd.member_deco.push_back({.field=field_idx++, .offset = offset}); offset += 16 * MAX_TENSOR_DIMS; 
    struct_ids.push_back(shape_id); // shape
    dsd.member_deco.push_back({.field=field_idx++, .offset = offset});
    struct_ids.push_back(data_id);  // data

    auto already_decorate_in{[&dsd] (const std::vector<DecorateStructDef>& targets) -> bool {
        for (const auto& it : targets) {
            if (it.struct_type_id == dsd.struct_type_id) return true;
        }
        return false;
    }};

    auto tensor_type_id{add_struct_dtype(struct_ids, reuse)};
    dsd.deco = DECO_NONE;
    dsd.struct_type_id = tensor_type_id;

    if (should_decorate(sc) && !already_decorate_in(decos)) {
        decos.push_back(dsd);
        code_gen_.push_struct_decorate(dsd);
    }

    return tensor_type_id;
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

    ConstCompositeDef sd;
    auto tensor_type_id{add_tensor_type(tensor.tt, SC_NONE)};
    sd.type_id = tensor_type_id;

    // setup dims
    sd.elem_ids.push_back(add_const(DT_UINT32, tensor.tt.dims));

    // setup shape
    int num_elems{1};
    std::vector<id_t> shape(MAX_TENSOR_DIMS);
    for (int i = 0; i < MAX_TENSOR_DIMS; ++i) {
        num_elems *= i < tensor.tt.dims ? tensor.tt.shape[i] : 1;
        shape.at(i) = add_const(DT_UINT32, i < tensor.tt.dims ? tensor.tt.shape[i] : 0);
    }
    auto uint_id{add_dtype(DT_UINT32)};
    auto shape_id{add_array_dtype(uint_id, MAX_TENSOR_DIMS, SC_NONE)};
    sd.elem_ids.push_back(add_const_array(shape_id, shape));

    // setup data
    std::vector<id_t> data(num_elems);
    for (int i = 0; i < num_elems; ++i) {
        data.at(i) = add_raw_const(tensor.tt.dtype, i, tensor);
    }
    auto dtype_id{add_dtype(tensor.tt.dtype)};
    auto data_id{add_array_dtype(dtype_id, num_elems, SC_NONE)};
    sd.elem_ids.push_back(add_const_array(data_id, data));

    auto const_struct_matched{[&sd] (const ConstCompositeDef& target) -> bool {
        if (target.type_id != sd.type_id) {
            return false;
        }
        return sd.elem_ids == target.elem_ids;
    }};

    bool should_create_const_tensor{true};
    for (const auto& it : dfs) {
        if (const_struct_matched(it)) {
            sd.id = it.id;
            should_create_const_tensor = false;
            break;
        }
    }

    if (should_create_const_tensor) {
        sd.id = alloc_id();
        dfs.push_back(sd);
        code_gen_.push_const_composite(sd);
    }

    TensorMeta tm;
    tm.name = tensor.tt.name;
    tm.dims = tensor.tt.dims;
    tm.dtype = tensor.tt.dtype;
    tm.tensor_id = sd.id;
    tm.dtype_id = add_dtype(tensor.tt.dtype);
    memcpy(tm.shape, tensor.tt.shape, sizeof(tensor.tt.shape[0]) * MAX_TENSOR_DIMS);
    global_tensors_.insert(std::make_pair(tensor.tt.name, tm));

    return sd.id;
}

id_t Program::add_shared_tensor(const Tensor& tensor)
{
    auto storage_class{SC_WORKGROUP};
    auto tensor_type_id{add_tensor_type(tensor.tt, storage_class)};

    VarDef vd;
    vd.type_pointer_id = add_type_pointer(tensor_type_id, storage_class);
    vd.storage_class = storage_class;
    vd.id = alloc_id();
    code_gen_.push_variable(vd);

    TensorMeta tm;
    tm.name = tensor.tt.name;
    tm.dims = tensor.tt.dims;
    tm.dtype = tensor.tt.dtype;
    tm.tensor_id = vd.id;
    tm.dtype_id = add_dtype(tensor.tt.dtype);
    memcpy(tm.shape, tensor.tt.shape, sizeof(tensor.tt.shape[0]) * MAX_TENSOR_DIMS);
    global_tensors_.insert(std::make_pair(tensor.tt.name, tm));

    return vd.id;
}


id_t Program::add_raw_const(DType dtype, int elem_idx, const Tensor& tensor)
{
    switch (dtype) {
        case DT_FLOAT: return add_const(dtype, tensor.at<DT_FLOAT>(elem_idx));
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

    global_funcs_.insert(std::make_pair(fh.id, fh));
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
    auto func_id{add_function_prologue(void_type_id)};

        const auto alpha{gemm.alpha};
        const auto beta{gemm.beta};
        add_const_tensor(gemm.B);
        add_const_tensor(gemm.C);
        add_shared_tensor(gemm.Y);

        const auto& A{global_tensors_.at(gemm.A.tt.name)};
        const auto& B{global_tensors_.at(gemm.B.tt.name)};
        const auto& C{global_tensors_.at(gemm.C.tt.name)};
        const auto& Y{global_tensors_.at(gemm.Y.tt.name)};

    add_function_epilogue();
    layers_.push_back(func_id);
}

void Program::add_relu(const OpRelu& relu)
{
    auto void_type_id{add_void_type()};
    auto func_id{add_function_prologue(void_type_id)};

        const auto& X{global_tensors_.at(relu.X.tt.name)};
        // infer Y from X
        Tensor tensor_Y;
        tensor_Y.tt.name = relu.Y.tt.name;
        tensor_Y.tt.dims = X.dims;
        tensor_Y.tt.dtype = X.dtype;
        tensor_Y.tt.row_major = true;
        memcpy(tensor_Y.tt.shape, X.shape, MAX_TENSOR_DIMS * sizeof(X.shape[0]));
        add_shared_tensor(tensor_Y);
        const auto& Y{global_tensors_.at(relu.Y.tt.name)};

        invocation_boundary_check(func_id, Y.tensor_id, 0);
        invocation_boundary_check(func_id, Y.tensor_id, 1);
        // define relu operation
        // auto invo_id{global_invocation_id()};
        // auto uint32_id{add_dtype(DT_UINT32)};
        // auto invo_id_x{add_var(uint32_id, SC_FUNCTION)};
        // auto invo_id_x_tp{add_type_pointer(uint32_id, SC_INPUT)};
        // auto pointer{access_chain(func_id, invo_id_x_tp, invo_id, 0)};
        // id_t invo_x_object{load_var(uint32_id, pointer)};
        // store_var(invo_id_x, invo_x_object);
        // auto invo_x = gl_GlobalInvocationID.x;
        // auto invo_y = gl_GlobalInvocationID.y;
        // auto input = X.load(x, v);
        // input = max(input, 0);
        // Y.store(input);

    add_function_epilogue();
    layers_.push_back(func_id);
}

id_t Program::add_function_call(id_t id)
{
    FunctionCallDef fcd;
    fcd.func_id = id;
    fcd.id = alloc_id();
    fcd.return_type_id = find_function_def(id).return_type_id;
    code_gen_.push_function_call(fcd);
    return fcd.id;
}

void Program::add_control_barrier(Scope exe_scope, Scope mem_scope, MemSemantic mem_semantics)
{
    ControlBarrierDef cbd;

    cbd.exe_scope_id = add_const(DType::DT_UINT32, static_cast<uint32_t>(exe_scope));
    cbd.mem_scope_id = add_const(DType::DT_UINT32, static_cast<uint32_t>(mem_scope));
    cbd.mem_semantics_id = add_const(DType::DT_UINT32, static_cast<uint32_t>(mem_semantics));

    code_gen_.push_control_barrier(cbd);
}

id_t Program::add_vector_dtype(id_t component_type_id, int count)
{
    static std::vector<VectorDef> defs;

    for (const auto& it : defs) {
        if (it.component_type_id == component_type_id && it.count == count) {
            return it.id;
        }
    }

    VectorDef vd{.id = alloc_id(), .component_type_id = component_type_id, .count = count};
    defs.push_back(vd);
    code_gen_.push_vector_dtype(vd);

    return vd.id;
}

id_t Program::global_invocation_id()
{
    static bool has_defined{false};
    static id_t id{0};
    if (has_defined) {
        return id;
    }

    auto uint_type_id{add_dtype(DT_UINT32)};
    auto vec_uint_3{add_vector_dtype(uint_type_id, 3)};
    id = add_var(vec_uint_3, SC_INPUT);
    has_defined = true;

    DecorateBuiltInDef deco;
    deco.var_id = id;
    deco.built_in = BI_GLOBAL_INVOCATION_ID;
    code_gen_.push_builtin_decorate(deco);

    return id;
}

id_t Program::load_var(id_t dtype_id, id_t pointer)
{
    LoadDef ld;
    ld.pointer = pointer;
    ld.type_id = dtype_id;
    ld.id = alloc_id();
    code_gen_.push_load(ld);
    return ld.id;
}

void Program::store_var(id_t pointer, id_t object)
{
    StoreDef sd;
    sd.pointer = pointer;
    sd.object = object;
    code_gen_.push_store(sd);
}

id_t Program::access_chain(id_t func_id, id_t type_id, id_t base_id, const std::vector<uint32_t>& indices)
{
    static std::vector<AccessChainDef> dfs;
    AccessChainDef acd;

    acd.index_ids.reserve(indices.size());
    for (auto index: indices) {
        acd.index_ids.push_back(add_const(DT_UINT32, index));
    }

    acd.func_id = func_id;
    acd.base_id = base_id;
    acd.type_id = type_id;

    // reusable check
    for (const auto& it: dfs) {
        if (it.func_id == acd.func_id && it.base_id == acd.base_id && it.index_ids == acd.index_ids) {
            return it.id;
        }
    }

    acd.id = alloc_id();
    code_gen_.push_access_chain(acd);
    dfs.push_back(acd);
    return acd.id;
}

void Program::invocation_boundary_check(id_t func_id, id_t tensor_id, uint32_t index)
{
    InvocationBoundCheckDef def;
    def.label_id_ret = alloc_id();
    def.label_id_next = alloc_id();
    def.bool_type_id = add_dtype(DT_BOOL);
    def.condition_id = alloc_id();
    // for invocation
    def.invo_id = global_invocation_id();
    def.invo_comp_type_id = add_dtype(DT_UINT32);
    def.invo_comp_type_ptr_id = add_type_pointer(def.invo_comp_type_id, SC_INPUT);
    def.invo_comp_ptr = access_chain(func_id, def.invo_comp_type_ptr_id, def.invo_id, {index});
    def.invo_comp_id = load_var(def.invo_comp_type_id, def.invo_comp_ptr);
    // for tensor
    def.tensor_id = tensor_id;
    def.tensor_shape_comp_type_id = add_dtype(DT_UINT32);
    def.tensor_shape_comp_type_ptr_id = add_type_pointer(def.tensor_shape_comp_type_id, SC_WORKGROUP);
    def.tensor_shape_comp_ptr_id = access_chain(func_id, def.tensor_shape_comp_type_ptr_id, tensor_id, {1, index});
    def.tensor_shape_comp_id = load_var(def.tensor_shape_comp_type_id, def.tensor_shape_comp_ptr_id);
    code_gen_.push_snippet_invo_bound_check(def);
}

FunctionHeaderDef& Program::find_function_def(id_t id)
{
    return global_funcs_.at(id);
}
