<!-- paginate: skip -->

<br/>
<br/>

# Techniques avancées de génération de code pour la performance

<br/>

> Jules Pénuchot

<br/>

**Université Paris-Saclay**
*Equipe ParSys*

Thèse sous la direction de *Joël Falcou*
Soutenue le 24/09/2024 à l'Université Paris-Saclay

---
<!-- paginate: true -->

# Le calcul haute performance
<!-- 10mn -->

<br/>

**H**igh-**P**erformance **C**omputing *(HPC)*

<br/>

- **Matériel performant**
  *processeurs multi-coeurs, SIMD, GPUs*


- **Abstractions performantes**
  *langages, APIs, bibliothèques*

<!-- "Abstractions performantes": qui ne pénalisent pas le temps d'exécution -->

---

## Un paysage de plus en plus complexe

<br/>

**Le matériel:**

- Plus de *parallélisme(s)*: CPUs de plus en plus parallèles, Fujitsu A64FX
- Plus de *spécialisation*: Cerebras WSE-3, TPU

**Les bibliothèques et applications:**

- Des domaines de plus en plus *diversifiés*
- Une compatibilité de plus en plus *large*
<br/>

*Comment assurer la portabilité et la pérennité du code haute performance?*

---

## Le millefeuille d'abstractions du HPC en C++

<br/>

![width:1700px](images/millefeuille-hpc.svg)
<!-- https://ivanceras.github.io/svgbob-editor/

.--------------------------.
| "Haut niveau: Intention" |
'--------------------------'

  ^ "Bibliothèques expressives, DSELs: Blaze, Eigen..."
  |
  | "Bibliothèques HPC: BLAS, CUBS, cuDNN, EVE, Thrust, LAPACK..."
  |
  | "Primitives, extensions, APIs: Fonctions intrinsèques, noyaux GPU, OpenMP..."
  |
  v "Instructions CPU/GPU: ARM, x86, RISC-V, PTX..."

.------------------------.
| "Bas niveau: Matériel" |
'------------------------'
-->

---

## Concilier abstraction et performance

- Il faut des abstractions pour gérer la complexité
- Il faut qu'elles soient performantes

-> Il faut faire de la metaprog

---

## La métaprogrammation pour la performance

**Métaprogramme:** programme prenant du code en entrée ou en sortie.
<br/>

Existe: en LISP, en C, en Rust, en D, etc.

En C++, les bibliothèques HPC utilisent très majoritairement
la *métaprogrammation de templates*
<br/>

- **Intérêt:** évaluation partielle, composabilité, nouvelles abstractions
- **Exemples:** Thrust, CUBS, EVE, HPX
<br/>

Peut-on aller plus haut en niveau d'abstraction ?
*Oui, via les Domain Specific Embedded Languages*

---

## Les Domain Specific Embedded Languages (DSELs)

En C++, les langages dédiés sont basés sur la **surcharge d'opérateurs**,
utilisant des **expression templates** pour la génération de code.

**Expression templates:** représentation d'expressions algébriques
sous forme d'arborescences de templates de types.

<!-- TODO: verifier que ca compile + modifier le type -->
*Exemple: Blaze*
```c++
blaze::DynamicVector<int> vec_a({4, -2, 5}), vec_b({2, 5, -3});

auto expr = vec_a + trans(vec_b); // Add<DynamicVector<int>, DynamicVector<int>>
blaze::DynamicVector<int> vec_c = expr; // Génération de code à l'assignation
```

---

## Problématiques des DSEL pour le HPC

<!--Encore plus loin:
**C**ompile-**T**ime **R**egular **E**xpressions *(CTRE)*, Hana Dusíková
*DSEL ne reposant pas sur la syntaxe C++ (hors HPC)*-->


- Temps de compilation

- Difficulté de la métaprogrammation de templates

- DSELs limités à la syntaxe C++

---

## Problématiques des DSEL pour le HPC

