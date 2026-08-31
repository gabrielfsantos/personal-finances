#include <gtest/gtest.h>

#include <stdexcept>

#include "core/src/currency.h"

namespace {
// NOLINTNEXTLINE(google-build-using-namespace)
using namespace Finances::Core;

TEST(CurrencyTests, ValidConstruction) {
    const Currency eur("EUR", 2);

    EXPECT_EQ(eur.code(), "EUR");
    EXPECT_EQ(eur.decimals(), 2);
}

TEST(CurrencyTests, ValidDecimals) {
    EXPECT_NO_THROW(Currency("EUR", 0));
    EXPECT_NO_THROW(Currency("EUR", 2));
    EXPECT_NO_THROW(Currency("EUR", 6));
}

TEST(CurrencyTests, InvalidCodeThrows) {
    EXPECT_THROW(Currency("EU", 2), std::invalid_argument);
    EXPECT_THROW(Currency("EURO", 2), std::invalid_argument);
    EXPECT_THROW(Currency("eur", 2), std::invalid_argument);
}

TEST(CurrencyTests, InvalidDecimalsThrows) { EXPECT_THROW(Currency("EUR", 7), std::invalid_argument); }

TEST(CurrencyTests, ComparisonWorks) {
    const Currency eur1("EUR", 2);
    const Currency eur2("EUR", 2);
    const Currency usd("USD", 2);

    EXPECT_TRUE(eur1 == eur2);
    EXPECT_TRUE(eur1 != usd);
}

}  // namespace
