# Appendix D — Type Deduction: The Complete Reference

> `auto`, `decltype`, `decltype(auto)`, template argument deduction, forwarding
> references, and CTAD all "figure out a type". They follow **related but distinct**
> rules. Getting them exactly right is a hallmark of expertise (Scott Meyers,
> *Effective Modern C++*, Items 1–4). `[Primer §2.5.2, §16.2; standard [temp.deduct]]`

---

## D.1 Template argument deduction — three cases

For `template<class T> void f(ParamType param);` called as `f(expr)`, deduction of
`T` depends on the **form of `ParamType`**.

### Case 1 — ParamType is a reference/pointer (but NOT a forwarding ref): `T&`, `const T&`, `T*`

```
   • If expr is a reference, IGNORE the reference-ness.
   • Then PATTERN-MATCH expr's type against ParamType to find T.
   • const is PRESERVED.
```

```cpp
template<class T> void f(T& p);
int x = 1; const int cx = x; const int& rx = x;
f(x);    // T=int,        param=int&
f(cx);   // T=const int,  param=const int&     (const kept)
f(rx);   // T=const int,  param=const int&     (ref stripped, const kept)

template<class T> void g(const T& p);
g(x);    // T=int,        param=const int&
g(cx);   // T=int,        param=const int&     (const "absorbed" by the const in ParamType)
```

### Case 2 — ParamType is a forwarding (universal) reference: `T&&`

```
   • If expr is an LVALUE: T is deduced as an LVALUE REFERENCE (T = U&),
     and reference collapsing makes param an lvalue ref.
   • If expr is an RVALUE: the normal Case-1 rules apply (T = U, param = U&&).
   ⇒ This is the ONLY case where T can be deduced as a reference type.
```

```cpp
template<class T> void f(T&& p);    // forwarding reference
int x = 1; const int cx = x;
f(x);    // x is lvalue → T=int&,        param=int&     (collapsed)
f(cx);   // lvalue      → T=const int&,  param=const int&
f(5);    // rvalue      → T=int,         param=int&&
```

### Case 3 — ParamType is by value: `T`

```
   • IGNORE reference-ness of expr.
   • IGNORE top-level const / volatile (you get your own copy).
   • Arrays and functions DECAY to pointers.
```

```cpp
template<class T> void f(T p);
const int cx = 1; const int& rx = cx;
f(cx);   // T=int  (const dropped — it's a copy)
f(rx);   // T=int  (ref & const dropped)
int arr[5]; f(arr);   // T=int*  (array decays)
void g(); f(g);       // T=void(*)()  (function decays)
```

### Array & function deduction subtlety

```cpp
template<class T> void byVal(T a);    // T = int*   (decay)
template<class T> void byRef(T& a);   // T = int[5] (NO decay!) — can read the size!
template<class T, std::size_t N>
constexpr std::size_t arraySize(T(&)[N]) noexcept { return N; }   // deduce array length
int data[42]; static_assert(arraySize(data) == 42);
```

🧠 By-reference parameters **preserve array/function types** (no decay); by-value
parameters decay. This is how `std::size`/`std::ssize` and `arraySize` recover an
array's length.

---

## D.2 `auto` deduction — same rules as templates (with ONE exception)

```
   auto  x = expr;        behaves like template Case 3 (by value): strips ref + top-const, decays.
   auto& x = expr;        like Case 1 (by ref): keeps const, no decay.
   auto&& x = expr;       like Case 2 (forwarding ref): lvalue→T&, rvalue→T&&.
   const auto x = expr;   adds const.
   auto* p = expr;        deduces pointer type.
```

```cpp
int x = 1; const int cx = x; const int& rx = x;
auto  a = cx;     // int            (top-const dropped)
auto& b = cx;     // const int&     (const kept)
auto&& c = x;     // int&           (lvalue → forwarding ref)
auto&& d = 5;     // int&&          (rvalue)
const auto e = x; // const int
auto  arr2 = "hi";// const char*    (array decays)
```

### THE EXCEPTION — braced initializers

```cpp
auto a = {1, 2, 3};   // std::initializer_list<int>   (auto special rule)
auto b = {1};         // std::initializer_list<int>
auto c{1};            // int       (C++17; single element)
// template<class T> void f(T);  f({1,2,3});  // ❌ template deduction CANNOT deduce
                                              //    initializer_list — this is the difference
```

🧠 **`auto` deduces a braced list as `std::initializer_list`; template deduction
refuses to deduce it at all.** This is the one place where `auto` and template
deduction diverge.

---

## D.3 `decltype` — the type "as declared"

`decltype(e)` reports the *declared* type of an entity or the type+value-category
of an expression — **without** stripping references or const, and **without
evaluating** `e`.

