# Module 12 — Lambdas & Functional C++

> A lambda is syntactic sugar for a compiler-generated **functor** (a class with
> `operator()`). Understanding what the compiler actually generates demystifies
> captures, `mutable`, dangling-capture bugs, and `std::function` overhead.
> `[Primer §10.3]`

---

## 12.1 Anatomy of a lambda

```cpp
auto add = [base = 10](int x) mutable noexcept -> int { return base + x; };
//        └─ capture ─┘ └param┘   └spec┘         └ret┘   └─ body ─┘
add(5);   // 15
```

```
   [captures] (params) specifiers -> return_type { body }
    │          │        │            │              │
    │          │        │            │              └ what it does
    │          │        │            └ usually deduced; needed for complex returns
    │          │        └ mutable / noexcept / constexpr / consteval / static (C++23)
    │          └ like any function's parameters (auto ⇒ generic, C++14)
    └ which surrounding variables to capture, and how (by value / by reference)
```

---

## 12.2 What the compiler generates 🧠

A lambda is an unnamed **closure type** — a struct with the captures as members
and `operator()` as the body:

```cpp
int base = 10;
auto f = [base](int x){ return base + x; };

// is roughly equivalent to:
struct __Lambda {
    int base;                                   // captured by value ⇒ a member
    int operator()(int x) const { return base + x; }  // const by default!
};
__Lambda f{base};
```

```
   lambda  ⇄  unnamed class with:
   • one member per captured variable
   • operator() holding the body
   • operator() is CONST unless you write 'mutable'
   Each lambda has a UNIQUE, unnamable type ⇒ store with 'auto'.
```

🧠 Because each lambda is its own type, two lambdas with identical bodies have
*different* types. This matters for templates and for storing them (use `auto`, or
type-erase with `std::function`).

---

## 12.3 Captures — the heart of it

```cpp
int a = 1, b = 2;
[a]       (){ return a; };       // capture a BY VALUE (copy, frozen at creation)
[&a]      (){ a = 5; };          // capture a BY REFERENCE (alias to the real a)
[a, &b]   (){ return a + b; };   // mixed
[=]       (){ return a + b; };   // capture EVERYTHING used, by value
[&]       (){ a = b; };          // capture EVERYTHING used, by reference
[=, &b]   (){ };                 // default by value, but b by reference
[&, a]    (){ };                 // default by reference, but a by value
[this]    (){ return member_; }; // capture the enclosing object's 'this' (by pointer!)
[*this]   (){ return member_; }; // C++17: capture a COPY of *this
[x = a+1] (){ return x; };       // INIT capture (C++14): create a new member 'x'
[p = std::move(uptr)](){ };      // move-capture a unique_ptr
```

```
   BY VALUE  [a]   : a SNAPSHOT taken when the lambda is CREATED.
                     Later changes to the outer 'a' are NOT seen.
   BY REF    [&a]  : an ALIAS. Sees later changes — but DANGLES if 'a' dies first.
```

⚠️ **The #1 lambda bug — dangling reference capture:**

```cpp
std::function<int()> makeCounter() {
    int n = 0;
    return [&n]{ return ++n; };   // ⚠️ n is local; destroyed when function returns
}                                  //    → returned lambda references dead memory (UB)
// FIX: capture by value / init-capture
std::function<int()> makeCounter2() {
    return [n = 0]() mutable { return ++n; };   // ✅ n lives inside the closure
}
```

⚠️ `[=]` does **not** copy `this`'s members — it copies the `this` *pointer* (via
`[this]`). So `[=]` in a member function can still dangle if the object dies. Use
`[*this]` (C++17) to copy the object, or capture needed members explicitly.

---

## 12.4 `mutable`, generic, and `constexpr` lambdas

```cpp
auto counter = [n = 0]() mutable { return ++n; };  // 'mutable' ⇒ operator() is non-const,
counter(); counter();                              //   can modify by-value captures. ⇒ 1,2

auto gen = [](auto x, auto y){ return x + y; };    // GENERIC lambda (C++14): operator() is a template
gen(1, 2); gen(1.5, 2.5); gen("a"s, "b"s);

auto sq = [](int x) constexpr { return x*x; };     // usable in constant expressions
static_assert(sq(4) == 16);

// C++20: explicit template params on lambdas
auto first = []<class T>(const std::vector<T>& v){ return v.front(); };
```

