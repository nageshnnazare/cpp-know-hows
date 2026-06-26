# Cheatsheet — Initialization Forms

```cpp
int a = 5;        // copy initialization
int b(5);         // direct initialization
int c{5};         // direct-list (brace)  ← PREFER (rejects narrowing, no garbage)
int d = {5};      // copy-list
int e{};          // value init ⇒ 0
int f;            // default init ⇒ GARBAGE at block scope ⚠️ (zero for statics)
auto g = 5;       // type deduced (int)
```

## Why prefer braces `{}`
- Rejects **narrowing**: `int x{3.14};` is an error; `int x = 3.14;` silently truncates.
- **Value-initializes**: `int e{};` is 0, never garbage.
- Uniform across types (members, arrays, containers, aggregates).

## The two big brace traps ⚠️
```cpp
std::vector<int> v1(5);     // 5 elements, all 0
std::vector<int> v2{5};     // ONE element: 5     (initializer_list ctor wins!)
std::vector<int> v3(5, 1);  // 5 elements, all 1
std::vector<int> v4{5, 1};  // TWO elements: 5, 1

Widget w();                 // MOST VEXING PARSE: declares a function, not an object
Widget w;                   // ✅ object
Widget w{};                 // ✅ object
```

## Member initialization (classes)
```cpp
struct S {
    int x = 0;                       // default member initializer
    const int y;                     // MUST use ctor init list
    int& r;                          // MUST use ctor init list
    S(int a, int b) : y(a), r(globalRef) , x(b) {}
    // ⚠️ members init in DECLARATION order (x, y, r), not init-list order
};
```

## constexpr / const family
```cpp
const int     k = f();       // immutable; init may be runtime
constexpr int n = 10;        // must be a constant expression
consteval int ce(int);       // C++20: must run at compile time
constinit int g = ce(3);     // C++20: guaranteed compile-time init (still mutable)
```

## Aggregate / designated (C++20)
```cpp
struct P { int x, y, z; };
P p{1, 2, 3};
P q{.x = 1, .z = 3};         // designated initializers; .y == 0
```

## Rule of thumb
```
   Use {} everywhere EXCEPT:
   • when you specifically want a non-list constructor (e.g. vector(size))
   • auto x{...} pitfalls — auto x{1} is int (since C++17), auto x{1,2} is an error
```
