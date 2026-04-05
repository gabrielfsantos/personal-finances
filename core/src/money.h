#pragma once
#include <compare>

#include "core/src/currency.h"

namespace Finances::Core {
class Money {
   public:
    long amount;
    Currency currency;

    Money(long amount, const Currency& currency);

    bool operator==(const Money&) const = default;
};

}  // namespace Finances::Core
