#include "core/src/money.h"

#include <charconv>
#include <cmath>
#include <limits>
#include <stdexcept>

#include "core/src/money_errors.h"

namespace Finances::Core {

namespace {

std::int64_t pow10(unsigned int n) {
    std::int64_t result = 1;
    while (n-- > 0) {
        result *= 10;
    }
    return result;
}

std::int64_t safe_add(std::int64_t a, std::int64_t b) {
    if ((b > 0 && a > std::numeric_limits<std::int64_t>::max() - b) ||
        (b < 0 && a < std::numeric_limits<std::int64_t>::min() - b)) {
        throw MoneyOverflowError("addition overflow");
    }
    return a + b;
}

}  // namespace

Money::Money(Currency currency, std::int64_t minor_units) : minor_units_(minor_units), currency_(std::move(currency)) {}

Money Money::from_major(Currency currency, std::int64_t major) {
    const auto factor = pow10(currency.decimals());
    if (major > 0 && major > std::numeric_limits<std::int64_t>::max() / factor) {
        throw MoneyOverflowError("Money from_major overflow");
    }

    if (major < 0 && major < std::numeric_limits<std::int64_t>::min() / factor) {
        throw MoneyOverflowError("Money from_major underflow");
    }
    return Money(std::move(currency), major * factor);
}

Money Money::from_decimal_string(Currency currency, std::string_view text) {
    // Simple format: [-]digits[.digits]

    bool negative = false;
    if (!text.empty() && text.front() == '-') {
        negative = true;
        text.remove_prefix(1);
    }

    auto dot_pos = text.find('.');
    std::string_view int_part = text.substr(0, dot_pos);
    std::string_view frac_part = (dot_pos == std::string_view::npos) ? std::string_view{} : text.substr(dot_pos + 1);

    std::int64_t integer = 0;
    if (!int_part.empty()) {
        auto res = std::from_chars(int_part.data(), int_part.data() + int_part.size(), integer);
        if (res.ec != std::errc{} || res.ptr != int_part.data() + int_part.size()) {
            throw MoneyParseError("Invalid integer part in decimal string");
        }
    }

    auto decimals = currency.decimals();
    if (static_cast<unsigned int>(frac_part.size()) > decimals) {
        throw MoneyParseError("Too many fractional digits for currency");
    }

    std::int64_t fraction = 0;
    if (!frac_part.empty()) {
        auto res = std::from_chars(frac_part.data(), frac_part.data() + frac_part.size(), fraction);
        if (res.ec != std::errc{} || res.ptr != frac_part.data() + frac_part.size()) {
            throw MoneyParseError("Invalid fractional part in decimal string");
        }
    }

    auto factor = pow10(decimals);
    auto frac_factor = pow10(decimals - static_cast<unsigned int>(frac_part.size()));

    if ((integer > 0 && integer > std::numeric_limits<std::int64_t>::max() / factor) ||
        (integer < 0 && integer < std::numeric_limits<std::int64_t>::min() / factor)) {
        throw MoneyParseError("Integer part overflow");
    }

    auto minor = safe_add(integer * factor, fraction * frac_factor);
    if (negative && minor != 0 && minor == std::numeric_limits<std::int64_t>::min()) {
        throw MoneyParseError("Cannot negate INT64_MIN");
    }

    if (negative) {
        minor = -minor;
    }

    return Money(std::move(currency), minor);
}

std::string Money::to_string() const {
    auto decimals = currency_.decimals();
    std::int64_t factor = pow10(decimals);

    std::int64_t value = minor_units_;
    bool negative = value < 0;
    if (negative) {
        value = -value;
    }

    std::int64_t integer = value / factor;
    std::int64_t fraction = value % factor;

    std::string result;
    result.reserve(32);
    result += std::string(currency_.code());
    result += ' ';
    if (negative) {
        result += '-';
    }
    result += std::to_string(integer);
    if (decimals > 0) {
        result += '.';
        auto frac_str = std::to_string(fraction);
        if (frac_str.size() < static_cast<std::size_t>(decimals)) {
            result.append(decimals - frac_str.size(), '0');
        }
        result += frac_str;
    }
    return result;
}

Money& Money::operator+=(const Money& other) {
    if (currency_ != other.currency_) {
        throw MoneyCurrencyMismatchError("Cannot add Money with different currencies");
    }
    minor_units_ = safe_add(minor_units_, other.minor_units_);
    return *this;
}

Money& Money::operator-=(const Money& other) {
    if (currency_ != other.currency_) {
        throw MoneyCurrencyMismatchError("Cannot subtract Money with different currencies");
    }
    minor_units_ = safe_add(minor_units_, -other.minor_units_);
    return *this;
}

std::strong_ordering operator<=>(const Money& lhs, const Money& rhs) {
    if (lhs.currency_ != rhs.currency_) {
        throw MoneyCurrencyMismatchError("Cannot compare Money with different currencies");
    }
    return lhs.minor_units_ <=> rhs.minor_units_;
}

}  // namespace Finances::Core
