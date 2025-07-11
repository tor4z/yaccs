#include "yaccs/code_gen/program.hpp"
#include "yaccs/code_gen/def.hpp"
#include "yaccs/code_gen/utils.hpp"
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
    auto type_id{add_type(tensor.dtype)};

}

void Program::add_output(const TensorType& tensor)
{
    auto type_id{add_type(tensor.dtype)};
}

void Program::dump_ir()
{
    std::ofstream ofs{name_, std::ios::out};
    code_gen_.assemble(ofs);
    ofs.close();
}


uint Program::add_type(DType dt)
{
    static std::unordered_map<DType, id_t> ids_;
    if (ids_.find(dt) == ids_.end()) {
        const auto id{alloc_id()};
        ids_.insert(std::make_pair(dt, id));
        code_gen_.push_type(dt, id);
    }
    return ids_.at(dt);
}