#include "core/src/transaction.h"
#include <random>

static std::string generate_id()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);

    const char *hex = "0123456789abcdef";
    std::string uuid(32, '0');

    for (int i = 0; i < 32; i++)
    {
        uuid[i] = hex[dis(gen)];
    }

    return uuid;
}

Transaction::Transaction(
    TransactionType type,
    Money amount,
    std::string category_id,
    long timestamp,
    std::optional<std::string> description)
    : id(generate_id()),
      type(type),
      amount(amount),
      category_id(category_id),
      timestamp(timestamp),
      description(description)
{
}
