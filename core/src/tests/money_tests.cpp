#include <gtest/gtest.h>

#include "core/src/currency.h"
#include "core/src/money.h"

namespace {
using namespace Finances::Core;

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
    EXPECT_THROW(Money::from_decimal_string(eur_, "1.234"), std::invalid_argument);
}

TEST_F(MoneyTests, GIVEN_TooManyFractionDigits_WHEN_Parsed_THEN_Throws_1) {
    EXPECT_THROW(Money::from_decimal_string(eur_, "A.23"), std::invalid_argument);
}

TEST_F(MoneyTests, GIVEN_TooManyFractionDigits_WHEN_Parsed_THEN_Throws_2) {
    EXPECT_THROW(Money::from_decimal_string(eur_, "1.A"), std::invalid_argument);
}

TEST_F(MoneyTests, GIVEN_TooManyFractionDigits_WHEN_Parsed_THEN_Throws_3) {
    EXPECT_THROW(Money::from_decimal_string(eur_, "A"), std::invalid_argument);
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

TEST_F(MoneyTests, GIVEN_DifferentCurrencies_WHEN_Added_THEN_Throws) {
    Currency usd("USD", 2);
    Money a = Money::from_major(eur_, 1);
    Money b = Money::from_major(usd, 1);

    EXPECT_THROW(a + b, std::logic_error);
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

    EXPECT_THROW((void)(a <=> b), std::logic_error);
}

}  // namespace
