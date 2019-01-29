#ifndef GUPTA_ARG_HEADER
#define GUPTA_ARG_HEADER

#include <map>
#include <string>
#include <string_view>
#include <vector>
#include <cctype>

namespace gupta {

static std::map<std::string_view, std::vector<std::string_view>>
OptParser(int argc, const char *argv[],
          const std::vector<std::vector<std::string>> &ValidOptions) {
  std::map<std::string_view, std::vector<std::string_view>> Options;

  int iarg = 1;
  auto remove_dashes = [&](const std::string_view &s) -> std::string_view {
    if (s.length()) {
      auto f = s.find_last_of('-');
      auto e = s.find_first_of('=');
      if (f == s.npos || f > e || *s.begin() != '-')
        throw std::invalid_argument("not a valid option at postion " +
                                 std::to_string(iarg) + ": " + std::string(s));
      if (e == s.npos)
        e = 0;
      else
        e = s.length() - e;
      return std::string_view(s.data() + f + 1, s.length() - f - e - 1);
    }
    return std::string_view{};
  };
  while (iarg < argc) {
    std::string_view arg(argv[iarg]);
    auto opt = remove_dashes(arg);
    /*
     std::cout << arg << "->" << opt << std::endl;*/

    for (const auto &i : ValidOptions) {
      for (const auto &j : i) { /*
         std::cout << "Comparing " << j << " and " << opt << std::endl;*/
        if (j.length() == opt.length()) {
          if (std::equal(opt.begin(), opt.end(), j.begin(),
                         [](const auto c1, const auto c2) {
                           return std::tolower(c1) == std::tolower(c2);
                         })) {
            /*
                        std::cout << opt << " == " << j << std::endl;*/
            goto ValidOptionfound;
          }
        } /*else
          std::cout << opt << " != " << j << std::endl;*/
      }
    }

    throw std::invalid_argument{"invalid option at " + std::to_string(iarg) +
                             ": " + std::string(arg) /*+ ": \"" +
                             std::string(opt) + "\""*/};
  ValidOptionfound: /*
     std::cout << "found " << arg << std::endl;*/
    iarg++;
    auto equalSignPos = arg.find('=');
    auto &opts = Options[opt];
    if (equalSignPos != arg.npos)
      opts.emplace_back(arg.data() + equalSignPos + 1,
                        arg.length() - equalSignPos - 1);
    else
      for (; iarg < argc && argv[iarg][0] != '-'; iarg++)
        opts.emplace_back(argv[iarg]);
  }
  return Options;
}
} // namespace gupta
#endif // ! GUPTA_GARG_HEADER