🧠 By-value captures are *const* inside the lambda unless you add `mutable`.
`mutable` changes the *copy* held by the closure, not the original variable.

---

## 12.5 Lambdas + STL algorithms (the everyday use)

```cpp
std::vector<int> v{5,1,4,2,3};
std::sort(v.begin(), v.end(), [](int a, int b){ return a > b; });   // descending
auto n = std::count_if(v.begin(), v.end(), [](int x){ return x>2; });
std::for_each(v.begin(), v.end(), [sum=0](int x) mutable { sum+=x; });

int threshold = 3;
auto big = std::find_if(v.begin(), v.end(),
                        [threshold](int x){ return x > threshold; });
```

This is the dominant reason lambdas exist: inline, named-at-the-point-of-use
predicates and comparators for the algorithm library.

---

## 12.6 `std::function` — type erasure & its cost

A lambda's type is unnamable; `std::function<Sig>` is a polymorphic wrapper that
can hold *any* callable matching the signature.

```cpp
#include <functional>
std::function<int(int)> f;
f = [](int x){ return x + 1; };       // a lambda
f = std::negate<int>{};               // a functor
f = someFreeFunction;                 // a function pointer
int r = f(10);
```

```
   auto lambda            std::function<int(int)>
   ┌───────────────┐       ┌────────────────────────────────┐
   │ concrete type │       │ type-erased wrapper:           │
   │ inlinable     │       │  • may HEAP-ALLOCATE the       │
   │ zero overhead │       │    callable (small-object-opt  │
   │ unnamable     │       │    avoids it for tiny ones)    │
   └───────────────┘       │  • call is an INDIRECT call    │
                           │    (not inlined) — has overhead│
                           └────────────────────────────────┘
```

🧠 Use `auto`/templates for callables in hot paths (zero overhead). Use
`std::function` only when you genuinely need to **store heterogeneous callables**
in one type (callbacks, a `std::vector<std::function<...>>`, plugin tables). C++23
adds `std::move_only_function` (can hold move-only callables, e.g. those capturing
`unique_ptr`) and `std::function_ref` (non-owning).

---

## 12.7 Higher-order patterns

```cpp
// returning a lambda (a closure factory)
auto multiplier(int factor) {
    return [factor](int x){ return x * factor; };   // capture by value (safe to return)
}
auto triple = multiplier(3);
triple(10);   // 30

// std::bind (largely superseded by lambdas — prefer lambdas)
using namespace std::placeholders;
auto add = [](int a, int b){ return a+b; };
auto add10 = std::bind(add, 10, _1);    // ⇒ prefer: auto add10 = [](int b){ return 10+b; };

// composition / pipelines: see ranges (Module 14)
```

🧠 Modern guidance: **prefer lambdas over `std::bind`** — lambdas are clearer,
faster (no placeholder machinery), and easier for the compiler to inline.

---

## 12.8 Immediately-invoked lambda for complex const init

```cpp
const auto config = []{
    Config c;
    c.load("file");
    c.validate();
    return c;                 // run setup logic, then assign to a const
}();                          // ← note the trailing () : invoke right now
```

This lets you initialize a `const` (or member) with multi-step logic while keeping
it `const`. A clean, common expert idiom.

---

## 12.9 Checklist

- [ ] You can mentally rewrite a lambda as its equivalent functor struct.
- [ ] You capture by value by default and know the dangling-reference trap.
- [ ] You understand `mutable`, generic (`auto`), and `constexpr` lambdas.
- [ ] You use `auto`/templates for callables and reserve `std::function` for type erasure.
- [ ] You prefer lambdas over `std::bind`.
- [ ] You use the immediately-invoked-lambda idiom for complex `const` init.

➡ Next: [Module 13 — Exceptions & Error Handling](13_exceptions_error_handling.md)