<!--Encore plus loin:
**C**ompile-**T**ime **R**egular **E**xpressions *(CTRE)*, Hana Dusíková
*DSEL ne reposant pas sur la syntaxe C++ (hors HPC)*-->

<br/>
<br/>

- Temps de compilation
  *Quels outils pour analyser les temps de compilation?*

- Difficulté de la métaprogrammation de templates
  *Faut-il abstraire la métaprogrammation?*

- DSELs limités à la syntaxe C++
  *Quel intérêt pour le HPC ?*

---

# Les travaux de cette thèse

<br/>

**Portabilité des bibliothèques HPC "classiques"**

- Génération de noyaux de calcul SIMD

**Analyse des temps de compilation**

- Nouvelle méthode de benchmarking pour les métaprogrammes

**Techniques d'implémentation des DSELs**

- Nouvelles méthodes pour leur implémentation
- DSEL arbitraires appliqués au HPC

---

<!-----

# Contexte: pourquoi C++ ?

C++ est la plateforme de choix pour le calcul haute performance
*et la métaprogrammation*

**Langages GPGPU**

- CUDA, SYCL, et HIP sont basés sur C++

**Bibliothèques HPC**

- Parllélisme: OpenMP, HPX, BLAS, FFTW
- Squelettes algorithmiques: Thrust, rocThrust

**Bibliothèques génératives**

- CTRE, Eigen, Blaze, EVE, xSIMD, CTPG

---

C++ permet de plus en plus d'évaluation partielle **à la compilation**

C++98: templates
C++11: parameter packs

Ouvre le champ pour la métaprogrammation de templates

---

C'est utilisé pour générer du code optimisé à la compilation

gemv, expression templates

----->

## Génération de noyaux de calcul SIMD

<br/>
<br/>

**Ge**neral **M**atrix-**V**ector multiply *(GEMV)*
*matrice column-major*

- Implémentée en assembleur dans OpenBLAS

- Optimisée manuellement pour chaque architecture

- Le coût des abstractions est **critique**
<br/>

*Est-il possible de générer ce code au lieu de le réimplémenter
pour chaque architecture?*

---

### Optimisation de GEMV

**Deux techniques pour son optimisation:**

- Utilisation des instructions vectorielles
- Déroulage des boucles pour exploiter le pipelining des instructions

*3 étapes en SIMD: Broadcast, produit, réduction*
![width:800px](1-current-metaprogramming/images/gemv-fig1.svg)

---

### Génération de noyaux GEMV performants

Générer du code pour exploiter ces techniques,
quelle que soit l'architecture

<br/>

- Exploiter les architectures SIMD de manière portable,
  et dont la taille est connue à la compilation
  *boost.simd/EVE*

- Automatiser le déroulage
  *déroulage automatique par template metaprogramming*

---


```cpp
template <typename T, std::size_t M, std::size_t N>
void gemv(mat<T, M, N> &mat, vec<T, N> &vec, vec<T, N> &r) {
  constexpr auto Size = eve::wide<T>::static_size;
  constexpr auto SIMD_M = eve::align(M, eve::under{Size});
  constexpr auto SIMD_N = eve::align(N, eve::under{Size});

  for_constexpr<0, SIMD_N,Size>([](auto j) {
    eve::wide<T> pvec(&vec[j]);
    eve::wide<T> mulp_arr[Size];
    for_constexpr<0, Size>(
        [&](auto idx) { mulp_arr[idx] = eve::broadcast<idx>(pvec); });

    for_constexpr<0, SIMD_M>([&](auto I) {
      eve::wide<T> resp(&res[i + (I * Size)]);

      for_constexpr<0, Size>([&](auto J) {
        eve::wide<T> matp(&mat(i + (I * Size), j + J));
        resp = eve::fma(matp, mulp_arr[J], resp);
        eve::store(resp, &r[i + (I * Size)]);
      });
    }
    // Scalar code follows ...
}
```
---

