#include <memory>

/// Value-based representation of a tree, usable in constexpr evaluations
struct tree_t {
  int value;
  std::unique_ptr<tree_t> left;
  std::unique_ptr<tree_t> right;

  /// Constexpr constructor
  constexpr tree_t(int value_, std::unique_ptr<tree_t> left_,
                   std::unique_ptr<tree_t> right_)
      : value(value_), left(std::move(left_)), right(std::move(right_)) {}
};

/// Generates an arbitrary tree
constexpr std::unique_ptr<tree_t> gen_tree() {
  return std::make_unique<tree_t>(
      3,
      std::make_unique<tree_t>(2, nullptr,
                               std::make_unique<tree_t>(4, nullptr, nullptr)),
      nullptr);
}

/// Accumulates the value from a tree returned by TreeGeneratorFunction.
/// NB: TreeGeneratorFunction is expected to return a
/// std::unique_ptr<tree_t>.
template <auto TreeGeneratorFunction> constexpr auto codegen() {
  // Pointer is null -> Leaf
  if constexpr (constexpr bool Cond = !TreeGeneratorFunction(); Cond) {
    return []() constexpr { return 0; };
  }
  // Pointer isn't null -> Node
  else {
    // Codegen for top value
    constexpr auto EvalSelf = []() { return TreeGeneratorFunction()->value; };
    // Codegen for left child
    constexpr auto EvalLeft =
        codegen<[]() { return std::move(TreeGeneratorFunction()->left); }>();
    // Codegen for right child
    constexpr auto EvalRight =
        codegen<[]() { return std::move(TreeGeneratorFunction()->right); }>();
    return [EvalLeft, EvalRight, EvalSelf]() {
      return EvalSelf() + EvalLeft() + EvalRight();
    };
  }
}

static_assert(codegen<&gen_tree>()() == 3 + 2 + 4);

template <int value, typename left, typename right>
struct expression_template_tree_t {};

struct null_t {};

template <auto TreeGeneratorFunction> constexpr auto to_expression_template() {
  // Pointer is null -> Leaf
  if constexpr (constexpr bool Cond = !TreeGeneratorFunction(); Cond) {
    return null_t{};
  }
  // Pointer isn't null -> Node
  else {
    constexpr auto Value = TreeGeneratorFunction()->value;

    using LeftAsET = decltype(to_expression_template<[]() constexpr {
      return std::move(TreeGeneratorFunction()->left);
    }>());

    using RightAsET = decltype(to_expression_template<[]() constexpr {
      return std::move(TreeGeneratorFunction()->right);
    }>());

    return expression_template_tree_t<Value, LeftAsET, RightAsET>{};
  }
}

/// The tree, transformed into an expression template
using generated_type = decltype(to_expression_template<&gen_tree>());

/// The expected value, matching gen_tree's result
using expected_type = expression_template_tree_t<
    3,
    expression_template_tree_t<2, null_t,
                               expression_template_tree_t<4, null_t, null_t>>,
    null_t>;

/// Asserting types are identical
static_assert(std::is_same_v<generated_type, expected_type>);
