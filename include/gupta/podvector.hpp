#ifndef GUPTA_PODVECTOR_HPP
#define GUPTA_PODVECTOR_HPP

#include <cstdlib>
#include <new>
#include <type_traits>
#include <utility>

namespace gupta {
namespace detail {
namespace podvector {
	
template <typename T> T * malloc(size_t elm_count) {
  auto p = std::malloc(elm_count * sizeof(T));
  if (!p)
	  throw std::bad_alloc{};
  return static_cast<T *>(p);
}

template <typename T> T * realloc(T *&old_block, size_t elm_count) {
	auto p = std::realloc(static_cast<void *>(old_block), elm_count * sizeof(T));
	if (!p) {
		std::free(old_block);
		old_block = nullptr;
		throw std::bad_alloc{};
	}
	return static_cast<T *>(p);
}

} // namespace podvector
} // namespace detail

template <typename PodType,
          typename = std::enable_if_t<std::is_pod<PodType>::value>>
class podvector {
public:
  using value_type = PodType;
  using size_type = size_t;
  using pointer = value_type *;
  using iterator = pointer;
  using const_iterator = const value_type *;

  ~podvector() {
    std::free(m_memory);
  }

  podvector(size_type initial_size = 0) {
    alloc(max(initial_size,1));
    m_size = initial_size;
  }

  podvector(size_type initial_size, const value_type &value)
      : podvector(initial_size) {
    for (auto &v : *this)
      v = value;
  }

  podvector(const podvector &other) : podvector(other.m_size) {
    copy(other.begin(),other.end());
  }

  podvector(podvector &&other)
      : m_memory{other.m_memory},
        m_capacity{other.m_capacity}, m_size{other.m_size} {
    other.alloc(1);
  }

  podvector &operator=(const podvector &rhs) {
    if (this != &rhs) {
      resize(rhs.m_size);
	  copy(rhs.begin(),rhs.end());
    }
    return *this;
  }

  podvector &operator=(podvector &&rhs) {
    if (this != &rhs) {
      std::swap(m_memory,rhs.m_memory);
	  std::swap(m_capacity,rhs.m_capacity);
	  std::swap(m_size,rhs.m_size);
    }
    return *this;
  }

  void resize(size_type new_size) {
    if (new_size > m_size)
      change_capacity(new_size);
    m_size = new_size;
  }

  void reserve(size_type new_capacity) {
    if (m_capacity < new_capacity) {
      auto n = max(m_capacity,1);
	  while(n < new_capacity)
		  n *= 2;
	  change_capacity(n);
	}
  }

  void push_back(const value_type &new_elm) {
    if (m_size + 1 > m_capacity)
      change_capacity(max(m_capacity * 2, size_type{1}));
    m_memory[m_size++] = new_elm;
  }

  void pop_back() { m_size--; }
  
  template<typename... Args>
  void emplace_back(Args&&... args) {
	if (m_size + 1 > m_capacity)
      change_capacity(max(m_capacity * 2, size_type{1}));
    new (m_memory + m_size++) value_type{args...};
  }

  auto size() const { return m_size; }
  auto capacity() const { return m_capacity; }
  auto& front() { return m_memory[0]; }
  const auto& front() const { return m_memory[0]; }
  auto& back() { return m_memory[m_size - 1]; }
  const auto& back() const { return m_memory[m_size - 1]; }
  pointer data() { return m_memory; }
  const pointer data() const { return m_memory; }
  iterator begin() { return m_memory; }
  const_iterator begin() const { return m_memory; }
  iterator end() { return m_memory + m_size; }
  const_iterator end() const { return m_memory + m_size; }
  value_type &operator[](size_type pos) { return m_memory[pos]; }
  const value_type &operator[](size_type pos) const { return m_memory[pos]; }

private:
  pointer m_memory;
  size_type m_size, m_capacity;
  void alloc(size_type capacity) {
    m_capacity = capacity;
    m_size = 0;
    m_memory =
        static_cast<pointer>(detail::podvector::malloc<value_type>(capacity));
  }
  void change_capacity(size_type new_capacity) {
    m_capacity = new_capacity;
    m_memory = detail::podvector::realloc<value_type>(m_memory, new_capacity);
  }
  void copy(const_iterator first,const_iterator last) {
	for(auto r = begin(); first != last; ++r,++first) {
		*r = *first;
	}
  }
  static constexpr size_type max(size_type a,size_type b) {
	  return a>b?a:b;
  }
};  

} // namespace gupta

#endif