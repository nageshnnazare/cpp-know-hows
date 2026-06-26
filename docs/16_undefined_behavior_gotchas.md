# Module 16 — Undefined Behavior & Expert Gotchas

> Undefined Behavior (UB) is the single most important concept distinguishing a
> C++ expert. The standard grants the compiler total freedom when your program has
> UB — it may "work", crash, leak, or be optimized in ways that delete your code.
> This module is a field guide to UB and the traps that bite even veterans.

---

## 16.1 The three kinds of "undefinedness"

```
   ┌─────────────────────────────────────────────────────────────────────────┐
   │ UNDEFINED behavior   : NO restrictions. Anything may happen. The        │
   │   compiler may assume it NEVER occurs and optimize accordingly. ⚠️⚠️⚠️   │
   │   e.g. signed overflow, OOB access, use-after-free, data race, null     │
   │        deref, reading uninitialized memory, violating strict aliasing.  │
   │                                                                         │
   │ UNSPECIFIED behavior : one of several valid outcomes; no diagnostic.    │
   │   e.g. order of evaluation of function arguments. Program stays valid.  │
   │                                                                         │
   │ IMPLEMENTATION-DEFINED: like unspecified, but the implementation must   │
   │   DOCUMENT its choice. e.g. sizeof(int), whether char is signed.        │
   └─────────────────────────────────────────────────────────────────────────┘
```

🧠 The killer insight: **the optimizer assumes UB never happens.** It uses that
assumption to transform code. So UB doesn't just give a wrong value at that line —
it can make *unrelated* code vanish or behave nonsensically.

```cpp
int foo(int* p) {
    int x = *p;        // compiler now ASSUMES p != nullptr (else UB)
    if (!p) return 0;  // ⚠️ this check is DELETED — p was already assumed non-null!
    return x;
}
```

---

## 16.2 The UB catalog (memorize the common ones)

```
   CATEGORY                    EXAMPLE                                   DETECTOR
   ───────────────────────────────────────────────────────────────────────────────
   signed integer overflow     INT_MAX + 1                               UBSan
   shift out of range          1 << 40 (on 32-bit int); x << -1          UBSan
   out-of-bounds access        v[v.size()]; arr[-1]; ptr past end deref  ASan
   use-after-free / -return     return &local; use after delete          ASan
   double free                  delete p; delete p;                       ASan
   null pointer dereference     *(int*)nullptr                            UBSan/ASan
   uninitialized read           int x; use(x);                            MSan/valgrind
   data race                    unsynchronized shared write              TSan
   invalid downcast             static_cast to wrong derived, deref      UBSan(vptr)
   strict aliasing violation    read float via int* (see 16.4)           sometimes UBSan
   misaligned access            reinterpret to over-aligned type         UBSan(align)
   infinite loop w/o side-effect compiler may assume forward progress    —
   calling through wrong type   function pointer type mismatch            UBSan(cfi)
   modifying a const object     const_cast then write a真const            —
   invalid enum value           cast 99 into enum with values 0..3        UBSan(enum)
   returning from non-void w/o   missing return statement                 UBSan/-Wreturn-type
```

---

## 16.3 Lifetime & dangling traps

```cpp
std::string_view sv = std::string("temp");      // ⚠️ dangles immediately (Mod 04)
const int& r = std::vector<int>{1,2,3}[0];       // ⚠️ vector temp dies; r dangles
auto it = v.begin(); v.push_back(x); *it;        // ⚠️ realloc invalidated it (Mod 10)
for (auto& kv : getMap()["k"]) {}                // ⚠️ temp map dies (pre-C++23, Mod 02)

const std::string& good = std::string("temp");   // ✅ const& EXTENDS the temp's life
auto&& also = std::string("temp");               // ✅ but ONLY for the immediate temp,
                                                  //    not subobjects of a function result
```

```
   LIFETIME-EXTENSION RULE: binding a temporary to a const lvalue ref or an
   rvalue ref extends its life to the reference's scope — BUT this does NOT
   chain through function return values or container element access.
```

🧠 Returning references/pointers/views/iterators into local or temporary storage
is the most common dangling bug. If a function returns a reference, you must be
able to point to the object (with longer lifetime) that it refers to.

---

## 16.4 Strict aliasing & type punning 🧠

The compiler assumes two pointers of *incompatible* types don't refer to the same
memory. Violating this lets it cache/reorder loads wrongly.

```cpp
float f = 1.0f;
int*  pi = reinterpret_cast<int*>(&f);
int   bits = *pi;            // ⚠️ UB: reading a float's storage through int* aliases

// ✅ legal ways to reinterpret bytes:
int bits1; std::memcpy(&bits1, &f, sizeof f);     // always OK
auto bits2 = std::bit_cast<int>(f);               // C++20 — OK, constexpr-friendly
```

```
   ALLOWED to alias ANY object:  char*, unsigned char*, std::byte*
   (these may legally read the bytes of any object)
   NOT allowed: read an object of type X through a pointer to unrelated type Y.
   -fno-strict-aliasing disables the assumption (a crutch, not a fix).
```

---

## 16.5 Integer & arithmetic surprises

```cpp
INT_MAX + 1;                 // ⚠️ signed overflow = UB (NOT wraparound!)
UINT_MAX + 1u;               // ✅ unsigned wraps to 0 (well-defined, modular)
int x = -1; x >> 1;          // implementation-defined (arithmetic vs logical shift)
1 << 31;                     // ⚠️ UB for 32-bit int (overflows sign bit)
-INT_MIN;                    // ⚠️ UB (no positive counterpart in two's complement)
int q = 5 / 0;               // ⚠️ UB (integer divide by zero); 5.0/0.0 is inf (defined)
size_t n = v.size() - 1;     // ⚠️ if v is empty, n = HUGE (unsigned wrap) → OOB loop
```

