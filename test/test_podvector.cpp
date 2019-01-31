#include <Gupta/podvector.hpp>
#include <cassert>
#include <vector>

// template<typename T> using vector = std::vector<T>;
template <typename T> using vector = gupta::podvector<T>;

struct test {
  int a, b;
  test *n;
};

int operator==(const test &lhs, const test &other) {
  return lhs.a == other.a && lhs.b == other.b && lhs.n == other.n;
}

int main() {
  vector<int> v;
  auto is = 10, value = 10;
  vector<int> v2(is, value);
  for (int i = 0; i < is; i++)
    assert(v2[i] == value);
  v = std::move(v2);
  for (int i = 0; i < is; i++)
    assert(v[i] == value);
  v2 = v;
  for (int i = 0; i < is; i++)
    assert(v2[i] == value);
  v2.reserve(20);
  assert(v2.capacity() == 20);
  v2.emplace_back(0);
  vector<test> tv;
  is = 100000;
  for (auto i = 0; i < is; i++) {
    tv.emplace_back(1, 1, nullptr);
    assert(tv.back().a == 1);
    assert(tv.back().b == 1);
    assert(tv.back().n == nullptr);
  }
  assert(tv.size() == is);
  {
    auto tv2 = tv;
    assert(tv2.size() == tv.size());
    for (size_t i = 0; i < tv.size(); i++) {
      assert(tv[i].a == tv2[i].a);
      assert(tv[i].b == tv2[i].b);
      assert(tv[i].n == tv2[i].n);
      tv[i].a = 546;
      assert(tv[i].a != tv2[i].a);
    }
  }
}