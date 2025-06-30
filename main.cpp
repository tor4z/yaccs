#include <cstdint>
#include <iostream>
#include <fstream>
#include <sstream>
#include "program.hpp"


int main()
{
    Program prog;

    auto float32{prog.get_type(DType::FLOAT, 32)};

    auto in_var{prog.new_var(float32, StorageClass::INPUT)};
    in_var->decorate(Decoration(Decoration::LOCATION, 0));

    auto out_var{prog.new_var(float32, StorageClass::OUTPUT)};
    out_var->decorate(Decoration(Decoration::LOCATION, 1));

    auto entry{prog.new_fn(prog.get_type(DType::VOID), {in_var->id(), out_var->id()})};
    entry->as_entry();


    entry->op_assign(in_var, out_var);

    std::cout << "Code:\n"
        << "----\n"
        << prog
        << "----\n";

    std::ofstream ofs("test.spvasm", std::ios::out);
    ofs << prog;
    return 0;
}
