#include <cstdint>
#include <iostream>
#include <fstream>
#include <sstream>
#include "program.hpp"


int main()
{
    Program prog;

    auto float32{prog.get_type(DType::FLOAT, 32)};
    auto vec4{prog.get_type(DType::VEC, float32, 4)};

    // auto constant{prog.new_const(float32, 1.0f)};
    auto constant_vec4{prog.new_const(vec4, std::vector<float>{1.0f, 2.0f, 3.0f, 4.0f})};

    auto in_var{prog.new_var(float32, StorageClass::INPUT)};
    in_var->decorate(Decoration(Decoration::LOCATION, 0));

    auto out_var{prog.new_var(float32, StorageClass::OUTPUT)};
    out_var->decorate(Decoration(Decoration::LOCATION, 1));

    auto out_vec{prog.new_var(vec4, StorageClass::OUTPUT)};
    out_vec->decorate(Decoration(Decoration::LOCATION, 2));

    auto entry{prog.new_fn(prog.get_type(DType::VOID), {in_var->id(), out_var->id(), out_vec->id()})};
    entry->as_entry();

    entry->op_assign(out_var, in_var);
    entry->op_assign(out_vec, constant_vec4);

    std::cout << "Code:\n"
        << "----\n"
        << prog
        << "----\n";

    std::ofstream ofs("test.spvasm", std::ios::out);
    ofs << prog;
    return 0;
}
