#pragma once
#include <stdexcept>
#include <string>

namespace Finances::Core {

class MoneyError : public std::runtime_error {
   public:
    using std::runtime_error::runtime_error;
};

class MoneyParseError : public MoneyError {
   public:
    explicit MoneyParseError(const std::string& msg) : MoneyError("MoneyParseError: " + msg) {}
};

class MoneyOverflowError : public MoneyError {
   public:
    explicit MoneyOverflowError(const std::string& msg) : MoneyError("MoneyOverflowError: " + msg) {}
};

class MoneyCurrencyMismatchError : public MoneyError {
   public:
    explicit MoneyCurrencyMismatchError(const std::string& msg) : MoneyError("MoneyCurrencyMismatchError: " + msg) {}
};

class MoneyInvalidArgumentError : public MoneyError {
   public:
    explicit MoneyInvalidArgumentError(const std::string& msg) : MoneyError("MoneyInvalidArgumentError: " + msg) {}
};

}  // namespace Finances::Core
