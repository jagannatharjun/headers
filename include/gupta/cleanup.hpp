#ifndef CLEANUP_HPP
#define CLEANUP_HPP

#include <exception>
#include <utility>

namespace gupta {

template <typename Function> class destructor {
public:
  destructor(Function f) : m_function{std::move(f)} {}
  ~destructor() { m_function(); }
  destructor(destructor &&) noexcept = default;
  destructor &operator=(destructor &&) noexcept = default;
  destructor &operator=(const destructor &) = delete;
  destructor(const destructor &) = delete;

private:
  Function m_function;
};

template <typename Function> class OnFailure {
public:
  OnFailure(Function f)
      : F{std::move(f)}, UncaughtExceptionsCount_{std::uncaught_exceptions()} {}
  OnFailure(const OnFailure &) = delete;
  OnFailure(OnFailure &&) = default;
  OnFailure &operator=(const OnFailure &) = delete;
  OnFailure &operator=(OnFailure &&) = default;
  ~OnFailure() {
    if (UncaughtExceptionsCount_ != std::uncaught_exceptions()) {
      F();
    }
  }

private:
  Function F;
  int UncaughtExceptionsCount_;
};

template <typename Function> class OnSuccess {
public:
  OnSuccess(Function f)
      : F{std::move(f)}, UncaughtExceptionsCount_{std::uncaught_exceptions()} {}
  OnSuccess(const OnSuccess &) = delete;
  OnSuccess(OnSuccess &&) = default;
  OnSuccess &operator=(const OnSuccess &) = delete;
  OnSuccess &operator=(OnSuccess &&) = default;
  ~OnSuccess() {
    if (UncaughtExceptionsCount_ == std::uncaught_exceptions()) {
      F();
    }
  }

private:
  Function F;
  int UncaughtExceptionsCount_;
};

namespace detail {
class CleanUpDestructorDummy {};
class CleanUpFailureDummy {};
class CleanUpSuccessDummy {};
} // namespace detail

} // namespace gupta

template <typename Function>
gupta::destructor<Function> operator+(gupta::detail::CleanUpDestructorDummy,
                                      Function f) {
  return gupta::destructor<Function>(f);
}

template <typename Function>
gupta::OnFailure<Function> operator+(gupta::detail::CleanUpFailureDummy,
                                     Function f) {
  return gupta::OnFailure<Function>(f);
}

template <typename Function>
gupta::OnSuccess<Function> operator+(gupta::detail::CleanUpSuccessDummy,
                                     Function f) {
  return gupta::OnSuccess<Function>(f);
}

#define _JOIN_IMPL_(X, Y) X##Y
#define _JOIN_(X, Y) _JOIN_IMPL_(X, Y)
#ifdef __COUNTER__
#define _UNIQUE_VAR_(X) _JOIN_(X, __COUNTER__)
#else
#define _UNIQUE_VAR_(X) JOIN(X, __LINE__)
#endif

#define SCOPE_EXIT                                                             \
  auto _UNIQUE_VAR_(SCOPE_EXIT) =                                              \
      gupta::detail::CleanUpDestructorDummy{} + [&]()

#define SCOPE_FAILURE                                                          \
  auto _UNIQUE_VAR_(SCOPE_FAILURE) =                                           \
      gupta::detail::CleanUpFailureDummy{} + [&]()

#define SCOPE_SUCCESS                                                          \
  auto _UNIQUE_VAR_(SCOPE_SUCCESS) =                                           \
      gupta::detail::CleanUpSuccessDummy{} + [&]()

#endif
