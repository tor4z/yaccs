#include <iostream>
#include <sstream>
#include <string_view>


class CodeGen
{
public:
    CodeGen() = default;
    ~CodeGen() = default;
    void put(const std::string_view& ir);
    void dump(const std::string_view& file_name);
    std::string dump_str();
private:
    std::stringstream ss_;
    std::string_view code_;
}; // class CodeGen


void CodeGen::put(const std::string_view& ir)
{
    ss_ << ir << "\n";
}


void CodeGen::dump(const std::string_view& file_name)
{

}


std::string CodeGen::dump_str()
{
    return ss_.str();
}



int main()
{
    CodeGen spirv;
    spirv.put("test");
    std::cout << "Code:\n" << spirv.dump_str() << "\n";
    return 0;
}
