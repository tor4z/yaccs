#include "yaccs/baker/layer1/layer1.hpp"
#include "yaccs/baker/layer1/utils.hpp"
#include "yaccs/baker/utils.hpp"
#include <cstring>


Layer1::Layer1()
{
    code_gen_.push_header();
    std450_ = ext::Ext(this, "GLSL.std.450");
}

void Layer1::set_entry(id_t main_id)
{
    push_entry_listed_id(global_invocation_id());

    EntryDef ed;
    ed.local_size_x = 4;
    ed.local_size_y = 4;
    ed.local_size_z = 1;
    ed.input_ids = entry_listed_ids_;
    ed.main_id = main_id;
    code_gen_.push_entry(ed);
}

id_t Layer1::add_dtype(DType dtype)
{
    static std::unordered_map<DType, id_t> defs_;
    if (defs_.find(dtype) == defs_.end()) {
        const auto id{alloc_id()};
        defs_.insert(std::make_pair(dtype, id));
        code_gen_.push_dtype(dtype, id);
    }
    return defs_.at(dtype);
}

id_t Layer1::add_type_pointer(id_t type_id, StorageClass sc)
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

id_t Layer1::add_var(id_t type_id, StorageClass sc, id_t initializer)
{
    VarDef vd;
    vd.id = alloc_id();
    vd.initializer_id = initializer;
    vd.storage_class = sc;
    vd.type_pointer_id = add_type_pointer(type_id, sc);
    code_gen_.push_variable(vd);
    return vd.id;
}

id_t Layer1::add_struct_dtype(const std::vector<id_t>& dtypes, bool reuse)
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

id_t Layer1::add_array_dtype(id_t dtype, uint32_t length, StorageClass sc, bool reuse)
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

id_t Layer1::add_const_array(id_t arr_type, const std::vector<id_t>& elem_ids)
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


id_t Layer1::add_const_struct(id_t type_id, const std::vector<id_t>& elem_ids)
{
    static std::vector<ConstCompositeDef> dfs;

    ConstCompositeDef sd;
    sd.type_id = type_id;
    sd.id = alloc_id();
    sd.elem_ids = elem_ids;

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
        dfs.push_back(sd);
        code_gen_.push_const_composite(sd);
    }

    return sd.id;
}

void Layer1::add_struct_decorate(id_t type_id, Decoration deco, StorageClass sc,
    const std::vector<std::pair<uint32_t, uint32_t>>& member_deco)
{
    static std::vector<DecorateStructDef> decos;

    DecorateStructDef dsd;
    dsd.deco = deco;
    dsd.struct_type_id = type_id;
    for (const auto& it : member_deco) {
        dsd.member_deco.push_back({.field = it.first, .offset = it.second});
    }

    auto already_decorate_in{[&dsd] (const std::vector<DecorateStructDef>& targets) -> bool {
        for (const auto& it : targets) {
            if (it.struct_type_id == dsd.struct_type_id) return true;
        }
        return false;
    }};

    if (should_decorate(sc) && !already_decorate_in(decos)) {
        decos.push_back(dsd);
        code_gen_.push_struct_decorate(dsd);
    }
}

void Layer1::add_binding(id_t var_id, int binding, int set)
{
    DecorateSetBindingDef binding_deco;

    binding_deco.binding = binding;
    binding_deco.set = set;
    binding_deco.target = var_id;
    code_gen_.push_decorate_set_binding(binding_deco);
}

id_t Layer1::add_void_type()
{
    static bool defined{false};
    static id_t id{};
    if (defined) return id;

    id = alloc_id();
    code_gen_.push_void_type(id);
    defined = true;
    return id;
}

id_t Layer1::add_function_type(id_t return_type_id)
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

id_t Layer1::add_label()
{
    auto id{alloc_id()};
    code_gen_.push_label(id);
    return id;
}

id_t Layer1::add_function_prologue(id_t return_type_id)
{
    auto fn_type_id{add_function_type(return_type_id)};
    FunctionHeaderDef fh{
        .return_type_id = return_type_id,
        .function_type_id = fn_type_id,
        .open_label_id = alloc_id(),
        .id = alloc_id()
    };

    global_funcs_.insert(std::make_pair(fh.id, fh));
    code_gen_.push_function(fh); 
    return fh.id;
}

