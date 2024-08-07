<!-- paginate: skip -->

# Jules Pénuchot

<br/>
<br/>
<br/>

> Techniques avancées de génération de code pour la performance

<br/>

**Université Paris-Saclay**
*Equipe ParSys*

Thèse sous la direction de *Joël Falcou*

---
<!-- paginate: true -->

# Introduction: un millefeuille d'abstractions
<!-- 10mn -->


- **Matériel performant** (architectures multi-coeurs, SIMD, GPUs)

- **Abstractions performantes** (langages et bibliothèques)

<!-- "Abstractions performantes": qui ne pénalisent pas le temps d'exécution -->

```
Haut niveau: Intention, plus expressif

  /\
  ||   Bibliothèques expressives: Blaze, Eigen...
  ||
  ||   Bibliothèques HPC: BLAS, CUBS, cuDNN, EVE, Thrust, LAPACK, OpenMP...
  ||
  ||   Primitives: Fonctions intrinsèques, noyaux GPU, appels système...
  ||
  ||   Instructions CPU/GPU: ARM, x86, RISC-V, PTX...
  \/

Bas niveau: Spécificités matérielles
```

---

## Performances: le matériel

Perf: le matos s'en occupe, ok.
On a des outils (SYCL, OpenMP, Thrust)

Peut-on générer plus de code pour le HPC ? Avec quelles perfs ?

---

## Performances: les bibliothèques "métier"

Expressivité, que faire?

Blaze, Eigen... et au-dela?

Comment je peux avoir des formules en LaTeX math
(si c'est possible/raisonnable)..?

---

# Contexte
<!-- 10mn -->

C++ est la plateforme de choix pour le calcul haute performance

**Parallélisme**

- OpenMP
- HPX

**GPGPU**

- CUDA
- SYCL
- HIP

**Bibliothèques**

- Blaze
- Eigen

---

C++ permet de plus en plus d'évaluation partielle **à la compilation**

C++98: templates
C++11: parameter packs

Ouvre le champ pour la métaprogrammation de templates

---

C'est utilisé pour générer du code optimisé à la compilation

gemv, expression templates

---

## GEMV

SIMD auto
déroulage auto

---

## L'évaluation directe de code C++ à la compilation

Historique:

* C++11: constexpr
* C++17: if constexpr
* C++20: alloc constexpr, std::vector, std::string
* C++23: std::unique_ptr
* C23: `#embed`

---

## Etat de l'art des DSEL

Parsing compile-time, mais avec quel niveau de passage à l'échelle?
*(performances et maintenabilité)*

#### CTRE

- Template metaprogramming

- Performances raisonnables pour des cas de petite taille (Regex)

- Maintenabilité limitée (requiert des connaissances en TMP)

<br/>

-> **La programmation constexpr peut-elle apporter du mieux ?**

---

# Contribution
<!-- 20mn -->

<br/>

## poacher

DSELs implémentés en C++ via la **métaprogrammation constexpr** pour le parsing, avec une représentation de code dynamique *(std::unique_ptr)*

<br/>

## ctbench

Méthodologie et outils pour la mesure de **temps de compilation** des métaprogrammes, avec les *données de profiling de Clang*

---

## poacher

Comment exploiter la programmation constexpr pour implémenter des DSELs ?

Contraintes sur la mémoire dynamique

Faire le pont entre la programmation constexpr et les templates

Minimiser la métaprogrammation de templates

Deux langages: Brainfuck, et Tiny Math Language (TML)

---

## poacher - un exemple simple pour commencer: Brainfuck

| Token | Sémantique            |
|-------|-----------------------|
| `>`   | `ptr++;`              |
| `<`   | `ptr--;`              |
| `+`   | `++(*ptr);`           |
| `-`   | `--(*ptr);`           |
| `.`   | `putchar(*ptr);`      |
| `,`   | `(*ptr) = getchar();` |
| `[`   | `while(*ptr) {`       |
| `]`   | `}`                   |

* 1 token = un noeud d'AST
* Parsing trivial
* Langage structuré
* Turing complet

---

## poacher - le parser Brainfuck

C++ ordinaire
On rajoute constexpr partout
On fait une AST classique avec std::unique_ptr

-> C'est plié

---

## poacher - premier backend de génération de code

Vieux réflexe: on génère des expression templates

Problématique: comment transformer des structures qui ne passent pas en NTTP (car elles pointent vers de la mémoire dynamique) en arborescences de types?

On passe pas la mémoire, on passe des lambdas qui génèrent chaque noeud:

```
constexpr std::vector<std::vector<int>> get_vector() {
  return {{1, 2, 3}, {4, 5, 6}};
}

// Pas bien:
// constexpr std::vector<int> subvec_0 = get_vector()[0];

// Bien:
constexpr auto get_subvec_0 = []() { return get_vector()[0]; }
```

---

Perfs ???

On peut compiler Hello World (106 noeuds):

```
++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]
>>.>---.+++++++..+++.>>.<-.<.+++.------.--------.>>+.>++.
```

On ne peut pas compiler Mandelbrot (~11'000 noeuds)

Problème: comment mesurer efficacement les temps de compilation

---

## ctbench

Mesure de temps de compilation via le profiler de Clang

Orienté C++: API CMake, configuration JSON, bibliothèque C++

Génère des graphes dans plusieurs formats: SVG, PNG, etc.

S'adapte à d'autres compilateurs *(mesure de temps d'exécution)*

Permet de **filtrer, agréger, et analyser les évènements de compilation**
de manière configurable, puis de tracer des courbes

---

## ctbench - exemple simple

https://github.com/jpenuchot/ctbench

- Entiers sous forme de types

```cpp
template <std::size_t N> struct ct_uint_t {
  static constexpr std::size_t value = N;
};
```

- Addition d'un pack d'entiers

```cpp
constexpr auto foo() {
  return []<std::size_t... Is>(std::index_sequence<Is...>) {
    return sum(ct_uint_t<Is>{}...);
  }
  (std::make_index_sequence<BENCHMARK_SIZE>{});
}

constexpr std::size_t result = decltype(foo())::value;
```

---

- Récursion
```cpp
template<typename ... Ts> constexpr auto sum();

template <> constexpr auto sum() { return ct_uint_t<0>{}; }
template <typename T> constexpr auto sum(T const &) { return T{}; }

template <typename T, typename... Ts>
constexpr auto sum(T const &, Ts const &...tl) {
  return ct_uint_t<T::value + decltype(sum(tl...))::value>{};
}
```

- Expansion de parameter pack

```cpp
template<typename ... Ts> constexpr auto sum();

template <> constexpr auto sum() { return ct_uint_t<0>{}; }

template <typename... Ts> constexpr auto sum(Ts const &...) {
  return ct_uint_t<(Ts::value + ... + 0)>{};
}
```

---

![width:1200px](images/ctbench-example.svg)

---

BF - 3e tentative

---

## TML

---

RPN & Shunting Yard

---

Implem constexpr: parser ordinaire + generateur de code a base de lecture RPN

---

Conclusion - 5mn:

On a des nouvelles techniques pour écrire des métaprogrammes
avec du C++ régulier

Plusieurs méthodes:

- Pass-by-generator: prototypage et petits DSELs
- Sérialisation vers NTTP: langages complexes
- Fonctionnent pour le calcul hautes performances

Nouvelle méthodologie pour le benchmarking des temps de compilation
