#include <array>
#include <concepts>
#include <memory>
#include <variant>
#include <vector>

#include <kumi/tuple.hpp>

//
// Tree structure with inheritance polymorphism
//

/// Enum type to differentiate nodes
enum node_kind_t {
  constant_v,
  add_v,
};

/// Node base type
struct node_base_t {
  node_kind_t kind;

  constexpr node_base_t(node_kind_t kind_)
      : kind(kind_) {}
  constexpr virtual ~node_base_t() = default;
};

/// Tree pointer type
using tree_ptr_t = std::unique_ptr<node_base_t>;

/// Checks that an object is a tree generator
template <typename T>
concept tree_generator = requires(T fun) {
  { fun() } -> std::same_as<tree_ptr_t>;
};

/// Constant node type
struct constant_t : node_base_t {
  int value;
  constexpr constant_t(int value_)
      : node_base_t(constant_v), value(value_) {}
};

/// Addition node type
struct add_t : node_base_t {
  tree_ptr_t left;
  tree_ptr_t right;
  constexpr add_t(tree_ptr_t left_, tree_ptr_t right_)
      : node_base_t(add_v), left(std::move(left_)),
        right(std::move(right_)) {}
};

/// Generates an arbitrary tree
constexpr tree_ptr_t gen_tree() {
  return std::make_unique<add_t>(
      std::make_unique<add_t>(
          std::make_unique<constant_t>(1),
          std::make_unique<constant_t>(2)),
      std::make_unique<constant_t>(3));
}

//
// PASS-BY-GENERATOR
//

namespace pass_by_generator {

/// Accumulates the value from a tree returned by
/// TreeGenerator. TreeGenerator() is expected to
/// return a std::unique_ptr<tree_t>.
template <tree_generator auto Fun>
constexpr auto codegen() {
  static_assert(Fun() != nullptr, "Ill-formed tree");

  constexpr node_kind_t Kind = Fun()->kind;

  if constexpr (Kind == add_v) {
    // Recursive codegen for left and right children:
    // for each child, a generator function is
    // generated and passed to codegen.
    auto eval_left = codegen<[]() {
      return static_cast<add_t &&>(*Fun()).left;
    }>();
    auto eval_right = codegen<[]() {
      return static_cast<add_t &&>(*Fun()).right;
    }>();

    return
        [=]() { return eval_left() + eval_right(); };
  }

  else if constexpr (Kind == constant_v) {
    constexpr auto Constant =
        static_cast<constant_t const &>(*Fun()).value;
    return [=]() { return Constant; };
  }
}

// Validity check
constexpr auto eval = codegen<gen_tree>();
static_assert(eval() == 1 + 2 + 3);

} // namespace pass_by_generator

//
// EXPRESSION TEMPLATE
//

namespace et {

/// Type representation of a constant
template <int Value> struct et_constant_t {};
/// Type representation of an addition
template <typename Left, typename Right>
struct et_add_t {};

/// Accumulates the value from a tree returned by
/// TreeGenerator. TreeGenerator() is expected to
/// return a std::unique_ptr<tree_t>.
template <tree_generator auto Fun>
constexpr auto to_expression_template() {
  static_assert(Fun() != nullptr, "Ill-formed tree");

  constexpr node_kind_t Kind = Fun()->kind;

  if constexpr (Kind == add_v) {
    // Recursive type generation using the
    // pass-by-generator technique
    using TypeLeft =
        decltype(to_expression_template<[]() {
          return static_cast<add_t &&>(*Fun()).left;
        }>());
    using TypeRight =
        decltype(to_expression_template<[]() {
          return static_cast<add_t &&>(*Fun()).right;
        }>());

    return et_add_t<TypeLeft, TypeRight>{};
  }

  else if constexpr (Kind == constant_v) {
    constexpr auto Value =
        static_cast<constant_t &>(*Fun()).value;
    return et_constant_t<Value>{};
  }
}

template <int Value>
constexpr auto
codegen_impl(et_constant_t<Value> /*unused*/) {
  return []() { return Value; };
}

template <typename ExpressionLeft,
          typename ExpressionRight>
constexpr auto
codegen_impl(et_add_t<ExpressionLeft,
                      ExpressionRight> /*unused*/) {
  auto eval_left = codegen_impl(ExpressionLeft{});
  auto eval_right = codegen_impl(ExpressionRight{});
  return [=]() { return eval_left() + eval_right(); };
}

template <tree_generator auto Fun>
constexpr auto codegen() {
  using ExpressionTemplate =
      decltype(to_expression_template<Fun>());
  return codegen_impl(ExpressionTemplate{});
}

// Validity check
constexpr auto eval = codegen<gen_tree>();
static_assert(eval() == 1 + 2 + 3);

} // namespace et

//
// FLAT
//

