#include <cassert>

#include "core/src/money.h"

int main() {
  Money a(100, Currency::EUR);
  Money b(100, Currency::EUR);
  assert(a == b);

  bool threw = false;
  try {
    Money(-10, Currency::EUR);
  } catch (...) {
    threw = true;
  }
  assert(threw);

  return 0;
}
