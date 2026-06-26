# Module 09 — Templates & Generic Programming

> Templates are C++'s mechanism for writing code parameterized by **types** and
> **values**, instantiated and type-checked at compile time. They power the STL,
> enable zero-overhead abstraction, and are the basis of metaprogramming.
> `[Primer §16]`

---

## 9.1 The core idea: code that writes code

```cpp
template<class T>
T max(T a, T b) { return a < b ? b : a; }   // a PATTERN, not yet code

max(3, 7);        // instantiates max<int>(int,int)   ← compiler generates it
max(2.5, 1.5);    // instantiates max<double>(...)
max("a"s, "b"s);  // instantiates max<std::string>(...)
```

```
   template<class T> ...        SOURCE PATTERN (no machine code emitted)
            │  instantiated on demand for each set of template arguments
            ▼
   max<int>      max<double>      max<std::string>   ← separate generated functions
   (compiler stamps out one specialization per distinct argument list)
```

🧠 A template is **not** compiled until **instantiated**. This is why template
definitions live in headers (the full body must be visible at each use), and why
template error messages are long (they surface at instantiation).

`typename` and `class` are interchangeable in `template<...>` parameter lists;
`typename` is also required to say "this dependent name is a type" (9.7).

---

## 9.2 Function templates: deduction & explicit args

```cpp
template<class T> void f(T x);
f(42);              // T = int (deduced)
f<double>(42);      // T = double (explicit), 42 converted

template<class R, class T> R cast_to(T x){ return static_cast<R>(x); }
cast_to<int>(3.9);  // R explicit (can't deduce return type), T = double deduced
```

Deduction strips top-level const/ref like `auto` (Module 01). For `T&&`
parameters it forms forwarding references (Module 06).

```
   PARAM FORM     ARGUMENT      DEDUCED T        param type
   f(T  x)        int / int&    int              int      (copy; const/ref stripped)
   f(T& x)        int           int              int&
   f(const T& x)  int / const   int              const int&
   f(T&& x)       lvalue int    int&             int&     (collapses) ← forwarding ref
   f(T&& x)       rvalue int    int              int&&
```

---

## 9.3 Class templates

```cpp
template<class T, std::size_t N>
class Array {                      // T = element type, N = a NON-TYPE param (a value)
    T data_[N];
public:
    constexpr std::size_t size() const { return N; }
    T&       operator[](std::size_t i)       { return data_[i]; }
    const T& operator[](std::size_t i) const { return data_[i]; }
};
Array<int, 4> a;     // T=int, N=4
```

**Class template argument deduction (CTAD, C++17)** lets you omit args:

```cpp
std::vector v{1, 2, 3};       // deduces std::vector<int>
std::pair  p{1, 2.0};         // std::pair<int,double>
// custom deduction guide:
template<class T> Array(std::initializer_list<T>) -> Array<T, /*...*/>;  // (illustrative)
```

Template parameter kinds:

```
   • TYPE parameters:      template<class T>
   • NON-TYPE params(NTTP): template<int N>, <std::size_t>, <auto>, (C++20: float, structural types)
   • TEMPLATE template params: template<template<class> class Container>
   • parameter packs (variadic): template<class... Ts>
```

---

## 9.4 Specialization: full & partial

```cpp
template<class T> struct TypeName            { static constexpr auto value = "unknown"; };
template<>        struct TypeName<int>       { static constexpr auto value = "int"; };   // FULL
template<>        struct TypeName<double>    { static constexpr auto value = "double"; };

template<class T> struct TypeName<T*>        { static constexpr auto value = "pointer"; }; // PARTIAL
template<class T> struct TypeName<T[]>       { static constexpr auto value = "array"; };   // PARTIAL
```

```
   PRIMARY template      ── the general case
   FULL specialization   ── exact match for specific args (functions: prefer overloading)
   PARTIAL specialization── pattern match (T*, T&, std::vector<T>, ...) — CLASSES ONLY
```

⚠️ Function templates **cannot** be partially specialized — use overloading or
`if constexpr` instead.

---

## 9.5 Variadic templates & parameter packs

```cpp
template<class... Ts>                 // Ts is a TYPE pack
void printAll(const Ts&... args) {    // args is a FUNCTION param pack
    (std::cout << ... << args) << '\n';   // C++17 FOLD EXPRESSION
}
printAll(1, " hi ", 3.14, '!');       // expands to cout << 1 << " hi " << 3.14 << '!'
```

```
   FOLD EXPRESSIONS (C++17)  — collapse a pack with a binary operator
   (... op pack)      left fold      ((a op b) op c) op d
   (pack op ...)      right fold     a op (b op (c op d))
   (init op ... op pack) / (pack op ... op init)  with an initial value

   sizeof...(Ts)     ← number of elements in the pack
```

