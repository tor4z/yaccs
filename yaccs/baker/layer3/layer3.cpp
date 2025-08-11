#include "yaccs/baker/layer3.hpp"
#include "yaccs/baker/def.hpp"
#include "yaccs/baker/utils.hpp"
#include "yaccs/dtype.hpp"
#include "yaccs/tensor.hpp"


Layer3::Layer3()
{
}

void Layer3::set_name(const std::string& name)
{
    name_  = name;
}

void Layer3::set_main()
{
    push_entry_listed_id(global_invocation_id());

    EntryDef ed;
    ed.local_size_x = 4;
    ed.local_size_y = 4;
    ed.local_size_z = 1;
    ed.input_ids = entry_listed_ids_;
    ed.main_id = add_function_prologue(add_void_type());
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

void Layer3::add_input(const TensorType& tensor_type)
{
    DecorateSetBindingDef binding_deco;
    DecorateStructDef struct_deco;
    auto storage_class{SC_STORAGE_BUFFER};

    auto type_id{add_tensor_type(tensor_type, storage_class, false)};
    auto input_tensor_type{add_struct_dtype({type_id}, false)};
    auto var_id{add_var(input_tensor_type, storage_class)};

    struct_deco.deco = DECO_BLOCK;
    struct_deco.struct_type_id = input_tensor_type;
    struct_deco.member_deco.push_back(DecorateStructDef::FieldOffset{.field = 0, .offset = 0});
    code_gen_.push_struct_decorate(struct_deco);

    binding_deco.binding = 0;
    binding_deco.set = 0;
    binding_deco.target = var_id;
    code_gen_.push_decorate_set_binding(binding_deco);

    TensorMeta tm;
    tm.name = tensor_type.name;
    tm.dtype = tensor_type.dtype;
    tm.storage_class = storage_class;
    tm.id = var_id;
    tm.dtype_id = add_dtype(tensor_type.dtype);
    tm.dtype_pointer_id = add_type_pointer(tm.dtype_id, storage_class);
    global_tensors_.insert(std::make_pair(tensor_type.name, tm));
    push_entry_listed_id(var_id);
}

void Layer3::add_output(const TensorType& tensor_type)
{
    DecorateSetBindingDef binding_deco;
    DecorateStructDef struct_deco;
    auto storage_class{SC_STORAGE_BUFFER};

    auto type_id{add_tensor_type(tensor_type, storage_class, false)};
    auto output_tensor_type{add_struct_dtype({type_id}, false)};
    auto var_id{add_var(output_tensor_type, storage_class)};

    struct_deco.deco = DECO_BLOCK;
    struct_deco.struct_type_id = output_tensor_type;
    struct_deco.member_deco.push_back(DecorateStructDef::FieldOffset{.field = 0, .offset = 0});
    code_gen_.push_struct_decorate(struct_deco);

    binding_deco.binding = 0;
    binding_deco.set = 1;
    binding_deco.target = var_id;
    code_gen_.push_decorate_set_binding(binding_deco);

    TensorMeta tm;
    tm.name = tensor_type.name;
    tm.dtype = tensor_type.dtype;
    tm.storage_class = storage_class;
    tm.id = var_id;
    tm.dtype_id = add_dtype(tensor_type.dtype);
    tm.dtype_pointer_id = add_type_pointer(tm.dtype_id, storage_class);
    global_tensors_.insert(std::make_pair(tensor_type.name, tm));
    push_entry_listed_id(var_id);
}

void Layer3::dump_ir()
{
    std::ofstream ofs{name_, std::ios::out};
    code_gen_.assemble(ofs);
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

    static std::vector<DecorateStructDef> decos;

    int num_elems{1};
    for (int i = 0; i < tt.dims; ++i) {
        num_elems *= tt.shape.at(i);
    }

    auto dtype_id{add_dtype(tt.dtype)};    // define type dtype
    auto uint_id{add_dtype(DT_UINT32)};               // define uint type
    auto shape_id{add_array_dtype(uint_id, tt.dims, sc, reuse)};
    auto data_id{add_array_dtype(dtype_id, num_elems, sc, reuse)};

    uint32_t offset{0};
    uint32_t field_idx{0};
    DecorateStructDef dsd;

    std::vector<id_t> struct_ids;
    dsd.member_deco.push_back({.field=field_idx++, .offset = offset}); offset += 4;
    struct_ids.push_back(uint_id);   // dims
    dsd.member_deco.push_back({.field=field_idx++, .offset = offset}); offset += 4 * tt.dims; 
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

id_t Layer3::add_const_tensor(const Tensor& tensor)
{
    static std::vector<ConstCompositeDef> dfs;

    ConstCompositeDef sd;
    auto tensor_type_id{add_tensor_type(tensor.tt, SC_NONE)};
    sd.type_id = tensor_type_id;

    // setup dims
    auto dims_type_id{add_dtype(DT_UINT32)};
    sd.elem_ids.push_back(add_const(DT_UINT32, tensor.tt.dims));

    // setup shape
    int num_elems{1};
    std::vector<id_t> shape(tensor.tt.dims);
    for (int i = 0; i < tensor.tt.dims; ++i) {
        num_elems *= i < tensor.tt.dims ? tensor.tt.shape[i] : 1;
        shape.at(i) = add_const(DT_UINT32, i < tensor.tt.dims ? tensor.tt.shape[i] : 0);
    }
    auto uint_id{add_dtype(DT_UINT32)};
    auto shape_type_id{add_array_dtype(uint_id, tensor.tt.dims, SC_GLOBAL_CONST)};
    sd.elem_ids.push_back(add_const_array(shape_type_id, shape));

    // setup data
    std::vector<id_t> data(num_elems);
    for (int i = 0; i < num_elems; ++i) {
        data.at(i) = add_raw_const(tensor.tt.dtype, i, tensor);
    }
    auto dtype_id{add_dtype(tensor.tt.dtype)};
    auto data_type_id{add_array_dtype(dtype_id, num_elems, SC_GLOBAL_CONST)};
    sd.elem_ids.push_back(add_const_array(data_type_id, data));

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
    tm.dtype = tensor.tt.dtype;
    tm.id = sd.id;
    tm.dims_id = sd.elem_ids.at(0);
    tm.shape_id = sd.elem_ids.at(1);
    tm.data_id = sd.elem_ids.at(2);
    tm.dims_type_id = dims_type_id;
    tm.shape_type_id = shape_type_id;
    tm.data_type_id = data_type_id;
    tm.storage_class = SC_GLOBAL_CONST;
    tm.dtype_id = add_dtype(tensor.tt.dtype);
    global_tensors_.insert(std::make_pair(tensor.tt.name, tm));

    return sd.id;
}

id_t Layer3::add_shared_tensor(const Tensor& tensor)
{
    auto storage_class{SC_WORKGROUP};
    auto tensor_type_id{add_tensor_type(tensor.tt, storage_class)};

    VarDef vd;
    vd.type_pointer_id = add_type_pointer(tensor_type_id, storage_class);
    vd.storage_class = storage_class;
    vd.initializer_id = 0;
    vd.id = alloc_id();
    code_gen_.push_variable(vd);

    TensorMeta tm;
    tm.name = tensor.tt.name;
    tm.dtype = tensor.tt.dtype;
    tm.id = vd.id;
    tm.storage_class = storage_class;
    tm.dtype_id = add_dtype(tensor.tt.dtype);
    global_tensors_.insert(std::make_pair(tensor.tt.name, tm));
    push_entry_listed_id(vd.id);

    return vd.id;
}

id_t Layer3::add_raw_const(DType dtype, int elem_idx, const Tensor& tensor)
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

void Layer3::add_gemm(const OpGemm& gemm)
{
    auto void_type_id{add_void_type()};
    auto func_id{add_function_prologue(void_type_id)};

        const auto alpha{gemm.alpha};
        const auto beta{gemm.beta};
        
        Tensor B_alpha{gemm.trans_b ? gemm.B.transpose() : gemm.B};
        Tensor C_beta{gemm.C};
        B_alpha.mul(gemm.alpha);
        C_beta.mul(gemm.beta);
        add_const_tensor(B_alpha);
        add_const_tensor(C_beta);
        add_shared_tensor(gemm.Y);

        const auto& A{global_tensors_.at(gemm.A.tt.name)};
        const auto& B{global_tensors_.at(gemm.B.tt.name)};
        const auto& C{global_tensors_.at(gemm.C.tt.name)};
        const auto& Y{global_tensors_.at(gemm.Y.tt.name)};

        auto A_shape0{access_tensor_shape_index(func_id, A, 0)};
        auto A_shape1{access_tensor_shape_index(func_id, A, 1)};
        auto B_shape0{access_tensor_shape_index(func_id, B, 0)};
        auto B_shape1{access_tensor_shape_index(func_id, B, 1)};
        auto A_dims{access_tensor_dims(func_id, A)};
        store_tensor_shape_element(func_id, Y, 0, gemm.trans_a ? A_shape1 : A_shape0);
        store_tensor_shape_element(func_id, Y, 1, B_shape1);
        store_tensor_dims(func_id, Y, A_dims);

        auto this_element_var{add_var(Y.dtype_id, SC_FUNCTION, add_const(Y.dtype, 0))};
        auto shape_element_type_id{add_dtype(DT_UINT32)};
        auto bo_mul{Y.dtype == DT_FLOAT ? BO_FMUL : BO_IMUL};
        auto bo_add{Y.dtype == DT_FLOAT ? BO_FADD : BO_IADD};
        auto invo_x{access_invocation_index(func_id, 0)};
        auto invo_y{access_invocation_index(func_id, 1)};
        
        invocation_boundary_check(func_id, Y, 0);
        invocation_boundary_check(func_id, Y, 1);
        
        ForLoopDef for_def{.i_boundary_id = gemm.trans_a ? A_shape0 : A_shape1};
        begin_for(for_def);
            auto i_id{load_var(for_def.i_type_id, for_def.i_var_id)};
            id_t A_row_begin{};
            id_t A_element_index{};
            if (gemm.trans_a) {
                A_row_begin = binary_op(BO_IMUL, func_id, shape_element_type_id, invo_y, A_shape0);
                A_element_index = binary_op(BO_IADD, func_id, shape_element_type_id, A_row_begin, i_id);
            } else {
                A_row_begin = binary_op(BO_IMUL, func_id, shape_element_type_id, invo_x, A_shape1);
                A_element_index = binary_op(BO_IADD, func_id, shape_element_type_id, A_row_begin, i_id);
            }
            auto B_row_begin{binary_op(BO_IMUL, func_id, shape_element_type_id, i_id, B_shape1)};
            auto B_element_index{binary_op(BO_IADD, func_id, shape_element_type_id, B_row_begin, invo_y)};
            auto A_element{load_tensor_element(func_id, A, A_element_index)};
            auto B_element{load_tensor_element(func_id, B, B_element_index)};
            auto AB_mul{binary_op(bo_mul, func_id, Y.dtype_id, A_element, B_element)};
            auto this_element_val{load_var(Y.dtype_id, this_element_var)};
            auto this_element_accu{binary_op(bo_add, func_id, Y.dtype_id, AB_mul, this_element_val)};
            store_var(this_element_var, this_element_accu);
        end_for(for_def);

        auto Y_shape1{access_tensor_shape_index(func_id, Y, 1)};
        auto AB_element_val{load_var(Y.dtype_id, this_element_var)};
        auto C_element_id{load_tensor_element(func_id, C, invo_x)};
        auto final_this_element_val{binary_op(bo_add, func_id, Y.dtype_id, AB_element_val, C_element_id)};
        store_tensor_element(func_id, Y, invo_x, Y_shape1, invo_y, final_this_element_val);

    add_function_epilogue();
    layers_.push_back(func_id);
}

void Layer3::add_relu(const OpRelu& relu)
{
    auto void_type_id{add_void_type()};
    auto func_id{add_function_prologue(void_type_id)};

        const auto& X{global_tensors_.at(relu.X.tt.name)};
        // infer Y from X
        Tensor tensor_Y;
        tensor_Y.tt.name = relu.Y.tt.name;
        tensor_Y.tt.dtype = X.dtype;
        tensor_Y.tt.row_major = true;
        add_shared_tensor(relu.Y);
        const auto& Y{global_tensors_.at(relu.Y.tt.name)};

        // Setup output tensor dims and shape
        auto X_shape0{access_tensor_shape_index(func_id, X, 0)};
        auto X_shape1{access_tensor_shape_index(func_id, X, 1)};
        auto X_dims_id{access_tensor_dims(func_id, X)};
        store_tensor_dims(func_id, Y, X_dims_id);
        store_tensor_shape_element(func_id, Y, 0, X_shape0);
        store_tensor_shape_element(func_id, Y, 1, X_shape1);

        // boundary check
        invocation_boundary_check(func_id, Y, 0);
        invocation_boundary_check(func_id, Y, 1);

        // relu operator eval
        auto invo_x{access_invocation_index(func_id, 0)};
        auto invo_y{access_invocation_index(func_id, 1)};
        auto x{load_tensor_element(func_id, X, invo_x, X_shape1, invo_y)};
        auto relu_result{std_450_.max(X.dtype, func_id, add_const(X.dtype, 0), x)};
        store_tensor_element(func_id, Y, invo_x, X_shape1, invo_y, relu_result);

    add_function_epilogue();
    layers_.push_back(func_id);
}

id_t Layer3::access_tensor_dims(id_t func_id, const TensorMeta& tm)
{
    std::vector<uint32_t> access_indices{};
    if (tm.storage_class == SC_UNIFORM) {
        access_indices = {0, 0};
    } else if (tm.storage_class == SC_STORAGE_BUFFER) {
        access_indices = {0, 0};
    } else if (tm.storage_class == SC_GLOBAL_CONST) {
        return tm.dims_id;
    } else {
        access_indices = {0};
    }

    auto dims_type_id = add_dtype(DT_UINT32);
    auto dims_type_ptr_id = add_type_pointer(dims_type_id, storage_class_for_accessment(tm.storage_class));
    auto dims_ptr_id = access_chain_indices(func_id, dims_type_ptr_id, tm.id, access_indices);
    return load_var(dims_type_id, dims_ptr_id);
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
    if (tm.storage_class == SC_UNIFORM) {
        access_indices = {0, 1, index};
    } else if (tm.storage_class == SC_STORAGE_BUFFER) {
        access_indices = {0, 1, index};
    } else if (tm.storage_class == SC_GLOBAL_CONST) {
        def.base_id = add_var(tm.shape_type_id, SC_FUNCTION, tm.shape_id);
        access_indices = {index};
    } else {
        access_indices = {1, index};
    }

    def.shape_comp_type_id = add_dtype(DT_UINT32);
    def.shape_comp_type_ptr_id = add_type_pointer(def.shape_comp_type_id, storage_class_for_accessment(tm.storage_class));
    def.shape_comp_ptr_id = access_chain_indices(func_id, def.shape_comp_type_ptr_id, def.base_id, access_indices);
    def.id = load_var(def.shape_comp_type_id, def.shape_comp_ptr_id);

    dfs.push_back(def);
    return def.id;
}

void Layer3::invocation_boundary_check(id_t func_id, const TensorMeta& tm, uint32_t index)
{
    IfDef if_def;
    auto cmp_op1_id{access_invocation_index(func_id, index)};
    auto cmp_op2_id{access_tensor_shape_index(func_id, tm, index)};

    begin_if(if_def, cmp_op1_id, CO_GT, cmp_op2_id);
        add_return();
    end_if(if_def);
}

id_t Layer3::load_tensor_element(id_t func_id, const TensorMeta& tm, id_t index_id)
{
    std::vector<id_t> access_index_ids{};
    auto data_index_id{add_const(DT_UINT32, 2)};
    auto tensor_index_id{add_const(DT_UINT32, 0)}; // for uniform input

    id_t base_id{tm.id};
    if (tm.storage_class == SC_UNIFORM) {
        access_index_ids = {tensor_index_id, data_index_id, index_id};
    } else if (tm.storage_class == SC_STORAGE_BUFFER) {
        access_index_ids = {tensor_index_id, data_index_id, index_id};
    } else if (tm.storage_class == SC_GLOBAL_CONST) {
        base_id = add_var(tm.data_type_id, SC_FUNCTION, tm.data_id);
        access_index_ids = {index_id};
    } else {
        access_index_ids = {data_index_id, index_id};
    }

    auto tensor_dtype_id{add_dtype(tm.dtype)};
    auto tensor_dtype_ptr_id{add_type_pointer(tensor_dtype_id, storage_class_for_accessment(tm.storage_class))};
    auto ptr{access_chain(func_id, tensor_dtype_ptr_id, base_id, access_index_ids)};
    return load_var(tensor_dtype_id, ptr);
}

id_t Layer3::load_tensor_element(id_t func_id, const TensorMeta& tm, id_t i, id_t step, id_t j)
{
    auto type_id{add_dtype(DT_UINT32)};
    auto i_mul_step{binary_op(BO_IMUL, func_id, type_id, i, step)};
    id_t index_id{binary_op(BO_IADD, func_id, type_id, i_mul_step, j)};
    return load_tensor_element(func_id, tm, index_id);
}

void Layer3::store_tensor_element(id_t func_id, const TensorMeta& tm, id_t index_id, id_t object_id)
{
    auto data_index_id{add_const(DT_UINT32, 2)};
    auto tensor_dtype_id{add_dtype(tm.dtype)};
    auto tensor_dtype_ptr_id{add_type_pointer(tensor_dtype_id, tm.storage_class)};
    auto tensor_index_id{add_const(DT_UINT32, 0)}; // for uniform input

    std::vector<id_t> access_index_ids{};
    if (tm.storage_class == SC_UNIFORM) {
        access_index_ids = {tensor_index_id, data_index_id, index_id};
    } else if (tm.storage_class == SC_STORAGE_BUFFER) {
        access_index_ids = {tensor_index_id, data_index_id, index_id};
    } else {
        access_index_ids = {data_index_id, index_id};
    }


    auto ptr{access_chain(func_id, tensor_dtype_ptr_id, tm.id, access_index_ids)};
    store_var(ptr, object_id);
}

void Layer3::store_tensor_element(id_t func_id, const TensorMeta& tm, id_t i, id_t step, id_t j, id_t object_id)
{
    auto type_id{add_dtype(DT_UINT32)};
    auto i_mul_step{binary_op(BO_IMUL, func_id, type_id, i, step)};
    id_t index_id{binary_op(BO_IADD, func_id, type_id, i_mul_step, j)};
    store_tensor_element(func_id, tm, index_id, object_id);
}

void Layer3::store_tensor_shape_element(id_t func_id, const TensorMeta& tm, uint32_t index, id_t object_id)
{
    auto shape_base_index_id{add_const(DT_UINT32, 1)};
    auto shape_index_id{add_const(DT_UINT32, index)};
    auto shape_type_id{add_dtype(DT_UINT32)};
    auto shape_type_ptr_id{add_type_pointer(shape_type_id, tm.storage_class)};

    std::vector<id_t> access_index_ids{};
    if (tm.storage_class == SC_UNIFORM || tm.storage_class == SC_STORAGE_BUFFER) {
        auto tensor_index_id{add_const(DT_UINT32, 0)}; // for uniform input
        access_index_ids = {tensor_index_id, shape_base_index_id, shape_index_id};
    } else {
        access_index_ids = {shape_base_index_id, shape_index_id};
    }

    auto ptr{access_chain(func_id, shape_type_ptr_id, tm.id, access_index_ids)};
    store_var(ptr, object_id);
}

void Layer3::store_tensor_dims(id_t func_id, const TensorMeta& tm, id_t object_id)
{
    std::vector<uint32_t> access_indices{};
    if (tm.storage_class == SC_UNIFORM || tm.storage_class == SC_STORAGE_BUFFER) {
        access_indices = {0, 0};
    } else {
        access_indices = {0};
    }

    auto dims_type_id{add_dtype(DT_UINT32)};
    auto dims_type_ptr_id{add_type_pointer(dims_type_id, tm.storage_class)};
    auto ptr{access_chain_indices(func_id, dims_type_ptr_id, tm.id, access_indices)};
    store_var(ptr, object_id);
}
