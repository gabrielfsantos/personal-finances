#include "core/src/money.h"

#include <stdexcept>

namespace Finances::Core {

Money::Money(long amount, Currency currency) : amount(amount), currency(currency) {
    if (amount < 0) {
        throw std::invalid_argument("Amount cannot be negative");
    }
}

}  // namespace Finances::Core
