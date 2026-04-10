#include <gtest/gtest.h>

#include "core/src/currency.h"

namespace {
using namespace Finances::Core;

TEST(CurrencyTests, ValidConstruction) {
    Currency eur("EUR", 2);

    EXPECT_EQ(eur.code(), "EUR");
    EXPECT_EQ(eur.decimals(), 2);
}

TEST(CurrencyTests, ValidDecimals) {
    EXPECT_NO_THROW(Currency eur1("EUR", 0));
    EXPECT_NO_THROW(Currency eur2("EUR", 2));
    EXPECT_NO_THROW(Currency eur3("EUR", 6));
}

TEST(CurrencyTests, InvalidCodeThrows) {
    EXPECT_THROW(Currency("EU", 2), std::invalid_argument);
    EXPECT_THROW(Currency("EURO", 2), std::invalid_argument);
    EXPECT_THROW(Currency("eur", 2), std::invalid_argument);
}

TEST(CurrencyTests, InvalidDecimalsThrows) { EXPECT_THROW(Currency("EUR", 7), std::invalid_argument); }

TEST(CurrencyTests, ComparisonWorks) {
    Currency eur1("EUR", 2);
    Currency eur2("EUR", 2);
    Currency usd("USD", 2);

    EXPECT_TRUE(eur1 == eur2);
    EXPECT_TRUE(eur1 != usd);
}

}  // namespace
