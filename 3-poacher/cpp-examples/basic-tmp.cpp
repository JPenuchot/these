#include <array>

// Template type accepting an integer
// as a non-type template parameter
template <unsigned N> struct fibonacci_t;

// General definition
template <unsigned N> struct fibonacci_t {
  static constexpr unsigned value =
      fibonacci_t<N - 2>::value +
      fibonacci_t<N - 1>::value;
};

// Specialization for cases 0 and 1
template <> struct fibonacci_t<0> {
  static constexpr unsigned value = 0;
};

template <> struct fibonacci_t<1> {
  static constexpr unsigned value = 1;
};

std::array<int, fibonacci_t<5>::value> some_array;
