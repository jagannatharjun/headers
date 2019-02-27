#include <cassert>
#include <gupta/dll.hpp>
#include <iostream>

namespace gupta::detail {
class testDllFunction {
public:
  void test() {
    gupta::DynamicLib dll("helloDll.dll");
    SHOW(dll.lastError());
    gupta::DllFunction<int(int)> f(dll, "returnArg");
    SHOW(f.lastError());
    static_assert(std::is_same_v<decltype(f.FPtr_), int(__stdcall *)(int)>);
    assert(f.FPtr_(5) == 5);
    assert(f(6) == 6);
    puts("Done");
  }
};
} // namespace gupta::detail

int main() {
  gupta::detail::testDllFunction dllFunc;
  dllFunc.test();
}
