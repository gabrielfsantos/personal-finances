#include <gtest/gtest.h>

#include "core/src/money.h"

namespace {
using namespace Finances::Core;

TEST(MoneyTest, EqualityWorks) {
    Money a(100, Currency::EUR);
    Money b(100, Currency::EUR);
    EXPECT_EQ(a, b);
}

TEST(MoneyTest, ThrowsOnNegativeAmount) { EXPECT_THROW(Money(-10, Currency::EUR), std::invalid_argument); }

}  // namespace
