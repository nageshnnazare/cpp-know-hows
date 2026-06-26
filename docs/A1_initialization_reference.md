# Appendix A — Initialization: The Complete Reference

> Initialization in C++ has **a dozen named forms** and subtle rules that decide
> whether you get zeros, garbage, a narrowing error, an `initializer_list`
> hijack, or the most vexing parse. This is the exhaustive reference.
> `[Primer §2.2, §3.3, §7.5; standard [dcl.init]]`

---

## A.1 The taxonomy of initialization

```
                              INITIALIZATION
   ┌─────────────────────────────────┼─────────────────────────────────────────┐
   default-init     value-init     direct-init    copy-init    list-init   aggregate-init
   T x;             T x{};         T x(a);        T x = a;     T x{a,b};   T x{m1,m2}
   (may be garbage) (zeros/ctor)   (ctor, ())     (= )         ({})        (members)
                                                                │
                                                   ┌────────────┴───────────┐
                                            direct-list-init          copy-list-init
                                            T x{...}                  T x = {...}
```

---

## A.2 Default initialization — `T x;`

```cpp
int  a;            // ⚠️ INDETERMINATE (garbage) at block scope; reading it is UB
int  g;            // (namespace/static scope) → zero-initialized to 0
std::string s;     // calls the default constructor → empty string (well-defined)
int arr[3];        // block scope: 3 indeterminate ints
struct P{int x;}; P p;   // p.x indeterminate (trivial default ctor does nothing)
```

```
   DEFAULT-INIT  T x;  (no initializer):
   • class type with a user/defaulted default ctor → that ctor runs.
   • built-in / trivial type at BLOCK scope        → INDETERMINATE value (UB to read).
   • built-in / trivial type at STATIC/namespace/thread scope → zero-initialized.
   • const object of a type with a trivial default ctor → ❌ ill-formed (must init).
```

⚠️ The single most common beginner-to-intermediate bug: `int sum;` in a function,
then `sum += x`. The initial value is garbage.

---

## A.3 Value initialization — `T x{};` / `T()`

```cpp
int  a{};                  // 0
int  b = int();            // 0
std::string s{};           // empty string (default ctor)
struct P{int x;}; P p{};   // p.x == 0 (zero-initialized, then trivial ctor)
int arr[3]{};              // {0,0,0}
auto* q = new int();       // points to a 0;   new int (without ()) → indeterminate
auto* r = new int[5]();    // 5 zeros
```

```
   VALUE-INIT  T{}  / T():
   • has a user-provided default ctor      → that ctor runs.
   • has only an implicit/defaulted ctor   → object is ZERO-initialized first,
                                             then the (trivial) ctor runs.
   • array                                  → each element value-initialized (zeros).
   ⇒ Value-init NEVER leaves built-in members as garbage. Prefer {} to avoid UB.
```

🧠 `T()` and `T{}` value-initialize; `T x;` default-initializes. For a class with a
user-provided default ctor they're identical, but for trivial/aggregate types the
difference is **zeros vs garbage**.

---

## A.4 Direct initialization — `T x(args);`

```cpp
int  a(5);                       // 5
std::string s(3, 'x');           // "xxx"  (calls string(size_t, char))
Widget w(1, 2);                  // calls Widget(int,int)
auto p = new Widget(1, 2);       // direct-init on the heap
```

```
   DIRECT-INIT  T x(args)  uses () and considers ALL constructors via overload
   resolution (including explicit ones). It does NOT prefer initializer_list ctors.
```

⚠️ **The most vexing parse**: `Widget w();` declares a **function** `w` returning
`Widget`, not an object. `Widget w(SomeType());` is also a function declaration.
Fix with braces: `Widget w{};` / `Widget w{SomeType{}};`.

---

## A.5 Copy initialization — `T x = a;`

```cpp
int  a = 5;                      // copy-init
std::string s = "hello";         // copy-init via converting ctor (const char* → string)
Widget w = 5;                    // ❌ if Widget(int) is explicit; ✅ if not
Widget w2 = Widget(5);           // copy-init from a prvalue (elided since C++17)
void f(std::string);  f("hi");   // argument passing is copy-init
return expr;                     // return is copy-init (then (N)RVO)
```

```
   COPY-INIT  T x = expr :
   • considers converting (non-explicit) constructors and conversion operators.
   • EXPLICIT constructors are NOT considered → explicit ctors can't be used here.
   • since C++17, T x = T(args) performs NO copy/move (guaranteed elision).
```

🧠 The practical difference between direct and copy init: **`explicit`
constructors are usable in direct-init `T x(a)` / `T x{a}` but NOT in copy-init
`T x = a`**. This is the whole point of `explicit`.

---

## A.6 List initialization — `T x{...}` and `T x = {...}`

The C++11 "uniform initialization". Two sub-forms:

```cpp
T x{a, b};        // direct-list-init  (explicit ctors allowed)
T x = {a, b};     // copy-list-init    (explicit ctors NOT allowed)
```

```
   LIST-INIT resolution order (CRUCIAL):
   1. If T is an aggregate → aggregate initialization (member by member).
   2. Else if the braced list is empty {} → value-initialization.
   3. Else if T has an initializer_list constructor that the list matches →
        that initializer_list ctor WINS (even over a "better" ctor!).
   4. Else → normal overload resolution over all constructors using the list args.
   5. NARROWING conversions in the list are ILL-FORMED (compile error).
```

### The initializer_list hijack ⚠️

