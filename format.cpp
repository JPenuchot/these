#include <vector>

constexpr std::vector<int> foo() {
  return {{1, 2, 3, 4, 5}};
}

// Compiles
if constexpr (constexpr bool val =
                  foo().size() == 5;
              val)
  ;

// Does not compile
if constexpr (foo().size() == 5)
  ;
