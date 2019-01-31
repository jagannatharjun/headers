#include <Gupta/format_io.hpp>
#include <cassert>

int main() {
  std::string f("adfadf");
  assert(gupta::format("%% %", 12) == "% 12");  
  auto f2 = gupta::format("%", f);
  assert(f == f2);
  print() << f2 << endl;
  
  auto limit = 1000;
  while(limit--) {
	//gupta::print("%% %",12);
	//char c[256];
	printf("%% %d",12);
	}
}