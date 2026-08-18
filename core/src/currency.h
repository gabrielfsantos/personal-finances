#pragma once
#include <compare>
#include <string>
#include <string_view>

namespace Finances::Core {

class Currency {
   public:
    explicit Currency(std::string_view code, unsigned int decimals);

    [[nodiscard]] auto code() const noexcept { return code_; }
    [[nodiscard]] constexpr auto decimals() const noexcept { return decimals_; }

    auto operator<=>(const Currency&) const noexcept = default;

   private:
    const static auto kMaxDecimals_ = 6U;
    std::string code_;       // ISO 4217 code (e.g., "EUR")
    unsigned int decimals_;  // Number of decimal places (e.g., 2)
};

}  // namespace Finances::Core
