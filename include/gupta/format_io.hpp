#ifndef GUPTA_FORMAT_HPP
#define GUPTA_FORMAT_HPP

#include <cstdio>
#include <string>
#include <tuple>
#include <type_traits>
#include <stdexcept>
#include <locale>
#include <codecvt>

namespace gupta {

using std::to_string;

static inline std::string to_string(std::string s) { return s; }

static inline std::string to_string(const char *s) { return s; }

template <typename T>
auto to_string(const T& s) -> decltype (s.string()) { return s.string(); }

std::string to_string(const std::wstring& s) {
    return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(s);
}

static inline std::string to_string(const void *ptr) { return std::to_string((uintmax_t)ptr); }


template <std::size_t static_index, typename... ArgumentTypes>
std::string runtime_get(std::size_t runtime_index,
                        std::tuple<ArgumentTypes...> &&rest) {
  if constexpr (static_index == std::size_t(-1)) {
    throw std::invalid_argument(
        "number of format symbols is more than arguments");
  } else {
    if (runtime_index == static_index) {
      if constexpr (std::is_same<decltype(std::get<static_index>(rest)),
                                 std::string>::value) {
        return std::get<static_index>(rest);
      } else {
        return to_string(std::get<static_index>(rest));
      }
    } else
      return to_string(
          runtime_get<static_index - 1>(runtime_index, std::move(rest)));
  }
}

template <typename... Ts>
std::string format(const char *str, const Ts &... args) {
  std::size_t current_index = 0;
  std::string res;
  for (; *str; str++) {
    if (*str != '%') {
      res += *str;
      continue;
    }
    if (*(str + 1) == '%') {
      res += '%';
      ++str;
    } else {
      res += runtime_get<sizeof...(Ts) - 1>(current_index,
                                            std::forward_as_tuple(args...));
      ++current_index;
    }
  }
  return res;
}

namespace detail {

struct _stdout_object {};
struct _stderr_object {
  _stderr_object() = default;
  _stderr_object(const _stderr_object &) = default;
  _stderr_object &operator=(const _stderr_object &) = default;
  ~_stderr_object() { std::fprintf(stderr, "\n"); }
};
struct _endl_type {};

static std::string to_string(_endl_type) { return "\n"; }

} // namespace detail


static detail::_endl_type endl;

namespace printing_shortcuts {

template <typename... Ts>
auto fprint(std::FILE *f, const char *str, Ts &&... args) {
  auto s = std::move(format(str, std::forward<Ts>(args)...));
  return fwrite(s.data(), 1, s.size(), f);
}

template <typename... Ts> inline auto print(const char *str, Ts &&... args) {
  return fprint(stdout, str, std::forward<Ts>(args)...);
}

template <typename... Ts> inline auto debug(const char *str, Ts &&... args) {
  return fprint(stderr, str, std::forward<Ts>(args)...) && fprint(stderr, "\n");
}

template <typename T>
detail::_stdout_object operator<<(const detail::_stdout_object &f,
                                  const T &arg) {
  auto s = to_string(arg);
  fwrite(s.data(), 1, s.size(), stdout);
  return f;
}

template <typename T>
const detail::_stderr_object &operator<<(const detail::_stderr_object &f,
                                         const T &arg) {
  auto s = to_string(arg);
  fwrite(s.data(), 1, s.size(), stderr);
  return f;
}

static inline detail::_stdout_object print() { return {}; }
static inline detail::_stderr_object debug() { return {}; }

} // namespace printing_shortcuts

using namespace printing_shortcuts;

} // namespace gupta

using namespace gupta::printing_shortcuts;
#define SHOW(X)                                                                \
  gupta::debug("%:%:%: % = %", __FILE__, __func__, __LINE__, #X, X);

#endif
