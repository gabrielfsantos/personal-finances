#pragma once
#include "core/src/currency.h"

namespace Finances::Core {
class Money {
   public:
    long amount;
    Currency currency;

    Money(long amount, Currency currency);
    bool operator==(const Money &other) const;
};

}  // namespace Finances::Core
