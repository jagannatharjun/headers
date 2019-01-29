#ifndef GUPTA_FORMAT_HPP
#define GUPTA_FORMAT_HPP

#include <string>
#include <tuple>
#include <cstdio>
#include <type_traits>

namespace gupta {

using std::to_string;

static std::string to_string(std::string s) {
	return s;
}

static std::string to_string(const char * s) {
	return s;
}

template <std::size_t static_index, typename... ArgumentTypes>
std::string runtime_get(std::size_t runtime_index,
                        std::tuple<ArgumentTypes...> &&rest) {
  if constexpr (static_index == std::size_t(-1)) {
    throw std::invalid_argument(
        "number of format symbols is more than arguments");
  } else {
    if (runtime_index == static_index) {
		if constexpr (std::is_same<decltype(std::get<static_index>(rest)),std::string>::value) {
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

class _stdout_object {};
class _stderr_object {};
class _endl_type {};

static std::string to_string(_endl_type) { return "\n"; }

} // namespace detail

namespace printing_shortcuts {

static detail::_endl_type endl;

template <typename... Ts>
auto fprint(std::FILE *f, const char *str, Ts &&... args) {
  auto s = std::move(format(str, std::forward<Ts>(args)...));
  return fwrite(s.data(), 1, s.size(), f);
}

template <typename... Ts> inline auto print(const char *str, Ts &&... args) {
  return fprint(stdout, str, std::forward<Ts>(args)...);
}

template <typename... Ts> inline auto debug(const char *str, Ts &&... args) {
  return fprint(stderr, str, std::forward<Ts>(args)...);
}

template <typename T>
detail::_stdout_object operator<<(detail::_stdout_object f, const T &arg) {
  auto s = to_string(arg);
  fwrite(s.data(), 1, s.size(), stdout);
  return f;
}

template <typename T>
detail::_stderr_object operator<<(detail::_stderr_object f, const T &arg) {
  auto s = to_string(arg);
  fwrite(s.data(), 1, s.size(), stderr);
  return f;
}

static detail::_stdout_object print() { return {}; }
static detail::_stderr_object debug() { return {}; }

} // namespace printing_shortcuts

using namespace printing_shortcuts;

} // namespace gupta

using namespace gupta::printing_shortcuts;

#endif
