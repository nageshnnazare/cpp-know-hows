# Module 14 — C++20: Concepts, Ranges, Coroutines, Modules & `<=>`

> C++20 is the biggest release since C++11. Its "big four" — **Concepts**,
> **Ranges**, **Coroutines**, **Modules** — reshape how we write generic code,
> data pipelines, async code, and how we structure programs. Plus a pile of
> smaller gems.

---

## 14.1 Concepts — named, checkable constraints on templates 🧠

Before C++20, constraining templates meant cryptic SFINAE (Module 09). Concepts
make requirements **first-class, readable, and produce good error messages**.

```cpp
#include <concepts>

template<class T>
concept Numeric = std::integral<T> || std::floating_point<T>;   // a named predicate

template<Numeric T>                 // constrained template parameter
T square(T x) { return x * x; }

square(5);     // ok
square("hi");  // ❌ clear error: "constraint Numeric<const char*> not satisfied"
```

Four equivalent ways to constrain:

```cpp
template<Numeric T> void f(T);                         // 1. constrained param
template<class T> requires Numeric<T> void g(T);       // 2. requires clause
template<class T> void h(T) requires Numeric<T>;       // 3. trailing requires
void k(Numeric auto x);                                // 4. abbreviated (terse)
```

Defining a concept with a `requires` expression (checks what operations are valid):

```cpp
template<class T>
concept Container = requires(T c) {
    typename T::value_type;            // must have a nested value_type
    { c.size() } -> std::convertible_to<std::size_t>;  // size() returns size_t-ish
    { c.begin() };                     // must have begin()/end()
    { c.end() };
    requires std::movable<T>;          // nested requirement
};
```

```
   requires(params) {
     expr;                              // expr must be valid
     typename T::nested;                // nested type must exist
     { expr } -> Concept;               // expr valid AND its type satisfies Concept
     requires bool_constexpr;           // a compile-time boolean must hold
   }
```

Standard library concepts (`<concepts>`): `same_as`, `convertible_to`,
`integral`, `floating_point`, `totally_ordered`, `equality_comparable`,
`movable`, `copyable`, `default_initializable`, `invocable`, `predicate`,
`derived_from`, `regular`, ...

🧠 Concepts also enable **overloading on constraints** — the *most constrained*
viable candidate wins, replacing tag dispatch / `enable_if` overload sets:

```cpp
void process(std::integral auto x);        // chosen for ints
void process(std::floating_point auto x);  // chosen for floats — no ambiguity
```

---

## 14.2 Ranges — composable, lazy algorithms 🧠

The Ranges library (`<ranges>`, `std::ranges::`) lets algorithms take a **range**
directly (not iterator pairs) and chains transformations with the `|` pipe.

```cpp
#include <ranges>
namespace rv = std::views;
std::vector<int> v{1,2,3,4,5,6,7,8,9,10};

auto result = v
    | rv::filter([](int x){ return x % 2 == 0; })   // keep evens
    | rv::transform([](int x){ return x * x; })      // square them
    | rv::take(3);                                    // first three
// result is a LAZY view: 4, 16, 36 — computed on demand during iteration
for (int x : result) std::cout << x << ' ';
```

```
   PIPELINE (lazy — nothing computed until iterated):
   v ─▶[filter even]─▶[transform x²]─▶[take 3]─▶ 4 16 36
       │ no intermediate containers allocated; each element flows through
       │ the pipeline one at a time (composition, not materialization).
```

```cpp
// range-based algorithms: no .begin()/.end() boilerplate, projections built in
std::ranges::sort(v);                                  // sort the whole container
std::ranges::sort(people, {}, &Person::age);           // sort BY a projection (age)
auto it = std::ranges::find(v, 42);
bool ok = std::ranges::all_of(v, [](int x){ return x>0; });

// common views
rv::iota(1, 6)            // 1 2 3 4 5   (lazy generated range)
rv::reverse               // reversed
rv::take(n) / rv::drop(n)
rv::keys / rv::values     // over a map
rv::join                  // flatten range-of-ranges
rv::split(delim)          // split into subranges
rv::common, rv::elements
```

🧠 Views are **non-owning and lazy** — cheap to copy, composable, no intermediate
allocations. ⚠️ Like `string_view`/`span`, a view that refers to a temporary
container can dangle. C++23 adds `ranges::to<Container>()` to materialize a view
into a real container.

---

## 14.3 Coroutines — functions that suspend and resume

A coroutine is a function containing `co_await`, `co_yield`, or `co_return`. It can
**pause** mid-execution and **resume** later — the basis for generators, async I/O,
and lazy sequences.

```cpp
   co_yield value;   // produce a value and SUSPEND (generators)
   co_await awaitable;// suspend until something is ready (async)
   co_return value;  // finish the coroutine
```

```
   normal function          coroutine
   ┌───────────────┐         ┌──────────────────────────────────────────┐
   │ call → run    │         │ call → run to first suspend point        │
   │ → return once │         │ ↩ suspend (state saved on the HEAP frame)│
   └───────────────┘         │ resume → run to next suspend ... etc.    │
                             │ → finally co_return                      │
                             └──────────────────────────────────────────┘
```

⚠️ C++20 ships the **low-level machinery** (`promise_type`, `coroutine_handle`,
`std::suspend_always/never`) but **no ready-made `generator`/`task` types** in the
standard library — you write them or use a library (cppcoro, Boost.Asio). C++23
adds `std::generator`.

