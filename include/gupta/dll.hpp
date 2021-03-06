#ifndef DLL_HPP
#define DLL_HPP

#include <gupta/format_io.hpp>
#include <gupta/windows.hpp>
#include <type_traits>

namespace gupta {

namespace detail {
class testDllFunction;
}

class DynamicLib {
public:
  DynamicLib(const DynamicLib &) = delete;
  DynamicLib &operator=(const DynamicLib &) = delete;
  template <typename CharT>
  DynamicLib(const CharT *dllName) : module_{NULL} {
    if constexpr (std::is_same<CharT, char>::value) {
      module_ = LoadLibraryA(dllName);
    } else if constexpr (std::is_same<CharT, wchar_t>::value) {
      module_ = LoadLibraryW(dllName);
    } else {
      gupta::debug("Not supported CharT: %\n", typeid(CharT).name());
    }
    if (!module_) {
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
  gupta::WinLastError lastError_;
}; // namespace gupta

template <typename> class DllFunction;

template <typename ReturnType, typename... Args>
class DllFunction<ReturnType(Args...)> {
public:
  friend class detail::testDllFunction;
  using function_type = ReturnType(__stdcall *)(Args...);
  DllFunction(DynamicLib &dll, const char *function_name)  {
    if (!(FPtr_ = (function_type)(GetProcAddress(dll.module(), function_name))))
      LastError_ = GetLastError();
  }
  ReturnType operator()(Args &&... args) {
    return FPtr_(std::forward<Args>(args)...);
  }
  auto lastError() { return LastError_; }

private:
  function_type FPtr_;
  gupta::WinLastError LastError_;
};

} // namespace gupta

#endif // DLL_HPP
