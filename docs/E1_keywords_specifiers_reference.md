# Appendix E — Keywords & Specifiers: The Complete Reference

> Every C++ keyword in one place, with deep treatment of the ones that confuse
> even experts: the `const` family, `auto`, `mutable`, `volatile`, `register`,
> the storage-class specifiers, and the keywords that have been **repurposed,
> deprecated, or removed** over the standards. `[Primer §2.4, §6.1.1; standard
> [lex.key], [dcl.stc], [dcl.type.cv]]`

---

## E.0 The four families of "modifiers"

These are constantly conflated. They are **orthogonal** — a declaration can carry
one from several families at once.

```
   ┌─────────────────────────────────────────────────────────────────────────────┐
   │ STORAGE-CLASS specifiers  (lifetime + linkage):                             │
   │     static   extern   thread_local   mutable    [register removed]          │
   │                                                                             │
   │ CV-qualifiers  (mutability/optimization of an object through a name):       │
   │     const     volatile                                                      │
   │                                                                             │
   │ const-EVALUATION specifiers  (when/whether a value is a constant):          │
   │     constexpr   consteval   constinit                                       │
   │                                                                             │
   │ FUNCTION specifiers  (how a function behaves):                              │
   │     inline   virtual   explicit   friend   noexcept                         │
   └─────────────────────────────────────────────────────────────────────────────┘
   e.g.  static constexpr const int& ...   mixes storage-class + const-eval + cv.
```

```
   const  vs  constexpr  vs  consteval  vs  constinit  (memorize this!)
   ─────────────────────────────────────────────────────────────────────────────
   const      : value won't change THROUGH THIS NAME. Init may be at run time.
   constexpr  : may be used in a constant expression; object MUST be const-init;
                function MAY run at compile time (or run time).
   consteval  : (C++20) function MUST run at compile time (immediate function).
   constinit  : (C++20) variable MUST be const-initialized (no init-order fiasco),
                but is still MUTABLE afterwards.
```

---

## E.1 `const` — the cv-qualifier of immutability

```cpp
const int n = 10;                 // n cannot be reassigned (init may be runtime)
int       x = const_fn();         // const_fn returns by const? no — top-level const on
const int y = runtime();          //   a return value is dropped for prvalues

const int* pc;     // LOW-LEVEL  const: *pc is read-only; pc itself is reassignable
int* const cp = &x;// TOP-LEVEL  const: cp fixed; *cp writable
const int* const z = &n;          // both
// read RIGHT to LEFT:  cp is a 'const pointer to int'.
```

```
   TOP-LEVEL const   : the object itself is const  (int* const, const int)
                       → DROPPED in copies, by-value params, auto/template deduction.
   LOW-LEVEL const    : what is pointed/referred to is const (const int*, const int&)
                       → PART of the type; preserved everywhere.
```

### const member functions

```cpp
class Cache {
    mutable int hits_ = 0;        // mutable: writable even in a const member (E.4)
    int value_ = 0;
public:
    int get() const { ++hits_; return value_; }  // 'this' is const Cache*
    void set(int v) { value_ = v; }              // non-const
};
```

```
   • A const member fn cannot modify members (except 'mutable' ones) and can only
     call other const member fns.
   • const and non-const overloads coexist; the object's constness selects one:
        int&       at(size_t);
        const int& at(size_t) const;
   • A const object may ONLY call const member functions.
```

### const-correctness rules of thumb 🧠

```
   ✅ Take read-only parameters as  const T&  (or by value for cheap types).
   ✅ Make every observer member function const.
   ✅ Return by const value rarely (it disables move on the result) — prefer plain T.
   ⚠️ const_cast-ing away const then WRITING is UB if the object was truly const.
   ⚠️ 'const' on a by-value parameter (void f(const int x)) only affects the body;
       it is NOT part of the function's type / signature (ignored for overloading).
```

Cross-references: [Module 01 §1.9](01_types_variables_operators.md),
[Module 03 §3.2/3.8](03_pointers_references_memory.md),
[Module 04 §4.1](04_const_arrays_strings_enums.md).

---

## E.2 `constexpr` / `consteval` / `constinit` — compile-time control

```cpp
constexpr int square(int x) { return x * x; }   // CAN be compile-time
constexpr int a = square(5);                     // IS compile-time (25)
int b = square(runtime());                       // same fn, run time — fine

consteval int must(int x) { return x + 1; }      // MUST be compile-time
constexpr int c = must(7);                        // ok
// int d = must(runtime());                       // ❌ needs a constant

constinit int g = square(3);                      // guaranteed compile-time INIT; mutable
// thread_local constinit T t = ...;              // common pairing (no dynamic init)
```