🧠 Because signed overflow is UB, the compiler assumes `x + 1 > x` is always true
for signed `x` — it will optimize loops based on that. This is why `-O2` can turn a
buggy signed loop into an infinite one. Use `-fsanitize=undefined` to catch it, and
prefer signed types for arithmetic but guard your bounds.

---

## 16.6 Initialization & object-model traps

```cpp
int x;                        // ⚠️ indeterminate at block scope (reading it is UB)
int y{};                      // ✅ value-initialized to 0
std::vector<int> v(5);        // 5 zero-initialized ints
std::vector<int> w{5};        // ⚠️ ONE element with value 5 (initializer_list wins!)
std::vector<int> a(5, 1);     // 5 ones      vs   {5,1} ⇒ two elements 5 and 1

Widget w();                   // ⚠️ most vexing parse: declares a FUNCTION (Mod 01)

std::map<K,V> m; auto& r = m[k];   // ⚠️ inserts a default-constructed V if k absent
```

🧠 The `{}` vs `()` difference for `vector` is a famous trap: brace-init prefers an
`initializer_list` constructor. Know that `vector<int>{5}` ≠ `vector<int>(5)`.

---

## 16.7 Self-reference, ordering & sequencing

```cpp
v[i] = i++;                   // pre-C++17 UB; C++17 unspecified — avoid
f(i++, i++);                  // unspecified argument order; avoid mutating same var
a = b = c;                    // OK (assignment is right-assoc, sequenced)
int i = 0; i = i++ ;          // ⚠️ confusing; don't
```

---

## 16.8 The "works on my machine" / silent-regression traps

```
   ⚠️ Adding a destructor → loses implicit move ops → silent perf regression (Mod 6)
   ⚠️ Non-noexcept move ctor → vector copies instead of moves (Mod 6/13)
   ⚠️ Passing shared_ptr by value in hot loops → atomic refcount churn (Mod 11)
   ⚠️ ODR violation across TUs (different flags/layout) → silent corruption (Mod 0)
   ⚠️ Static init order fiasco → globals used before construction (Mod 2)
   ⚠️ Virtual call in a constructor/destructor → calls the BASE version, not derived:
          struct B { B(){ init(); } virtual void init(); };  // calls B::init, not D's
   ⚠️ Slicing polymorphic objects passed/stored by value (Mod 7)
   ⚠️ Capturing locals by reference in a lambda that outlives them (Mod 12)
   ⚠️ Returning std::move(local) → disables NRVO (Mod 6)
   ⚠️ Comparing floats with == ; ignoring NaN (NaN != NaN is true)
```

🧠 **Virtual calls during construction/destruction** resolve to the
currently-constructed type, NOT the most-derived type — because the derived part
doesn't exist yet (or has been destroyed). Never call virtuals expecting dynamic
dispatch from a ctor/dtor.

---

## 16.9 Floating point gotchas

```cpp
0.1 + 0.2 == 0.3;            // ⚠️ false (binary FP can't represent 0.1 exactly)
std::abs((a-b)) < epsilon;   // compare with tolerance instead
double nan = 0.0/0.0;        // a NaN; nan == nan is FALSE; nan < x is false
std::isnan(x); std::isinf(x);// proper checks
// associativity is NOT guaranteed: (a+b)+c may differ from a+(b+c) under -ffast-math
```

---

## 16.10 The expert's defensive toolkit

```
   COMPILE & TEST WITH:
   -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion   (warnings)
   -Werror                                                            (in CI)
   -fsanitize=address,undefined        (ASan + UBSan: memory + UB at runtime)
   -fsanitize=thread                    (TSan: data races; separate build)
   -fsanitize=memory                    (MSan: uninitialized reads; clang)
   -D_GLIBCXX_ASSERTIONS / -D_LIBCPP_HARDENING_MODE=...  (hardened STL bounds checks)

   STATIC ANALYSIS:
   clang-tidy (bugprone-*, cppcoreguidelines-*), cppcheck, include-what-you-use

   TOOLS:
   valgrind (memcheck), gdb/lldb, compiler-explorer (godbolt) to read the asm,
   AddressSanitizer + fuzzing (libFuzzer) for input-driven code.
```

🧠 **The non-negotiable habit:** run your test suite at least once under
`-fsanitize=address,undefined`. It converts silent UB (the worst bugs) into loud,
located runtime errors. Most of the catalog in 16.2 is caught this way.

---

## 16.11 Mental model: how to think about UB

```
   1. C++ trades safety for performance: the standard says "if you do X, all bets
      are off" precisely so the compiler can skip checks and optimize.
   2. UB is a CONTRACT you must uphold; the compiler upholds its side (speed) only
      if you uphold yours (no UB).
   3. "It worked" ≠ "it's correct." UB can appear to work until a compiler upgrade,
      a new optimization level, or a different input.
   4. Prefer constructs that make UB impossible: RAII, smart pointers, containers,
      std::span/string_view (with lifetime care), algorithms, concepts.
```

---

## 16.12 Final expert checklist

- [ ] You can explain why UB can affect code "before" the offending line.
- [ ] You recognize the common UB categories and the sanitizer that finds each.
- [ ] You avoid dangling refs/views/iterators and know lifetime-extension limits.
- [ ] You respect strict aliasing; you pun via `memcpy`/`bit_cast`.
- [ ] You know signed overflow is UB and guard unsigned wrap at boundaries.
- [ ] You never call virtuals in ctors/dtors expecting dynamic dispatch.
- [ ] You build & test with `-Wall -Wextra` and `-fsanitize=address,undefined`.

🎓 **You've reached the end of the core curriculum.** Revisit the
[cheatsheets](../cheatsheets/) and build the [examples](../examples/) to cement it.
