<!-- paginate: skip -->

# Jules Pénuchot

Université Paris-Saclay
Parsys

Thèse sous la direction de Joel Falcou

**Techniques avancées de génération de code pour la performance**

---
<!-- paginate: true -->

# Intro
<!-- 10mn -->

Besoin de parallelisme pour les perfs

Perf + expressivité

---

Perf: le matos s'en occupe, ok.
On a des outils (SYCL, OpenMP, Thrust)

---

Expressivité, que faire?

Blaze... et au-dela?

Comment je peux avoir des formules en LaTeX math
(si c'est possible/raisonnable)..?

---

# Contexte
<!-- 10mn -->

C++ permet de plus en plus d'evaluation partielle **à la compilation**

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

Parsing compile-time, mais... avec quel niveau de passage à l'échelle?
(perfs **et** maintenabilité)

#### CTRE:

- Template metaprogramming

- Performances raisonnables pour des cas de petite taille (regex)

- Maintenabilité limitée (requiert des connaissances en TMP)

Est-ce que la programmation constexpr peut apporter du mieux ?

---

# Contribution
<!-- 20mn -->

#### poacher

DSELs implémentés en C++ via la programmation constexpr pour le parsing

#### ctbench

Méthodologie et outils pour la mesure de performances des métaprogrammes

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
* Parsing extremement simple
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

---

ctbench

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
