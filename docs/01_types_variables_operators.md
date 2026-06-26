# Module 01 — Types, Variables, Literals & Operators

> C++ is a **statically, strongly typed** language with a rich set of fundamental
> types and *very* subtle conversion rules. Integer promotion and the "usual
> arithmetic conversions" are the source of countless real bugs. `[Primer Ch.2, §4]`

---

## 1.1 The type system at a glance

```
                              C++ TYPES
                                  │
        ┌─────────────────────────┴───────────────────────────┐
   FUNDAMENTAL                                            COMPOUND
   ┌────────────────────────┐                  ┌───────────────────────────┐
   │ void                   │                  │ pointers      T*          │
   │ std::nullptr_t         │                  │ references    T& / T&&    │
   │ bool                   │                  │ arrays        T[N]        │
   │ char family            │                  │ functions     R(Args...)  │
   │ integer types          │                  │ enums         enum/class  │
   │ floating types         │                  │ class types   struct/class│
   └────────────────────────┘                  │ unions                    │
                                               │ pointer-to-member         │
                                               └───────────────────────────┘
```

---

## 1.2 Fundamental types & their guaranteed sizes

C++ does **not** fix sizes; it fixes *minimum ranges* and relationships. Use
`<cstdint>` when you need exact widths.

```
  type             typical 64-bit  guaranteed         notes
  ─────────────────────────────────────────────────────────────────────────
  bool             1 byte          —                  true/false; sizeof>=1
  char             1 byte          exactly 1 byte     signedness is impl-defined!
  signed char      1               >= 8 bits          use for "tiny int"
  unsigned char    1               >= 8 bits          the byte type; aliases anything
  short            2               >= 16 bits
  int              4               >= 16 bits          "natural" word, >=short
  long             8 (4 on Win)    >= 32 bits
  long long        8               >= 64 bits
  float            4               IEEE-754 single*    ~7 decimal digits
  double           8               IEEE-754 double*    ~15 decimal digits
  long double      8/16/80-bit     >= double
  (* IEEE-754 is near-universal but not strictly mandated)
```

Rule the standard *does* guarantee:
`sizeof(char)==1 <= sizeof(short) <= sizeof(int) <= sizeof(long) <= sizeof(long long)`.

🧠 **`char`, `signed char`, and `unsigned char` are THREE distinct types** (unlike
the int family where `int`==`signed int`). Whether plain `char` is signed is
implementation-defined — never rely on it. `std::byte` (C++17) is the type for
raw memory.

### Fixed-width types `<cstdint>`

```cpp
#include <cstdint>
int8_t  i8;    uint8_t  u8;     // exact width (if provided by platform)
int32_t i32;   uint64_t u64;
int_fast16_t f; int_least32_t l; // "fast"/"least" variants
intptr_t  p;   // integer big enough to hold a pointer
std::size_t  n;   // unsigned, result of sizeof, index type for containers
std::ptrdiff_t d; // signed, result of pointer subtraction
```

---

## 1.3 The signed/unsigned trap ⚠️

```cpp
unsigned u = 10;
int      s = -42;
std::cout << u + s;   // ⚠️ s converted to unsigned ⇒ huge number, not -32
```

```
   u + s  with u:unsigned, s:int
   ┌─────────────────────────────────────────────────┐
   │ "usual arithmetic conversions": the signed int  │
   │ is converted to unsigned BEFORE the addition.   │
   │ -42 as unsigned 32-bit = 4294967254             │
   │ result = 4294967254 + 10 = 4294967264  (wraps)  │
   └─────────────────────────────────────────────────┘
```

⚠️ Classic infinite loop:

```cpp
for (unsigned i = n; i >= 0; --i) { ... }   // i >= 0 is ALWAYS true; wraps to UINT_MAX
```

🧠 Expert rules:
- **Unsigned overflow is well-defined** (modular, wraps). **Signed overflow is UB.**
- Prefer signed integers for arithmetic/indices in your own code; the STL's use
  of `size_t` is a historical wart the committee itself regrets (see `ssize()`).
- Never compare signed and unsigned without thinking; turn on `-Wsign-compare`.

---

## 1.4 Integer promotion & the usual arithmetic conversions

Two separate mechanisms, applied in this order:

