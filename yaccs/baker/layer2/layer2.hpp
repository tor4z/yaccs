#ifndef YACCS_BAKER_LAYER2_H_
#define YACCS_BAKER_LAYER2_H_

#include "yaccs/baker/def.hpp"
#include "yaccs/baker/layer1/layer1.hpp"

struct Layer2
{
    Layer2();

    void begin_function(FunctionDef& def);
    void end_function(FunctionDef& def);

    void begin_for(ForLoopDef& def);
    void end_for(ForLoopDef& def);

    void begin_if(IfDef& def, id_t op1_id, CmpOp cmp_op, id_t op2_id);
    void end_if(IfDef& def);
private:
    Layer1 layer1_;
}; // class Program

#endif // YACCS_BAKER_LAYER2_H_