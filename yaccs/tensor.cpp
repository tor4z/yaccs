#include "yaccs/tensor.hpp"
#include "yaccs/dtype.hpp"
#include <cassert>
#include <cstring>
#include <iomanip>
#include <ios>
#include <utility>

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

std::ostream& operator<<(std::ostream& os, const Tensor& tensor)
{
    os << "Tensor [";
    for (int i = 0; i < tensor.tt.dims; ++i) {
        os << tensor.tt.shape[i];
        if (i != tensor.tt.dims - 1) {
            os << " * ";
        }
    }
    os << "] {\n";

    /// TODO: Looking for 2D+ support
    switch (tensor.tt.dtype) {
    case DT_FLOAT:
        for (int i = 0; i < tensor.tt.shape[0]; ++i) {
            for (int j = 0; j < tensor.tt.shape[1]; ++j) {
                os << std::setw(8) << std::fixed << std::setprecision(5)
                    << tensor.at<DT_FLOAT>(i, j) << ", ";
            }
            os << "\n";
        }
        break;
    default:
        assert(false && "Not implemented");
    }

    os << "}";
    return os;
}
