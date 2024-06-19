/// Code generation implementation for a while block
template <auto const &Ast, size_t InstructionPos = 0>
constexpr auto codegen(flat_while_t) {
  return [](program_state_t &s) {
    while (s.data[s.i]) {
      codegen<Ast, get<flat_while_t>(Ast[InstructionPos])
                       .block_begin>()(s);
    }
  };
}
