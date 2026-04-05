#pragma once
#include <compare>
#include <string>
#include <string_view>

namespace Finances::Core {

class Currency {
   public:
    Currency(std::string_view code, int decimals);

    std::string_view code() const noexcept { return code_; }
    constexpr int decimals() const noexcept { return decimals_; }

    auto operator<=>(const Currency&) const noexcept = default;

   private:
    std::string code_;  // ISO 4217 code (e.g., "EUR")
    int decimals_;      // Number of decimal places (e.g., 2)
};

}  // namespace Finances::Core
