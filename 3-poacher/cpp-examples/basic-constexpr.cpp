#include <algorithm>
#include <array>
#include <vector>

// Constexpr function generate returns
// a non-literal value
constexpr std::vector<int> generate() {
  return {1, 2, 3, 4, 5};
}

// Function template foo takes a polymorphic NTTP
template <auto bar> constexpr int foo() { return 1; }

// generate's return value cannot be stored in a
// constexpr variable or used as a NTTP, but it can be
// used to produce other literal

// constexpr auto a = generate();         // ERROR
constexpr auto b = generate().size(); // OK
// constexpr auto c = foo<generate()>();  // ERROR
constexpr auto d = foo<&generate>(); // OK

constexpr auto generate_as_array() {
  constexpr std::size_t array_size =
      generate().size();
  std::array<int, array_size> res{};
  std::ranges::copy(generate(), res.begin());
  return res;
}

constexpr auto e = generate_as_array();        // OK
constexpr auto f = foo<generate_as_array()>(); // OK
