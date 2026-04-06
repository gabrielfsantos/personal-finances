#include <gtest/gtest.h>

#include "core/src/currency.h"
#include "core/src/money.h"
#include "core/src/money_errors.h"

namespace {
using namespace Finances::Core;

using minorType = decltype(std::declval<Money>().minor_units());

class MoneyTests : public ::testing::Test {
   protected:
    Currency eur_{"EUR", 2};
};

TEST_F(MoneyTests, GIVEN_EUR_WHEN_FromMajor_THEN_MinorUnitsAreCorrect) {
    Money m = Money::from_major(eur_, 10);

    EXPECT_EQ(m.currency(), eur_);
    EXPECT_EQ(m.minor_units(), 1000);
    EXPECT_EQ(m.to_string(), "EUR 10.00");
}

TEST_F(MoneyTests, GIVEN_EUR_WHEN_FromMajor_with_negative_THEN_MinorUnitsAreCorrect) {
    Money m = Money::from_major(eur_, -10);

    EXPECT_EQ(m.currency(), eur_);
    EXPECT_EQ(m.minor_units(), -1000);
    EXPECT_EQ(m.to_string(), "EUR -10.00");
}

TEST_F(MoneyTests, GIVEN_EUR_WHEN_FromMajor_with_overflow_THEN_Throws) {
    EXPECT_THROW(Money::from_major(eur_, std::numeric_limits<minorType>::max()), MoneyOverflowError);
}

TEST_F(MoneyTests, GIVEN_EUR_WHEN_FromMajor_with_underflow_THEN_Throws) {
    EXPECT_THROW(Money::from_major(eur_, std::numeric_limits<minorType>::min()), MoneyOverflowError);
}

TEST_F(MoneyTests, GIVEN_DecimalString_WHEN_Parsed_THEN_MinorUnitsMatch) {
    Money m = Money::from_decimal_string(eur_, "123.45");

    EXPECT_EQ(m.minor_units(), 12345);
    EXPECT_EQ(m.to_string(), "EUR 123.45");
}

TEST_F(MoneyTests, GIVEN_NoFraction_WHEN_Parsed_THEN_FractionDefaultsToZero) {
    Money m = Money::from_decimal_string(eur_, "7");

    EXPECT_EQ(m.minor_units(), 700);
    EXPECT_EQ(m.to_string(), "EUR 7.00");
}

TEST_F(MoneyTests, GIVEN_OnlyFraction_WHEN_Parsed_THEN_IntegerDefaultsToZero) {
    Money m = Money::from_decimal_string(eur_, ".5");

    EXPECT_EQ(m.minor_units(), 50);
    EXPECT_EQ(m.to_string(), "EUR 0.50");
}

TEST_F(MoneyTests, GIVEN_EmptyString_WHEN_Parsed_THEN_MoneyIsZero) {
    Money m = Money::from_decimal_string(eur_, "");

    EXPECT_EQ(m.minor_units(), 0);
    EXPECT_EQ(m.to_string(), "EUR 0.00");
}

TEST_F(MoneyTests, GIVEN_OnlyInteger_WHEN_Parsed_THEN_FractionDefaultsToZero) {
    Money m = Money::from_decimal_string(eur_, "1.");

    EXPECT_EQ(m.minor_units(), 100);
    EXPECT_EQ(m.to_string(), "EUR 1.00");
}

TEST_F(MoneyTests, GIVEN_NegativeDecimal_WHEN_Parsed_THEN_MoneyIsNegative) {
    Money m = Money::from_decimal_string(eur_, "-1.23");

    EXPECT_EQ(m.minor_units(), -123);
    EXPECT_EQ(m.to_string(), "EUR -1.23");
}

TEST_F(MoneyTests, GIVEN_TooManyFractionDigits_WHEN_Parsed_THEN_Throws) {
    EXPECT_THROW(Money::from_decimal_string(eur_, "1.234"), MoneyParseError);
}

TEST_F(MoneyTests, GIVEN_InvalidInteger_WHEN_Parsed_THEN_Throws) {
    EXPECT_THROW(Money::from_decimal_string(eur_, "A.23"), MoneyParseError);
}

TEST_F(MoneyTests, GIVEN_InvalidFraction_WHEN_Parsed_THEN_Throws) {
    EXPECT_THROW(Money::from_decimal_string(eur_, "1.A"), MoneyParseError);
}

TEST_F(MoneyTests, GIVEN_InvalidNumber_WHEN_Parsed_THEN_Throws) {
    EXPECT_THROW(Money::from_decimal_string(eur_, "A"), MoneyParseError);
}

TEST_F(MoneyTests, GIVEN_InvalidNumber_WHEN_Parsed_THEN_Throws_1) {
    EXPECT_THROW(Money::from_decimal_string(eur_, "1A"), MoneyParseError);
}

TEST_F(MoneyTests, GIVEN_InvalidNumber_WHEN_Parsed_THEN_Throws_2) {
    EXPECT_THROW(Money::from_decimal_string(eur_, "1.2A"), MoneyParseError);
}

TEST_F(MoneyTests, GIVEN_IntegerPartOverflow_WHEN_Parsed_THEN_Throws) {
    std::string overflow_str = std::to_string(std::numeric_limits<minorType>::max() / 100 + 1) + ".00";

    EXPECT_THROW(Money::from_decimal_string(eur_, overflow_str), MoneyParseError);
}

TEST_F(MoneyTests, GIVEN_IntegerPartUnderflow_WHEN_Parsed_THEN_Throws) {
    Currency usd("USD", 0);
    std::string underflow_str =
            std::to_string(std::numeric_limits<minorType>::min())
                    .substr(1, std::string::npos);

    EXPECT_THROW(Money::from_decimal_string(usd, underflow_str), MoneyParseError);
}

TEST_F(MoneyTests, GIVEN_SameCurrency_WHEN_Added_THEN_ResultIsCorrect) {
    Money a = Money::from_decimal_string(eur_, "1.50");
    Money b = Money::from_decimal_string(eur_, "2.25");

    Money c = a + b;

    EXPECT_EQ(c.minor_units(), 375);
    EXPECT_EQ(c.to_string(), "EUR 3.75");
}

TEST_F(MoneyTests, GIVEN_SameCurrency_WHEN_Subtracted_THEN_ResultIsCorrect) {
    Money a = Money::from_decimal_string(eur_, "1.50");
    Money b = Money::from_decimal_string(eur_, "2.25");

    Money c = a - b;

    EXPECT_EQ(c.minor_units(), -75);
    EXPECT_EQ(c.to_string(), "EUR -0.75");
}

TEST_F(MoneyTests, GIVEN_SameCurrency_WHEN_AddedWithOverflow_THEN_Throws) {
    Money a = Money(eur_, std::numeric_limits<minorType>::max() - 100);
    Money b = Money(eur_, 225);

    EXPECT_THROW(a + b, MoneyOverflowError);
}

TEST_F(MoneyTests, GIVEN_SameCurrency_WHEN_AddedWithUnderflow_THEN_Throws) {
    Money a = Money(eur_, std::numeric_limits<minorType>::min() + 100);
    Money b = Money(eur_, 225);

    EXPECT_THROW(a - b, MoneyOverflowError);
}

TEST_F(MoneyTests, GIVEN_DifferentCurrencies_WHEN_Added_THEN_Throws) {
    Currency usd("USD", 2);
    Money a = Money::from_major(eur_, 1);
    Money b = Money::from_major(usd, 1);

    EXPECT_THROW(a + b, MoneyCurrencyMismatchError);
}

TEST_F(MoneyTests, GIVEN_DifferentCurrencies_WHEN_Subtracted_THEN_Throws) {
    Currency usd("USD", 2);
    Money a = Money::from_major(eur_, 1);
    Money b = Money::from_major(usd, 1);

    EXPECT_THROW(a - b, MoneyCurrencyMismatchError);
}

TEST_F(MoneyTests, GIVEN_SameCurrency_WHEN_Compared_THEN_OrderIsCorrect) {
    Money a = Money::from_decimal_string(eur_, "1.00");
    Money b = Money::from_decimal_string(eur_, "2.00");

    EXPECT_TRUE(a < b);
    EXPECT_TRUE(b > a);
}

TEST_F(MoneyTests, GIVEN_DifferentCurrencies_WHEN_Compared_THEN_Throws) {
    Currency usd("USD", 2);
    Money a = Money::from_major(eur_, 1);
    Money b = Money::from_major(usd, 1);

    EXPECT_THROW((void)(a <=> b), MoneyCurrencyMismatchError);
}

}  // namespace
