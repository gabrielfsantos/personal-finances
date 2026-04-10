#include "core/src/money.h"

#include <charconv>
#include <cmath>
#include <limits>
#include <stdexcept>

#include "core/src/money_errors.h"

namespace Finances::Core {

namespace {

auto pow10(unsigned int n) {
    std::int64_t result = 1;
    while (n-- > 0) {
        result *= 10;
    }
    return result;
}

auto safe_add(std::int64_t a, std::int64_t b) {
    if ((b > 0 && a > std::numeric_limits<std::int64_t>::max() - b) ||
        (b < 0 && a < std::numeric_limits<std::int64_t>::min() - b)) {
        throw MoneyOverflowError("addition overflow");
    }
    return a + b;
}

auto safe_sub(std::int64_t a, std::int64_t b) {
    if ((b > 0 && a < std::numeric_limits<std::int64_t>::min() + b) ||
        (b < 0 && a > std::numeric_limits<std::int64_t>::max() + b)) {
        throw MoneyOverflowError("subtraction overflow");
    }
    return a - b;
}

auto handleNegativeDecimalString(std::string_view& text) {
    if (text.front() == '-') {
        text.remove_prefix(1);

        if (text.empty()) {
            throw MoneyParseError("String contains only '-'");
        }

        return true;
    }

    return false;
}

void validateCharsAndDots(std::string_view text) {
    auto dot_count = 0u;
    for (auto c : text) {
        if (c == '.') {
            ++dot_count;
            continue;
        }
        if (c < '0' || c > '9') {
            throw MoneyParseError("Invalid character in decimal string");
        }
    }

    if (dot_count > 1) {
        throw MoneyParseError("Multiple decimal points");
    }
}

struct DecimalDetails {
    std::size_t dotPos;
    std::string_view integer;
    std::string_view fraction;
};

auto splitDecimalString(std::string_view text) {
    auto dot_pos = text.find('.');
    return DecimalDetails{
            .dotPos = dot_pos,
            .integer = text.substr(0, dot_pos),
            .fraction = (dot_pos == std::string_view::npos) ? std::string_view{} : text.substr(dot_pos + 1),
    };
}

void validateDecimalParts(const DecimalDetails& details) {
    if (details.integer.empty() && details.fraction.empty()) {
        throw MoneyParseError("String contains only '.'");
    }
    if (details.integer.empty() && !details.fraction.empty()) {
        throw MoneyParseError("Missing integer part before decimal point");
    }
    if (!details.integer.empty() && details.dotPos != std::string_view::npos && details.fraction.empty()) {
        throw MoneyParseError("Missing fractional part after decimal point");
    }
}

struct DecimalStrParts {
    bool negative;
    std::string_view integer;
    std::string_view fraction;
};

auto extractDecimalStrParts(std::string_view text) {
    if (text.empty()) {
        throw MoneyParseError("Empty string");
    }

    auto negative = handleNegativeDecimalString(text);

    validateCharsAndDots(text);

    auto decimalDetails = splitDecimalString(text);

    validateDecimalParts(decimalDetails);

    return DecimalStrParts{
            .negative = negative,
            .integer = decimalDetails.integer,
            .fraction = decimalDetails.fraction,
    };
}

auto convertIntegerPart(std::string_view integerPart) {
    std::int64_t integer = 0;
    if (!integerPart.empty()) {
        auto res = std::from_chars(integerPart.data(), integerPart.data() + integerPart.size(), integer);
        if (res.ec != std::errc{} || res.ptr != integerPart.data() + integerPart.size()) {
            throw MoneyParseError("Invalid integer part");
        }
    }
    return integer;
}

auto convertFractionalPart(std::string_view fractionPart) {
    std::int64_t fraction = 0;
    if (!fractionPart.empty()) {
        auto res = std::from_chars(fractionPart.data(), fractionPart.data() + fractionPart.size(), fraction);
        if (res.ec != std::errc{} || res.ptr != fractionPart.data() + fractionPart.size()) {
            throw MoneyParseError("Invalid fractional part");
        }
    }
    return fraction;
}

void validateFractionalPrecision(std::string_view fractionPart, unsigned int decimals) {
    if (fractionPart.size() > decimals) {
        throw MoneyParseError("Fractional digits exceed currency precision");
    }
}

struct Factors {
    std::int64_t integer;
    std::int64_t fraction;
};

auto computeScalingFactors(unsigned int decimals, std::size_t fractionSize) {
    return Factors{.integer = pow10(decimals), .fraction = pow10(decimals - static_cast<unsigned>(fractionSize))};
}

void validateIntegerOverflow(std::int64_t integer, std::int64_t factor) {
    if (integer > std::numeric_limits<std::int64_t>::max() / factor) {
        throw MoneyParseError("Integer part overflow");
    }
}

auto generateMinor(DecimalStrParts decimalStrParts, Currency currency) {
    auto integer = convertIntegerPart(decimalStrParts.integer);

    validateFractionalPrecision(decimalStrParts.fraction, currency.decimals());

    auto fraction = convertFractionalPart(decimalStrParts.fraction);

    auto factors = computeScalingFactors(currency.decimals(), decimalStrParts.fraction.size());

    validateIntegerOverflow(integer, factors.integer);

    auto minor = safe_add(integer * factors.integer, fraction * factors.fraction);

    if (decimalStrParts.negative) {
        minor = -minor;
    }

    return minor;
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

    auto minor = generateMinor(extractDecimalStrParts(text), currency);

    return Money(std::move(currency), minor);
}

std::string Money::to_string() const {
    auto decimals = currency_.decimals();
    auto factor = pow10(decimals);

    const bool negative = minor_units_ < 0;
    const auto value = negative ? (std::uint64_t{0} - static_cast<std::uint64_t>(minor_units_))
                                : static_cast<std::uint64_t>(minor_units_);

    const auto integer = value / static_cast<std::uint64_t>(factor);
    const auto fraction = value % static_cast<std::uint64_t>(factor);

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

    minor_units_ = safe_sub(minor_units_, other.minor_units_);
    return *this;
}

std::strong_ordering operator<=>(const Money& lhs, const Money& rhs) {
    if (lhs.currency_ != rhs.currency_) {
        throw MoneyCurrencyMismatchError("Cannot compare Money with different currencies");
    }

    return lhs.minor_units_ <=> rhs.minor_units_;
}

}  // namespace Finances::Core
