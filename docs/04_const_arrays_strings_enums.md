# Module 04 — const/constexpr, Arrays, Strings, Enums & Unions

> This module collects the "data" features: immutability (`const`/`constexpr`/
> `consteval`/`constinit`), the array story (raw arrays → `std::array`/`vector`),
> the string story (`char*` → `std::string`/`string_view`), and the enum/union
> machinery. `[Primer §2.4, §3.5, §19.3]`

---

## 4.1 `const` — the contract of immutability

`const` means "I promise not to modify this *through this name*". It is checked at
compile time and is a cornerstone of safe APIs.

```cpp
const int n = 10;          // n cannot be reassigned
int arr[n];                // (n is a constant expression here)

void f(const std::string& s);   // s read-only inside f
```

### Top-level vs low-level const

```
   int* const p;      ← TOP-LEVEL const: the pointer itself is const
   const int* p;      ← LOW-LEVEL  const: what it points to is const

   • Top-level const is DROPPED in copies / template/auto deduction.
   • Low-level const is PART of the type and is preserved.
```

```cpp
const int  ci = 1;
auto x = ci;         // int  (top-level const dropped)
const int* pc = &ci;
auto y = pc;         // const int*  (low-level const kept)
```

🧠 Const-correctness propagates: a `const` member function can only call other
`const` member functions and can't modify members (except `mutable` ones). Design
classes so that "observers" are `const` — it documents intent and enables
optimizations and use from `const` contexts.

---

## 4.2 `constexpr`, `consteval`, `constinit` — the compile-time family

```
   const       value won't change after init (run-time or compile-time init)
   constexpr   usable in a CONSTANT EXPRESSION; init at compile time if possible
   consteval   (C++20) function that MUST be evaluated at compile time
   constinit   (C++20) variable MUST be initialized at compile time
               (kills the static-init-order fiasco) — but is still mutable
```

```cpp
constexpr int square(int x){ return x*x; }
constexpr int a = square(5);          // 25 at compile time
static_assert(square(4) == 16);       // compile-time check

consteval int id(int x){ return x; }
constexpr int b = id(7);              // ok, compile-time
// int c = id(runtime());             // ❌ consteval needs constant

constinit int g = square(3);          // guaranteed compile-time init; g is mutable
```

What `constexpr` functions may do (rules relaxed each standard):
- C++11: a single `return`. C++14: loops, locals, `if`. C++20: `try/catch` (no
  throw at compile time), `new`/`delete` (transient), virtual calls, `std::vector`
  and `std::string` usable at compile time(!).

🧠 A `constexpr` object **must** be initialized by a constant expression; a
`constexpr` *function* merely *can* be. `if constexpr` (C++17) discards the
not-taken branch at compile time — essential for templates (Module 09).

---

## 4.3 Raw arrays — the C inheritance and its pitfalls

```cpp
int a[5] = {1,2,3};      // a[3]=a[4]=0 (rest value-initialized)
int b[]  = {1,2,3};      // size deduced ⇒ 3
char s[] = "hi";         // {'h','i','\0'} ⇒ size 3
int m[2][3] = {{1,2,3},{4,5,6}};   // row-major 2D
```

```
   int m[2][3]  laid out CONTIGUOUSLY, row-major:
   ┌────┬────┬────┬────┬────┬────┐
   │ 1  │ 2  │ 3  │ 4  │ 5  │ 6  │   m[i][j] == *(&m[0][0] + i*3 + j)
   └────┴────┴────┴────┴────┴────┘
     m[0]           m[1]
```

⚠️ **Array decay**: an array name converts to a pointer to its first element in
almost every expression. The size is **lost**.

```cpp
void f(int arr[]);        // ⚠️ actually f(int* arr) — sizeof(arr) is sizeof(int*)!
void g(int (&arr)[5]);    // ✅ reference to array of 5 — size preserved, type-safe
```

🧠 Because of decay, you can't pass raw arrays "by value" or know their size in a
function. **This is why `std::array` and `std::vector` exist.** Use
`std::size(arr)` / `std::ssize(arr)` (C++17/20) instead of the
`sizeof(a)/sizeof(a[0])` trick.

### `std::array` — a zero-overhead fixed array

```cpp
#include <array>
std::array<int, 5> a = {1,2,3,4,5};
a.size();          // knows its size; no decay
a.at(10);          // bounds-checked (throws); a[10] is not
for (int x : a) ...// works with range-for, algorithms, etc.
```

Same layout/perf as `int[5]`, but it's a real value type (copyable, returnable,
no decay).

---

## 4.4 Strings: `char*` → `std::string` → `std::string_view`

```
   C STRING                  std::string                std::string_view (C++17)
   const char* / char[]      owns a heap buffer          NON-owning {ptr,len} view
   NUL-terminated            grows, mutates, RAII         no allocation, no NUL needed
   manual length (strlen)    .size() O(1)                 .size() O(1)
   easy to overflow ⚠️       safe, dynamic                ⚠️ can dangle if source dies
```

```cpp
std::string s = "hello";
s += " world";                 // grows automatically
s.size(); s[0]; s.substr(1,3);
s.c_str();                     // NUL-terminated view for C APIs
auto pos = s.find("wor");

std::string_view sv = s;       // cheap view, no copy
void log(std::string_view m);  // ideal read-only string param: takes string, char*, literal
```