Classic recursive expansion (pre-fold style, still useful for per-element logic):

```cpp
void print() {}                                   // base case
template<class T, class... Rest>
void print(const T& first, const Rest&... rest) {
    std::cout << first;
    print(rest...);                               // peel one off, recurse
}
```

Perfect forwarding a pack (e.g. `make_unique`):

```cpp
template<class T, class... Args>
std::unique_ptr<T> my_make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));  // forward each
}
```

---

## 9.6 `if constexpr` — compile-time branching (C++17) 🧠

```cpp
template<class T>
auto to_string(const T& x) {
    if constexpr (std::is_arithmetic_v<T>)
        return std::to_string(x);      // ← only this branch compiled when T is numeric
    else if constexpr (std::is_same_v<T, std::string>)
        return x;
    else
        return std::string{x};         // the OTHER branches are DISCARDED, not just skipped
}
```

The discarded branch is not instantiated, so it may contain code that would be
invalid for the current `T`. This replaced most old SFINAE/tag-dispatch tricks.

---

## 9.7 Dependent names & `typename`/`template` disambiguators

```cpp
template<class C>
void f(C& c) {
    typename C::value_type x;        // ⚠️ 'typename' required: C::value_type is a dependent TYPE
    auto it = c.template get<0>();   // ⚠️ '.template' required: get is a dependent TEMPLATE
}
```

```
   Inside a template, the compiler doesn't yet know what C is, so:
   • prefix a dependent type name with 'typename'
   • prefix a dependent member template with 'template'
   Otherwise the parser assumes 'value_type' is a value and '<' is less-than.
```

---

## 9.8 SFINAE — "Substitution Failure Is Not An Error"

When substituting template args produces an ill-formed *signature* (not body), the
candidate is silently removed from the overload set rather than causing an error.

```cpp
// enable this overload only for integral T
template<class T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
void g(T x) { /* integer path */ }

template<class T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
void g(T x) { /* float path */ }
```

🧠 SFINAE was the pre-C++20 workhorse for constraints. **In C++20, prefer
Concepts** (Module 14) — they do the same job with readable syntax and better
errors. Know SFINAE because you'll read older code and library internals.

---

## 9.9 CRTP — static polymorphism (Curiously Recurring Template Pattern)

```cpp
template<class Derived>
struct Comparable {                              // mixin via CRTP
    bool operator!=(const Derived& o) const {
        const auto& self = static_cast<const Derived&>(*this);
        return !(self == o);                     // requires Derived::operator==
    }
};
struct Point : Comparable<Point> {
    int x, y;
    bool operator==(const Point& o) const { return x==o.x && y==o.y; }
};
```

```
   CRTP: a base templated on its OWN derived class.
   struct D : Base<D> { ... };
   → Base can call into D's methods via static_cast<D&>(*this) — resolved at
     compile time. Zero vtable, fully inlinable. Used for mixins, the
     enable_shared_from_this pattern, expression templates, policy-based design.
```

🧠 CRTP vs virtual: CRTP gives static dispatch (no runtime cost, but type is fixed
at compile time and you can't store heterogeneous objects in one container as the
same type). Virtual gives runtime flexibility at the cost of indirection.

---

## 9.10 Template metaprogramming & type traits

`<type_traits>` provides compile-time introspection and transformation:

```cpp
std::is_integral_v<T>            // bool: is T an integer type?
std::is_same_v<T, U>             // bool
std::is_base_of_v<B, D>          // bool
std::remove_reference_t<T>       // strip & / &&
std::remove_cv_t<T>              // strip const/volatile
std::decay_t<T>                  // what 'auto'/by-value deduction would give
std::conditional_t<C, A, B>      // C ? A : B   (a TYPE chosen at compile time)
std::common_type_t<Ts...>        // the common type of several
```

A tiny metaprogram (compile-time factorial):

```cpp
template<int N> struct Fact { static constexpr long value = N * Fact<N-1>::value; };
template<>      struct Fact<0>{ static constexpr long value = 1; };
static_assert(Fact<5>::value == 120);   // computed entirely at compile time

// modern equivalent: just a constexpr function
constexpr long fact(int n){ return n ? n*fact(n-1) : 1; }
static_assert(fact(5) == 120);
```

🧠 Modern TMP increasingly uses `constexpr` functions + `if constexpr` + Concepts
instead of recursive class templates. Reserve heavy template recursion for cases
that truly need type-level computation.

---

## 9.11 `static_assert` & compile-time contracts

```cpp
template<class T>
class FixedPoint {
    static_assert(std::is_integral_v<T>, "FixedPoint requires an integral storage type");
};
static_assert(sizeof(void*) == 8, "64-bit only");
```

