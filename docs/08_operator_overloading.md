# Module 08 — Operator Overloading (Complete Signature Reference)

> This module is a **reference bible** for operator overloading. It lists *every*
> overloadable operator, *every* canonical signature (member and non-member,
> const/ref-qualified, C++23 explicit-object forms), the rewrite rules, the
> conversion interactions, and the idioms. Use the [quick index](#80-quick-index)
> to jump. `[Primer §14; [over.oper], [over.binary], [over.unary] in the standard]`

---

## 8.0 Quick index

| Group | Operators | Member? | Section |
|-------|-----------|---------|---------|
| Assignment | `=` | **must be member** | [8.3](#83-assignment-operator-) |
| Compound assignment | `+= -= *= /= %= &= \|= ^= <<= >>=` | member (idiom) | [8.4](#84-compound-assignment-operators) |
| Binary arithmetic | `+ - * / %` | free (idiom) | [8.5](#85-binary-arithmetic--bitwise-operators) |
| Binary bitwise | `& \| ^ << >>` | free (idiom) | [8.5](#85-binary-arithmetic--bitwise-operators) |
| Unary arithmetic/logical | `+ - ! ~` | member (idiom) | [8.6](#86-unary-operators----) |
| Increment / decrement | `++ --` (pre/post) | member (idiom) | [8.7](#87-increment--decrement-) |
| Comparison (C++20) | `== <=>` (+ rewrites) | member or free | [8.8](#88-comparison-operators--and-c20-rewrites) |
| Comparison (legacy) | `== != < > <= >=` | member or free | [8.8](#88-comparison-operators--and-c20-rewrites) |
| Logical | `&& \|\|` | **avoid** | [8.9](#89-logical--and-comma--avoid) |
| Comma | `,` | **avoid** | [8.9](#89-logical--and-comma--avoid) |
| Subscript | `[]` | **must be member** | [8.10](#810-subscript-) |
| Function call | `()` | **must be member** | [8.11](#811-function-call-) |
| Member access | `-> ->*` | `->` must be member | [8.12](#812-member-access----and-) |
| Dereference / address | `*` `&` (unary) | member (idiom) | [8.13](#813-dereference--and-address-of--unary) |
| Conversion | `operator T()` | **must be member** | [8.14](#814-conversion-operators) |
| Allocation | `new new[] delete delete[]` | member or global | [8.15](#815-allocation--deallocation-new--delete) |
| Coroutine | `co_await` | member or free | [8.16](#816-co_await) |
| Stream I/O | `<< >>` | **free** (idiom) | [8.17](#817-stream-insertion--extraction-) |
| **NOT overloadable** | `:: . .* ?: sizeof typeid alignof noexcept # ##` | — | [8.1](#81-rules-what-can-and-cannot-be-overloaded) |

---

## 8.1 Rules: what can and cannot be overloaded

```
   OVERLOADABLE (38 operators):
     + - * / % ^ & | ~ ! = < > += -= *= /= %= ^= &= |=
     << >> >>= <<= == != <= >= <=> && || ++ -- , ->* ->
     ( ) [ ] new new[] delete delete[] co_await

   NOT OVERLOADABLE:
     ::   (scope resolution)        .    (member access)
     .*   (pointer-to-member)       ?:   (conditional)
     sizeof   alignof   typeid      noexcept   (and the preprocessor # ##)
```

Hard constraints (all enforced by the compiler):

1. You cannot create **new** operator tokens (no `**`, no `<>`).
2. You cannot change an operator's **arity**, **precedence**, or **associativity**
   (Module 01, §1.7). `a + b * c` always groups as `a + (b * c)` regardless of types.
3. At least one operand must be a **user-defined type** (class or enumeration). You
   cannot redefine `int + int`. (Enumerations count — you *can* overload `operator|`
   for a scoped enum.)
4. The operators `=`, `[]`, `()`, `->`, and the conversion operators must be
   **non-static member functions** (C++23 relaxes `[]` and `()` to allow `static`).
5. `new`/`delete` are special (they're really allocation functions, §8.15).
6. Overloading `&&`, `||`, `,` **silently removes** their built-in short-circuit /
   sequencing guarantees (§8.9) — avoid.
7. Default arguments are **not allowed** on overloaded operators (except `()`).

---

## 8.2 Member vs non-member — the decision rules

```
   a @ b   where @ is a binary operator and 'a' has type A:
     • if @ is a MEMBER of A:        a.operator@(b)      ← left operand is 'this', NOT convertible
     • if @ is a NON-MEMBER:         operator@(a, b)     ← BOTH operands can convert

   Consequence: symmetric operators where the LEFT operand might need a conversion
   (e.g. 2 + money) MUST be non-member, or the conversion can't apply to the left side.
```

Canonical placement (the "C++ operator etiquette"):

| Operator(s) | Placement | Why |
|-------------|-----------|-----|
| `= [] () -> ->* operator T()` | **member (required)** | language rule |
| `+= -= *= ...` (compound assign) | **member** | mutates left operand (`this`) |
| `++ --` | **member** | mutates `this` |
| unary `- + ! ~ *` (deref) | **member** | acts on `this` |
| binary `+ - * / % & \| ^ << >>` | **non-member (friend)** | symmetric; allows conversion on both sides |
| `== != < <= > >= <=>` | non-member or member | symmetric; member is fine in C++20 (rewrites cover both sides for class types) |
| `<< >>` (streams) | **non-member** | left operand is the stream, not your type |

🧠 The professional idiom: implement the mutating member (`operator+=`), then define
the binary operator as a non-member in terms of it (`operator+` calls `+=`). This
gives correct conversions, less code, and consistency.

---

## 8.3 Assignment operator `=`

`operator=` **must be a non-static member**. The compiler-generated forms are
*special members* (Module 05/06). User forms:

```cpp
class T {
public:
    // copy assignment — canonical signatures (pick ONE form):
    T& operator=(const T& rhs);            // most common
    T& operator=(T rhs);                   // copy-and-swap (by value) — handles copy & move
    T& operator=(T& rhs);                  // legal but unusual (can't bind const/temporaries)
    const T& operator=(const T&);          // legal but discouraged (breaks (a=b)=c chaining)

    // move assignment:
    T& operator=(T&& rhs) noexcept;

    // C++11 control:
    T& operator=(const T&) = default;      // request implicit
    T& operator=(const T&) = delete;       // forbid copy assignment

    // ref-qualified (C++11): restrict which value categories may be assigned TO
    T& operator=(const T&) &;              // only assignable to an LVALUE: a = b ok; f() = b error
    T& operator=(const T&) && = delete;    // forbid assigning to a temporary

    // templated assignment (NOT a special member; coexists with the real one):
    template<class U> T& operator=(const U& other);
};
```

```
   RETURN TYPE:  return T& (reference to *this) to allow chaining: a = b = c;
   SELF-ASSIGN:  guard against a = a (copy-and-swap does this for free).
   STRONG GUARANTEE: copy-and-swap gives commit-or-rollback (Module 13).
```

🧠 **Ref-qualifiers on `operator=`** stop nonsense like `std::string{} = "x";` or
`(a + b) = c;`. The standard containers are *not* ref-qualified for backward
compatibility, but for new value types, `T& operator=(...) &;` is good hygiene.

Full details of copy/move/defaulted/deleted assignment are in [Module 06](06_copy_move_semantics.md) and the [special-members reference](#section-cross-ref).

---

## 8.4 Compound assignment operators

`+= -= *= /= %= &= |= ^= <<= >>=`. **Members**, return `T&` to `*this`.

```cpp
class Vec {
    double x_, y_;
public:
    Vec& operator+=(const Vec& r) { x_+=r.x_; y_+=r.y_; return *this; }
    Vec& operator-=(const Vec& r) { x_-=r.x_; y_-=r.y_; return *this; }
    Vec& operator*=(double s)     { x_*=s;    y_*=s;    return *this; }
    Vec& operator/=(double s)     { x_/=s;    y_/=s;    return *this; }
    // bitwise compound (for integer-like types):
    // T& operator&=(const T&); T& operator|=(const T&); T& operator^=(const T&);
    // T& operator<<=(int);     T& operator>>=(int);
};
```

```
   SIGNATURE PATTERN (member):   T& operator?=(const Rhs& rhs);
   RETURN:                       T&  (reference to *this) — enables (a += b) += c
   NOTE:                         these are the "primary" mutating ops; build the
                                 binary versions (§8.5) on top of them.
```

---

## 8.5 Binary arithmetic & bitwise operators

`+ - * / %` and `& | ^ << >>`. **Non-member (friend)**, return **by value**,
take the first argument **by value** to enable RVO and reuse the compound op.

```cpp
class Vec {
    double x_, y_;
public:
    Vec(double x, double y) : x_(x), y_(y) {}
    Vec& operator+=(const Vec&);
    friend Vec operator+(Vec a, const Vec& b) { a += b; return a; }   // a is a local copy
    friend Vec operator-(Vec a, const Vec& b) { a -= b; return a; }
    friend Vec operator*(Vec a, double s)     { a *= s; return a; }
    friend Vec operator*(double s, Vec a)     { a *= s; return a; }   // 2.0 * v — symmetric!
};
```

```
   SIGNATURE PATTERNS:
     non-member (preferred): T  operator+(const T& a, const T& b);
                             T  operator+(T a, const T& b);        // by-value 1st arg + reuse +=
     member (allowed):       T  T::operator+(const T& rhs) const;  // but blocks lhs conversion
   RETURN:                    BY VALUE (a new object) — these don't modify operands.
   MIXED TYPES:               provide BOTH (T,U) and (U,T) overloads for symmetry,
                              OR rely on a converting constructor + non-member form.
```

⚠️ A **member** `operator+` makes `2.0 * v` fail to compile (no implicit conversion
on the `this` operand). Non-member + a converting ctor makes both `v * 2.0` and
`2.0 * v` work. This is the single most important reason to prefer free functions
for symmetric binary operators.

---

## 8.6 Unary operators `+ - ! ~ *` (and `&`)

**Members**, take no arguments (the operand is `this`).

```cpp
class Big {
public:
    Big  operator-() const;     // unary minus (negation): return -*this by value
    Big  operator+() const;     // unary plus  (usually identity / promotion)
    bool operator!() const;     // logical NOT (legacy; prefer explicit operator bool)
    Big  operator~() const;     // bitwise NOT
    // unary * and & are dereference / address-of — see §8.13
};
```

```
   SIGNATURE PATTERN (member):   ReturnType operator?() const;   // no parameters
   AS NON-MEMBER (rare):         ReturnType operator?(const T& operand);
   unary minus returns BY VALUE; do NOT modify *this.
```

⚠️ Overloading unary `operator&` (address-of) breaks generic code that takes
addresses (`std::addressof` exists precisely to bypass such abuse). Don't.

---

## 8.7 Increment & decrement `++ --`

The **dummy `int` parameter** distinguishes postfix from prefix.

```cpp
class Iter {
    T* p_;
public:
    Iter& operator++()    { ++p_; return *this; }                 // PREFIX: ++it  → returns ref
    Iter  operator++(int) { Iter tmp = *this; ++*this; return tmp; } // POSTFIX: it++ → returns OLD value
    Iter& operator--()    { --p_; return *this; }                 // prefix --
    Iter  operator--(int) { Iter tmp = *this; --*this; return tmp; } // postfix --
};
```

```
   PREFIX  (++x):  T& operator++();         increment, return *this BY REFERENCE (cheap)
   POSTFIX (x++):  T  operator++(int);      copy old, increment, return OLD value BY VALUE
                          ▲
                          └── unnamed dummy 'int' parameter — the ONLY way to tell them apart
   As non-member:  T& operator++(T&);  and  T operator++(T&, int);
```

🧠 Always implement postfix in terms of prefix. Prefer `++it` in your own code —
postfix forces a copy of the old state.

---

## 8.8 Comparison operators `<=>` and C++20 rewrites

This is the area most transformed by C++20. **Understand the rewrite machinery.**

### Pre-C++20: six separate operators

```cpp
class V {
    int a_, b_;
public:
    // member form (one operand convertible) OR non-member friend (both convertible)
    friend bool operator==(const V& x, const V& y){ return x.a_==y.a_ && x.b_==y.b_; }
    friend bool operator!=(const V& x, const V& y){ return !(x==y); }
    friend bool operator< (const V& x, const V& y){ return x.a_<y.a_ || (x.a_==y.a_ && x.b_<y.b_); }
    friend bool operator> (const V& x, const V& y){ return y<x; }
    friend bool operator<=(const V& x, const V& y){ return !(y<x); }
    friend bool operator>=(const V& x, const V& y){ return !(x<y); }
};
```

### C++20: one (or two) operators + automatic rewrites 🧠

```cpp
#include <compare>
struct V {
    int a_, b_;
    auto operator<=>(const V&) const = default;   // memberwise lexicographic ordering
    // a defaulted <=> ALSO implicitly provides a defaulted operator==
};
```

```
   THE REWRITE RULES (how the compiler synthesizes comparisons):
   ┌─────────────────────────────────────────────────────────────────────────┐
   │ a == b  → uses operator==              ; a != b → !(a == b)             │
   │ a <  b  → (a <=> b) <  0   ; a >  b → (a <=> b) >  0                    │
   │ a <= b  → (a <=> b) <= 0   ; a >= b → (a <=> b) >= 0                    │
   │ The compiler also tries REVERSED operands: if a==b isn't found,         │
   │   it tries b==a; if a<b via <=> isn't found, it tries 0 < (b <=> a).    │
   └─────────────────────────────────────────────────────────────────────────┘
   ⇒ Define operator<=> (ordering) and operator== (equality) — get all SIX, both
     argument orders, for free.
```

### Return type: the three comparison categories

```
   std::strong_ordering    : a==b ⇒ f(a)==f(b) for all f; values substitutable.
                             { less, equal, greater }                (e.g. int)
   std::weak_ordering      : equivalence ≠ equality (e.g. case-insensitive string).
                             { less, equivalent, greater }
   std::partial_ordering   : some pairs unordered (e.g. floating point NaN).
                             { less, equivalent, greater, unordered }
   Conversions: strong → weak → partial (you can return a stronger one where a
   weaker is expected). Compare the result with named helpers:
     std::is_lt(r) is_lteq(r) is_gt(r) is_gteq(r) is_eq(r) is_neq(r)
```

### Custom (non-defaulted) signatures

```cpp
struct Ci {                                   // case-insensitive, by one field
    std::string s;
    std::weak_ordering operator<=>(const Ci& o) const {
        // compare lowercased; weak because "ABC" and "abc" are equivalent, not equal
        return toLower(s) <=> toLower(o.s);
    }
    bool operator==(const Ci& o) const {      // ⚠️ custom <=> does NOT auto-generate ==
        return toLower(s) == toLower(o.s);
    }
};

// heterogeneous comparison (compare V with int):
struct V {
    int v;
    friend std::strong_ordering operator<=>(const V& a, int b){ return a.v <=> b; }
    friend bool operator==(const V& a, int b){ return a.v == b; }
    // rewrites give you  int < V , V > int , etc. automatically (reversed candidates)
};
```

```
   SIGNATURE PATTERNS:
     auto operator<=>(const T&) const = default;             // all 6 + ==, memberwise
     std::strong_ordering operator<=>(const T&) const;       // custom ordering
     friend std::weak_ordering operator<=>(const T&, const U&);  // heterogeneous
     bool operator==(const T&) const = default;              // memberwise equality
   RULES:
     • defaulted <=>  ⇒ also defaults ==.
     • user-provided <=> ⇒ does NOT generate == (define it separately).
     • == and != are "equality" operators; < <= > >= are "relational", derived from <=>.
```

⚠️ Why `==` is separate from `<=>`: equality can be **cheaper** than ordering
(e.g. compare `std::vector` sizes first for `==`, but you can't shortcut ordering).
The committee kept them independent so you can optimize `==`.

---

## 8.9 Logical `&& ||` and comma `,` — AVOID

You *can* overload these, but you **lose their built-in semantics**:

```
   built-in  a && b : evaluates a; if false, SKIPS b (short-circuit), sequenced.
   overloaded a && b : becomes a function call operator&&(a,b) ⇒ BOTH operands
                       ALWAYS evaluated, in UNSPECIFIED order, NO short-circuit.
   built-in  a , b  : evaluates a, discards it, sequenced-before b.
   overloaded a , b : function call; no sequencing guarantee.
```

🧠 Real-world: expression-template libraries historically (ab)used these; modern
code uses named functions or ranges. **Do not overload `&&`, `||`, `,`.**

---

## 8.10 Subscript `[]`

**Must be a member.** Provide a const and non-const overload (and ideally an
rvalue overload for move-out). C++23 allows **multiple subscript arguments** and
**`static`/explicit-object** forms.

```cpp
class Buffer {
    int* d_; std::size_t n_;
public:
    int&       operator[](std::size_t i)       { return d_[i]; }   // mutable elements
    const int& operator[](std::size_t i) const { return d_[i]; }   // const access
    // C++23: multidimensional subscript
    // double& operator[](std::size_t r, std::size_t c);
    // C++23: explicit object parameter ("deducing this") unifies const/non-const:
    // template<class Self> auto&& operator[](this Self&& self, std::size_t i) {
    //     return std::forward<Self>(self).d_[i];
    // }
};
```

```
   SIGNATURE PATTERNS (member only):
     Ref       operator[](Index i);
     ConstRef  operator[](Index i) const;
     Ref       operator[](Index r, Index c);   // C++23 multidim
   NOTE: no bounds checking by convention (use .at() for checked access, like std::vector).
```

---

## 8.11 Function call `()`

**Must be a member** (C++23 allows `static`). Any arity, default args allowed.
A class with `operator()` is a **function object / functor** — the basis of lambdas.

```cpp
struct Adder {
    int base;
    int  operator()(int x) const { return base + x; }            // unary
    int  operator()(int x, int y) const { return base + x + y; } // binary (overloaded)
};
struct Generator {
    int n = 0;
    int operator()() { return n++; }     // nullary, mutating ⇒ stateful
};
// C++23 static operator() (no per-object state needed):
struct Hash { static std::size_t operator()(std::string_view s); };
```

```
   SIGNATURE PATTERNS (member, any arity, default args OK):
     R operator()(Args...) [const] [noexcept];
     static R operator()(Args...);          // C++23
   USES: comparators, predicates, custom deleters, allocators, transparent functors.
```

---

## 8.12 Member access `->` and `->*`

`operator->` **must be a member**; it is special — it is applied **recursively**
until it yields a raw pointer. This is how smart pointers and iterators "drill".

```cpp
template<class T>
class Ptr {
    T* p_;
public:
    T*       operator->()       { return p_; }   // ptr->m  ≡  ptr.operator->()->m
    const T* operator->() const { return p_; }
    T&       operator*()  const { return *p_; }  // see §8.13
};
```

```
   RESOLUTION of  x->member :
     if x is a class type with operator->, replace x->member with
        (x.operator->())->member   and REPEAT until a raw pointer is reached.
   SIGNATURE: pointer-like  T* operator->();   (or another class with its own ->)
   ->*  (pointer-to-member through a smart pointer) is rarely overloaded:
        R& operator->*(MemberPtr);
```

---

## 8.13 Dereference `*` and address-of `&` (unary)

```cpp
template<class T>
class Ptr {
    T* p_;
public:
    T&  operator*()  const { return *p_; }   // unary deref: *ptr → the object
    T*  operator->() const { return p_; }
    // T* operator&();  // DON'T overload address-of — breaks std::addressof users
};
```

```
   SIGNATURE (member): T& operator*() const;   // unary, no params, returns the pointee
   Pair operator* with operator-> for pointer-like types (smart pointers, iterators).
```

---

## 8.14 Conversion operators

**Must be members.** No explicit return type written (it's in the name). Make them
`explicit` to avoid surprising implicit conversions.

```cpp
class Fraction {
    int n_, d_;
public:
    explicit operator double() const { return double(n_) / d_; }  // explicit conversion
    explicit operator bool()   const { return n_ != 0; }          // for if(frac), while, etc.
    operator std::string() const;                                  // implicit (use sparingly)
    // C++20: conditionally explicit
    // explicit(sizeof(int) > 2) operator int() const;
};
```

```
   SIGNATURE PATTERN (member):  [explicit] operator TargetType() const;
   • NO return type token (the type IS the name).
   • Can convert to references/pointers: operator T&(); operator const T*() const;
   • 'explicit' blocks implicit use but STILL allows:
        - static_cast<TargetType>(x)
        - contextual conversions to bool (if/while/&&/||/?: condition, !x)
   • An implicit conversion operator + a converting constructor on the other type
     can cause AMBIGUITY — prefer explicit and named conversion functions.
```

🧠 `explicit operator bool()` is the canonical "is this usable?" check used by
`std::optional`, smart pointers, and streams (`if (file)`). Because of the
"contextual conversion to bool" rule, it works in conditions without a cast but
won't silently convert to `int`.

---

## 8.15 Allocation & deallocation: `new`, `new[]`, `delete`, `delete[]`

These are not "operators on values" — they are **allocation functions** the
`new`/`delete` *expressions* call. You can overload them globally or per-class.

```cpp
class Tracked {
public:
    static void* operator new(std::size_t sz);                    // class-specific allocation
    static void  operator delete(void* p) noexcept;               // class-specific dealloc
    static void* operator new[](std::size_t sz);                  // array form
    static void  operator delete[](void* p) noexcept;
    static void* operator new(std::size_t sz, std::align_val_t a);// C++17 over-aligned
    static void  operator delete(void* p, std::size_t sz) noexcept;// sized delete (C++14)
    // placement forms (don't allocate; construct into given memory):
    static void* operator new(std::size_t, void* where) noexcept { return where; }
};

// global replacements (affect the whole program):
void* operator new(std::size_t sz);                  // replaceable
void  operator delete(void* p) noexcept;
void* operator new(std::size_t sz, std::nothrow_t) noexcept;  // new(std::nothrow) T
```

```
   new T(args)  ⇒  1) call operator new(sizeof(T))  → raw memory
                   2) construct T in that memory
                   3) yield T*
   delete p     ⇒  1) p->~T()
                   2) call operator delete(p)
   SIGNATURE RULES:
     operator new   returns void*, first param std::size_t (the byte count).
     operator delete returns void, first param void*, must be noexcept.
     extra params on operator new ⇒ "placement new": new (extra...) T(...).
     For each placement operator new you SHOULD provide a matching operator delete
     (called if the constructor throws), else you can leak.
```

🧠 Per-class `operator new`/`delete` power memory pools, arena allocators, leak
trackers, and `std::pmr`. Placement `new` (the `<new>` header form) is how
`std::vector` constructs elements in pre-allocated storage (Module 03).

---

## 8.16 `co_await`

C++20 coroutine operator. Overload to make a type "awaitable" (returns an awaiter).

```cpp
struct Task {
    auto operator co_await() const noexcept;   // returns an awaiter (await_ready/suspend/resume)
};
// free form also allowed:
// Awaiter operator co_await(Awaitable);
```

See [Module 14 §14.3](14_cpp20_features.md) for awaiter mechanics
(`await_ready`, `await_suspend`, `await_resume`).

---

## 8.17 Stream insertion `<<` & extraction `>>`

**Non-member** (the left operand is the stream). Return the stream reference for
chaining. Declare `friend` if private access is needed.

```cpp
#include <iostream>
class Point {
    int x_, y_;
public:
    friend std::ostream& operator<<(std::ostream& os, const Point& p) {
        return os << '(' << p.x_ << ", " << p.y_ << ')';
    }
    friend std::istream& operator>>(std::istream& is, Point& p) {
        char lparen, comma, rparen;
        is >> lparen >> p.x_ >> comma >> p.y_ >> rparen;          // parse "(x, y)"
        if (lparen!='(' || comma!=',' || rparen!=')') is.setstate(std::ios::failbit);
        return is;
    }
};
```

```
   SIGNATURE PATTERNS (non-member):
     std::ostream& operator<<(std::ostream& os, const T& value);  // output: T is const&
     std::istream& operator>>(std::istream& is, T& value);        // input:  T is non-const&
   RETURN the stream BY REFERENCE to enable  os << a << b << c;
   On parse failure set is.setstate(std::ios::failbit).
```

🧠 In C++20+ prefer providing a `std::formatter<T>` specialization so your type
works with `std::format`/`std::print` (type-safe, faster) — see Module 14 §14.6.
You can still keep `operator<<` for iostream compatibility.

---

## 8.18 Complete worked example — an arithmetic value type

```cpp
#include <compare>
#include <iostream>

class Rational {
    long n_ = 0, d_ = 1;
    void normalize();                       // reduce + keep denom positive
public:
    Rational() = default;
    Rational(long n, long d = 1) : n_(n), d_(d) { normalize(); }   // converting ctor

    // compound assignment (members, mutating) — the PRIMARY operations
    Rational& operator+=(const Rational& r);
    Rational& operator-=(const Rational& r);
    Rational& operator*=(const Rational& r);
    Rational& operator/=(const Rational& r);

    // unary
    Rational operator-() const { return Rational{-n_, d_}; }
    Rational operator+() const { return *this; }

    // increment (for completeness)
    Rational& operator++()    { n_ += d_; return *this; }
    Rational  operator++(int) { Rational t=*this; ++*this; return t; }

    // comparison (C++20): one line gives all six + reversed args + heterogeneous via ctor
    auto operator<=>(const Rational& r) const {
        return n_ * r.d_ <=> r.n_ * d_;     // strong_ordering
    }
    bool operator==(const Rational& r) const { return n_==r.n_ && d_==r.d_; }

    // explicit conversion
    explicit operator double() const { return double(n_) / d_; }

    // binary arithmetic (non-member friends, via compound ops) — symmetric, allow 2 + r
    friend Rational operator+(Rational a, const Rational& b){ a += b; return a; }
    friend Rational operator-(Rational a, const Rational& b){ a -= b; return a; }
    friend Rational operator*(Rational a, const Rational& b){ a *= b; return a; }
    friend Rational operator/(Rational a, const Rational& b){ a /= b; return a; }

    // stream output
    friend std::ostream& operator<<(std::ostream& os, const Rational& r){
        return os << r.n_ << '/' << r.d_;
    }
};
```

This single type exercises: converting ctor, member compound assign, non-member
binary ops, unary, increment (pre/post), `<=>`+`==`, explicit conversion, and
stream insertion — the full canonical operator set for a value type.

---

## 8.19 Pitfalls & guidelines (expert checklist)

```
   ⚠️ Member binary op blocks left-operand conversion (2 + obj fails) → use non-member.
   ⚠️ Forgetting to return *this / the stream → broken chaining.
   ⚠️ Custom operator<=> does NOT generate operator== — define both.
   ⚠️ Overloading && || , → loses short-circuit/sequencing.
   ⚠️ Overloading unary & → breaks std::addressof / generic code.
   ⚠️ Implicit conversion operator + converting ctor → ambiguity; prefer explicit.
   ⚠️ Postfix ++/-- without the dummy int param won't compile as postfix.
   ⚠️ operator delete that throws → undefined; must be noexcept.
   ✅ Implement binary ops via compound members. Provide const & non-const [].
   ✅ Default <=> and == when memberwise semantics are correct.
   ✅ Keep semantics intuitive (operator+ adds). Maintain consistency (== reflexive,
      < a strict weak ordering) so STL containers/algorithms behave.
   ✅ Provide a std::formatter specialization (C++20) for std::format support.
```

---

## 8.20 Checklist

- [ ] You can write the canonical signature for every overloadable operator.
- [ ] You choose member vs free per the etiquette table and know *why*.
- [ ] You implement binary ops via compound assignment members.
- [ ] You understand the C++20 `<=>`/`==` rewrite + reversed-candidate rules and the three orderings.
- [ ] You make conversions `explicit` and use `explicit operator bool`.
- [ ] You know `operator->` chains, `[]`/`()` C++23 forms, and `new`/`delete` mechanics.

➡ Next: [Module 09 — Templates & Generic Programming](09_templates_generic_programming.md)