```
   constexpr OBJECT  : must be initialized by a constant expression; implicitly const.
   constexpr FUNCTION: usable in constant expressions IF its args are constants;
                       otherwise runs at run time. (Rules relaxed each standard:
                       C++14 loops/locals, C++20 try/new/virtual/vector/string.)
   consteval FUNCTION: an "immediate function" — every call must produce a constant.
   constinit VARIABLE: forces static/thread init to be a constant expression
                       (kills the static-init-order fiasco) but stays modifiable.
   std::is_constant_evaluated() / if consteval (C++23): branch on the context.
```

🧠 `constexpr` on a function is a *capability*, not a *guarantee*; `consteval`
is the guarantee. `constexpr` on an object IS a guarantee of compile-time init.
A `constexpr` member function is **not** implicitly `const` since C++14.

Cross-references: [Module 02 §2.8](02_control_flow_and_functions.md),
[Module 04 §4.2](04_const_arrays_strings_enums.md).

---

## E.3 `auto` — placeholder type (and its removed old meaning)

```cpp
auto i = 5;             // int          (template-Case-3 deduction: strips ref/top-const, decays)
auto& r = i;            // int&
const auto ci = i;      // const int
auto* p = &i;           // int*
auto&& fwd = i;         // int&         (forwarding ref: lvalue → T&)
auto  s = "hi";         // const char*  (array decays)
auto  l = {1, 2, 3};    // std::initializer_list<int>   (the auto-only special rule!)

auto add(int a, int b) { return a + b; }         // C++14 deduced return type
auto f() -> int;                                  // trailing return type
decltype(auto) g(std::vector<int>& v){ return v[0]; }  // preserves ref/const exactly
void h(auto x);                                   // C++20 abbreviated function template
template<auto N> struct Fixed {};                 // C++17 'auto' non-type template parameter
Fixed<42> f1;  Fixed<'c'> f2;                      //   N's type is deduced per use
```

```
   auto deduction == template by-value deduction, with ONE exception:
     auto x = {…};  → std::initializer_list   (template deduction REFUSES braced lists).
   auto STRIPS:  top-level const/volatile + references; arrays/functions DECAY.
   To KEEP them: use auto&, const auto&, auto&&, or decltype(auto).
```

⚠️ **Historical note:** before C++11, `auto` was a (useless) *storage-class
specifier* meaning "automatic storage" (the default for locals). C++11
**repurposed** it for type deduction. You will never write the old `auto int x;`.

Full rules: [Appendix D — Type Deduction](D1_type_deduction_reference.md).

---

## E.4 `mutable` — TWO unrelated meanings 🧠

```cpp
// MEANING 1: a data member modifiable even through a const object / const member fn
class Logger {
    mutable std::mutex m_;          // locking a mutex in a const method is logically const
    mutable int reads_ = 0;
    int data_ = 0;
public:
    int read() const {              // const observer...
        std::lock_guard lk(m_);     // ...yet modifies m_ and reads_ (both mutable)
        ++reads_;
        return data_;
    }
};

// MEANING 2: makes a lambda's call operator non-const ⇒ by-value captures are modifiable
auto counter = [n = 0]() mutable { return ++n; };   // without 'mutable', ++n is an error
```

```
   mutable DATA MEMBER : exempt from a const object's const-ness. Use ONLY for
                         "logically const" state: caches, memoization, mutexes, stats.
   mutable LAMBDA      : operator() becomes non-const, so it may mutate its OWN copies
                         of by-value captures (does NOT affect the outer variables).
   ⚠️ mutable does NOT apply to references or static members.
```

Cross-references: [Module 01 §1.10](01_types_variables_operators.md),
[Module 12 §12.4](12_lambdas_functional.md).

---

## E.5 `volatile` — for hardware, NOT threads (and C++20 deprecations)

```cpp
volatile int* reg = (int*)0x40021000;   // memory-mapped I/O register
*reg = 1;                                // each read/write actually happens, in order,
int v = *reg;                            //   and is NOT optimized away or reordered (by the compiler)
```

```
   volatile tells the COMPILER: "this object may change/observe outside the program's
   knowledge (hardware register, signal handler same-thread). Don't elide or reorder
   accesses to it." It does NOT provide:
     • atomicity            (a volatile ++ is still a non-atomic read-modify-write)
     • inter-thread ordering / visibility (CPU may still reorder)
   ⚠️ volatile is NOT a threading tool. Use std::atomic (Module 15) for threads.
```

