#include <iostream>
#include <fstream>
#include <sstream>
#include "program.hpp"


int main()
{
    Program prog;

    auto entry{prog.new_fn(prog.get_type(DType::VOID)->id, {})};
    entry->as_entry();

    std::cout << "Code:\n"
        << "----\n"
        << prog
        << "----\n";

    std::ofstream ofs("test.spvasm", std::ios::out);
    ofs << prog;
    return 0;
}
