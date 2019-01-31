#include <assert.h>
#include <cstring>
#include <gupta/arg.hpp>

bool operator==(const std::string &s, const std::string_view &v) {
  if (s.length() == v.length())
    return !memcmp(s.data(), v.data(), s.length());
  return 0;
}

int main() {
  const char *arg[] = {"exe", "--insert", "hello ad", "---equal=123"};
  auto p =
      gupta::OptParser(sizeof arg / sizeof arg[0], arg, {{"insert", "equal"}});
  assert(p.size() == 2);
  assert(p.find("insert") != p.end());
  assert(p.find("equal") != p.end());
  assert(p["insert"].size() == 1);
  assert(p["insert"] == std::vector<std::string_view>{"hello ad"});
  assert(p["equal"] == std::vector<std::string_view>{"123"});

  try {
    const char *arg[] = {"exe", "--insert", "hello ad", "---equal=123", "a"};
    auto p = gupta::OptParser(sizeof arg / sizeof arg[0], arg,
                              {{"insert", "equal"}});
    assert(0 && "shouldn't reached here");
  } catch (std::invalid_argument &e) {
  } catch (...) {
    assert(0 && "unknown exception");
  }

  system("pause");
}