A conceptual generator (the shape of what you'd build):

```cpp
generator<int> fibonacci() {
    int a = 0, b = 1;
    while (true) {
        co_yield a;                 // hand out 'a', suspend
        a = std::exchange(b, a + b);
    }
}
for (int x : fibonacci()) { if (x > 100) break; use(x); }  // lazy, infinite-but-bounded
```

🧠 Coroutine frames are heap-allocated (the compiler may elide the allocation if
the lifetime is provably bounded — "HALO" optimization). Understand awaiter
mechanics before building async frameworks; for everyday use, prefer a library.

---

## 14.4 Modules — replacing the `#include` model

Modules fix the textual `#include` model (Module 00): no re-parsing headers per
TU, no macro leakage, real encapsulation, faster builds.

```cpp
// math.ixx  (module interface unit)
export module math;                 // declares the module

export int add(int a, int b) { return a + b; }   // exported (visible to importers)
int helper() { return 42; }                       // NOT exported (module-internal)

// main.cpp
import math;                        // no preprocessor, no header text pasted
int main() { return add(2, 3); }
```

```
   #include model                       module model
   ┌────────────────────────┐           ┌─────────────────────────┐
   │ header TEXT pasted &   │           │ compiled BMI (binary    │
   │ RE-PARSED in every TU  │           │ module interface) built │
   │ macros leak in/out     │           │ ONCE, imported fast     │
   │ order-sensitive        │           │ no macro leakage,       │
   │ slow builds            │           │ order-independent       │
   └────────────────────────┘           └─────────────────────────┘
```

🧠 Modules are powerful but **tooling/build-system support is still maturing**
(2024-2026). `import std;` (C++23) is the eventual replacement for including all
standard headers. Adopt incrementally; many projects still use headers.

---

## 14.5 `operator<=>` (recap) & defaulted comparisons

Covered in Module 08. C++20 lets you write one `<=>` (and often `==`) and get all
six comparisons, with `strong/weak/partial_ordering` categories.

```cpp
struct P { int x, y; auto operator<=>(const P&) const = default; };  // all comparisons
```

---

## 14.6 Smaller C++20 gems you should know

```cpp
// designated initializers (Module 05)
struct Cfg { int w=0, h=0; bool fs=false; };
Cfg c{.w=800, .h=600};                      // .fs defaults to false

// std::span — non-owning contiguous view (Module 10)
void f(std::span<const int> s);

// constexpr everything: std::vector/std::string usable at compile time;
// virtual constexpr; constexpr dynamic_cast/typeid; std::is_constant_evaluated()
if (std::is_constant_evaluated()) { /* compile-time path */ } else { /* runtime */ }

// consteval / constinit (Module 04)
consteval int ce(int x){ return x*x; }
constinit int g = ce(4);

// [[likely]] / [[unlikely]] branch hints
if (x) [[likely]] { hot(); } else [[unlikely]] { cold(); }

// using enum (Module 04)
// template lambdas, default-constructible/assignable stateless lambdas
auto cmp = []<class T>(T a, T b){ return a < b; };

// std::format — type-safe, fast formatting (printf/iostream successor)
#include <format>
std::string s = std::format("{:>8.2f} and {:#x}", 3.14159, 255);  // "    3.14 and 0xff"

// calendar & time zones in <chrono>; std::source_location (replaces __FILE__/__LINE__);
// std::bit_cast, <bit> (std::popcount, std::rotl, std::has_single_bit);
// std::ssize; math constants <numbers> (std::numbers::pi);
// atomic smart-pointer-free waiting: std::atomic<T>::wait/notify; std::barrier/latch/semaphore (Module 15)
```

🧠 `std::format` (and C++23 `std::print`) is the modern way to format text:
type-safe, extensible (`std::formatter` specializations), and faster than
iostreams. Prefer it over `printf` (unsafe) and stringstream (slow/verbose).

---

## 14.7 Putting it together — a concept-constrained ranges pipeline

```cpp
#include <ranges>
#include <concepts>
#include <vector>
#include <format>

template<std::ranges::input_range R>
    requires std::integral<std::ranges::range_value_t<R>>
auto sum_of_even_squares(R&& r) {
    namespace rv = std::views;
    long total = 0;
    for (auto x : r | rv::filter([](auto v){ return v % 2 == 0; })
                    | rv::transform([](auto v){ return v * v; }))
        total += x;
    return total;
}

int main() {
    std::vector<int> v{1,2,3,4,5,6};
    std::print("{}\n", sum_of_even_squares(v));  // 4 + 16 + 36 = 56  (C++23 std::print)
}
```

---

## 14.8 Checklist

- [ ] You constrain templates with concepts and overload on constraints.
- [ ] You build lazy, composable pipelines with ranges/views and use range algorithms.
- [ ] You understand coroutine suspend/resume and that C++20 gives only primitives.
- [ ] You know modules' benefits and current tooling caveats.
- [ ] You default `<=>`/`==` and pick the right ordering category.
- [ ] You use `std::format`/`std::print`, `std::span`, designated inits, `[[likely]]`.

➡ Next: [Module 15 — Concurrency & the Memory Model](15_concurrency_memory_model.md)
