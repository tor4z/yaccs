#include "yaccs/code_gen/exts/utils.hpp"
#include "yaccs/code_gen/exts/def.hpp"
#include <cassert>
#include <string>

namespace ext {

const std::string& as_string(BinaryOperator bo)
{
    static const std::string fmax{"FMax"};

    switch (bo) {
    case BO_FMAX:   return fmax;
    default:        assert(false && "Not Implement");
    }

    return fmax;    // return something to suppress compiler warning
}

} // namespace ext
