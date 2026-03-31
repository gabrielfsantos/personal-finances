#include <cassert>

#include "core/src/transaction.h"

int main() {
    Transaction t(TransactionType::Income, Money(100, Currency::EUR), "food", 12345, "Lunch");

    assert(t.id.size() == 32);
    assert(t.amount.amount == 100);

    return 0;
}