🧠 **C++20 deprecated most `volatile` uses**: compound assignment on volatile
(`v += 1`), pre/post `++`/`--` on volatile, `volatile`-qualified function
parameters and return types, and structured bindings of volatile. Plain
volatile loads/stores for MMIO remain valid. Don't add `volatile` unless you're
doing hardware/signal work.

Cross-references: [Module 01 §1.10](01_types_variables_operators.md),
[Module 15 §15.6](15_concurrency_memory_model.md).

---

## E.6 `register` — removed (but still reserved)

```cpp
// register int x = 5;   // ❌ C++17 removed 'register' as a storage class.
```

```
   • Pre-C++11: a hint "keep in a CPU register" (compilers ignored it; they optimize
     allocation far better than a human hint).
   • C++11: deprecated.
   • C++17: REMOVED as a storage-class specifier. The keyword is still RESERVED
     (you may not use it as an identifier), reserved for possible future use.
   ⇒ Never use it. Modern optimizers handle register allocation.
```

---

## E.7 Storage-class specifiers: `static`, `extern`, `thread_local`

```cpp
static int file_local = 0;     // namespace scope: INTERNAL linkage (TU-private)
void f() { static int calls = 0; ++calls; }  // local: ONE instance, lifetime = program,
                                              //   thread-safe init on first use (C++11)
class C { static int shared_; static void m(); };  // class member: one per class, no 'this'
static inline int s = 0;       // C++17 inline static member: define in-class, ODR-mergeable

extern int g_counter;          // DECLARATION only: "defined elsewhere", external linkage
extern const int K;            // share a const across TUs (const is internal by default!)
extern "C" void c_api(int);    // language linkage: no name mangling (C interop)

thread_local int tls = 0;      // one independent instance PER thread
```

```
   static (namespace/file) : internal linkage; prefer ANONYMOUS NAMESPACE in C++.
   static (local)          : static storage duration; init once, thread-safe (Meyers).
   static (member)         : shared across instances; no 'this'.
   extern                  : declares without defining; default linkage is external;
                             also 'extern "C"' for C language linkage (no mangling),
                             and 'extern template' to suppress instantiation (Mod 09 §9.14).
   thread_local            : per-thread storage; may combine with static/extern.
```

🧠 At namespace scope, a non-`const` variable has **external** linkage; a `const`/
`constexpr` one has **internal** linkage by default (a C++ quirk). To share one
constant across TUs use `inline constexpr` (C++17) or `extern const` + one definition.

Cross-references: [Module 00 §0.5](00_toolchain_and_compilation.md),
[Module 05 §5.10](05_classes_construction_lifetime.md).

---

## E.8 Function specifiers: `inline`, `explicit`, `virtual`, `friend`, `noexcept`

```cpp
inline int sq(int x){ return x*x; }     // ODR: definition may appear in many TUs (its REAL job)
inline int counter = 0;                  // C++17 inline VARIABLE (one shared definition in a header)

struct B {
    explicit B(int);                     // no implicit int → B conversion
    explicit operator bool() const;      // no implicit; but contextual-bool still works
    explicit(sizeof(int)>2) B(double);   // C++20 conditional explicit
    virtual void f();                    // dynamic dispatch (vtable)
    virtual void g() = 0;                // pure virtual ⇒ abstract
    void h() noexcept;                   // promises not to throw
    friend B operator+(const B&, const B&);  // grants private access (not a member)
};
```

```
   inline   : ODR-mergeable definition (NOT really "please inline this"). Also for
              inline variables (C++17) and the implicit inline of constexpr/members
              defined in-class.
   explicit : block implicit conversions for single-arg ctors and conversion operators;
              explicit(bool) is conditional (C++20).
   virtual  : enable runtime polymorphism; '= 0' makes it pure (Module 07).
   friend   : grant a function/class access to private/protected members (Module 05 §5.11).
   noexcept : exception specification; affects vector move choice & optimization (Mod 13).
```

Cross-references: [Module 00 §0.7](00_toolchain_and_compilation.md),
[Module 05 §5.3/5.8](05_classes_construction_lifetime.md),
[Module 07](07_inheritance_polymorphism.md),
[Module 13 §13.4](13_exceptions_error_handling.md).

---

## E.9 Template & type-introspection keywords

