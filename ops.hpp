#ifndef YACCSS_OPS_H_
#define YACCSS_OPS_H_

#include <string>

namespace yaccs {

struct Op
{
    virtual std::string str() const = 0;
}; // strut Op


struct OpType: public Op
{
    virtual std::string str() const = 0;
}; // strut OpType


struct OpCapability: public Op
{
    virtual std::string str() const override;
}; // struct OpCapability


struct OpMemoryModel: public Op
{
    virtual std::string str() const override;
}; // struct OpMemoryModel


struct OpEntryPoint: public Op
{
    virtual std::string str() const override;
}; // struct OpEntryPoint


struct OpTypeVoid: public OpType
{
    virtual std::string str() const override;
}; // struct OpTypeVoid


struct OpTypeFunction: public OpType
{
    virtual std::string str() const override;
}; // struct OpTypeFunction


struct OpFunction: public Op
{
    virtual std::string str() const override;
}; // struct OpFunction


struct OpLabel: public Op
{
    virtual std::string str() const override;
}; // struct OpLabel


struct OpReturn: public Op
{
    virtual std::string str() const override;
}; // struct OpReturn


struct OpFunctionEnd: public Op
{
    virtual std::string str() const override;
}; // struct OpFunctionEnd

} // yaccs

#endif // YACCSS_OPS_H_