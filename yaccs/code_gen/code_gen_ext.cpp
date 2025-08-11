#include "yaccs/code_gen/code_gen.hpp"
#include "yaccs/baker/exts/utils.hpp"
#include <cassert>

void CodeGen::push_ext_binary_opration(const ext::BinaryOpDef& bod)
{
    this_fn_.body_ss << "\t%" << bod.result_id << " = OpExtInst %" << bod.type_id << " %" << bod.ext_id << " "
        << as_string(bod.bo) << " %" << bod.op1_id << " %" << bod.op2_id << "\n";
}
