#include "yaccs/baker/exts/exts.hpp"
#include "yaccs/baker/exts/def.hpp"
#include "yaccs/baker/program.hpp"
#include "yaccs/baker/utils.hpp"
#include "yaccs/dtype.hpp"
#include <cassert>

namespace ext {

Ext::Ext(Program* program, const std::string& name)  
    : name_(name)
    , program_(program)
    , id_(alloc_id())
{
    ExtImportDef eid;
    eid.id = id_;
    eid.ext_name = name;
    program_->code_gen_.push_ext_import(eid);
}

id_t Ext::max(DType dtype, id_t func_id, id_t op1_id, id_t op2_id)
{
    BinaryOpDef bod;

    switch (dtype) {
    case DT_FLOAT:
        bod.bo = BO_FMAX;
        break;
    default:
        assert(false && "Not Implement");
    }

    bod.result_id = alloc_id();
    bod.func_id = func_id;
    bod.type_id = program_->add_dtype(dtype);
    bod.op1_id = op1_id;
    bod.op2_id = op2_id;
    bod.ext_id = id();

    program_->code_gen_.push_ext_binary_opration(bod);
    return bod.result_id;
}

} // namespace ext
