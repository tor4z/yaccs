#ifndef YACCS_BASE_H_
#define YACCS_BASE_H_

#include "def.hpp"
#include <cstdint>
#include <ostream>


namespace trait {

struct Identable
{
    virtual id_t id() = 0;
}; // struct Identable


struct Dumpable
{
    virtual ~Dumpable() = default;
    virtual void dump(std::ostream& os) const = 0;
}; // struct Dumpable


struct Decoratable
{
    virtual ~Decoratable() = default;
    virtual void decorate(Decoration dec) = 0;
}; // struct Decoratable

} // namespace trait



struct Op : public trait::Identable
{
    enum Type {
        INVALID = 0,
        CAPABILITY,
        MEMORY_MODEL,
        ENTRY_POINT,
        DECORATE,
        VARIABLE,
        FUNCTION,
        LOAD,
        STORE,
        LABEL,
        RETURN,
        FUNCTION_END,
        CONSTANT,
        CONSTANT_COMPOSITE,

        TYPE_POINTER,
        TYPE_VOID,
        TYPE_FUNCTION,
        TYPE_FLOAT,
        TYPE_VECTOR,
        TYPE_STRUCT,
    }; // enum Type

    Op(Type op_type, bool has_ret);
    virtual ~Op() = default;
    virtual id_t id() override { return id_; }

    const bool has_ret;
    const Type type;
private:
    const id_t id_;
}; // strut Op


const std::string& as_string(Decoration::Type dec);
const std::string& as_string(Op::Type op_type);
const std::string& as_string(StorageClass sc);


#endif // YACCS_BASE_H_
