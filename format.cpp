constexpr std::vector<std::vector<int>> get_vector() {
  return {{1, 2, 3}, {4, 5, 6}};
}

// Pas bien:
// constexpr std::vector<int> subvec_0 = get_vector()[0];

// Bien:
constexpr auto get_subvec_0 = []() { return get_vector()[0]; }
