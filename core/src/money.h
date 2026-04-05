#pragma once
#include <compare>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>

#include "core/src/currency.h"

namespace Finances::Core {
class Money {
   public:
    explicit Money(Currency currency, std::int64_t minor_units);

    // Convenient Factories
    static Money from_major(Currency currency, std::int64_t major);
    static Money from_decimal_string(Currency currency, std::string_view text);

    const Currency& currency() const noexcept { return currency_; }
    std::int64_t minor_units() const noexcept { return minor_units_; }

    std::string to_string() const;

    Money& operator+=(const Money& other);
    Money& operator-=(const Money& other);

    friend Money operator+(Money lhs, const Money& rhs) {
        lhs += rhs;
        return lhs;
    }

    friend Money operator-(Money lhs, const Money& rhs) {
        lhs -= rhs;
        return lhs;
    }

    friend bool operator==(const Money& lhs, const Money& rhs) noexcept {
        return lhs.currency_ == rhs.currency_ && lhs.minor_units_ == rhs.minor_units_;
    }

    friend std::strong_ordering operator<=>(const Money& lhs, const Money& rhs) {
        if (lhs.currency_ != rhs.currency_) {
            throw std::logic_error("Cannot compare Money with different currencies");
        }
        return lhs.minor_units_ <=> rhs.minor_units_;
    }

   private:
    std::int64_t minor_units_;
    Currency currency_;
};

}  // namespace Finances::Core
