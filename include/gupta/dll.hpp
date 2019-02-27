#ifndef DLL_HPP
#define DLL_HPP

#include <gupta/format_io.hpp>
#include <memory>
#include <type_traits>
#include <windows.h>

namespace gupta {

namespace detail {
class testDllFunction;
}

class DynamicLib {
public:
  DynamicLib(const DynamicLib &) = delete;
  DynamicLib &operator=(const DynamicLib &) = delete;
  template <typename CharT>
  DynamicLib(const CharT *dllName) : module_{NULL}, lastError_{0} {
    if constexpr (std::is_same<CharT, char>::value) {
      module_ = LoadLibraryA(dllName);
    } else if constexpr (std::is_same<CharT, wchar_t>::value) {
      module_ = LoadLibrary(dllName);
    } else {
      gupta::debug("Not supported CharT: %\n", typeid(CharT).name());
    }
    if (1 || !module_) {
      lastError_ = ::GetLastError();
    }
  }
  ~DynamicLib() {
    if (module_)
      FreeLibrary(module_);
  }
  auto module() { return module_; }
  auto lastError() { return lastError_; }
  bool isLoaded() { return module_ != NULL; }

private:
  HMODULE module_;
  int lastError_;
}; // namespace gupta

template <typename> class DllFunction;

template <typename ReturnType, typename... Args>
class DllFunction<ReturnType(Args...)> {
public:
  friend class detail::testDllFunction;
  using function_type = ReturnType(__stdcall *)(Args...);
  DllFunction(DynamicLib &dll, const char *function_name) : LastError_{0} {
    if (!(FPtr_ = (function_type)(GetProcAddress(dll.module(), function_name))))
      LastError_ = GetLastError();
  }
  ReturnType operator()(Args &&... args) {
    return FPtr_(std::forward<Args>(args)...);
  }
  int lastError() { return LastError_; }

private:
  function_type FPtr_;
  int LastError_;
};

} // namespace gupta

#endif // DLL_HPP
