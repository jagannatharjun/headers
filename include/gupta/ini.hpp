#ifndef GUPTA_INI_HPP
#define GUPTA_INI_HPP

#include <cstring>
#include <map>
#include <string>
#include <vector>

namespace gupta {

namespace {
struct stdstringInsenstive
    : std::binary_function<std::string, std::string, bool> {
  // case-independent (ci) compare_less binary function
  struct nocase_compare
      : public std::binary_function<unsigned char, unsigned char, bool> {
    bool operator()(const unsigned char &c1, const unsigned char &c2) const {
      return tolower(c1) < tolower(c2);
    }
  };
  bool operator()(const std::string &s1, const std::string &s2) const {
    return std::lexicographical_compare(s1.begin(), s1.end(), // source range
                                        s2.begin(), s2.end(), // dest range
                                        nocase_compare());    // comparison
  }
};
void trim(std::string &s) {
  while (s.front() == ' ' || s.front() == '\r' || s.front() == '\n')
    s.replace(0, 1, "");
  while (s.back() == ' ' || s.back() == '\r' || s.back() == '\n') {
    s.pop_back();
  }
}
} // namespace

using ParsedIni =
    std::map<std::string,
             std::map<std::string, std::string, stdstringInsenstive>,
             stdstringInsenstive>;

// template<typename Buffer>
using Buffer = std::vector<uint8_t>;
ParsedIni ParseIni(const Buffer &buf) {
  ParsedIni IniMap;
  for (auto b = buf.begin(); b != buf.end();) {
    auto GetChar = [&]() { return (char)*b; };
    auto isEndReached = [&]() { return b == buf.end(); };
    auto AdvanceTillTrue = [&](auto &&f) {
      while (!isEndReached() && f(GetChar()))
        b++;
      return b;
    };
    auto AdvanceTillFound = [&](char c) {
      // result : *b == buf.end() || *b == c
      return AdvanceTillTrue([=](char c1) { return c != c1; });
    };
    auto AdvanceTillNewLine = [&]() {
      AdvanceTillTrue([](char c) { return c != '\r' && c != '\n'; });
      return AdvanceTillTrue([](char c) { return c == '\r' || c == '\n'; });
    };
    std::string Section, Key, Value;
    {
      auto SectionHeaderStart = AdvanceTillFound('[');
      if (isEndReached())
        break;
      SectionHeaderStart++; // previously points to '[' or buf.end(){checked for
                            // above}
      auto SectionHeaderEnd = AdvanceTillFound(']');
      b++; // points to ']'
      if (isEndReached())
        break;
      while (SectionHeaderStart != SectionHeaderEnd)
        Section.push_back((char)*SectionHeaderStart++);
      trim(Section);
    }

    {
      auto KeyLineStart = AdvanceTillNewLine();
      auto KeyLineEnd =
          AdvanceTillTrue([](char c) { return c != '\r' && c != '\n'; });
      while (KeyLineStart != KeyLineEnd && *KeyLineStart != '=')
        Key.push_back((char)*KeyLineStart++);
      if (KeyLineStart == KeyLineEnd)
        continue;
      ++KeyLineStart;
      while (KeyLineStart != KeyLineEnd)
        Value.push_back((char)*KeyLineStart++);
      trim(Value);
      trim(Key);
    }
    IniMap[Section][Key] = Value;
  }
  return IniMap;
}

} // namespace gupta

#endif // INI_H
