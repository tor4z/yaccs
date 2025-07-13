#ifndef YACCS_OPS_H_
#define YACCS_OPS_H_

#include "yaccs/tensor.hpp"

struct Op {};


/**
 * @brief Gemm Operator definition
 * 
 * ref: https://onnx.ai/onnx/operators/onnx__Gemm.html
 */
struct OpGemm: public Op
{
    std::string name;
    std::string op_type;
    float alpha;
    float beta;
    int trans_a;
    int trans_b;
    Tensor A;
    Tensor B;
    Tensor C;
    Tensor Y;
}; // struct OpGemm

/**
 * @brief Relu Operator definition
 * 
 * ref: https://onnx.ai/onnx/operators/onnx__Relu.html
 */
struct OpRelu: public Op
{
    std::string name;
    std::string op_type;
    Tensor X;
    Tensor Y;
}; // struct OpRelu

#endif // YACCS_OPS_H_
