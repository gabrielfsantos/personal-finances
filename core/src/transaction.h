#pragma once
#include <optional>
#include <string>

#include "core/src/money.h"

namespace Finances::Core {

enum class TransactionType { Income, Expense };

class Transaction {
   public:
    std::string id;
    TransactionType type;
    Money amount;
    std::string category_id;
    long timestamp;
    std::optional<std::string> description;

    Transaction(TransactionType type, Money amount, const std::string& category_id, long timestamp,
                const std::optional<std::string>& description);
};

}  // namespace Finances::Core
