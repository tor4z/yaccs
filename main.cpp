#include <fstream>
#include <iostream>
#include <sstream>
#include <string_view>
#include "ops.hpp"

class CodeGen
{
public:
    CodeGen() = default;
    ~CodeGen() = default;
    void put(const yaccs::Op* op);
    void dump(const std::string_view& file_name);
    std::string dump_str();
private:
    std::stringstream ss_;
    std::string_view code_;
}; // class CodeGen


void CodeGen::put(const yaccs::Op* op)
{
    ss_ << op->str() << "\n";
}


void CodeGen::dump(const std::string_view& file_name)
{
    std::ofstream ofs(file_name.data(), std::ios::out);
    ofs << ss_.str();
    ofs.close();
}


std::string CodeGen::dump_str()
{
    return ss_.str();
}



int main()
{
    CodeGen spirv;
    yaccs::OpCapability opc;
    yaccs::OpMemoryModel opm;
    yaccs::OpEntryPoint ope;
    yaccs::OpTypeVoid optv;
    yaccs::OpTypeFunction optf;
    yaccs::OpFunction opf;
    yaccs::OpLabel oplab;
    yaccs::OpReturn oprt;
    yaccs::OpFunctionEnd opfe;
    spirv.put(&opc);
    spirv.put(&opm);
    spirv.put(&ope);
    spirv.put(&optv);
    spirv.put(&optf);
    spirv.put(&opf);
    spirv.put(&oplab);
    spirv.put(&oprt);
    spirv.put(&opfe);

    std::cout << "Code:\n"
        << "----\n"
        << spirv.dump_str()
        << "----\n";
    
    spirv.dump("test.spvasm");
    return 0;
}
