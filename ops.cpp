#include "ops.hpp"

namespace yaccs {

std::string OpCapability::str() const
{
    return "OpCapability Shader";
}


std::string OpMemoryModel::str() const
{
    return "OpMemoryModel Logical GLSL450";
}


std::string OpEntryPoint::str() const
{
    return "OpEntryPoint GLCompute %3 \"main\"";
}


std::string OpTypeVoid::str() const
{
    return "%1 = OpTypeVoid";
}


std::string OpTypeFunction::str() const
{
    return "%2 = OpTypeFunction %1";
}


std::string OpFunction::str() const
{
    return "%3 = OpFunction %1 None %2";
}


std::string OpLabel::str() const
{
    return "%4 = OpLabel";
}


std::string OpReturn::str() const
{
    return "OpReturn";
}


std::string OpFunctionEnd::str() const
{
    return "OpFunctionEnd";
}

} // yaccs
