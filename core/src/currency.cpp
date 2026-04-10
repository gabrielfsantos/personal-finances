#include "core/src/currency.h"

#include <stdexcept>

namespace Finances::Core {

namespace {
constexpr auto is_ascii_upper(char c) noexcept { return c >= 'A' && c <= 'Z'; }

constexpr auto is_valid_code(std::string_view code) noexcept {
    return code.size() == 3 && is_ascii_upper(code[0]) && is_ascii_upper(code[1]) && is_ascii_upper(code[2]);
}

}  // namespace

Currency::Currency(std::string_view code, unsigned int decimals) : code_(code), decimals_(decimals) {
    if (!is_valid_code(code)) {
        throw std::invalid_argument("Currency code must be 3 uppercase letters");
    }

    if (decimals > 6) {
        throw std::invalid_argument("Currency decimals must be between 0 and 6");
    }
}

}  // namespace Finances::Core