![width:1300px](1-current-metaprogramming/images/gemv-fig3.svg)
*Code généré vs OpenBLAS - x86 (Intel i5-7200)*

---

![width:1300px](1-current-metaprogramming/images/gemv-fig4.svg)
*Code généré vs OpenBLAS - ARM (ARM Cortex A57)*

---

### Conclusion sur GEMV

<br/>

- Les performances des noyaux générés sont **très bonnes**

- Le code est **compact**

- Le code est **portable**
<br/>

Mais...

- Les temps de compilation sont **plus longs**

*Quels outils pour les analyser ?*

---

# Les travaux de cette thèse

<br/>
<br/>

**Portabilité des bibliothèques HPC "classiques"**

**Analyse des temps de compilation**

- Nouvelle méthode de benchmarking pour les métaprogrammes

**Techniques d'implémentation des DSELs**

---

## L'évaluation directe de code C++ à la compilation

Croissance du support et de l'utilisation de la métaprogrammation:

- C++98: templates récursifs
- C++11: parameter pack, constexpr
- C++17: if constexpr
- C++20: concepts, alloc constexpr, std::vector, std::string
- C++23: std::unique_ptr

Le temps de **compilation** ne doit pas prendre le pas
sur le temps de **développement**, voire sur le temps **d'exécution**.

**Comment comparer l'efficacité des techniques de métaprogrammation?**
Exemple: *if constexpr* vs *SFINAE* vs *concepts*

Il faut, *comme pour le runtime*, avoir des outils de mesure et d'analyse
**des temps de compilation**.

---

## ctbench

<br/>

**Objectif:** Mesure et analyse de temps de compilation via le profiler de Clang

**Orienté C++:** API CMake, configuration JSON, bibliothèque C++,
utilisation du préprocesseur pour les benchmarks

**Fonctionnalités:**

- Permet de **filtrer, agréger, et analyser les évènements de compilation**
  de manière configurable, puis de tracer des courbes

- Génère des graphes dans plusieurs formats: SVG, PNG, etc.

- S'adapte à d'autres compilateurs *(mesure de temps d'exécution)*

---

### ctbench - exemple

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

### ctbench - exemple

- **1e implémentation:** récursion
```cpp
template<typename ... Ts> constexpr auto sum();
template <> constexpr auto sum() { return ct_uint_t<0>{}; }
template <typename T> constexpr auto sum(T const &) { return T{}; }

template <typename T, typename... Ts>
constexpr auto sum(T const &, Ts const &...tl) {
  return ct_uint_t<T::value + decltype(sum(tl...))::value>{};
}
```

- **2e implémentation:** expansion de parameter pack

```cpp
template<typename ... Ts> constexpr auto sum();
template <> constexpr auto sum() { return ct_uint_t<0>{}; }

template <typename... Ts> constexpr auto sum(Ts const &...) {
  return ct_uint_t<(Ts::value + ... + 0)>{};
}
```

---

![width:1800px](images/ctbench-example.svg)
*Comparaison du temps de compilation, récursion vs parameter pack*

---

## Conclusion sur ctbench

- On dispose d'outils

- On peut générer des graphes

Mais quels métaprogrammes?

---

# Les travaux de cette thèse

<br/>
<br/>

**Portabilité des bibliothèques HPC "classiques"**

**Analyse des temps de compilation**

**Techniques d'implémentation des DSELs**

- Nouvelles méthodes pour leur implémentation
- DSEL arbitraires appliqués au HPC

---

## Etat de l'art des DSEL

Parsing compile-time, mais avec quel niveau de passage à l'échelle?
*(performances et maintenabilité)*

#### CTRE

- Template metaprogramming

- Performances raisonnables pour des cas de petite taille (Regex)

- Maintenabilité limitée (requiert des connaissances en TMP)

EXEMPLE


-> **La programmation constexpr peut-elle apporter du mieux ?**

---

## poacher

NARRATIF

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

- 1 token = un noeud d'AST
- Parsing trivial
- Langage structuré
- Turing complet

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
