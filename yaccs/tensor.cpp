#include "yaccs/tensor.hpp"
#include "yaccs/dtype.hpp"
#include <algorithm>
#include <cassert>
#include <cstring>
#include <iomanip>
#include <ios>
#include <utility>

TensorType::TensorType()
    : dims(0)
{
    memset(shape, 0, MAX_TENSOR_DIMS * sizeof(shape[0]));
}

TensorType::TensorType(const TensorType& tt)
{
    memcpy(shape, tt.shape, MAX_TENSOR_DIMS * sizeof(tt.shape[0]));
    name = tt.name;
    dtype = tt.dtype;
    dims = tt.dims;
    row_major = tt.row_major;
}

TensorType::TensorType(TensorType&& tt)
{
    // copy
    memcpy(shape, tt.shape, MAX_TENSOR_DIMS * sizeof(tt.shape[0]));
    name = tt.name;
    dtype = tt.dtype;
    dims = tt.dims;
    row_major = tt.row_major;
    // clear
    memset(tt.shape, 0, MAX_TENSOR_DIMS * sizeof(tt.shape[0]));
    tt.name = "";
    tt.dtype = DT_UNDEFINED;
    tt.dims = 0;
    tt.row_major = false;
}

TensorType& TensorType::operator=(const TensorType& tt)
{
    memcpy(shape, tt.shape, MAX_TENSOR_DIMS * sizeof(tt.shape[0]));
    name = tt.name;
    dtype = tt.dtype;
    dims = tt.dims;
    row_major = tt.row_major;
    return *this;
}

TensorType& TensorType::operator=(TensorType&& tt)
{
    if (&tt != this) {
        // copy
        memcpy(shape, tt.shape, MAX_TENSOR_DIMS * sizeof(tt.shape[0]));
        name = tt.name;
        dtype = tt.dtype;
        dims = tt.dims;
        row_major = tt.row_major;
        // clear
        memset(tt.shape, 0, MAX_TENSOR_DIMS * sizeof(tt.shape[0]));
        tt.name = "";
        tt.dtype = DT_UNDEFINED;
        tt.dims = 0;
        tt.row_major = false;
    }
    return *this;
}

int TensorType::transposed_idx(int i) const
{
    if (!row_major && dims > 1) {
        int c{i / shape[1]};
        int r{i % shape[1]};
        i = r * shape[0] + c;
    }
    return i;
}

Tensor Tensor::transpose() const
{
    assert(tt.dims > 1 && "Bad transpose operation");

    Tensor result;
    result.tt.name = tt.name;
    result.tt.dtype = tt.dtype;
    result.tt.dims = tt.dims;
    result.tt.row_major = !tt.row_major;
    memcpy(result.tt.shape, tt.shape, MAX_TENSOR_DIMS * sizeof(tt.shape[0]));
    std::swap(result.tt.shape[tt.dims - 2], result.tt.shape[tt.dims - 1]);
    result.data = data;
    return result;
}

void Tensor::mul(float x)
{
    int num_elems{1};
    for (int i = 0; i < tt.dims; ++i) {
        num_elems *= tt.shape[i];
    }

    switch (tt.dtype) {
    case DT_FLOAT:
        for (int i = 0; i < num_elems; ++i) {
            auto v{at<DT_FLOAT>(i)};            
            set<DT_FLOAT>(i, v * x);            
        }
        break;
    default:
        assert(false && "Not implement");
    }
}

std::ostream& operator<<(std::ostream& os, const Tensor& tensor)
{
    int num_elems{1};
    os << "Tensor [";
    if (tensor.tt.dims == 1) {
        num_elems = tensor.tt.shape[0];
        os << "1 * " << tensor.tt.shape[0];
    } else {
        for (int i = 0; i < tensor.tt.dims; ++i) {
            num_elems *= tensor.tt.shape[i];
            os << tensor.tt.shape[i];
            if (i != tensor.tt.dims - 1) {
                os << " * ";
            }
        }
    }
    os << "] {\n";

    /// TODO: Looking for 2D+ support
    switch (tensor.tt.dtype) {
    case DT_FLOAT:
        for (int i = 0; i < num_elems; ++i) {
            os << std::setw(8) << std::fixed << std::setprecision(5)
                << tensor.at<DT_FLOAT>(i) << ", ";
            if ((i + 1) % std::max(1, tensor.tt.shape[1]) == 0) os << "\n";
        }
        break;
    default:
        assert(false && "Not implemented");
    }

    os << "}";
    return os;
}
