#ifndef YACCS_DTYPE_H_
#define YACCS_DTYPE_H_

#include <cstdint>

#define DT_FLOAT_BYTES 4
#define DT_UINT8_BYTES 1
#define DT_INT8_BYTES 1
#define DT_UINT16_BYTES 2
#define DT_INT16_BYTES 2
#define DT_INT32_BYTES 3
#define DT_INT64_BYTES 8
#define DT_BOOL_BYTES 1
#define DT_FLOAT16_BYTES 2
#define DT_DOUBLE_BYTES 8
#define DT_UINT32_BYTES 4
#define DT_UINT64_BYTES 8
#define DT_COMPLEX64_BYTES 8
#define DT_COMPLEX128_BYTES 16
#define DT_BFLOAT16_BYTES 2
#define DT_FLOAT8E4M3FN_BYTES 1
#define DT_FLOAT8E4M3FNUZ_BYTES 1
#define DT_FLOAT8E5M2_BYTES 1
#define DT_FLOAT8E5M2FNUZ_BYTES 1
#define DT_UINT4_BYTES 1
#define DT_INT4_BYTES 1
#define DT_FLOAT4E2M1_BYTES 1

enum DType {
    DT_UNDEFINED = 0,
    // Basic types.
    DT_FLOAT = 1,   // float
    DT_UINT8 = 2,   // uint8_t
    DT_INT8 = 3,    // int8_t
    DT_UINT16 = 4,  // uint16_t
    DT_INT16 = 5,   // int16_t
    DT_INT32 = 6,   // int32_t
    DT_INT64 = 7,   // int64_t
    DT_STRING = 8,  // string
    DT_BOOL = 9,    // bool

    // IEEE754 half-precision floating-point format (16 bits wide).
    // This format has 1 sign bit, 5 exponent bits, and 10 mantissa bits.
    DT_FLOAT16 = 10,

    DT_DOUBLE = 11,
    DT_UINT32 = 12,
    DT_UINT64 = 13,
    DT_COMPLEX64 = 14,     // complex with float32 real and imaginary components
    DT_COMPLEX128 = 15,    // complex with float64 real and imaginary components

    // Non-IEEE floating-point format based on IEEE754 single-precision
    // floating-point number truncated to 16 bits.
    // This format has 1 sign bit, 8 exponent bits, and 7 mantissa bits.
    DT_BFLOAT16 = 16,

    // Non-IEEE floating-point format based on papers
    // FP8 Formats for Deep Learning, https://arxiv.org/abs/2209.05433,
    // 8-bit Numerical Formats For Deep Neural Networks, https://arxiv.org/pdf/2206.02915.pdf.
    // Operators supported FP8 are Cast, CastLike, QuantizeLinear, DequantizeLinear.
    // The computation usually happens inside a block quantize / dequantize
    // fused by the runtime.
    DT_FLOAT8E4M3FN = 17,    // float 8, mostly used for coefficients, supports nan, not inf
    DT_FLOAT8E4M3FNUZ = 18,  // float 8, mostly used for coefficients, supports nan, not inf, no negative zero
    DT_FLOAT8E5M2 = 19,      // follows IEEE 754, supports nan, inf, mostly used for gradients
    DT_FLOAT8E5M2FNUZ = 20,  // follows IEEE 754, supports nan, not inf, mostly used for gradients, no negative zero

    // 4-bit integer data types
    DT_UINT4 = 21,  // Unsigned integer in range [0, 15]
    DT_INT4 = 22,   // Signed integer in range [-8, 7], using two's-complement representation

    // 4-bit floating point data types
    DT_FLOAT4E2M1 = 23,

    // Future extensions go here.
}; // enum DType

inline uint32_t dtype_bytes(DType dtype)
{
    switch (dtype) {
    case DT_FLOAT:          return DT_FLOAT_BYTES;
    case DT_UINT8:          return DT_UINT8_BYTES;
    case DT_INT8:           return DT_INT8_BYTES;
    case DT_UINT16:         return DT_UINT16_BYTES;
    case DT_INT16:          return DT_INT16_BYTES;
    case DT_INT32:          return DT_INT32_BYTES;
    case DT_INT64:          return DT_INT64_BYTES;
    case DT_BOOL:           return DT_BOOL_BYTES;
    case DT_FLOAT16:        return DT_FLOAT16_BYTES;
    case DT_DOUBLE:         return DT_DOUBLE_BYTES;
    case DT_UINT32:         return DT_UINT32_BYTES;
    case DT_UINT64:         return DT_UINT64_BYTES;
    case DT_COMPLEX64:      return DT_COMPLEX64_BYTES;
    case DT_COMPLEX128:     return DT_COMPLEX128_BYTES;
    case DT_BFLOAT16:       return DT_BFLOAT16_BYTES;
    case DT_FLOAT8E4M3FN:   return DT_FLOAT8E4M3FN_BYTES;
    case DT_FLOAT8E4M3FNUZ: return DT_FLOAT8E4M3FNUZ_BYTES;
    case DT_FLOAT8E5M2:     return DT_FLOAT8E5M2_BYTES;
    case DT_FLOAT8E5M2FNUZ: return DT_FLOAT8E5M2FNUZ_BYTES;
    case DT_UINT4:          return DT_UINT4_BYTES;
    case DT_INT4:           return DT_INT4_BYTES;
    case DT_FLOAT4E2M1:     return DT_FLOAT4E2M1_BYTES;
    default:                return 1;
    }
}

#endif // YACCS_DTYPE_H_
