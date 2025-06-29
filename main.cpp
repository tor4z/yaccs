#include <cstdint>
#include <iostream>
#include <fstream>
#include <sstream>
#include "program.hpp"


int main()
{
    Program prog;

    auto entry{prog.new_fn(prog.get_type(DType::VOID), {})};
    entry->as_entry();
    auto float32{prog.get_type(DType::FLOAT, 32)};
    auto var{prog.new_var(float32, StorageClass::INPUT)};
    var->decorate(Decoration(Decoration::LOCATION, 0));

    std::cout << "Code:\n"
        << "----\n"
        << prog
        << "----\n";

    std::ofstream ofs("test.spvasm", std::ios::out);
    ofs << prog;
    return 0;
}
