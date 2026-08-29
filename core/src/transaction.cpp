#include "core/src/transaction.h"

#include <chrono>
#include <cstddef>
#include <random>
#include <string>
#include <string_view>
#include <utility>

#include "core/src/money.h"

namespace Finances::Core {
namespace {
std::string generate_id() {
    thread_local std::mt19937 gen(std::random_device{}());
    constexpr int kMaxHexValue = 15;
    std::uniform_int_distribution dis(0, kMaxHexValue);

    constexpr std::size_t kUuidSize = 32;
    constexpr std::string_view hex = "0123456789abcdef";
    std::string uuid(kUuidSize, '0');

    for (std::size_t i = 0; i < kUuidSize; i++) {
        uuid[i] = hex.at(static_cast<std::size_t>(dis(gen)));
    }

    return uuid;
}
}  // namespace

Transaction::Transaction(Type type, Money amount, std::string category_id, std::chrono::year_month_day date,
                         std::string description)
    : id_(generate_id()),
      type_(type),
      amount_(std::move(amount)),
      category_id_(std::move(category_id)),
      date_(date),
      description_(std::move(description)) {}

std::string_view Transaction::getId() const noexcept { return id_; }

std::string_view Transaction::getCategoryId() const noexcept { return category_id_; }

Transaction::Type Transaction::getType() const noexcept { return type_; }

Money Transaction::getAmount() const { return amount_; }

std::chrono::year_month_day Transaction::getDate() const noexcept { return date_; }

std::string_view Transaction::getDescription() const noexcept { return description_; }

}  // namespace Finances::Core