namespace flat {

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

/// Defining max std::size_t value as an equivalent to
/// std::nullptr.
constexpr std::size_t null_index = std::size_t(0) - 1;

/// Serializes the current subtree and returns
/// the top node's index to the caller.
constexpr std::size_t
serialize_impl(tree_ptr_t const &top,
               std::vector<flat_node_t> &out) {
  // nullptr translates directly to null_index
  if (top == nullptr) {
    return null_index;
  }

  // Allocating space for the destination node
  std::size_t dst_index = out.size();
  out.emplace_back();

  if (top->kind == add_v) {
    auto const &typed_top =
        static_cast<add_t const &>(*top);

    // Serializing left and right subtrees,
    // initializing the new node
    out[dst_index] = {flat_add_t{
        .left = serialize_impl(typed_top.left, out),
        .right =
            serialize_impl(typed_top.right, out)}};
  }

  if (top->kind == constant_v) {
    auto const &typed_top =
        static_cast<constant_t const &>(*top);
    out[dst_index] = {
        flat_constant_t{.value = typed_top.value}};
  }

  return dst_index;
}

/// Returns a serialized representation of a pointer
/// tree.
constexpr std::vector<flat_node_t>
serialize(tree_ptr_t const &tree) {
  std::vector<flat_node_t> result;
  serialize_impl(tree, result);
  return result;
}

/// Evaluates a tree generator function into a
/// serialized array.
template <tree_generator auto Fun>
constexpr auto serialize_as_array() {
  constexpr std::size_t Size =
      serialize(Fun()).size();
  std::array<flat_node_t, Size> result;
  std::ranges::copy(serialize(Fun()), result.begin());
  return result;
}

/// Generates code from a tree serialized into a
/// static array, with CurrentIndex being the index of
/// the starting node which defaults to 0, ie. the top
/// node of the whole tree.
template <auto const &Tree, std::size_t Index>
constexpr auto codegen_aux() {
  static_assert(Index != null_index,
                "Ill-formed tree (null index)");

  if constexpr (std::holds_alternative<flat_add_t>(
                    Tree[Index])) {
    constexpr auto top =
        std::get<flat_add_t>(Tree[Index]);

    // Recursive code generation for left and right
    // children
    auto eval_left = codegen_aux<Tree, top.left>();
    auto eval_right = codegen_aux<Tree, top.right>();

    // Code generation for current node
    return
        [=]() { return eval_left() + eval_right(); };
  }

  else if constexpr (std::holds_alternative<
                         flat_constant_t>(
                         Tree[Index])) {
    constexpr auto top =
        std::get<flat_constant_t>(Tree[Index]);
    constexpr int Value = top.value;
    return []() { return Value; };
  }
}

/// Stores serialized representations of tree
/// generators' results.
template <tree_generator auto Fun>
static constexpr auto tree_as_array =
    serialize_as_array<Fun>();

/// Takes a tree generator function as non-type
/// template parameter and generates the lambda
/// associated to it.
template <tree_generator auto Fun>
constexpr auto codegen() {
  return codegen_aux<tree_as_array<Fun>, 0>();
}

// Validity check
constexpr auto eval = codegen<gen_tree>();
static_assert(eval() == 1 + 2 + 3);

} // namespace flat

//
// REVERSE POLISH NOTATION
//

namespace rpn {

/// Type for RPN representation of a constant
struct rpn_constant_t {
  int value;
};

/// Type for RPN representation of an addition
struct rpn_add_t {};

/// Type for RPN representation of an arbitrary symbol
using rpn_node_t =
    std::variant<rpn_constant_t, rpn_add_t>;

/// RPN equivalent of gen_tree
constexpr std::vector<rpn_node_t> gen_rpn_tree() {
  return {rpn_constant_t{1}, rpn_constant_t{2},
          rpn_add_t{}, rpn_constant_t{3},
          rpn_add_t{}};
}

/// Returns the result of an RPN generator function as
/// an array.
template <auto Fun> constexpr auto eval_as_array() {
  constexpr std::size_t Size = Fun().size();
  std::array<rpn_node_t, Size> result;
  std::ranges::copy(Fun(), result.begin());
  return result;
}

/// Codegen implementation.
/// Reads tokens one by one, updates the stack
/// consequently by consuming and/or stacking
/// operands. Operands are functions that evaluate
/// parts of the subtree.
template <auto const &RPNTree, std::size_t Index = 0>
constexpr auto
codegen_impl(kumi::product_type auto stack) {
  // The end result is the last top stack operand
  if constexpr (Index == RPNTree.size()) {
    static_assert(stack.size() == 1, "Invalid tree");
    return kumi::back(stack);
  }

  else if constexpr (std::holds_alternative<
                         rpn_constant_t>(
                         RPNTree[Index])) {
    // Append the constant operand
    auto new_operand = [=]() {
      return get<rpn_constant_t>(RPNTree[Index])
          .value;
    };
    return codegen_impl<RPNTree, Index + 1>(
        kumi::push_back(stack, new_operand));
  }

  else if constexpr (std::holds_alternative<
                         rpn_add_t>(RPNTree[Index])) {
    // Fetching 2 top elements and popping them off
    // the stack
    auto left = kumi::get<stack.size() - 1>(stack);
    auto right = kumi::get<stack.size() - 2>(stack);
    auto stack_remainder =
        kumi::pop_back(kumi::pop_back(stack));

    // Append new operand and process next element
    auto new_operand = [=]() {
      return left() + right();
    };
    return codegen_impl<RPNTree, Index + 1>(
        kumi::push_back(stack_remainder,
                        new_operand));
  }
}

/// Stores static array representations of RPN
/// generators' results.
template <auto Fun>
static constexpr auto rpn_as_array =
    eval_as_array<Fun>();

/// Code generation function.
template <auto Fun> constexpr auto codegen() {
  return codegen_impl<rpn_as_array<Fun>, 0>(
      kumi::tuple{});
}

/// Validity check
constexpr auto eval = codegen<gen_rpn_tree>();
static_assert(eval() == 1 + 2 + 3);

} // namespace rpn
