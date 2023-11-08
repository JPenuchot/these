# OOPSLA 2023 Paper #758 Reviews and Comments

Paper #758 Arbitrary Syntax Languages Embedding in C++23

## Review #758A

- Overall merit: C. Weak reject
- Reviewer expertise: Z. Outsider

### Paper summary

This paper illustrates the application of constexpr computations in the design
and implementation of Domain-Specific Embedded Languages (DSELs) with arbitrary
syntaxes.
The author provides two concrete examples: a Brainfuck (BF) language
and a mathematical expression.
The work is enriched with detailed code illustrations and explores varying
design strategies through experimentation.

### Assessment

I'm inclined not to accept this paper in its current form,
though I wouldn't be strongly opposed to acceptance.

Pros:

- The novelty of this work is commendable, providing a significant enough
delta from existing studies.

- The Brainfuck (BF) experiment appears robust, with a thorough exploration
and complexity analysis of various backends.

Cons:

As a non-expert on meta-programming, I find several issues related to the
presentation and clarity of the paper. Certain sections are difficult to
understand, and technical details are often vaguely outlined.

While the core of the work seems solid and potentially valuable to the
community, the paper needs substantial revision before it's publication-ready.

Firstly, a more comprehensive background section would greatly benefit readers,
particularly in explaining specific C++ terminology and techniques such as
'Expression Templates'. As these techniques are mentioned repeatedly and form
part of the backend implementation and results, it's crucial for readers to
grasp their significance. I had to search on the topic in order to understand
brefily about what is going on. Providing a more thorough background would make
the paper self-contained and reader-friendly.

Secondly, some technical specifics are not adequately clarified. For instance,
in section 4.4, the authors state that the compile time of the expression
template approach incurs a higher cost than the pass-by-generator method,
but fail to explain why.

While this might be common knowledge within the C++ meta-programming community,
it's far from clear for those less familiar with the field.

Moreover, the implementation and experiment of the mathematical formula
metacompiler are heavily dependent on the Blaze library. However, no context or
explanation about Blaze and its role in the implementation is provided.

Section 6.3 appears to describe three setups - with Blaze, without Blaze, and
Blaze alone - but Figure 4 only displays two results. The conclusion of section
6.3 then asserts that Blaze accounts for most of the compiler execution time,
again without sufficient context or explanation. This makes the section
extremely challenging to comprehend for those unfamiliar with Blaze.

In summary, while the paper seems to have potential, it requires revisions for
clarity and completeness, particularly for readers outside the C++
meta-programming community. The paper is only 16 pages long and has more than
enough room to address these issues.

### Comments for authors

Line 14: Please expand "Domain Specific Embedded Language" before using the
acronym DSEL for the first time.

Line 38: This sentence could benefit from clarity and precision improvements.

Section 2.2: It would enhance the section to provide a few DSL examples,
ideally showcasing their C++ syntax.

Section 4.3: This section needs revision for better clarity. The non-functional
backends are extensively discussed, yet it's stated immediately afterward that
these attempts will not be detailed further. This structure is confusing.

Line 148: three backends here?

Line 168: The correlation between supporting nested loops and the decision to
use a polymorphic tree for representation isn't clear. A linear data structure
can also express nested loops.

Line 198: Could you simplify by stating that ast_token_t stores all tokens
except loop begins and loop ends?

Line 232: The lengthy sentence detailing various implementations would be
easier to read if broken down into a list.

Line 524: Please revise "ie." to "i.e."

Line 635: This sentence is difficult to understand on first read. To clarify,
does a wide AST simply generate a sequence of non-nested while loops?

Line 644: The phrasing here is awkward. Consider saying that et has the highest
cost, pass_by_generator is second, and flat provides the best performance.

Line 684: The sentence beginning with "Finally, ..." is repetitive.

Fig1: The experiment figures seem out of place. They have inconsistent font
compare to the text and low resolution. Those are standard plost, these issues
could be easily fixed using a native LaTeX library.

Line 698: The choice to experiment with Blaze is unclear to non-specialists

Line 699: The sentence starting with "In this subsection..." is long
and confusing.

6.3: As Blaze is heavily utilized in this section, please provide a brief
description of the library and its technical background.

## Review #758B

- Overall merit: C. Weak reject
- Reviewer expertise: X. Expert

### Paper summary

This paper pushes the limits of `constexpr` to embed an arbitrary
language within a string constant and, through a combination of
compile-time computation and templates, compile the program within the
string to C++. The authors demonstrate the technique with two toy
languages and explore some trade-offs in ease of implementation versus
compile time.

### Assessment

The authors have done a service for the C++ community in figuring out
how to make `constexpr` perform an especially useful kind of
compile-time computation: to compile an alternate or extended
language. For me, the takeaway was that it doesn't work so well, yet;
to get scalable performance, the authors had to parse a string
constant into another intermediate string constant that serializes the
parse. A more natural approach using templates, in contrast, triggers
quadratic compile times. But beyond this result that seems specific to
C++'s unusual design choices, it's difficult to see a more general
lesson.

### Comments for authors