---

## 9.12 Variable templates, alias templates & template template parameters

### Variable templates (C++14)

```cpp
template<class T> constexpr T pi = T(3.14159265358979323846);  // a templated VARIABLE
double d = pi<double>;   float f = pi<float>;

template<class T> constexpr bool is_pointer_v = std::is_pointer<T>::value;  // the _v traits
```

### Alias templates

```cpp
template<class T> using Vec = std::vector<T>;          // alias TEMPLATE (typedef can't do this)
Vec<int> v;                                            // == std::vector<int>
template<class T> using ElementType = typename T::value_type;
```

```
   typedef       : alias for ONE concrete type (no parameters)
   using alias   : same as typedef for concrete types
   alias TEMPLATE: parameterized alias — typedef CANNOT express this
   ⚠️ alias templates are NEVER deduced and cannot be specialized.
```

### Template template parameters

```cpp
template<class T, template<class, class> class Container = std::vector>
class Stack {
    Container<T, std::allocator<T>> data_;   // the container TYPE is itself a template
};
Stack<int> s;                                // uses std::vector<int>
Stack<int, std::deque> s2;                   // uses std::deque<int>
```

```
   template<template<class...> class TT> struct X;   // TT is a TEMPLATE, not a type
   Use a variadic template-template param  template<class...> class  to accept
   containers with any number of (defaulted) parameters portably.
```

---

## 9.13 Class Template Argument Deduction (CTAD) & deduction guides

CTAD (C++17) lets you omit class template arguments; the compiler deduces them
from the constructor arguments.

```cpp
std::vector v{1, 2, 3};        // deduces std::vector<int>
std::pair  p{1, 2.0};          // std::pair<int, double>
std::lock_guard lk{mtx};       // std::lock_guard<std::mutex>
std::array a{1, 2, 3};         // std::array<int, 3>
```

User-defined **deduction guides** steer CTAD when constructor deduction isn't
enough (e.g. deducing from iterators, or decaying types):

```cpp
template<class T>
struct Box { T value; Box(T v) : value(v) {} };
Box(const char*) -> Box<std::string>;          // guide: Box{"hi"} ⟶ Box<std::string>
Box b{"hi"};                                    // value is std::string, not const char*

template<class It>
struct Range { It begin_, end_; Range(It b, It e); };
template<class It> Range(It, It) -> Range<It>;  // deduce element/iterator type

// implicit guides are generated from constructors; aggregates get them in C++20.
```

```
   DEDUCTION GUIDE syntax:   TemplateName(param-types) -> TemplateName<args>;
   • placed at namespace scope, after the class.
   • used ONLY for CTAD (when you write the class name without <...>).
```

---

## 9.14 Explicit instantiation, `extern template` & instantiation control

```cpp
// header: declaration only — suppress implicit instantiation in every TU
extern template class std::vector<MyType>;     // "don't instantiate here"

// ONE .cpp: explicit instantiation definition — emit the code exactly once
template class std::vector<MyType>;            // full class
template int max<int>(int, int);               // a function specialization
```

```
   IMPLICIT instantiation: compiler stamps out code on first use, in EVERY TU
     that uses it (linker merges duplicates) → slower builds, bigger objects.
   EXPLICIT instantiation (template class X<T>;): force generation once.
   extern template: tell other TUs NOT to instantiate (pair with one explicit def).
   ⇒ Use the pair to cut compile time / code bloat for heavily-used instantiations.
```

---

## 9.15 Overload resolution among templates & partial ordering

When multiple templates (and non-templates) match, C++ applies a precise ranking:

```
   1. A non-template function that is an exact (or better) match BEATS any template.
   2. Among templates, the MORE SPECIALIZED one wins ("partial ordering of templates").
   3. Among class template partial specializations, the more specialized matches.
   4. If still tied → AMBIGUOUS (error).
   5. Constraints (C++20 concepts): a more-constrained candidate beats a less-constrained.
```

```cpp
template<class T> void f(T);        // (a) generic
template<class T> void f(T*);       // (b) more specialized (pointer)
void f(int*);                       // (c) non-template

int  i; f(i);    // (a)  — only generic matches
int* p; f(p);    // (c)  — non-template beats both templates
double* d; f(d); // (b)  — more specialized template than (a)
```

🧠 "More specialized" means: every argument list that matches the specialized one
also matches the general one, but not vice-versa. This is *partial ordering*, the
same mechanism that picks the best class partial specialization (§9.4).

---

## 9.16 Fold expressions — complete reference (C++17)

