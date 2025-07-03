#ifndef YACCS_CODE_GEN_H_
#define YACCS_CODE_GEN_H_

#include "ops.hpp"
#include "types.hpp"
#include <ostream>
#include <string>
#include <iomanip>

struct CodeGen
{
    static void gen(std::ostream& ss, const OpCapability* op);
    static void gen(std::ostream& ss, const OpMemoryModel* op);
    static void gen(std::ostream& ss, const OpEntryPoint* op);
    static void gen(std::ostream& ss, const OpDecorate* op);
    static void gen(std::ostream& ss, const OpFunction* op);
    static void gen(std::ostream& ss, const OpLabel* op);
    static void gen(std::ostream& ss, const OpLoad* op);
    static void gen(std::ostream& ss, const OpStore* op);
    static void gen(std::ostream& ss, const OpReturn* op);
    static void gen(std::ostream& ss, const OpFunctionEnd* op);
    static void gen(std::ostream& ss, const OpTypePointer* op);
    static void gen(std::ostream& ss, const OpVariable* op);

    template<typename T>
    static void gen(std::ostream& ss, const OpConstant<T>* op);
    static void gen(std::ostream& ss, const OpConstantComposite* op);
    static void gen(std::ostream& ss, const Op* op);
}; // class CodeGen

#endif // YACCS_CODE_GEN_H_
