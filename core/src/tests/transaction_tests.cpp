#include <gtest/gtest.h>

#include "core/src/transaction.h"

TEST(TransactionTest, CreatesValidTransaction) {
    Transaction t(TransactionType::Income, Money(100, Currency::EUR), "food", 12345, "Lunch");

    EXPECT_EQ(t.amount.amount, 100);
    EXPECT_EQ(t.category_id, "food");
    EXPECT_EQ(t.description.value(), "Lunch");
    EXPECT_EQ(t.type, TransactionType::Income);
}

TEST(TransactionTest, Generates32CharId) {
    Transaction t(TransactionType::Expense, Money(50, Currency::EUR), "transport", 99999, "Bus");

    EXPECT_EQ(t.id.size(), 32);
}
