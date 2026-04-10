#include <gtest/gtest.h>

#include <format>
#include <memory>

#include "core/src/currency.h"
#include "core/src/money.h"
#include "core/src/money_errors.h"

namespace {
using namespace Finances::Core;

class MoneyTests : public ::testing::Test {
   protected:
    Currency eur_{"EUR", 2};
};

TEST_F(MoneyTests, GIVEN_EUR_WHEN_FromMajor_THEN_MinorUnitsAreCorrect) {
    auto m = Money::from_major(eur_, 10);

    EXPECT_EQ(m.currency(), eur_);
    EXPECT_EQ(m.minor_units(), 1000);
    EXPECT_EQ(m.to_string(), "EUR 10.00");
}

TEST_F(MoneyTests, GIVEN_EUR_WHEN_FromMajor_with_negative_THEN_MinorUnitsAreCorrect) {
    auto m = Money::from_major(eur_, -10);

    EXPECT_EQ(m.currency(), eur_);
    EXPECT_EQ(m.minor_units(), -1000);
    EXPECT_EQ(m.to_string(), "EUR -10.00");
}

TEST_F(MoneyTests, GIVEN_EUR_WHEN_FromMajor_with_overflow_THEN_Throws) {
    EXPECT_THROW(Money::from_major(eur_, std::numeric_limits<std::int64_t>::max()), MoneyOverflowError);
}

TEST_F(MoneyTests, GIVEN_EUR_WHEN_FromMajor_with_underflow_THEN_Throws) {
    EXPECT_THROW(Money::from_major(eur_, std::numeric_limits<std::int64_t>::min()), MoneyOverflowError);
}

TEST_F(MoneyTests, GIVEN_DecimalString_WHEN_Parsed_THEN_MinorUnitsMatch) {
    auto m = Money::from_decimal_string(eur_, "123.45");

    EXPECT_EQ(m.minor_units(), 12345);
    EXPECT_EQ(m.to_string(), "EUR 123.45");
}

TEST_F(MoneyTests, GIVEN_NoFraction_WHEN_Parsed_THEN_FractionDefaultsToZero) {
    auto m = Money::from_decimal_string(eur_, "7");

    EXPECT_EQ(m.minor_units(), 700);
    EXPECT_EQ(m.to_string(), "EUR 7.00");
}

TEST_F(MoneyTests, GIVEN_NegativeDecimal_WHEN_Parsed_THEN_MoneyIsNegative) {
    auto m = Money::from_decimal_string(eur_, "-1.23");

    EXPECT_EQ(m.minor_units(), -123);
    EXPECT_EQ(m.to_string(), "EUR -1.23");
}

TEST_F(MoneyTests, GIVEN_TooManyFractionDigits_WHEN_Parsed_THEN_Throws) {
    EXPECT_THROW(Money::from_decimal_string(eur_, "1.234"), MoneyParseError);
}

TEST_F(MoneyTests, GIVEN_TooManyZerosToTheRight_WHEN_Parsed_THEN_Throws) {
    EXPECT_THROW(Money::from_decimal_string(eur_, "1.230000"), MoneyParseError);
}

TEST_F(MoneyTests, GIVEN_InvalidInteger_WHEN_Parsed_THEN_Throws) {
    EXPECT_THROW(Money::from_decimal_string(eur_, "A.23"), MoneyParseError);
}

TEST_F(MoneyTests, GIVEN_InvalidFraction_WHEN_Parsed_THEN_Throws) {
    EXPECT_THROW(Money::from_decimal_string(eur_, "1.A"), MoneyParseError);
}

TEST_F(MoneyTests, GIVEN_InvalidIntegerCharacter_WHEN_Parsed_THEN_Throws) {
    EXPECT_THROW(Money::from_decimal_string(eur_, "A"), MoneyParseError);
}

TEST_F(MoneyTests, GIVEN_IntegerNumberAndInvalidCharacter_WHEN_Parsed_THEN_Throws) {
    EXPECT_THROW(Money::from_decimal_string(eur_, "1A"), MoneyParseError);
}

TEST_F(MoneyTests, GIVEN_FractionalNumberAndInvalidCharacter_WHEN_Parsed_THEN_Throws) {
    EXPECT_THROW(Money::from_decimal_string(eur_, "1.2A"), MoneyParseError);
}

TEST_F(MoneyTests, GIVEN_SingleCharacterMinus_WHEN_Parsed_THEN_Throws) {
    EXPECT_THROW(Money::from_decimal_string(eur_, "-"), MoneyParseError);
}

TEST_F(MoneyTests, GIVEN_DotWithoutNumbers_WHEN_Parsed_THEN_Throws) {
    EXPECT_THROW(Money::from_decimal_string(eur_, "."), MoneyParseError);
}

TEST_F(MoneyTests, GIVEN_IntegerPartAbsent_WHEN_Parsed_THEN_Throws) {
    EXPECT_THROW(Money::from_decimal_string(eur_, ".5"), MoneyParseError);
}

TEST_F(MoneyTests, GIVEN_NegativeNumberAndAbsentIntegerPart_WHEN_Parsed_THEN_Throws) {
    EXPECT_THROW(Money::from_decimal_string(eur_, "-.5"), MoneyParseError);
}

TEST_F(MoneyTests, GIVEN_LeadingZeros_WHEN_Parsed_THEN_IgnoresLeadingZeros) {
    auto a = Money::from_decimal_string(eur_, "0001.23");

    EXPECT_EQ(a.minor_units(), 123);
    EXPECT_EQ(a.to_string(), "EUR 1.23");
}

TEST_F(MoneyTests, GIVEN_MoreThanOneDecimalPoint_WHEN_Parsed_THEN_Throws) {
    EXPECT_THROW(Money::from_decimal_string(eur_, "1.2.3"), MoneyParseError);
}

TEST_F(MoneyTests, GIVEN_TwoMinusSigns_WHEN_Parsed_THEN_Throws) {
    EXPECT_THROW(Money::from_decimal_string(eur_, "--1.23"), MoneyParseError);
}

TEST_F(MoneyTests, GIVEN_MinusSignInFractionalPart_WHEN_Parsed_THEN_Throws) {
    EXPECT_THROW(Money::from_decimal_string(eur_, "1.-23"), MoneyParseError);
}

TEST_F(MoneyTests, GIVEN_IntegerNumberAndInvalidCharacterAndFractionalPart_WHEN_Parsed_THEN_Throws) {
    EXPECT_THROW(Money::from_decimal_string(eur_, "1A.23"), MoneyParseError);
}

TEST_F(MoneyTests, GIVEN_IntegerPartOverflow_WHEN_Parsed_THEN_Throws) {
    const auto overflow_str = std::format("{}.00", std::numeric_limits<std::int64_t>::max() / 100 + 1);

    EXPECT_THROW(Money::from_decimal_string(eur_, overflow_str), MoneyParseError);
}

TEST_F(MoneyTests, GIVEN_AbsoluteIntegerPartOverflow_WHEN_Parsed_THEN_Throws) {
    Currency usd("USD", 0);
    const std::string min_str = std::to_string(std::numeric_limits<std::int64_t>::min());

    EXPECT_THROW(Money::from_decimal_string(usd, min_str), MoneyParseError);
}

TEST_F(MoneyTests, GIVEN_SameCurrency_WHEN_Added_THEN_ResultIsCorrect) {
    auto a = Money::from_decimal_string(eur_, "1.50");
    auto b = Money::from_decimal_string(eur_, "2.25");

    auto c = a + b;

    EXPECT_EQ(c.minor_units(), 375);
    EXPECT_EQ(c.to_string(), "EUR 3.75");
}

TEST_F(MoneyTests, GIVEN_SameCurrency_WHEN_Subtracted_THEN_ResultIsCorrect) {
    auto a = Money::from_decimal_string(eur_, "1.50");
    auto b = Money::from_decimal_string(eur_, "2.25");

    auto c = a - b;

    EXPECT_EQ(c.minor_units(), -75);
    EXPECT_EQ(c.to_string(), "EUR -0.75");
}

TEST_F(MoneyTests, GIVEN_SameCurrency_WHEN_AddedWithOverflow_THEN_Throws) {
    auto a = Money(eur_, std::numeric_limits<std::int64_t>::max() - 100);
    auto b = Money(eur_, 225);

    EXPECT_THROW(a + b, MoneyOverflowError);
}

TEST_F(MoneyTests, GIVEN_SameCurrency_WHEN_AddedWithUnderflow_THEN_Throws) {
    auto a = Money(eur_, std::numeric_limits<std::int64_t>::min() + 100);
    auto b = Money(eur_, 225);

    EXPECT_THROW(a - b, MoneyOverflowError);
}

TEST_F(MoneyTests, GIVEN_DifferentCurrencies_WHEN_Added_THEN_Throws) {
    Currency usd("USD", 2);
    auto a = Money::from_major(eur_, 1);
    auto b = Money::from_major(usd, 1);

    EXPECT_THROW(a + b, MoneyCurrencyMismatchError);
}

TEST_F(MoneyTests, GIVEN_DifferentCurrencies_WHEN_Subtracted_THEN_Throws) {
    Currency usd("USD", 2);
    auto a = Money::from_major(eur_, 1);
    auto b = Money::from_major(usd, 1);

    EXPECT_THROW(a - b, MoneyCurrencyMismatchError);
}

TEST_F(MoneyTests, GIVEN_SameCurrency_WHEN_Compared_THEN_OrderIsCorrect) {
    auto a = Money::from_decimal_string(eur_, "1.00");
    auto b = Money::from_decimal_string(eur_, "2.00");

    EXPECT_TRUE(a < b);
    EXPECT_TRUE(b > a);
}

TEST_F(MoneyTests, GIVEN_DifferentCurrencies_WHEN_Compared_THEN_Throws) {
    Currency usd("USD", 2);
    auto a = Money::from_major(eur_, 1);
    auto b = Money::from_major(usd, 1);

    EXPECT_THROW((void)(a <=> b), MoneyCurrencyMismatchError);
}

}  // namespace
