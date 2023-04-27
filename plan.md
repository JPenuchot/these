Intro

- Part 1: Codegen through metaprog (new metaprog -> part 2)
  - SotA metaprog (Can't read)
    - C++ (Can't read)
    - Reflection
    - Brigand/mp11
    - Biblio: CTRE, Phoenix, Eigen/Blaze/NT2
    - Can't read
  - Application HPC:
    - GEMV (Low level codegen)
    - Blaze (ET codegen -> Expression level)
  - Conclusion: Limited syntax, ET are so 1990, long compile times

- Part 2: Metaprog, compilation beyond expression templates (contribution)
  - Methodology: ctbench
  - BF: Metacompilaion of longs programs, trivial Turing-complete language
    Many examples, easy to generate code, many backends:
    What codegen strategy is the best?
  - Algebra: Realistic metacompiled language for HPC

- Conclusion
  - Summary
  - Future work:
    - LR(1) parser generation for constexpr parsing
    - JIT compilation (C++ is the new Julia)
    - LaTeX parsing for reproductibility