```cpp
template<class T> ...        // class == typename in a template parameter list
typename T::value_type x;    // disambiguate a DEPENDENT TYPE (Module 09 §9.7)
obj.template get<0>();       // disambiguate a DEPENDENT MEMBER TEMPLATE
decltype(expr)               // the type of an expression (keeps ref/const; unevaluated)
concept Num = ...;           // C++20: define a named constraint
requires C<T>                // C++20: constrain; also the requires-expression
sizeof(T) / sizeof...(pack)  // size in bytes / pack element count (unevaluated)
alignas(64) / alignof(T)     // alignment control / query
static_assert(cond, "msg")   // compile-time assertion
```

🧠 `class` and `typename` are interchangeable when **introducing** a template type
parameter; only `typename` works to flag a **dependent type name** inside a template.

Cross-references: [Module 09](09_templates_generic_programming.md),
[Module 14 §14.1](14_cpp20_features.md).

---

## E.10 Casts, memory & coroutine keywords

```cpp
static_cast<T>(e)  dynamic_cast<T>(e)  const_cast<T>(e)  reinterpret_cast<T>(e)  // Mod 01 §1.9
new  new[]  delete  delete[]                       // allocation expressions (Mod 03, Mod 08 §8.15)
sizeof  alignof  typeid  noexcept(expr)            // unevaluated operators (don't run operand)
co_await  co_yield  co_return                      // coroutines (Module 14 §14.3)
this                                               // pointer to current object (E.11)
```

---

## E.11 Control-flow, literal & misc keywords (quick table)

```
   if else switch case default for while do break continue goto return
   try catch throw                  (Module 13)
   true false nullptr this          (literals / the object pointer)
   namespace using                  (Appendix C)
   enum  enum class  union  struct  class   (Modules 04, 05)
   operator                         (operator overloading, Module 08)
   and or not bitand bitor xor compl and_eq or_eq xor_eq not_eq  (alt tokens for && || ! & | ^ ~ ...)
```

---

## E.12 The complete keyword list (with status)

```
   STANDARD (usable):
   alignas alignof asm auto bool break case catch char char8_t char16_t char32_t
   class concept const consteval constexpr constinit const_cast continue
   co_await co_return co_yield decltype default delete do double dynamic_cast
   else enum explicit export extern false float for friend goto if inline int
   long mutable namespace new noexcept nullptr operator private protected public
   reinterpret_cast return short signed sizeof static static_assert static_cast
   struct switch template this thread_local throw true try typedef typeid typename
   union unsigned using virtual void volatile wchar_t while
   (alt-token keywords) and and_eq bitand bitor compl not not_eq or or_eq xor xor_eq

   IDENTIFIERS WITH SPECIAL MEANING (not reserved everywhere):
   final override   (class/function virt-specifiers — usable as ordinary names elsewhere)
   import module    (C++20 modules — context-sensitive)

   REPURPOSED / DEPRECATED / REMOVED:
   auto      — repurposed C++11 (was a storage class; old meaning gone)
   register  — REMOVED as storage class C++17 (still reserved)
   export    — old meaning (exported templates) removed C++11; REUSED for modules C++20
   volatile  — many uses DEPRECATED in C++20 (compound assign, ++/--, params/returns)
   throw(T)  — dynamic exception specifications REMOVED C++17 (use noexcept)
```

```
   STATUS LEGEND:
   ✅ usable          — normal keyword
   ♻️ repurposed     — auto, export
   ⚠️ deprecated     — most volatile uses (C++20)
   ❌ removed         — register (C++17), throw(spec) (C++17), old export (C++11)
   🔒 reserved        — register (kept reserved even after removal)
```

---

## E.13 Quick decision guide

```
   Need a compile-time constant value?            constexpr (object) / consteval (function)
   Need run-time-init but never reassigned?        const
   Need to kill static-init-order fiasco?          constinit (or a function-local static)
   Want the compiler to deduce a type?             auto / decltype(auto) / template
   Need a member writable in a const method?       mutable (and only for logically-const state)
   Hardware register / signal flag (same thread)?  volatile
   Cross-thread shared mutable state?              std::atomic / a mutex  — NOT volatile
   TU-private free function/variable?              anonymous namespace (not 'static')
   Share one definition across TUs from a header?  inline (function/variable)
   Block implicit conversions?                     explicit
   Want a fast "is this register?" hint?           (nothing — 'register' is gone; trust -O2)
```

➡ Related: [Appendix A — Initialization](A1_initialization_reference.md),
[Appendix D — Type Deduction](D1_type_deduction_reference.md),
[Module 00 — Linkage](00_toolchain_and_compilation.md)