```cpp
std::vector<int> a(5);     // FIVE zero ints          (direct-init, (size))
std::vector<int> b{5};     // ONE int with value 5    (initializer_list wins!)
std::vector<int> c(5, 1);  // five 1s                 ((size,value))
std::vector<int> d{5, 1};  // TWO ints: 5 and 1       (initializer_list wins!)

std::string e(3, 'x');     // "xxx"
std::string f{3, 'x'};     // ⚠️ a 2-char string {char(3), 'x'} — surprise!
```

### Narrowing is rejected

```cpp
int   x{3.14};     // ❌ error: narrowing double → int
int   y = 3.14;    // ⚠️ allowed: silently truncates to 3
char  c{300};      // ❌ error: 300 doesn't fit in char (if it's a constant)
unsigned u{-1};    // ❌ error: narrowing signed → unsigned
```

```
   NARROWING conversions forbidden in braces:
   • floating → integer ;  long double → double ;  larger int → smaller int
   • signed → unsigned (and vice versa) when the value can't be represented
   This is a MAJOR safety reason to prefer {} initialization.
```

---

## A.7 Aggregate initialization

```cpp
struct Point { int x, y, z; };
Point a{1, 2, 3};          // x=1 y=2 z=3
Point b{1};                // x=1 y=0 z=0  (missing members value-initialized)
Point c{};                 // all zero
Point d{.x=1, .z=3};       // designated initializers (C++20); y=0; must be in order

int arr[]{1, 2, 3};        // arrays are aggregates
struct Derived : Point { int w; };   // C++17: aggregates may have public bases
Derived e{{1,2,3}, 4};     // base then member
```

```
   AGGREGATE = array, or class with: no user-declared/inherited ctors, no private/
   protected non-static data members, no virtual functions, no private/virtual bases.
   Members are initialized in DECLARATION order; extras → value-init.
   No constructor is involved at all.
```

---

## A.8 Reference initialization

```cpp
int x = 1;
int&        lr  = x;                 // lvalue ref binds an lvalue
const int&  cr  = 5;                 // const lvalue ref binds a temporary → LIFETIME EXTENDED
int&&       rr  = 5;                 // rvalue ref binds a temporary → LIFETIME EXTENDED
const int&  cr2 = x;                 // const ref to an lvalue (fine)
// int&     bad = 5;                 // ❌ non-const lvalue ref can't bind a temporary
// int&     bad2 = cr;               // ❌ can't drop const

struct S { const std::string& m; };  // ⚠️ a reference member that binds a temporary in a
S s{std::string("tmp")};             //    ctor does NOT get lifetime extension → dangles!
```

```
   LIFETIME EXTENSION: binding a temporary directly to a const lvalue ref or an
   rvalue ref extends the temporary's life to the reference's lifetime.
   ⚠️ Does NOT extend through: function return values, reference data members,
      or subobjects of a returned object. (See Module 16 §16.3.)
```

---

## A.9 Initialization in special contexts

```cpp
// 1) member initializer list (Module 05 §5.4) — DIRECT-init each member, in
//    DECLARATION order:
struct W { int a; std::string b; W(int x) : a(x), b("hi") {} };

// 2) default member initializers (C++11):
struct C { int n = 0; std::vector<int> v{1,2,3}; };

// 3) static local — initialized ONCE, thread-safe, on first control flow through:
int& counter(){ static int n = 0; return n; }

// 4) static/global — zero-init (static init) THEN dynamic init (ordered within a TU,
//    UNORDERED across TUs → static init order fiasco, Module 02 §2.10):
int g = compute();   // dynamic init

// 5) new-expressions:
new int;        // indeterminate         new int();   // zero       new int{};   // zero
new int[3];     // indeterminate         new int[3]{};// zeros      new T(args); // ctor

// 6) range-for, structured bindings, lambdas init-capture all use these rules.
auto [p, q] = std::pair{1, 2};            // structured binding (copy-init of a hidden object)
```

---

## A.10 `auto` with braces (a notorious corner)

```cpp
auto a = 1;        // int
auto b{1};         // int        (since C++17; was initializer_list<int> in C++11/14)
auto c = {1};      // std::initializer_list<int>
auto d = {1, 2};   // std::initializer_list<int>
auto e{1, 2};      // ❌ error (direct-list with >1 element + auto)
```

```
   RULE (C++17): auto x{single} → the element type (int).
                 auto x = {list} → std::initializer_list<T>.
   Prefer 'auto x = value;' to avoid surprises.
```

---

## A.11 Decision guide

```
   Want zero/empty default?            T x{};        (value-init — never garbage)
   Calling a specific constructor?      T x(args);    (() avoids initializer_list hijack)
   General object init?                 T x{args};    (rejects narrowing, uniform)
   Need an explicit ctor?               T x{args};    (NOT T x = args)
   A container with N copies?           vector<T> v(n, val);   // () not {}!
   A container with these elements?     vector<T> v{a, b, c};  // {} = initializer_list
   Aggregate / struct of data?          Agg a{.field = v, ...};
```

🧠 **Rule of thumb:** prefer `{}` everywhere for the narrowing protection and the
no-garbage guarantee — **except** when you specifically need a non-`initializer_list`
constructor (the classic case: `std::vector<T>(size)`), where you must use `()`.

➡ Related: [Module 01 §1.6](01_types_variables_operators.md), [Module 05 §5.4](05_classes_construction_lifetime.md), [Appendix D — Type Deduction](D1_type_deduction_reference.md)
