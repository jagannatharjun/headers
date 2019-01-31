#include <cassert>
#include <gupta/ini.hpp>
#include <vector>

int main() {
  {
#define NEWLINE "\r\n"
    const char Ini[] = "" NEWLINE "[Section]" NEWLINE "Key = Value" NEWLINE;
    std::vector<uint8_t> buf((uint8_t *)Ini, (uint8_t *)Ini + sizeof Ini);
    auto i = gupta::ParseIni(buf);
    assert(i["Section"]["Key"] == "Value");
  }
  {
#define NEWLINE "\r\n"
    const char Ini[] =
        "" NEWLINE "[Section]" NEWLINE "Key = Value" NEWLINE "" NEWLINE
        "[Section2]" NEWLINE "Key = Value" NEWLINE "" NEWLINE
        "[Section2]" NEWLINE "K[e]y = V[alue]" NEWLINE;
    std::vector<uint8_t> buf((uint8_t *)Ini, (uint8_t *)Ini + sizeof Ini);
    auto i = gupta::ParseIni(buf);

    assert(i["Section"]["Key"] == "Value");
    assert(i.find("Section2") != i.end());
    assert(i["Section2"]["Key"] == "Value");
    assert(i["Section2"]["K[e]y"] == "V[alue]");
    assert(i["Section2"]["k[e]y"] == "V[alue]");
  }
}
