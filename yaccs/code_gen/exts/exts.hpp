#ifndef YACCS_EXTS_H_
#define YACCS_EXTS_H_

#include "yaccs/code_gen/def.hpp"
#include "yaccs/dtype.hpp"
#include <string>

struct Program;

namespace ext {

struct Ext
{
    Ext() : program_(nullptr) {}
    Ext(Program* program, const std::string& name);

    id_t max(DType dtype, id_t func_id, id_t op1_id, id_t op2_id);

    id_t id() const { return id_; }
    const std::string name() const { return name_; }
private:
    Program* program_;
    std::string name_;
    id_t id_;
}; // struct Ext

} // namespace ext

#endif // YACCS_EXTS_H_
