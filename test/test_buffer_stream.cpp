#include <cassert>
#include <gupta/buffer.hpp>
#include <gupta/format_io.hpp>
#include <vector>

template <typename buffer_type> void testBufferStream() {
  gupta::buffer_stream<buffer_type> bs;
  auto testSize = 4, limit = testSize;
  std::vector<int> numbers;
  while (limit--) {
    auto rint = rand() % 1000000;
    bs << rint;
    numbers.push_back(rint);
  }
  assert(bs.getbuffer().size() == sizeof(int) * testSize);
  bs.rewind();
  for (auto ri : numbers) {
    int rint = 0;
    bs >> rint;
    assert(rint == ri);
  }
}

int main() {
  testBufferStream<gupta::buffer>();
  testBufferStream<std::vector<gupta::byte>>();
}