```
  STEP 1 — INTEGER PROMOTION (applies to small types in arithmetic):
     bool, char, signed/unsigned char, short, char8/16/32_t, wchar_t, enum
       └─▶ promoted to int   (or unsigned int if int can't hold all values)

  STEP 2 — USUAL ARITHMETIC CONVERSIONS (bring both operands to a common type):
     if either is long double      → long double
     else if either is double      → double
     else if either is float       → float
     else  (both now integers)     → apply integer-rank/sign rules:
          higher rank wins; on tie, unsigned wins (the trap in 1.3)
```

```cpp
char a = 1, b = 2;
auto c = a + b;          // a,b promoted to int ⇒ c is int (NOT char!)
static_assert(std::is_same_v<decltype(c), int>);

short s = 30000;
s = s + s;               // computed in int (no overflow), then narrowed back ⇒ may overflow short
```

🧠 This is why `decltype(a + b)` for two `char`s is `int`, and why
`std::cout << someChar + 0` prints a number while `std::cout << someChar` prints
a glyph.

---

## 1.5 Literals — exact spellings matter

```cpp
42        // int
42u 42U   // unsigned int
42l 42L   // long          42ll/42LL long long      42ull unsigned long long
0x2A 0X2A // hex            052 octal (leading 0!)   0b101010 binary (C++14)
1'000'000 // digit separators (C++14) — purely cosmetic

3.14      // double
3.14f     // float          3.14L long double
1e-9      // scientific     0x1.8p3 hex float (== 12.0)

'A'       // char           '\n' '\t' '\0' escapes   '\x41' hex   '\101' octal
u8'A'     // char8_t (C++20)  u'A' char16_t  U'A' char32_t  L'A' wchar_t

"hello"            // const char[6]  (includes '\0')
"line1\n" "line2"  // adjacent string literals concatenate
R"(raw \n no escape)"        // raw string literal
R"delim(can contain )" )delim" // custom delimiter

true false  nullptr           // keyword literals
```

User-defined literals (C++11) and standard ones:

```cpp
using namespace std::literals;
auto s  = "hi"s;        // std::string, not const char*
auto dur = 100ms;       // std::chrono::milliseconds
auto cx = 3.0 + 4.0i;   // std::complex<double>
```

---

## 1.6 Variables, initialization forms, and `auto`

```cpp
int a = 5;          // copy initialization
int b(5);           // direct initialization
int c{5};           // direct-list (brace) init — PREFERRED
int d = {5};        // copy-list init
int e{};            // value init ⇒ 0   (e is zero, not garbage)
int f;              // default init ⇒ UNINITIALIZED garbage (for built-ins, at block scope) 
```

```
  WHY PREFER BRACES {}  ─────────────────────────────────────────────────┐
  • {} forbids NARROWING conversions:                                    │
        int x{3.14};   // compile error (good!)                          │
        int y = 3.14;  // silently truncates to 3                        │
  • {} value-initializes ⇒ no accidental garbage.                        │
  • One exception ("most vexing parse" relief, see below).               │
  └──────────────────────────────────────────────────────────────────────┘
```

**The most vexing parse**: `Widget w();` declares a *function* named `w`
returning `Widget`, not an object. Use `Widget w;` or `Widget w{};`.

`auto` deduces the type (drops top-level `const`/`&` unless you ask):

```cpp
auto i = 5;              // int
auto& r = i;             // int&
const auto ci = i;       // const int
auto* p = &i;            // int*
auto x = ci;             // int  (top-level const dropped)
decltype(auto) y = (i);  // int& — decltype(auto) preserves ref/const exactly
```

🧠 `auto` strips references and top-level const; `decltype(expr)` keeps them and
distinguishes `decltype(x)` (declared type) from `decltype((x))` (value-category
→ adds `&` for lvalues). This matters in generic return types.

---

## 1.7 Operators — precedence, associativity, and the gotchas

Abbreviated precedence table (high → low). Full table: `[Primer §4.12]`.

```
  level   operators                                  associativity
  ───────────────────────────────────────────────────────────────
   1  ::  (scope)                                     left
   2  a()  a[]  a->b  a.b  a++  a--  (postfix)        left
   3  ++a --a  +a -a  !a ~a  *p  &x  (unary)          RIGHT
      (T)x  sizeof  new delete  co_await
   4  .*  ->*  (pointer-to-member)                    left
   5  *  /  %                                         left
   6  +  -                                            left
   7  <<  >>                                          left
   8  <=>  (three-way, C++20)                         left
   9  <  <=  >  >=                                    left
  10  ==  !=                                          left
  11  &     12  ^     13  |                           left
  14  &&    15  ||                                    left
  16  ?:  =  +=  -=  ... (assignment, ternary)        RIGHT
  17  ,  (comma)                                      left
```

