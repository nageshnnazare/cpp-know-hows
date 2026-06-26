# C++ Mastery — From First Principles to C++20 Expert

> A single-stop, deeply-documented guide to the C++ language. Every concept is
> explained from the ground up, with ASCII diagrams of the memory/object/compile
> model, runnable code, and *minute* details that separate a competent C++
> programmer from an expert. Cross-references to **C++ Primer (5th ed., Lippman,
> Lajoie, Moo)** are given as `[Primer §x.y]` so you can dig deeper.

---

## How to use this guide

1. Read the modules in `docs/` **in order** — each builds on the previous one.
2. Compile and step through the matching files in `examples/`.
3. Keep `cheatsheets/` open while you code.

```
            ┌──────────────────────────────────────────┐
            │            C++ MASTERY MAP               │
            └──────────────────────────────────────────┘

   FOUNDATIONS              CORE LANGUAGE           OBJECT MODEL
  ┌───────────────┐        ┌───────────────┐       ┌────────────────┐
  │ 00 Toolchain  │        │ 03 Pointers & │       │ 05 Classes     │
  │ 01 Types/Ops  │  ───▶  │    Memory     │ ───▶  │ 06 Copy/Move   │
  │ 02 Flow/Fns   │        │ 04 const/str  │       │ 07 Inheritance │
  └───────────────┘        └───────────────┘       │ 08 Operators   │
                                                   └────────────────┘

   GENERIC / LIBRARY        MODERN & SAFETY         CONCURRENCY / DEEP
  ┌───────────────┐        ┌───────────────┐       ┌────────────────┐
  │ 09 Templates  │        │ 11 Smart ptrs │       │ 15 Threads/    │
  │ 10 STL        │  ───▶  │ 12 Lambdas    │ ───▶  │    Memory model│
  │               │        │ 13 Exceptions │       │ 16 UB & expert │
  │               │        │ 14 C++20      │       │    gotchas     │
  └───────────────┘        └───────────────┘       └────────────────┘
```

## Table of contents

| # | Module | Key topics |
|---|--------|-----------|
| 00 | [Toolchain & Compilation Model](docs/00_toolchain_and_compilation.md) | translation units, linker, ODR, preprocessing, build systems |
| 01 | [Types, Variables, Literals & Operators](docs/01_types_variables_operators.md) | fundamental types, integer promotion, literals, operator precedence |
| 02 | [Control Flow & Functions](docs/02_control_flow_and_functions.md) | branching, loops, function model, overloading, default args |
| 03 | [Pointers, References & the Memory Model](docs/03_pointers_references_memory.md) | stack/heap, pointers vs references, `new`/`delete`, alignment |
| 04 | [const/constexpr, Arrays, Strings, Enums, Unions](docs/04_const_arrays_strings_enums.md) | const-correctness, `constexpr`/`consteval`, `std::string`, scoped enums |
| 05 | [Classes: Construction & Lifetime](docs/05_classes_construction_lifetime.md) | members, ctors, init lists, RAII, `this`, static members |
| 06 | [Copy, Move & the Rules of 0/3/5](docs/06_copy_move_semantics.md) | value categories, copy/move ctors, rvalue refs, RVO |
| 07 | [Inheritance & Polymorphism](docs/07_inheritance_polymorphism.md) | vtables, virtual, abstract classes, slicing, multiple inheritance |
| 08 | [Operator Overloading](docs/08_operator_overloading.md) | member vs free, conversions, `<=>`, idioms |
| 09 | [Templates & Generic Programming](docs/09_templates_generic_programming.md) | function/class templates, variadics, SFINAE, CRTP, metaprogramming |
| 10 | [The STL: Containers, Iterators, Algorithms](docs/10_stl_containers_algorithms.md) | container internals, iterator categories, complexity, allocators |
| 11 | [Smart Pointers & RAII](docs/11_smart_pointers_raii.md) | `unique_ptr`, `shared_ptr`, control blocks, ownership |
| 12 | [Lambdas & Functional C++](docs/12_lambdas_functional.md) | closures, captures, `std::function`, generic lambdas |
| 13 | [Exceptions & Error Handling](docs/13_exceptions_error_handling.md) | stack unwinding, exception safety, `noexcept`, alternatives |
| 14 | [C++20: Concepts, Ranges, Coroutines, Modules, `<=>`](docs/14_cpp20_features.md) | the big four + smaller gems |
| 15 | [Concurrency & the Memory Model](docs/15_concurrency_memory_model.md) | threads, atomics, happens-before, `std::jthread` |
| 16 | [Undefined Behavior & Expert Gotchas](docs/16_undefined_behavior_gotchas.md) | UB catalog, aliasing, lifetime traps, optimization surprises |

### Expert reference appendices

These are deep, "look it up when you're stuck" references with exhaustive tables.

| App | Reference | Covers |
|-----|-----------|--------|
| A | [Initialization — complete reference](docs/A1_initialization_reference.md) | every init form (default/value/direct/copy/list/aggregate), narrowing, the `vector{5}` trap, reference binding |
| B | [Overload Resolution & Conversions](docs/B1_overload_resolution_reference.md) | candidate→viable→best ranking, full conversion ranks, tie-breakers, C++20 reversed/rewritten candidates |
| C | [Name Lookup, ADL & Namespaces](docs/C1_name_lookup_adl_reference.md) | ordinary lookup, ADL/Koenig, the `swap` two-step, niebloids, two-phase lookup, name hiding |
| D | [Type Deduction](docs/D1_type_deduction_reference.md) | template deduction (3 cases), `auto`/`decltype`/`decltype(auto)`, forwarding refs, reference collapsing, CTAD |
| E | [Keywords & Specifiers](docs/E1_keywords_specifiers_reference.md) | every keyword; deep dives on `const`/`constexpr`/`consteval`/`constinit`, `auto`, `mutable`, `volatile`, `register`, storage classes; deprecated/removed/repurposed status |

> **Reference-bible modules.** Several modules are written as exhaustive references:
> [Module 05](docs/05_classes_construction_lifetime.md) catalogs *every* kind of
> constructor / destructor / assignment + all member-function qualifiers;
> [Module 08](docs/08_operator_overloading.md) lists *every* overloadable operator
> with all canonical signatures (member, non-member, C++23 forms);
> [Module 09](docs/09_templates_generic_programming.md) covers CTAD, deduction
> guides, partial ordering, fold forms, `requires`, and two-phase lookup in depth.

## Quick start

```bash
cd cpp_mastery/examples
./build.sh           # builds every example with -std=c++20 -Wall -Wextra
./build.sh 06_move   # build & run a single example
```

## Cheatsheets

- [Value categories](cheatsheets/value_categories.md)
- [Special member functions](cheatsheets/special_members.md)
- [Big-O of STL operations](cheatsheets/stl_complexity.md)
- [Initialization forms](cheatsheets/initialization.md)

---

### Conventions used in this guide

- `// ⮕` marks the output or important result of a line.
- ASCII boxes show **memory layout**, **object layout**, or **control/data flow**.
- ⚠️ marks a common bug or undefined behavior.
- 🧠 marks an *expert-level* nuance worth memorizing.
- `[Primer §x.y]` references the C++ Primer 5th edition.
