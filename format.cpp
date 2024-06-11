template <typename = void> constexpr auto foo() {
  return
      []<std::size_t... Is>(std::index_sequence<Is...>) {
        return sum(ct_uint_t<Is>{}...);
      }(std::make_index_sequence<BENCHMARK_SIZE>{});
}

constexpr std::size_t result = decltype(foo())::value;