Common precedence bugs:

```cpp
if (a & MASK == 0)    // ⚠️ parses as a & (MASK==0)   — == binds tighter than &
if ((a & MASK) == 0)  // ✅

*p++;                 // *(p++): dereference p, THEN increment the pointer
(*p)++;               // increment the pointed-to value

x << 1 + 2;           // x << (1+2) == x<<3, because + binds tighter than <<
```

### Evaluation order (a top expert topic)

```
  • The OPERANDS of most operators have UNSEQUENCED evaluation order.
    f(g(), h());      // g() and h() may run in EITHER order (impl-defined)
  • Modifying a variable twice with no sequencing = UNDEFINED BEHAVIOR:
        i = i++ + 1;          // ⚠️ UB before C++17, unspecified-ish after
        a[i] = i++;           // ⚠️ UB / unspecified
  • SEQUENCED guarantees you CAN rely on:
        &&  ||  ,  ?:   evaluate left operand fully first (with sequence point)
        C++17: a.b(), the object expr is sequenced before args;
               assignment RHS is sequenced before the store (since C++17)
```

🧠 Since C++17, `i = i++ + 1;` is no longer UB but its value is unspecified — still
write code that doesn't depend on operand ordering.

---

## 1.8 `sizeof`, `alignof`, and `decltype`

```cpp
sizeof(int)        // size in bytes (size_t), compile-time, no evaluation of operand
sizeof expr        // parentheses optional for an expression
sizeof(arr)/sizeof(arr[0])   // old array length trick (prefer std::size(arr))
alignof(double)    // required alignment (e.g. 8)
alignas(64) int x; // force 64-byte alignment (cache-line)
decltype(x + y)    // the TYPE of the expression, unevaluated
```

🧠 `sizeof` never evaluates its operand: `sizeof(f())` does **not** call `f`. Same
for `decltype`, `noexcept(expr)`, and `typeid` (mostly). These are *unevaluated
contexts*.

---

## 1.9 Type conversions: implicit vs explicit casts

```
  IMPLICIT (the compiler inserts them):
    • promotions (char→int), arithmetic conversions
    • array→pointer decay, function→pointer
    • Derived* → Base* , T* → void*, anything → bool (in conditions)
    • user-defined converting constructors / conversion operators

  EXPLICIT C++ casts (named, searchable, intentional):
    static_cast<T>(e)       well-defined, related types (int↔double, up/down* class)
    const_cast<T>(e)        add/remove const/volatile ONLY
    reinterpret_cast<T>(e)  reinterpret bit pattern (pointers/ints) — dangerous
    dynamic_cast<T>(e)      runtime-checked downcast in polymorphic hierarchies
    (T)e  /  T(e)           C-style: tries static→const→reinterpret — AVOID
```

```cpp
double d = static_cast<double>(3) / 2;   // 1.5, not 1
int*  p  = reinterpret_cast<int*>(0x1000); // raw address — you own the consequences
char* s  = const_cast<char*>(cstr);      // remove const (UB to WRITE if originally const)
```

⚠️ `const_cast`-ing away const and then *writing* through it is UB if the object
was actually declared `const`. `reinterpret_cast` between unrelated pointer types
and dereferencing usually violates strict aliasing (Module 16).

---

## 1.10 `volatile`, `register`, `mutable` — what they really mean

- `volatile`: "this memory may change outside the program's control (memory-mapped
  I/O, signal handler)". It is **not** for threading — use atomics (Module 15).
- `register`: deprecated/removed hint; ignore.
- `mutable`: allows a data member to be modified even through a `const` member
  function (e.g. a cache or mutex). `[Primer §7.1.1]`

```cpp
class Cache {
    mutable std::optional<int> memo;   // can be set inside a const getter
public:
    int get() const { if(!memo) memo = compute(); return *memo; }
};
```

---

## 1.11 Checklist for experts

- [ ] You can predict the type of any mixed arithmetic expression.
- [ ] You never mix signed/unsigned without intent; you know unsigned wraps, signed overflow is UB.
- [ ] You default to `{}` initialization and know when narrowing is rejected.
- [ ] You reach for named casts, never C-style casts.
- [ ] You know `sizeof`/`decltype`/`noexcept` don't evaluate their operands.

➡ Next: [Module 02 — Control Flow & Functions](02_control_flow_and_functions.md)
