#include <gtest/gtest.h>

#include <cstdint>
#include <format>
#include <limits>
#include <string>

#include "core/src/currency.h"
#include "core/src/money.h"
#include "core/src/money_errors.h"

namespace {
// NOLINTNEXTLINE(google-build-using-namespace)
using namespace Finances::Core;

class MoneyTests : public ::testing::Test {
   protected:
    const Currency eur_{"EUR", 2};
};

TEST_F(MoneyTests, GivenEurWhenFromMajorThenMinorUnitsAreCorrect) {
    const auto money = Money::from_major(eur_, 10);

    EXPECT_EQ(money.currency(), eur_);
    EXPECT_EQ(money.minor_units(), 1000);
    EXPECT_EQ(money.to_string(), "EUR 10.00");
}

TEST_F(MoneyTests, GivenEurWhenFromMajorWithNegativeThenMinorUnitsAreCorrect) {
    const auto money = Money::from_major(eur_, -10);

    EXPECT_EQ(money.currency(), eur_);
    EXPECT_EQ(money.minor_units(), -1000);
    EXPECT_EQ(money.to_string(), "EUR -10.00");
}

TEST_F(MoneyTests, GivenEurWhenFromMajorWithOverflowThenThrows) {
    EXPECT_THROW(Money::from_major(eur_, std::numeric_limits<std::int64_t>::max()), MoneyOverflowError);
}

TEST_F(MoneyTests, GivenEurWhenFromMajorWithUnderflowThenThrows) {
    EXPECT_THROW(Money::from_major(eur_, std::numeric_limits<std::int64_t>::min()), MoneyOverflowError);
}

TEST_F(MoneyTests, GivenDecimalStringWhenParsedThenMinorUnitsMatch) {
    const auto money = Money::from_decimal_string(eur_, "123.45");

    EXPECT_EQ(money.minor_units(), 12345);
    EXPECT_EQ(money.to_string(), "EUR 123.45");
}

TEST_F(MoneyTests, GivenNoFractionWhenParsedThenFractionDefaultsToZero) {
    const auto money = Money::from_decimal_string(eur_, "7");

    EXPECT_EQ(money.minor_units(), 700);
    EXPECT_EQ(money.to_string(), "EUR 7.00");
}

TEST_F(MoneyTests, GivenNegativeDecimalWhenParsedThenMoneyIsNegative) {
    const auto money = Money::from_decimal_string(eur_, "-1.23");

    EXPECT_EQ(money.minor_units(), -123);
    EXPECT_EQ(money.to_string(), "EUR -1.23");
}

TEST_F(MoneyTests, GivenTooManyFractionDigitsWhenParsedThenThrows) {
    EXPECT_THROW(Money::from_decimal_string(eur_, "1.234"), MoneyParseError);
}

TEST_F(MoneyTests, GivenTooManyZerosToTheRightWhenParsedThenThrows) {
    EXPECT_THROW(Money::from_decimal_string(eur_, "1.230000"), MoneyParseError);
}

TEST_F(MoneyTests, GivenInvalidIntegerWhenParsedThenThrows) {
    EXPECT_THROW(Money::from_decimal_string(eur_, "A.23"), MoneyParseError);
}

TEST_F(MoneyTests, GivenInvalidFractionWhenParsedThenThrows) {
    EXPECT_THROW(Money::from_decimal_string(eur_, "1.A"), MoneyParseError);
}

TEST_F(MoneyTests, GivenInvalidIntegerCharacterWhenParsedThenThrows) {
    EXPECT_THROW(Money::from_decimal_string(eur_, "A"), MoneyParseError);
}

TEST_F(MoneyTests, GivenIntegerNumberAndInvalidCharacterWhenParsedThenThrows) {
    EXPECT_THROW(Money::from_decimal_string(eur_, "1A"), MoneyParseError);
}

TEST_F(MoneyTests, GivenFractionalNumberAndInvalidCharacterWhenParsedThenThrows) {
    EXPECT_THROW(Money::from_decimal_string(eur_, "1.2A"), MoneyParseError);
}

TEST_F(MoneyTests, GivenSingleCharacterMinusWhenParsedThenThrows) {
    EXPECT_THROW(Money::from_decimal_string(eur_, "-"), MoneyParseError);
}

TEST_F(MoneyTests, GivenDotWithoutNumbersWhenParsedThenThrows) {
    EXPECT_THROW(Money::from_decimal_string(eur_, "."), MoneyParseError);
}

TEST_F(MoneyTests, GivenIntegerPartAbsentWhenParsedThenThrows) {
    EXPECT_THROW(Money::from_decimal_string(eur_, ".5"), MoneyParseError);
}

TEST_F(MoneyTests, GivenNegativeNumberAndAbsentIntegerPartWhenParsedThenThrows) {
    EXPECT_THROW(Money::from_decimal_string(eur_, "-.5"), MoneyParseError);
}

TEST_F(MoneyTests, GivenLeadingZerosWhenParsedThenIgnoresLeadingZeros) {
    const auto money = Money::from_decimal_string(eur_, "0001.23");

    EXPECT_EQ(money.minor_units(), 123);
    EXPECT_EQ(money.to_string(), "EUR 1.23");
}

TEST_F(MoneyTests, GivenMoreThanOneDecimalPointWhenParsedThenThrows) {
    EXPECT_THROW(Money::from_decimal_string(eur_, "1.2.3"), MoneyParseError);
}

TEST_F(MoneyTests, GivenTwoMinusSignsWhenParsedThenThrows) {
    EXPECT_THROW(Money::from_decimal_string(eur_, "--1.23"), MoneyParseError);
}

TEST_F(MoneyTests, GivenMinusSignInFractionalPartWhenParsedThenThrows) {
    EXPECT_THROW(Money::from_decimal_string(eur_, "1.-23"), MoneyParseError);
}

TEST_F(MoneyTests, GivenIntegerNumberAndInvalidCharacterAndFractionalPartWhenParsedThenThrows) {
    EXPECT_THROW(Money::from_decimal_string(eur_, "1A.23"), MoneyParseError);
}

TEST_F(MoneyTests, GivenIntegerPartOverflowWhenParsedThenThrows) {
    const auto overflow_str = std::format("{}.00", std::numeric_limits<std::int64_t>::max() / 100 + 1);

    EXPECT_THROW(Money::from_decimal_string(eur_, overflow_str), MoneyParseError);
}

TEST_F(MoneyTests, GivenAbsoluteIntegerPartOverflowWhenParsedThenThrows) {
    const Currency usd("USD", 0);
    const std::string min_str = std::to_string(std::numeric_limits<std::int64_t>::min());

    EXPECT_THROW(Money::from_decimal_string(usd, min_str), MoneyParseError);
}

TEST_F(MoneyTests, GivenSameCurrencyWhenAddedThenResultIsCorrect) {
    const auto money_a = Money::from_decimal_string(eur_, "1.50");
    const auto money_b = Money::from_decimal_string(eur_, "2.25");

    const auto money_c = money_a + money_b;

    EXPECT_EQ(money_c.minor_units(), 375);
    EXPECT_EQ(money_c.to_string(), "EUR 3.75");
}

TEST_F(MoneyTests, GivenSameCurrencyWhenSubtractedThenResultIsCorrect) {
    const auto money_a = Money::from_decimal_string(eur_, "1.50");
    const auto money_b = Money::from_decimal_string(eur_, "2.25");

    const auto money_c = money_a - money_b;

    EXPECT_EQ(money_c.minor_units(), -75);
    EXPECT_EQ(money_c.to_string(), "EUR -0.75");
}

TEST_F(MoneyTests, GivenSameCurrencyWhenAddedWithOverflowThenThrows) {
    const auto money_a = Money(eur_, std::numeric_limits<std::int64_t>::max() - 100);
    const auto money_b = Money(eur_, 225);

    EXPECT_THROW(money_a + money_b, MoneyOverflowError);
}

TEST_F(MoneyTests, GivenSameCurrencyWhenAddedWithUnderflowThenThrows) {
    const auto money_a = Money(eur_, std::numeric_limits<std::int64_t>::min() + 100);
    const auto money_b = Money(eur_, 225);

    EXPECT_THROW(money_a - money_b, MoneyOverflowError);
}

TEST_F(MoneyTests, GivenDifferentCurrenciesWhenAddedThenThrows) {
    const Currency usd("USD", 2);
    const auto money_a = Money::from_major(eur_, 1);
    const auto money_b = Money::from_major(usd, 1);

    EXPECT_THROW(money_a + money_b, MoneyCurrencyMismatchError);
}

TEST_F(MoneyTests, GivenDifferentCurrenciesWhenSubtractedThenThrows) {
    const Currency usd("USD", 2);
    const auto money_a = Money::from_major(eur_, 1);
    const auto money_b = Money::from_major(usd, 1);

    EXPECT_THROW(money_a - money_b, MoneyCurrencyMismatchError);
}

TEST_F(MoneyTests, GivenSameCurrencyWhenComparedThenOrderIsCorrect) {
    const auto money_a = Money::from_decimal_string(eur_, "1.00");
    const auto money_b = Money::from_decimal_string(eur_, "2.00");

    EXPECT_TRUE(money_a < money_b);
    EXPECT_TRUE(money_b > money_a);
}

TEST_F(MoneyTests, GivenDifferentCurrenciesWhenComparedThenThrows) {
    const Currency usd("USD", 2);
    const auto money_a = Money::from_major(eur_, 1);
    const auto money_b = Money::from_major(usd, 1);

    EXPECT_THROW((void)(money_a <=> money_b), MoneyCurrencyMismatchError);
}

}  // namespace