void Layer1::add_function_epilogue()
{
    code_gen_.push_function_end();
}

id_t Layer1::add_function_call(id_t id)
{
    FunctionCallDef fcd;
    fcd.func_id = id;
    fcd.id = alloc_id();
    fcd.return_type_id = find_function_def(id).return_type_id;
    code_gen_.push_function_call(fcd);
    return fcd.id;
}

void Layer1::add_control_barrier(Scope exe_scope, Scope mem_scope, MemSemantic mem_semantics)
{
    ControlBarrierDef cbd;

    cbd.exe_scope_id = add_const(DType::DT_UINT32, static_cast<uint32_t>(exe_scope));
    cbd.mem_scope_id = add_const(DType::DT_UINT32, static_cast<uint32_t>(mem_scope));
    cbd.mem_semantics_id = add_const(DType::DT_UINT32, static_cast<uint32_t>(mem_semantics));

    code_gen_.push_control_barrier(cbd);
}

id_t Layer1::add_vector_dtype(id_t component_type_id, int count)
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

id_t Layer1::global_invocation_id()
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

id_t Layer1::load_var(id_t dtype_id, id_t pointer)
{
    LoadDef ld;
    ld.pointer = pointer;
    ld.type_id = dtype_id;
    ld.id = alloc_id();
    code_gen_.push_load(ld);
    return ld.id;
}

void Layer1::store_var(id_t pointer, id_t object)
{
    StoreDef sd;
    sd.pointer = pointer;
    sd.object = object;
    code_gen_.push_store(sd);
}

id_t Layer1::access_chain(id_t func_id, id_t type_id, id_t base_id, const std::vector<id_t>& index_ids)
{
    static std::vector<AccessChainDef> dfs;
    AccessChainDef acd;

    acd.index_ids = index_ids;
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

id_t Layer1::access_chain_indices(id_t func_id, id_t type_id, id_t base_id, const std::vector<uint32_t>& indices)
{
    std::vector<id_t> index_ids{};
    index_ids.reserve(indices.size());
    for (auto index: indices) {
        index_ids.push_back(add_const(DT_UINT32, index));
    }

    return access_chain(func_id, type_id, base_id, index_ids);
}

id_t Layer1::access_invocation_index(id_t func_id, uint32_t index)
{
    static std::vector<AccessInvocationEelementDef> dfs;
    AccessInvocationEelementDef def;

    for (const auto& it: dfs) {
        if (it.func_id == func_id && it.index == index) {
            return it.id;
        }
    }

    def.invo_id = global_invocation_id();
    def.invo_comp_type_id = add_dtype(DT_UINT32);
    def.invo_comp_type_ptr_id = add_type_pointer(def.invo_comp_type_id, SC_INPUT);
    def.invo_comp_ptr_id = access_chain_indices(func_id, def.invo_comp_type_ptr_id, def.invo_id, {index});
    def.id = load_var(def.invo_comp_type_id, def.invo_comp_ptr_id);
    def.func_id = func_id;
    def.index = index;

    dfs.push_back(def);
    return def.id;
}

id_t Layer1::binary_op(BinaryOperator bo, id_t func_id, id_t type_id, id_t op1_id, id_t op2_id)
{
    static std::vector<BinaryOpDef> dfs;

    for (const auto& it: dfs) {
        if (it.func_id == func_id && it.bo == bo && it.op1_id == op1_id && it.op2_id == op2_id) {
            return it.result_id;
        }
    }

    BinaryOpDef bod;
    bod.result_id = alloc_id();
    bod.op1_id = op1_id;
    bod.op2_id = op2_id;
    bod.type_id = type_id;
    bod.func_id = func_id;
    bod.bo = bo;

    code_gen_.push_binary_operation(bod);
    return bod.result_id;
}

void Layer1::add_return()
{
    code_gen_.push_return();
}

FunctionHeaderDef& Layer1::find_function_def(id_t id)
{
    return global_funcs_.at(id);
}

void Layer1::push_entry_listed_id(id_t id)
{
    for (auto it : entry_listed_ids_) {
        if (it == id) {
            return;
        }
    }
    entry_listed_ids_.push_back(id);
}

