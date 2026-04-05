#include <gtest/gtest.h>

#include "core/src/money.h"

namespace {
using namespace Finances::Core;

TEST(MoneyTest, EqualityWorks) {
    Money a(100, Currency("EUR", 2));
    Money b(100, Currency("EUR", 2));
    EXPECT_EQ(a, b);
}

TEST(MoneyTest, InequalityWorks) {
    Money a(100, Currency("EUR", 2));
    Money b(200, Currency("EUR", 2));
    Money c(100, Currency("USD", 2));
    EXPECT_NE(a, b);
    EXPECT_NE(a, c);
}

TEST(MoneyTest, ThrowsOnNegativeAmount) { EXPECT_THROW(Money(-10, Currency("EUR", 2)), std::invalid_argument); }

}  // namespace