```cpp
int x = 0; const int cx = 1; const int& rx = cx; int arr[3]; struct S{ int m; };
decltype(x)       // int
decltype(cx)      // const int
decltype(rx)      // const int&        (reference KEPT — unlike auto)
decltype(arr)     // int[3]            (NO decay)
decltype(S{}.m)   // int               (named member → declared type)
decltype((x))     // int&  ⚠️ PARENTHESES: (x) is an lvalue EXPRESSION → adds &
decltype(x + 0)   // int               (prvalue expression)
decltype(std::move(x)) // int&&        (xvalue)
```

```
   decltype(name)   → the DECLARED type of the entity (keeps ref/const, no decay).
   decltype(expr)   → depends on value category of the expression:
        prvalue  →  T
        lvalue   →  T&     ← note: decltype((x)) is int&, decltype(x) is int!
        xvalue   →  T&&
   decltype does NOT evaluate its operand (unevaluated context).
```

---

## D.4 `decltype(auto)` — perfect type preservation

```cpp
int  x = 1;
auto            a = (x);   // int    (auto strips, drops the ref)
decltype(auto)  b = (x);   // int&   (decltype(auto) applies decltype rules to the init)
decltype(auto)  c = x;     // int
decltype(auto)  d = std::move(x); // int&&
```

Its killer use is **perfect-forwarding return types** in generic wrappers:

```cpp
template<class Container, class Index>
decltype(auto) access(Container&& c, Index i) {
    return std::forward<Container>(c)[i];   // returns EXACTLY what operator[] returns,
}                                            // preserving T& / const T& / T&& correctly
// 'auto' here would return by VALUE (a copy); decltype(auto) keeps the reference.
```

⚠️ Beware `decltype(auto)` returning a reference to a local or to a parenthesized
local (`return (local);` → returns `T&` to a dead object). UB (Module 16).

---

## D.5 Forwarding references & reference collapsing

```
   REFERENCE COLLAPSING RULES (only the compiler forms references-to-references):
     T&  &   → T&        T&  &&  → T&
     T&& &   → T&        T&& &&  → T&&
   "if either is an lvalue ref, result is lvalue ref; else rvalue ref."

   A FORWARDING reference is precisely:  template<class T> ... (T&& x)
   or  auto&& x = ...;   (deduced context, exactly the form T&&).
   NOT a forwarding reference:  const T&& , std::vector<T>&& , a non-deduced T&&.
```

```cpp
template<class T> void relay(T&& x) {
    callee(std::forward<T>(x));   // T&& + std::forward<T> = perfect forwarding
}
// relay(lvalue):  T=U&  → forward<U&> casts to lvalue
// relay(rvalue):  T=U   → forward<U>  casts to rvalue
```

🧠 `std::move` unconditionally casts to rvalue; `std::forward<T>` casts to rvalue
**only if `T` was deduced from an rvalue**. Use `forward` exclusively with
forwarding references; use `move` for concrete rvalue refs / when you're done with a value.

---

## D.6 CTAD — class template argument deduction (recap)

```cpp
std::pair p{1, 2.0};          // std::pair<int, double>
std::vector v{1, 2, 3};       // std::vector<int>
std::lock_guard g{m};         // std::lock_guard<std::mutex>
```

CTAD deduces a **class** template's arguments from constructor calls, optionally
guided by **deduction guides** (Module 09 §9.13). It uses its own overload set of
synthesized "deduction functions"; it is **not** the same as function template
argument deduction, though it reuses much of the machinery.

---

## D.7 `auto` return types & abbreviated function templates

```cpp
auto add(int a, int b) { return a + b; }            // C++14: deduced return (by value rules)
auto& front(std::vector<int>& v){ return v[0]; }    // deduced reference return
decltype(auto) at(std::vector<int>& v, int i){ return v[i]; }  // exact category

void f(auto x);                                     // C++20 abbreviated template = template<class T> void f(T)
auto g(std::integral auto x);                        // constrained abbreviated template
```

```
   auto            return  → uses by-value (template Case 3) rules: strips ref/const.
   decltype(auto)  return  → uses decltype rules: preserves ref/const exactly.
   trailing 'auto f() -> T' → write the return type after the params (needed when it
                              depends on the params via decltype).
```

---

## D.8 Quick comparison table

```
   INITIALIZER expr type:  int lvalue 'x' | const int 'cx' | const int& 'rx' | rvalue 5
   ───────────────────────────────────────────────────────────────────────────────────
   auto          v = e     int            | int            | int               | int
   auto&         v = e     int&           | const int&     | const int&        | (ill-formed)
   auto&&        v = e     int&           | const int&     | const int&        | int&&
   const auto&   v = e     const int&     | const int&     | const int&        | const int&
   decltype(e)             int            | const int      | const int&        | int  (prvalue)
   decltype((e))           int&           | const int&     | const int&        | int  (prvalue)
   decltype(auto) v=e      (same as decltype of the initializer expression)
```

➡ Related: [Module 01 §1.6](01_types_variables_operators.md), [Module 06 §6.7](06_copy_move_semantics.md), [Module 09 §9.2](09_templates_generic_programming.md), [Appendix A — Initialization](A1_initialization_reference.md)
