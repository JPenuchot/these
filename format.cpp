/// Serialized representation of a constant
struct flat_constant_t {
  int value;
};

/// Serialized representation of an addition
struct flat_add_t {
  std::size_t left;
  std::size_t right;
};

/// Serialized representation of a node
using flat_node_t =
    std::variant<flat_add_t, flat_constant_t>;

/// null_index replaces std::nullptr.
constexpr std::size_t null_index = std::size_t(0) - 1;
