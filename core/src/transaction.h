#pragma once
#include <chrono>
#include <string>
#include <string_view>

#include "core/src/money.h"

namespace Finances::Core {
class Transaction {
   public:
    enum class Type { Income, Expense };

    Transaction(Type type, Money amount, std::string category_id, std::chrono::year_month_day date,
                std::string description);

    [[nodiscard]] std::string_view getId() const noexcept;
    [[nodiscard]] std::string_view getCategoryId() const noexcept;
    [[nodiscard]] Type getType() const noexcept;
    [[nodiscard]] Money getAmount() const;
    [[nodiscard]] std::chrono::year_month_day getDate() const noexcept;
    [[nodiscard]] std::string_view getDescription() const noexcept;

   private:
    std::string id_;
    Type type_;
    Money amount_;
    std::string category_id_;
    std::chrono::year_month_day date_;
    std::string description_;
};

}  // namespace Finances::Core