```
   FORM                       EXPANSION (for pack e1, e2, ..., eN)
   ──────────────────────────────────────────────────────────────────────────
   (pack op ...)              unary RIGHT fold : e1 op (e2 op (... op eN))
   (... op pack)              unary LEFT  fold : ((e1 op e2) op ...) op eN
   (pack op ... op init)      binary RIGHT fold: e1 op (e2 op (... op (eN op init)))
   (init op ... op pack)      binary LEFT  fold: (((init op e1) op e2) ...) op eN

   EMPTY-PACK rules: only &&(→true), ||(→false), and ,(→void()) are allowed
   to fold an empty pack. Others need a binary fold with an init value.
```

```cpp
template<class... T> bool all(T... x)      { return (x && ...); }       // unary right
template<class... T> auto sum(T... x)      { return (x + ...); }        // unary right
template<class... T> auto sum0(T... x)     { return (0 + ... + x); }    // binary, safe on empty
template<class... T> void print(T... x)    { ((std::cout << x << ' '), ...); } // comma fold
template<class... T> auto count(T...)      { return sizeof...(T); }     // pack size
```

---

## 9.17 `requires` expressions & constraints (C++20 reference)

(Used heavily in [Module 14 §14.1](14_cpp20_features.md); here is the syntax catalog.)

```cpp
template<class T>
concept Sortable = requires(T& c, typename T::value_type v) {
    typename T::value_type;                          // TYPE requirement
    c.begin();  c.end();                             // SIMPLE requirement (must be valid)
    { c.size() } -> std::convertible_to<std::size_t>;// COMPOUND requirement (valid + type)
    { c.push_back(v) } noexcept;                     // COMPOUND with noexcept
    requires std::movable<T>;                        // NESTED requirement (a bool)
};

template<class T> requires Sortable<T> void sort_it(T&);   // requires clause
void sort_it(Sortable auto&);                              // terse syntax
```

```
   FOUR requirement kinds inside requires{...}:
     simple    :  expr;                       expr must be well-formed
     type      :  typename T::member;         the type must exist
     compound  :  { expr } noexcept -> Concept; valid, [non-throwing], result models Concept
     nested    :  requires predicate;          a constexpr bool that must hold
   A 'requires requires' you'll see is: 'requires' clause + ad-hoc 'requires' expression.
```

---

## 9.18 Dependent names & two-phase lookup (deep)

```
   Templates are checked in TWO phases:
   Phase 1 (at DEFINITION): non-dependent names are looked up and bound; syntax checked.
   Phase 2 (at INSTANTIATION): dependent names are looked up using the template args,
                               via ordinary lookup + ADL at the point of instantiation.
```

```cpp
template<class T>
struct D : Base<T> {
    void f() {
        // x();             // ❌ if x is in Base<T>: NOT found in phase 1 (dependent base)
        this->x();          // ✅ 'this->' makes it dependent → looked up in phase 2
        Base<T>::x();       // ✅ explicit qualification also works
        typename T::type y; // ✅ 'typename' : T::type is a dependent TYPE
        T::template get<0>();// ✅ '.template'/'::template' : dependent member template
    }
};
```

🧠 Names from a **dependent base class** are not visible unqualified — you must use
`this->` or qualify them. This surprises people coming from non-template code where
base members are visible directly.

---

## 9.19 Costs & pitfalls

```
   ⚠️ Code bloat: each instantiation generates separate code. Many distinct
      types ⇒ larger binary. (Mitigate: type-erase, extract non-dependent code.)
   ⚠️ Compile-time cost: heavy templates slow the build.
   ⚠️ Error messages: long; concepts (C++20) improve them dramatically.
   ⚠️ Two-phase lookup: names are checked at definition (non-dependent) and at
      instantiation (dependent). GCC/Clang are strict; old MSVC was lax.
   ✅ Put only the template-dependent bits in templates; share the rest.
```

---

## 9.20 Checklist

- [ ] You can write function/class templates, NTTPs, and variadic templates with folds.
- [ ] You use variable templates, alias templates, and template template parameters.
- [ ] You understand CTAD and can write deduction guides.
- [ ] You use explicit/`extern template` instantiation to control build cost.
- [ ] You can predict template overload resolution & partial ordering.
- [ ] You know all four fold forms and the empty-pack rules.
- [ ] You write `requires` expressions with all four requirement kinds.
- [ ] You use `if constexpr` for compile-time branching.
- [ ] You know when `typename`/`.template`/`this->` disambiguators are required (two-phase lookup).
- [ ] You understand SFINAE but reach for Concepts (Module 14) in new code.
- [ ] You can apply CRTP for zero-overhead static polymorphism.
- [ ] You use `<type_traits>` and `static_assert` for compile-time logic/contracts.

➡ Next: [Module 10 — The STL: Containers, Iterators, Algorithms](10_stl_containers_algorithms.md)
