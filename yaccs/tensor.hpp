#ifndef YACCS_TENSOR_H_
#define YACCS_TENSOR_H_

#include "yaccs/dtype.hpp"
#include <cassert>
#include <cstdint>
#include <endian.h>
#include <ostream>
#include <string>
#include <vector>

#define MAX_TENSOR_DIMS 6


struct TensorType
{
    TensorType();
    int transpose_idx(int i) const;

    int shape[MAX_TENSOR_DIMS];
    std::string name;
    DType dtype;
    int dims;
    bool row_major;
}; // struct TensorType

struct Tensor
{
    TensorType tt;
    std::vector<char> data;

    Tensor transpose() const;
    void mul(float x);
    template<DType DT>
    void set(int i, float x);
    template<DType DT>
    auto at(int i) const;
    template<DType DT>
    auto at(int i0, int i1) const;
}; // struct Tensor

template<>
inline void Tensor::set<DT_FLOAT>(int i, float x)
{
    uint32_t v{*reinterpret_cast<uint32_t*>(&x)};
    *reinterpret_cast<uint32_t*>(data.data() + tt.transpose_idx(i) * DT_FLOAT_BYTES) = htole32(v);
}

template<>
inline auto Tensor::at<DT_FLOAT>(int i) const
{
    auto raw{le32toh(*reinterpret_cast<const uint32_t*>(data.data() + tt.transpose_idx(i) * DT_FLOAT_BYTES))};
    float v{*reinterpret_cast<float*>(&raw)};
    return v;
}

template<>
inline auto Tensor::at<DT_FLOAT>(int i0, int i1) const
{
    if (tt.row_major) {
        return at<DT_FLOAT>(i0 * tt.shape[tt.dims - 1] + i1);
    } else {
        /// TODO: Cache killer
        return at<DT_FLOAT>(i1 * tt.shape[tt.dims - 2] + i0);
    }
}

std::ostream& operator<<(std::ostream& os, const Tensor& tensor);

#endif // YACCS_TENSOR_H_
