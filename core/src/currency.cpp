#include "core/src/currency.h"

#include <stdexcept>

namespace Finances::Core {

constexpr bool is_valid_code(std::string_view code) {
    return code.size() == 3 && std::isupper(code[0]) && std::isupper(code[1]) && std::isupper(code[2]);
}

Currency::Currency(std::string_view code, int decimals) : code_(code), decimals_(decimals) {
    if (!is_valid_code(code)) {
        throw std::invalid_argument("Currency code must be 3 uppercase letters");
    }

    if (decimals < 0 || decimals > 6) {
        throw std::invalid_argument("Currency decimals must be between 0 and 6");
    }
}

}  // namespace Finances::Core
