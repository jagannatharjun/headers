#include <cassert>
#include <gupta/cleanup.hpp>

void testDestructor() {
  int i = 0;
  {
    SCOPE_EXIT { i++; };
  }
  assert(i == 1);
  try {
    SCOPE_EXIT { i++; };
    throw "a";
  } catch (...) {
  }
  assert(i == 2);

  i = 0;
  {
    SCOPE_EXIT { i++; };
    SCOPE_EXIT { i++; };
    SCOPE_EXIT { i++; };
    SCOPE_EXIT { i++; };
    assert(i == 0);
  }
  assert(i == 4);
}

void testFailure() {
  int i = 0;
  try {
    SCOPE_FAILURE { i = 1; };
    SCOPE_SUCCESS { i = 2; };
    throw "10";
  } catch (...) {
  }
  assert(i == 1);
  i = 0;
  {
    SCOPE_FAILURE { i++; };
    SCOPE_SUCCESS { i++; };
  }
  assert(i == 1);
}

int main() {
  testDestructor();
  testFailure();
  system("pause");
}
