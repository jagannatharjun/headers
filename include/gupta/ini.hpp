#ifndef GUPTA_INI_HPP
#define GUPTA_INI_HPP

#pragma once

#include <cstring>
#include <map>
#include <string>
#include <vector>

namespace gupta {

namespace {

void trim(std::string &s) {
  while (s.size() > 0 && (s.front() == ' ' || s.front() == '\r' || s.front() == '\n'))
    s.replace(0, 1, "");
  while (s.size() > 0 && (s.back() == ' ' || s.back() == '\r' || s.back() == '\n')) {
    s.pop_back();
  }
}
} // namespace

struct stdstringInsenstive {
  // case-independent (ci) compare_less binary function
  struct nocase_compare {
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

using ParsedIni =
    std::map<std::string,
             std::map<std::string, std::string, stdstringInsenstive>,
             stdstringInsenstive>;

template <typename Buffer> ParsedIni ParseIni(const Buffer &buf) {
  ParsedIni IniMap;
  std::string lastLine;
  auto b = buf.begin();
  auto isEndReached = [&]() { return b == buf.end(); };
  auto extractChar = [&]() -> char { return isEndReached() ? 0 : (char)*b++; };
  auto extractLine = [&]() {
    std::string line;
    for (char c; (c = extractChar());) {
      if (c == '\r' || c == '\n')
        break;
      line.push_back(c);
    }
    return line;
  };
  while (!isEndReached()) {
    auto sectionLine = lastLine.empty() ? extractLine() : lastLine;
    trim(sectionLine);
    if (sectionLine.empty())
      continue;
    if (sectionLine.front() != '[' || sectionLine.back() != ']')
      continue;

    // remove '[' && ']'
    sectionLine.replace(0, 1, "");
    sectionLine.pop_back();

    while (!isEndReached()) {
      auto keyLine = extractLine();
      trim(keyLine);
      if (keyLine.empty())
        continue;
      if (keyLine.size() > 1 &&
          (keyLine.front() == '[' && keyLine.back() == ']')) {
        lastLine = std::move(keyLine);
        break;
      } else {
        lastLine.clear();
      }

      std::string key, value;
      auto klb = keyLine.begin();
      while (klb != keyLine.end() && *klb != '=')
        key.push_back(*klb++);
      if (*klb != '=')
        continue;
      klb++; // points to '='
      while (klb != keyLine.end())
        value.push_back(*klb++);

      trim(key);
      trim(value);

      IniMap[sectionLine][key] = value;
    }
  }
  return IniMap;
}

} // namespace gupta

#endif // INI_H
