#ifndef CLEANUP_HPP
#define CLEANUP_HPP

#include <utility>

namespace gupta {

template <typename Function> class destructor {
public:
  destructor(Function f) : m_function{std::move(f)} {}
  ~destructor() { m_function(); }
  destructor(const destructor &) = delete;
  destructor(destructor &&) = default;
  destructor &operator=(const destructor &) = delete;
  destructor &operator=(destructor &&) = default;

private:
  Function m_function;
};

} // namespace gupta

#endif
