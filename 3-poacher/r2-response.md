# R2 response

The reviews highlight a lack of introduction on the technical background
of the techniques presented and evaluated in this paper, notably:

- Non-type template parameters (NTTP) have not been properly defined.
  This was expressed explicitely by one of the reviewers,
  and another reviewer found the use of `auto`
  as a template argument unclear in listing 1.

- What expression templates are is still unclear for non experts,

- The Blaze library was not properly introduced.

We will first list the revisions that will be made to the paper,
starting with the substantial ones and finishing with the small ones.
After that we will answer the reviewers' questions in order,
and comment on issues raised in the reviews.

## Change list

### Substantial revisions

1. Technical background introduction

To address the lack of technical background introduction,
section 3 will be revised into a broader technical introduction on
- `constexpr` functions,
- template metaprogramming (including clarifications about type and
  non-type template parameters),
- and expression templates.

2. Blaze introduction

Blaze was not properly introduced. An larger introduction will be added
with examples in section 5, which we hope will give a better context
for this paper and the math expression parser.

### Small revisions

- Line counts for backend implementations will be added in 4.4.

- Slow compilation times for the first two backends
  will be foreshadowed at the end of 4.3.2
  to make the transition to 4.3.3 more natural.

- Line count for the AST definition will be added in 4.1.

- Figure 1 will be re-generated with a better resolution.
  The graphs were are generated using a purpose-built tool

- The math expression parser will be revised to support a small subset
  of LaTeX math expressions, as reviewers pointed out the similarity between
  the syntax of the proposed language and C++ iteself.

- Many sentences were pointed out for being difficult to read.
  These will be rephrased, and typos will be corrected.

## Response to questions and comments

### Review #758A

> Section 6.3 appears to describe three setups - with Blaze, without Blaze, and
Blaze alone - but Figure 4 only displays two results. The conclusion of section
6.3 then asserts that Blaze accounts for most of the compiler execution time,
again without sufficient context or explanation. This makes the section
extremely challenging to comprehend for those unfamiliar with Blaze.

This is an error from our part. The paper initially had an additional case
where the formulas would be parsed, and the generated functions would be called
on regular scalar values instead of Blaze vector containers.
This would have been very confusing because the programs generated without Blaze
would have been completely different.

### Review #758B

> Q1: I'm unclear on how much the compile-time trouble that motivates an
intermediate string representation is specific to separating parsing
from compilation. That separation is standard and good practice, but
could the problem also have been avoided by going more directly from
the input string to the generated code?

This could have been avoided, but not without
falling back to template metaprogramming for parsing because
`constexpr` functions cannot emit code by themselves.

Another solution is to parse code directly to a serialized representation
and use the parse result directly as a non-type template parameter.
This is not done in section 4 as the goal was to evaluate techniques to
exploit a rich structure for abstract syntax trees.
However, the math expression compiler in section 5 uses that technique
as the Shunting Yard algorithm returns a serialized representation of formulas.

> Q2: For some applications, it would be useful to have an embedded
language syntax with static parsing, but where execution is just as
well performed by an interpreter. For those cases, the code to be
generated can merely reconstruct the AST and invoke an interpreter.
Would that approach have a reasonable compile time via templates after
parsing, or would even just compiling to an AST construction run into
the same problem with quadratic compile times?

Yes, it is possible. If we take the 3rd backend for the Brainfuck parser,
the serialized representation can be stored in a `constinit`-qualified
variable to be interpreted at runtime.
`constinit` is a C++20 specifier that is just like `static`
with the added guarantee that the qualified variable has a constant
initialization time.
This guarantee requires the variable to be initialized at compile time.

> The paper's second example moves in a somewhat more realistic
direction compared to BF, but it's unsatisfying that the embedded
language here is practically a subset of C++. Something realistic but
also radically different would be more compelling, such as a little
logic-programming language along the lines of Prolog or Datalog.

This is true. Our findings come from the will to experiment with compile time
LaTeX math expression parsing for high performance code generation using Blaze.
Parsing LaTeX requires much more implementation effort than what is found in the paper
but parsing a subset of it would only require minor changes to the existing
math expression parser.

## Review #758C

> Code size.  It seems to me that you could compare the code size of the three
strategies: how much code does the user need to write for the three strategies,
and related to the size of the concrete/abstract grammars?

Great point.

Here's the number of lines for the parser, the AST,
and all three backends using `sloccount`:

- AST: 119
- Parser: 45
- Flat: 154
- Generator: 76
- ET: 77

The flat backend has a higher line count due to the serialization process
and the definition of a serialized representation.
Every backend has more or less boilerplate code depending
on the intermediate representation, and the functions that generate the code
are pretty much of the same size.
As stated in the change list, these figures will be added to the paper.

> Fig 3. Even with the linear approach, the processing time seems quite slow:
49.68 seconds to compile a 11000 token DSL program. Would it be quicker by using
an embedded DSL that relied on C++ syntax, and didn't require DSL parsing?

This is not the scope of this paper, DSEL implementations using C++ syntax
through operator overloading have been around for a decade.
Blaze is a DSEL based on C++ syntax and it takes about 10 seconds to
compile an expression with less than 50 tokens in total (cf. figure 4).
This is of course a very unfair comparison, but this is the level of performance
that is usually expected from current DSELs based on C++ syntax.

> The paper is very oriented towards C++ details, rather than principle
approaches. For this reason, I don't think the paper is a good fit for OOPSLA.
I think it would fit better at a C++ workshop.

We think that the work presented here fits into the "practical investigations
of programming languages" topic as mentioned in the CFP.
