#pragma once
#include <compare>

#include "core/src/currency.h"

namespace Finances::Core {
class Money {
   public:
    long amount;
    Currency currency;

    Money(long amount, Currency currency);
    auto operator<=>(const Money&) const = default;
};

}  // namespace Finances::Core
