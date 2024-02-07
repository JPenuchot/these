# PhD thesis document

[Read it](https://raw.githubusercontent.com/jpenuchot/these/main/main.pdf)

This is a public repository for my PhD thesis.
I'm using it mostly to keep track of my work,
have a CI to build document previews frequently,
and make it available to curious readers.

You'll see me working on it on Tuesdays and Wednesdays.

## What this is about

C++ metaprogramming techniques, embedding languages in C++ using
`constexpr` programming for parsing, studying the thin barrier between
`constexpr` function output and C++ templates for code generation,
generic programming for high performance computing,
and compiler execution time analysis.

## Who this is for

My PhD advisor, C++ nerds, or anyone else interested in metaprogramming or HPC.

## A few thesis pro-tips for myself and others)

- Set up you environment right. If you're familiar with LSP,
  use [TexLab](https://github.com/latex-lsp/texlab/).

- Read [this free book](https://www.dickimaw-books.com/latex/thesis/) for
  useful tips on bibliography, glossaries, etc.

- Use subfiles, not include. Subfiles helps with automatic forward search.
  When adding subfiles, keep all header stuff in the main document.
  You cannot have subfiles that incorporate other subfiles.
  All subfiles must be declared in the main LaTeX file which serves as a
  main "glue" document. It should not be problematic even for a large thesis.

- Use the following form for `\begin{lstlisting}`:

  ```latex
  \begin{lstlisting}[
    language=c++,
    caption=A random caption,
    label=a_random_label
  ]{}

  // ...

  \end{lstlisting}
  ```

  The trainling `{}` after the parameter block helps
  syntax highlighting with code editors such as Kate.
