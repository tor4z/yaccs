#include "yaccs/code_gen/program.hpp"
#include "yaccs/code_gen/def.hpp"
#include "yaccs/code_gen/utils.hpp"
#include <cassert>
#include <cstring>
#include <sys/types.h>
#include <unordered_map>
#include <fstream>
#include <utility>


Program::Program()
{
    code_gen_.push_header();
}

void Program::set_name(const std::string& name) { name_  = name; }

void Program::add_input(const TensorType& tensor)
{
    auto type_id{add_dtype(tensor.dtype)};
    auto dt_int{add_dtype(DT_INT32)};
    
}

void Program::add_output(const TensorType& tensor)
{
    auto type_id{add_dtype(tensor.dtype)};
    auto dt_int{add_dtype(DT_INT32)};
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

id_t Program::add_struct_type(const std::vector<id_t>& dtypes)
{
    assert(dtypes.size() <= MAX_STRUCT_FIELDS && "struct containting too much fields");
    static std::vector<StructTypeDef> defs_;

    auto struct_match{[&dtypes] (const StructTypeDef& sd) -> bool {
        if (sd.num_fields != dtypes.size()) return false;
        for (int i = 0; i < sd.num_fields; ++i) {
            if (sd.fields[i] != dtypes.at(i)) return false;
        }
        return true;
    }};

    for (const auto& it : defs_) {
        if (struct_match(it)) {
            return it.id;
        }
    }

    StructTypeDef sd{.id = alloc_id(), .num_fields = dtypes.size()};
    memcpy(sd.fields, dtypes.data(), sizeof(sd.fields[0]) * sd.num_fields);
    defs_.push_back(sd);
    code_gen_.push_struct_type(sd);
    return sd.id;
}

id_t Program::add_array_type(id_t dtype, int length)
{
    static std::vector<ArrTypeDef> defs_;

    for (const auto& it : defs_) {
        if (it.dtype == dtype && it.length == length) {
            return it.id;
        }
    }

    ArrTypeDef arr{.dtype = dtype, .length = length, .id = alloc_id()};
    defs_.push_back(arr);
    code_gen_.push_array_type(arr);
    return arr.id;
}
