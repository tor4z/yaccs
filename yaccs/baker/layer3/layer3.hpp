#ifndef YACCS_BAKER_LAYER3_H_
#define YACCS_BAKER_LAYER3_H_

#include "yaccs/baker/def.hpp"
#include "yaccs/baker/layer2/layer2.hpp"
#include "yaccs/tensor.hpp"
#include "yaccs/onnx/ops.hpp"
#include <unordered_map>

struct Layer3
{
    Layer3();
    void set_name(const std::string& name);
    void set_main();
    void dump_ir();

    void add_input(const TensorType& tensor_type);
    void add_output(const TensorType& tensor_type);
    void add_gemm(const OpGemm& gemm);
    void add_relu(const OpRelu& relu);
private:
    std::vector<id_t> layers_;  // layers in order
    std::unordered_map<std::string, TensorMeta> global_tensors_;
    std::string name_;
    Layer1* layer1_;
    Layer2* layer2_;

    id_t add_const_tensor_element(DType dtype, int elem_idx, const Tensor& tensor);
    id_t add_const_tensor(const Tensor& tensor);
    id_t add_shared_tensor(const Tensor& tensor);
    id_t add_tensor_type(const TensorType& tensor_type, StorageClass sc, bool reuse=true);

    void invocation_boundary_check(id_t func_id, const TensorMeta& tm, uint32_t index);
    id_t access_tensor_dims(id_t func_id, const TensorMeta& tm);
    id_t access_tensor_shape_index(id_t func_id, const TensorMeta& tm, uint32_t index);
    id_t load_tensor_element(id_t func_id, const TensorMeta& tm, id_t index_id);
    id_t load_tensor_element(id_t func_id, const TensorMeta& tm, id_t i, id_t step, id_t j);
    void store_tensor_element(id_t func_id, const TensorMeta& tm, id_t index_id, id_t object_id);
    void store_tensor_element(id_t func_id, const TensorMeta& tm, id_t i, id_t step, id_t j, id_t object_id);
    void store_tensor_shape_element(id_t func_id, const TensorMeta& tm, uint32_t index, id_t object_id);
    void store_tensor_dims(id_t func_id, const TensorMeta& tm, id_t object_id);
}; // class Program

#endif // YACCS_BAKER_LAYER3_H_
