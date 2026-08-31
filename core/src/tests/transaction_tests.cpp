#include <gtest/gtest.h>

#include <chrono>

#include "core/src/currency.h"
#include "core/src/money.h"
#include "core/src/transaction.h"

namespace {
// NOLINTNEXTLINE(google-build-using-namespace)
using namespace Finances::Core;

TEST(TransactionTest, CreatesValidTransaction) {
    // NOLINTBEGIN(misc-include-cleaner)
    const Transaction transaction(Transaction::Type::Income,
                                  Money(Currency("EUR", 2), 100),
                                  "food",
                                  std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now()),
                                  "Lunch");
    // NOLINTEND(misc-include-cleaner)

    EXPECT_EQ(transaction.getAmount().minor_units(), 100);
    EXPECT_EQ(transaction.getCategoryId(), "food");
    EXPECT_EQ(transaction.getDescription(), "Lunch");
    EXPECT_EQ(transaction.getType(), Transaction::Type::Income);
}

TEST(TransactionTest, Generates32CharId) {
    // NOLINTBEGIN(misc-include-cleaner)
    const Transaction transaction(Transaction::Type::Expense,
                                  Money(Currency("EUR", 2), 50),
                                  "transport",
                                  std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now()),
                                  "Bus");
    // NOLINTEND(misc-include-cleaner)

    EXPECT_EQ(transaction.getId().size(), 32);
}

}  // namespace
