#include "yaccs/baker/layer3/layer3.hpp"
#include "yaccs/baker/def.hpp"
#include "yaccs/baker/layer1/layer1.hpp"
#include "yaccs/baker/layer2/def.hpp"
#include "yaccs/baker/layer2/layer2.hpp"
#include "yaccs/dtype.hpp"
#include "yaccs/tensor.hpp"


void Layer3::add_gemm(const OpGemm& gemm)
{
    FunctionDef fdef;
    layer2_->begin_function(fdef, T_VOID);
        const auto func_id{fdef.id};
        const auto alpha{gemm.alpha};
        const auto beta{gemm.beta};
        
        Tensor B_alpha{gemm.trans_b ? gemm.B.transpose() : gemm.B};
        Tensor C_beta{gemm.C};
        B_alpha.mul(gemm.alpha);
        C_beta.mul(gemm.beta);
        add_const_tensor(B_alpha);
        add_const_tensor(C_beta);
        add_shared_tensor(gemm.Y);

        const auto& A{global_tensors_.at(gemm.A.tt.name)};
        const auto& B{global_tensors_.at(gemm.B.tt.name)};
        const auto& C{global_tensors_.at(gemm.C.tt.name)};
        const auto& Y{global_tensors_.at(gemm.Y.tt.name)};

        auto A_shape0{access_tensor_shape_index(func_id, A, 0)};
        auto A_shape1{access_tensor_shape_index(func_id, A, 1)};
        auto B_shape0{access_tensor_shape_index(func_id, B, 0)};
        auto B_shape1{access_tensor_shape_index(func_id, B, 1)};
        auto A_dims{access_tensor_dims(func_id, A)};
        store_tensor_shape_element(func_id, Y, 0, gemm.trans_a ? A_shape1 : A_shape0);
        store_tensor_shape_element(func_id, Y, 1, B_shape1);
        store_tensor_dims(func_id, Y, A_dims);

        auto this_element_var{layer1_->add_var(Y.dtype_id, SC_FUNCTION, layer1_->add_const(Y.dtype, 0))};
        auto shape_element_type_id{layer1_->add_dtype(DT_UINT32)};
        auto bo_mul{Y.dtype == DT_FLOAT ? BO_FMUL : BO_IMUL};
        auto bo_add{Y.dtype == DT_FLOAT ? BO_FADD : BO_IADD};
        auto invo_x{layer1_->access_invocation_index(func_id, 0)};
        auto invo_y{layer1_->access_invocation_index(func_id, 1)};
        
        invocation_boundary_check(func_id, Y, 0);
        invocation_boundary_check(func_id, Y, 1);
        
        ForLoopDef for_def{.i_boundary_id = gemm.trans_a ? A_shape0 : A_shape1};
        layer2_->begin_for(for_def);
            auto i_id{layer1_->load_var(for_def.i_type_id, for_def.i_var_id)};
            id_t A_row_begin{};
            id_t A_element_index{};
            if (gemm.trans_a) {
                A_row_begin = layer1_->binary_op(BO_IMUL, func_id, shape_element_type_id, invo_y, A_shape0);
                A_element_index = layer1_->binary_op(BO_IADD, func_id, shape_element_type_id, A_row_begin, i_id);
            } else {
                A_row_begin = layer1_->binary_op(BO_IMUL, func_id, shape_element_type_id, invo_x, A_shape1);
                A_element_index = layer1_->binary_op(BO_IADD, func_id, shape_element_type_id, A_row_begin, i_id);
            }
            auto B_row_begin{layer1_->binary_op(BO_IMUL, func_id, shape_element_type_id, i_id, B_shape1)};
            auto B_element_index{layer1_->binary_op(BO_IADD, func_id, shape_element_type_id, B_row_begin, invo_y)};
            auto A_element{load_tensor_element(func_id, A, A_element_index)};
            auto B_element{load_tensor_element(func_id, B, B_element_index)};
            auto AB_mul{layer1_->binary_op(bo_mul, func_id, Y.dtype_id, A_element, B_element)};
            auto this_element_val{layer1_->load_var(Y.dtype_id, this_element_var)};
            auto this_element_accu{layer1_->binary_op(bo_add, func_id, Y.dtype_id, AB_mul, this_element_val)};
            layer1_->store_var(this_element_var, this_element_accu);
        layer2_->end_for(for_def);

        auto Y_shape1{access_tensor_shape_index(func_id, Y, 1)};
        auto AB_element_val{layer1_->load_var(Y.dtype_id, this_element_var)};
        auto C_element_id{load_tensor_element(func_id, C, invo_x)};
        auto final_this_element_val{layer1_->binary_op(bo_add, func_id, Y.dtype_id, AB_element_val, C_element_id)};
        store_tensor_element(func_id, Y, invo_x, Y_shape1, invo_y, final_this_element_val);
    layer2_->end_function(fdef);
    layers_.push_back(func_id);
}

void Layer3::add_relu(const OpRelu& relu)
{
    FunctionDef fdef;
    layer2_->begin_function(fdef, T_VOID);
        const auto func_id{fdef.id};
        const auto& X{global_tensors_.at(relu.X.tt.name)};
        // infer Y from X
        Tensor tensor_Y;
        tensor_Y.tt.name = relu.Y.tt.name;
        tensor_Y.tt.dtype = X.dtype;
        tensor_Y.tt.row_major = true;
        add_shared_tensor(relu.Y);
        const auto& Y{global_tensors_.at(relu.Y.tt.name)};

        // Setup output tensor dims and shape
        auto X_shape0{access_tensor_shape_index(func_id, X, 0)};
        auto X_shape1{access_tensor_shape_index(func_id, X, 1)};
        auto X_dims_id{access_tensor_dims(func_id, X)};
        store_tensor_dims(func_id, Y, X_dims_id);
        store_tensor_shape_element(func_id, Y, 0, X_shape0);
        store_tensor_shape_element(func_id, Y, 1, X_shape1);

        // boundary check
        invocation_boundary_check(func_id, Y, 0);
        invocation_boundary_check(func_id, Y, 1);

        // relu operator eval
        auto invo_x{layer1_->access_invocation_index(func_id, 0)};
        auto invo_y{layer1_->access_invocation_index(func_id, 1)};
        auto x{load_tensor_element(func_id, X, invo_x, X_shape1, invo_y)};
        auto relu_result{layer1_->std450()->max(X.dtype, func_id, layer1_->add_const(X.dtype, 0), x)};
        store_tensor_element(func_id, Y, invo_x, X_shape1, invo_y, relu_result);
    layer2_->end_function(fdef);
    layers_.push_back(func_id);
}