The presentation is generally clear, although there are some
challenges for readers who are only somewhat familiar with modern C++.
In the first line of List 1 (line 111 overall), I'm unclear on the
role of `auto` in place of a template argument. I think that `auto`
without a name may mean that the template argument is ignored, and
maybe the point of listing 1 is to say something about how
constantness is propagated by passing `generate()` as an argument,
even if the argument is ignored.

In a first reading, I was confused about the point of section 4.3.3.
The progress from 4.3.1 to 4.3.2 makes sense: going from a hand-rolled
generator to using something more general. But 4.3.3 doesn't continue
that progression, and instead jumps to something much lower-level.
Section 4.4 explains, but it would be good to foreshadow that the more
obvious preceding two techniques turn out to compile slowly.

The paper's second example moves in a somewhat more realistic
direction compared to BF, but it's unsatisfying that the embedded
language here is practically a subset of C++. Something realistic but
also radically different would be more compelling, such as a little
logic-programming language along the lines of Prolog or Datalog.

Typos and small suggestions:

Line 202: "constains" should be "contains"

Line 461: Surely should be "Dijkstra", but this citation and line 463
need to have entries in the bibliography.

Figures 2 and 4 would be easier to interpret if they consistently set
the y-axis at 0.

### Questions for author response

Q1: I'm unclear on how much the compile-time trouble that motivates an
intermediate string representation is specific to separating parsing
from compilation. That separation is standard and good practice, but
could the problem also have been avoided by going more directly from
the input string to the generated code?

Q2: For some applications, it would be useful to have an embedded
language syntax with static parsing, but where execution is just as
well performed by an interpreter. For those cases, the code to be
generated can merely reconstruct the AST and invoke an interpreter.
Would that approach have a reasonable compile time via templates after
parsing, or would even just compiling to an AST construction run into
the same problem with quadratic compile times?

## Review #758C

- Overall merit: D. Reject
- Reviewer expertise: Y. Knowledgeable

### Paper summary

The paper describes an approach to implementing DSLs in C++.

The idea is to use C++ metaprogramming in order to do parsing and code
generation for the DSL at C++ compile-time, and to compare different strategies
for this.
In particular, newer features in C++, like compile-time virtuals and dynamic
allocation are investigated.
Two example DSLs are created: One for the tiny Brainfuck language and one larger
for linear algebra expressions.
A small empirical study is performed, showing that one of the approaches is
linear in the size of the DSL program, and the two others are quadratic.

### Assessment

I found the presented strategies very difficult to follow. There are concrete
examples, which is good, but conceptual descriptions are missing. Furthermore,
the concrete examples are very difficult to understand without detailed
knowledge of C++.

The paper is very oriented towards C++ details, rather than principle
approaches. For this reason, I don't think the paper is a good fit for OOPSLA.
I think it would fit better at a C++ workshop.

### Comments for authors

- 94 What is a non-type template parameter?

- 101-102 You say that constexpr variables are different from non-constexpr
variables. I don't understand what you mean.

- 104-106 You say that "constexpr allocated memory is not transient" and that it
cannot be stored in constexpr variables. I don't understand. If you cannot store
it, it sounds very transient to me.

- 107-108 "The use of any value ..." I did not understand what you mean here.

- 110-117 Listing 1. I did not understand the examples.

- Section 4. I think giving an example of the abstract syntax would help.
Essentially, you have an abstract syntax that you encode using C++. Making that
clearer would help a reader to understand the example. The whole section is very
implementation-oriented. It is difficult to distill the essential conceptual
approach here.

- 246-248 You write "Despite values containing constepxr ..." I could not parse
this sentence.

- 252 "First we define a binary tree data structure". Please explain what this
tree is used for. You already have an AST, right? What is the role of this new
tree?

- 255-275 (Listing 4). It seems like the reader has to be familiar with the C++
standard library to understand this. For example, what do the constants 3, 2,
and 4 mean?

- Section 4. I understand that you investigate three different strategies, but I
didn't understand how they work. Maybe some figures would help.

- Section 4.4. You talk about "implementation complexity". I'm not sure what you
mean exactly. The discussion is very informal.

- Fig 2. It would seem better if the legend lists the approaches in the same
order as you described them earlier. Also, the connection could be clearer. I
can of course guess what "et", "flat", and "pass_by_generator" mean, but it
should be explained.

- 698-710. If the reader is not very familiar with Blaze, the discussion is
confusing. Does Blaze do both parsing and code generation, and which of these
components do you us? In the beginning of the discussion, it seems that you
discuss three variants: with Blaze, without Blaze, Blaze alone. I don't
understand what these mean exactly. I also did not understand why you then only
measure two alternatives (no-formula-blaze and formula-blaze) and not the third.

### Questions for author response

- Code size.  It seems to me that you could compare the code size of the three
strategies: how much code does the user need to write for the three strategies,
and related to the size of the concrete/abstract grammars?

- Fig 3. Even with the linear approach, the processing time seems quite slow:
49.68 seconds to compile a 11000 token DSL program. Would it be quicker by using
an embedded DSL that relied on C++ syntax, and didn't require DSL parsing?
