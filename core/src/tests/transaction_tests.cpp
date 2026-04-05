#include <gtest/gtest.h>

#include "core/src/transaction.h"

namespace {
using namespace Finances::Core;

TEST(TransactionTest, CreatesValidTransaction) {
    Transaction t(TransactionType::Income, Money(Currency("EUR", 2), 100), "food", 12345, "Lunch");

    EXPECT_EQ(t.amount.minor_units(), 100);
    EXPECT_EQ(t.category_id, "food");
    EXPECT_EQ(t.description.value(), "Lunch");
    EXPECT_EQ(t.type, TransactionType::Income);
}

TEST(TransactionTest, Generates32CharId) {
    Transaction t(TransactionType::Expense, Money(Currency("EUR", 2), 50), "transport", 99999, "Bus");

    EXPECT_EQ(t.id.size(), 32);
}

}  // namespace
