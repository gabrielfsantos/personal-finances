#include "core/src/money.h"

#include <stdexcept>

Money::Money(long amount, Currency currency) : amount(amount), currency(currency) {
    if (amount < 0) {
        throw std::invalid_argument("Amount cannot be negative");
    }
}

bool Money::operator==(const Money &other) const { return amount == other.amount && currency == other.currency; }