### Small String Optimization (SSO) 🧠

```
   std::string for a short value ("hi"):
   ┌────────────────────────────────────────────┐
   │ [ inline buffer ~15 chars ] [size] [flag]  │  ← stored INSIDE the object,
   └────────────────────────────────────────────┘     NO heap allocation!
   std::string for a long value:
   ┌────────────────────────────────────────────┐
   │ [ ptr → heap ] [size] [capacity]           │  ← heap buffer
   └────────────────────────────────────────────┘
```

Most implementations store ~15–22 chars inline before allocating. This is why
small strings are fast and why `sizeof(std::string)` is ~24–32 bytes.

⚠️ `string_view` does **not** own; never return a `string_view` to a temporary:

```cpp
std::string_view bad() { return std::string("temp"); }  // ⚠️ dangles immediately
```

⚠️ A `string_view` is **not** guaranteed NUL-terminated; don't pass `sv.data()` to
a C function expecting NUL termination.

---

## 4.5 Enums — unscoped vs scoped

```cpp
enum Color { Red, Green, Blue };          // UNSCOPED (C-style)
enum class Dir { North, South, East };    // SCOPED (C++11) — PREFER THIS

Color c = Green;          // names leak into enclosing scope
int  i = Green;           // ⚠️ implicitly converts to int (1)

Dir  d = Dir::North;      // must qualify
// int j = Dir::North;    // ❌ no implicit conversion (good!)
int  j = static_cast<int>(Dir::North);
```

```
   enum Color { ... }          enum class Dir { ... }
   ─────────────────           ───────────────────────
   names in enclosing scope    names scoped to Dir::
   implicit → int  ⚠️          NO implicit conversion ✅
   underlying type unspecified can specify: enum class E : std::uint8_t {...}
```

Control the underlying type and use enums as flags:

```cpp
enum class Perm : unsigned { Read = 1, Write = 2, Exec = 4 };
// define bitwise operators to combine (operator overloading, Module 08)
```

🧠 C++20 `using enum`:
```cpp
void f(Dir d){
    using enum Dir;       // bring enumerators into scope locally
    if (d == North) ...   // no Dir:: needed inside this block
}
```

---

## 4.6 Unions & the type-safe alternative

A **union** stores one of several members in the *same* memory; only one is
active at a time.

```cpp
union U { int i; float f; char bytes[4]; };  // sizeof(U) == 4 (largest member)
U u; u.i = 1065353216; // reading u.f now is type-punning...
```

```
   union U overlays all members at the same address:
   ┌────────────────────────────┐
   │  i (int)   ░░ same 4 bytes │
   │  f (float) ░░ same 4 bytes │
   │  bytes[4]  ░░ same 4 bytes │
   └────────────────────────────┘
```

⚠️ Reading a non-active union member is **type punning** and is UB in C++ (it's
*defined* in C). The legal way to reinterpret bytes is `std::bit_cast` (C++20) or
`memcpy`:

```cpp
float f = 1.0f;
auto bits = std::bit_cast<std::uint32_t>(f);   // ✅ well-defined reinterpretation
```

🧠 For a value that is "one of several types", use **`std::variant`** (C++17): a
type-safe tagged union with `std::visit`, `std::get`, `std::holds_alternative`.
Raw unions are for low-level/ABI code only.

```cpp
std::variant<int, std::string, double> v = 42;
v = "now a string";
std::visit([](auto&& x){ std::cout << x; }, v);   // type-safe dispatch
```

---

## 4.7 `using`, `typedef`, and type aliases

```cpp
typedef unsigned long ulong;        // old style
using ulong = unsigned long;        // C++11 — reads left-to-right, PREFER
using Fn    = int(*)(int, int);     // function pointer alias (much clearer)
template<class T> using Vec = std::vector<T>;   // alias TEMPLATE (typedef can't do this)
Vec<int> v;                         // == std::vector<int>
```

---

## 4.8 `std::optional`, `std::pair`, `std::tuple` (data plumbing)

```cpp
std::optional<int> parse(std::string_view s);  // "maybe an int" — better than sentinel/-1
if (auto r = parse(s)) use(*r);                 // contextually-bool, deref to get value

std::pair<int,std::string> p{1,"a"};
auto [id, name] = p;                            // structured bindings (C++17)

std::tuple<int,double,char> t{1, 2.0, 'c'};
auto [a, b, c] = t;
std::get<1>(t);                                 // 2.0
```

🧠 Prefer `std::optional<T>` over magic sentinels (`-1`, `nullptr`, empty string)
to express "no value". Prefer structured bindings over `.first/.second/get<>`.

---

## 4.9 Checklist

- [ ] You distinguish top-level vs low-level const and design const-correct APIs.
- [ ] You know the `const`/`constexpr`/`consteval`/`constinit` distinctions.
- [ ] You understand array decay and default to `std::array`/`std::vector`.
- [ ] You use `std::string_view` for read-only params and know its dangling trap.
- [ ] You prefer `enum class`, and `std::variant` over raw unions.

➡ Next: [Module 05 — Classes: Construction & Lifetime](05_classes_construction_lifetime.md)
