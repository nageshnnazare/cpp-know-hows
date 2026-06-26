# Module 05 — Classes: Construction & Lifetime (Complete Reference)

> A **reference bible** for class members, *every* kind of constructor, destructor,
> and assignment operator, member-function qualifiers (cv, ref, `noexcept`,
> `virtual`, `= 0`, C++23 "deducing this"), initialization order, RAII, static
> members, and `friend`. `[Primer §7, §13.1; standard [class], [class.ctor],
> [class.dtor], [special]]`

---

## 5.0 Quick index

- [5.1 struct vs class & access](#51-struct-vs-class-and-access-control)
- [5.2 object layout & `this`](#52-object-layout--this)
- [5.3 The complete constructor catalog](#53-the-complete-constructor-catalog)
- [5.4 Member initializer lists (full rules)](#54-member-initializer-lists--the-full-rules)
- [5.5 The complete destructor catalog](#55-the-complete-destructor-catalog)
- [5.6 The complete assignment-operator catalog](#56-the-complete-assignment-operator-catalog)
- [5.7 Construction/destruction order](#57-the-full-constructiondestruction-order)
- [5.8 Member-function qualifiers (full reference)](#58-member-function-qualifiers--complete-reference)
- [5.9 Special member functions & generation rules](#59-special-member-functions--generation-rules)
- [5.10 static members](#510-static-members)
- [5.11 friend](#511-friend)
- [5.12 Aggregates](#512-aggregates--aggregate-initialization)
- [5.13 RAII](#513-raii--the-central-idiom)

---

## 5.1 struct vs class and access control

The **only** language difference: default member access **and** default
inheritance access.

```cpp
struct S { int x; };     // members public by default;  struct D : Base → public Base
class  C { int x; };     // members private by default;  class  D : Base → private Base
```

```
   ACCESS SPECIFIERS               applies to members AND base-class subobjects
   public:     anyone                         (the interface)
   protected:  this class + derived classes
   private:    this class + its friends only  (the implementation / invariant)
```

🧠 Encapsulation exists to protect **invariants**. If a type has no invariant, a
`struct` (all public) is appropriate. Access is checked at compile time and is
about *names*, not memory — a `private` member still occupies object storage and is
visible to `friend`s.

---

## 5.2 Object layout & `this`

```
   class Point { int x; int y; void move(int,int); };
   Point p;
   ┌──────── object p (sizeof == 8)  ────────┐
   │  x : int (4B)  │  y : int (4B)          │
   └─────────────────────────────────────────┘
   p.move(1,2)   ≡   Point::move(&p, 1, 2)
                                  ▲ implicit 'this' (Point* const; const Point* in const member)
```

- Non-static member functions receive an implicit `this` pointer (C++23 lets you
  name it explicitly — §5.8 "deducing this").
- Data members are laid out in **declaration order** (with padding); members with
  the same access are guaranteed in order. Member functions cost **no** per-object
  space (only one copy of the code).
- `static` data members and member functions have **no** `this`.

---

## 5.3 The complete constructor catalog

A constructor initializes an object's members and establishes its invariant. Here
is **every** kind.

### (1) Default constructor — `T()`

```cpp
class T {
public:
    T();                 // user-provided default ctor
    T() = default;       // explicitly defaulted (compiler generates it)
    T() = delete;        // no default ctor (T() is ill-formed)
    constexpr T();       // usable in constant expressions
    T() noexcept;        // promises not to throw
};
```

```
   • IMPLICITLY declared & defined if you declare NO constructors at all.
   • SUPPRESSED the moment you declare ANY other constructor (add T()=default to get it back).
   • TRIVIAL default ctor: implicitly defined, members are built-in/trivial,
     no virtual, no base init → does NOTHING (leaves members uninitialized!).
   • value-init  T()  vs default-init  T t;  differ for trivial types (Appendix A).
```

### (2) Parameterized constructor

```cpp
class Rectangle {
    double w_, h_;
public:
    Rectangle(double w, double h) : w_(w), h_(h) {}
};
```

### (3) Converting constructor (implicit) vs `explicit`

```cpp
struct A { A(int); };            // CONVERTING: enables implicit int → A
struct B { explicit B(int); };   // explicit: no implicit conversion

void fa(A); void fb(B);
fa(5);            // ✅ implicit 5 → A
// fb(5);         // ❌ explicit
fb(B{5});         // ✅
```

A constructor callable with a **single** argument (or all-but-one defaulted)
defines an implicit conversion **unless** marked `explicit`.

```cpp
// C++20 conditional explicit:
template<class T>
struct Wrapper {
    explicit(!std::is_convertible_v<T, int>) Wrapper(T);  // explicit only sometimes
};
```

🧠 **Default to `explicit`** on single-argument constructors. `explicit` also
suppresses copy-list-initialization: `A a = {5};` is rejected for an `explicit` ctor.

### (4) Copy constructor — `T(const T&)`

```cpp
class T {
public:
    T(const T&);              // canonical copy ctor
    T(T&);                    // legal but rare (can't copy const/temporaries)
    T(const volatile T&);     // legal; needed only for volatile sources
    T(const T&) = default;    // request the implicit memberwise copy
    T(const T&) = delete;     // non-copyable type (e.g. unique ownership)
};
```

```
   A copy constructor has a first parameter of type  T& / const T& / volatile T& /
   const volatile T&  AND no other params (or the rest defaulted).
   ⚠️ T(T) (by value) is NOT a copy ctor — it's ill-formed (infinite copy).
   The implicit copy ctor does MEMBERWISE copy (copies each base & member).
```

### (5) Move constructor — `T(T&&)`

```cpp
class T {
public:
    T(T&&) noexcept;          // canonical move ctor — ALWAYS mark noexcept (Module 06)
    T(T&&) = default;
    T(T&&) = delete;          // move-disabled (rare)
};
```

```
   First parameter is an rvalue reference  T&&  (no other non-defaulted params).
   Leaves the source in a VALID BUT UNSPECIFIED state. noexcept matters: vector
   uses moves only if the move ctor is noexcept (else it copies). See Module 06.
```

### (6) Delegating constructor (C++11)

```cpp
class Widget {
    int id_; std::string name_;
public:
    Widget(int id, std::string name) : id_(id), name_(std::move(name)) {}  // target
    Widget()           : Widget(0, "unnamed") {}        // delegates to the target ctor
    Widget(int id)     : Widget(id, "unnamed") {}       // delegates
};
```

```
   A constructor may delegate to ANOTHER constructor of the SAME class in its
   member-init list (and ONLY that — you can't also init members in the same list).
   ⚠️ A cycle of delegations is undefined behavior.
   The target ctor runs fully (members constructed) before the delegating body.
```

### (7) Inheriting constructors (C++11) — `using Base::Base;`

```cpp
struct Base { Base(int); Base(int, int); Base(const std::string&); };
struct Derived : Base {
    using Base::Base;       // inherit ALL of Base's constructors
    int extra_ = 0;         // gets its default member initializer
};
Derived d{42};              // calls the inherited Base(int); extra_ = 0
```

```
   'using Base::Base;' makes Base's constructors usable to construct Derived
   (Derived's own members use their default member initializers).
   ⚠️ Does not inherit default/copy/move ctors; if Derived adds members WITHOUT
   default initializers, those would be uninitialized — give them = {} defaults.
```

### (8) `std::initializer_list` constructor

```cpp
#include <initializer_list>
class Seq {
    std::vector<int> data_;
public:
    Seq(std::initializer_list<int> il) : data_(il) {}   // braced-list ctor
    Seq(std::size_t n, int value);                       // "n copies" ctor
};
Seq a{1, 2, 3};      // ⟶ initializer_list ctor: {1,2,3}
Seq b(3, 7);         // ⟶ (n,value) ctor: 3 sevens
Seq c{3, 7};         // ⚠️ initializer_list WINS over (n,value): elements {3,7}
```

```
   🧠 BRACE INITIALIZATION STRONGLY PREFERS an initializer_list constructor if one
   is viable — even over a "better" matching constructor. This is THE vector{5} vs
   vector(5) trap (Module 16). Use () when you want a non-list constructor.
```

### (9) `constexpr` / `consteval` constructor

```cpp
struct Point {
    int x, y;
    constexpr Point(int x, int y) : x(x), y(y) {}   // usable at compile time
};
constexpr Point origin{0, 0};                        // built at compile time
```

A `constexpr` constructor lets objects be constructed in constant expressions
(literal types). `consteval` ctors (C++20) must run at compile time.

### (10) Defaulted & deleted constructors

```cpp
struct T {
    T() = default;                 // generate the trivial/implicit one
    T(const T&) = delete;          // ban copying
    T(int) = delete;               // ban a specific overload (also useful on free fns)
};
```

`= default` can be **in-class** or **out-of-line** (`T::T() = default;` in the
`.cpp`, which can change triviality/ABI). `= delete` makes any use a compile error.

### (11) Aggregate (no constructor) — see §5.12

### (12) Templated / perfect-forwarding constructor (and its trap)

```cpp
class S {
    std::string s_;
public:
    template<class T>
    explicit S(T&& x) : s_(std::forward<T>(x)) {}   // ⚠️ greedy forwarding ctor
    S(const S&);                                     // copy ctor
};
// ⚠️ S a; S b{a};  may call the TEMPLATE (T=S&) instead of the copy ctor for
//    a non-const lvalue, because T&& is an exact match. Constrain it:
//    template<class T> requires (!std::is_same_v<std::decay_t<T>, S>) S(T&&);
```

🧠 A forwarding constructor `template<class T> S(T&&)` is a near-universal match
and can **hijack** copy/move. Constrain it with a concept/`enable_if` excluding
`S` itself. This is a famous expert gotcha (Scott Meyers, *Effective Modern C++* Item 26–27).

### Triviality, literal types, POD (terminology)

```
   TRIVIAL ctor:        implicitly provided, no work (no base/member init, no virtual).
   TRIVIALLY COPYABLE:  can be copied with memcpy (no user copy/move/dtor; trivial ones).
   STANDARD-LAYOUT:     C-compatible layout (no mixed access, no virtual, etc.).
   POD (deprecated term)= trivial + standard-layout.
   LITERAL TYPE:        usable in constexpr (constexpr ctor, trivial dtor, ...).
   Check with: std::is_trivial_v, std::is_trivially_copyable_v, std::is_standard_layout_v.
```

---

## 5.4 Member initializer lists — the full rules

```cpp
class Widget {
    const int   id_;        // const     → MUST be in init list
    std::string name_;
    int&        ref_;       // reference → MUST be in init list
    Gadget      g_;         // no default ctor → MUST be in init list
public:
    Widget(int id, std::string n, int& r)
        : id_(id),                 // direct-initialize each member
          name_(std::move(n)),
          ref_(r),
          g_(id)                   // construct g_ with id
    { /* body: all members already constructed */ }
};
```

```
   ❌ assignment in body                ✅ member initializer list
   Widget(int id){ id_ = id; }          Widget(int id) : id_(id) {}
   (member is default-constructed       (member is DIRECT-initialized once;
    THEN assigned ⇒ two ops; and         works for const, references, and
    impossible for const/ref/no-default) types without a default ctor)
```

**Rules an expert must know:**

1. **Initialization order is DECLARATION order**, NOT the order written in the
   list. The compiler reorders to declaration order; `-Wreorder` warns.

   ```cpp
   class C { int a; int b;
   public: C(int x) : b(x), a(b) {} };  // ⚠️ a init FIRST (declared first) using uninit b!
   ```

2. **You MUST use the init list for**: `const` members, reference members, members
   of a type with no accessible default constructor, and base-class subobjects
   needing non-default construction.

3. **Default member initializers** (C++11) provide a fallback; the init list overrides.

   ```cpp
   class Config {
       int retries_ = 3;                 // default member initializer
       std::string host_ = "localhost";
   public:
       Config() = default;               // uses defaults
       Config(int r) : retries_(r) {}    // overrides retries_ only
   };
   ```

4. Bases are initialized **before** members (§5.7), regardless of list order.

5. A delegating constructor (§5.3-6) may **not** also list members.

---

## 5.5 The complete destructor catalog

A destructor releases resources when an object's lifetime ends. Exactly **one**
destructor per class (no overloading, no parameters).

### (1) Default / trivial destructor

```cpp
class T { public: ~T() = default; };   // or implicitly provided if none declared
```

```
   TRIVIAL destructor: implicitly provided, all members/bases trivially destructible,
   not virtual → does nothing. Enables memcpy-style relocation, literal types.
```

### (2) User-defined destructor

```cpp
class File {
    std::FILE* f_;
public:
    ~File() { if (f_) std::fclose(f_); }   // release the resource
};
```

### (3) Virtual destructor

```cpp
class Base { public: virtual ~Base() = default; };  // REQUIRED for polymorphic deletion
class Derived : public Base { Big data_; };
Base* p = new Derived;
delete p;   // ✅ runs ~Derived then ~Base, because ~Base is virtual
```

```
   RULE (Core Guidelines C.35): a base class destructor is either
     • PUBLIC and VIRTUAL    (you delete through Base*), or
     • PROTECTED and NON-virtual (you cannot delete through Base* — prevents the bug).
   ⚠️ Non-virtual base dtor + delete via Base* = UB (only ~Base runs → slice/leak).
```

### (4) Pure virtual destructor (with a definition)

```cpp
class AbstractBase {
public:
    virtual ~AbstractBase() = 0;          // pure ⇒ class is abstract
};
AbstractBase::~AbstractBase() {}          // ⚠️ MUST still provide a body (called by derived dtors)
```

A pure virtual destructor makes a class abstract while still being invoked during
derived destruction — hence it **needs a definition**.

### (5) Defaulted / deleted destructor

```cpp
class T { public: ~T() = default; };   // explicit default
class U { public: ~U() = delete; };    // cannot be destroyed (only via placement/leak; rare)
```

### (6) `constexpr` destructor (C++20)

```cpp
struct Lit { constexpr ~Lit() {} };    // enables constexpr objects with non-trivial dtors
```

### (7) `noexcept` nature

```
   Destructors are IMPLICITLY noexcept(true). A destructor that throws during
   stack unwinding (another exception in flight) ⇒ std::terminate(). NEVER throw
   from a destructor (Module 13). To allow throwing (don't): ~T() noexcept(false).
```

```
   DESTRUCTOR FACTS:
   • exactly one per class; no params; no return; cannot be overloaded.
   • runs members in REVERSE declaration order, then bases in reverse (§5.7).
   • virtual if any base's dtor is virtual.
   • calling delete on a base* needs the base dtor virtual.
   • explicit call p->~T() is legal (used with placement new, Module 03).
```

---

## 5.6 The complete assignment-operator catalog

(Detailed semantics in [Module 06](06_copy_move_semantics.md); here is the *signature* reference.)

```cpp
class T {
public:
    // (1) copy assignment — canonical forms
    T& operator=(const T&);            // standard
    T& operator=(T);                   // copy-and-swap (by value) — covers copy AND move
    T& operator=(T&);                  // legal, rare

    // (2) move assignment
    T& operator=(T&&) noexcept;

    // (3) defaulted / deleted
    T& operator=(const T&) = default;  // memberwise copy assign
    T& operator=(T&&)      = default;  // memberwise move assign
    T& operator=(const T&) = delete;   // non-assignable

    // (4) ref-qualified (restrict assignable value category)
    T& operator=(const T&) &;          // only to lvalues: a = b ok; f() = b ERROR
    T& operator=(const T&) && = delete;

    // (5) templated / heterogeneous assignment (NOT a special member)
    template<class U> T& operator=(const U&);
    T& operator=(std::initializer_list<int>);   // assign from a braced list
};
```

```
   RETURN T& to *this (enables a = b = c). Handle SELF-ASSIGNMENT (copy-and-swap
   does automatically). The implicit copy/move assignment does memberwise assign.
   ⚠️ Declaring a copy/move assignment affects what else is auto-generated (§5.9).
```

---

## 5.7 The full construction/destruction order

```
   Constructing a Derived object proceeds STRICTLY in this order:
   1. VIRTUAL base classes   (once, by the most-derived class, in DFS-left-to-right order)
   2. DIRECT non-virtual bases   (in declaration order of the base list)
   3. NON-STATIC data members    (in DECLARATION order — NOT init-list order)
   4. the constructor BODY { }

   Destruction is the EXACT REVERSE:
   1. the destructor body { }
   2. members destroyed in REVERSE declaration order
   3. non-virtual bases in reverse
   4. virtual bases in reverse
```

```cpp
struct A { A(){std::puts("A");} ~A(){std::puts("~A");} };
struct B { B(){std::puts("B");} ~B(){std::puts("~B");} };
struct Base { Base(){std::puts("Base");} ~Base(){std::puts("~Base");} };
struct C : Base {
    A a;   // declared 1st
    B b;   // declared 2nd
    C(){std::puts("C body");}
    ~C(){std::puts("~C body");}
};
// Construct order: Base, A, B, "C body"
// Destruct order : "~C body", ~B, ~A, ~Base
```

🧠 **Virtual calls during construction/destruction resolve to the
currently-constructing type**, not the most-derived type — the derived part
doesn't exist yet (or has already been destroyed). Never expect dynamic dispatch
from a ctor/dtor (Module 16 §16.8).

---

## 5.8 Member-function qualifiers — complete reference

A non-static member function can carry many qualifiers. Full grammar:

```
   return-type  Class::name (params)  cv-qual  ref-qual  noexcept-spec  virt-spec  trailing-return  body/=default/=delete/=0
                                        │        │         │              │
   const/volatile ──────────────────────┘        │         │              └ override / final
   & / && (lvalue/rvalue) ───────────────────────┘         └ noexcept / noexcept(expr)
```

### cv-qualifiers: `const` / `volatile`

```cpp
class T {
    mutable int cache_;
public:
    int  get() const;          // 'this' is const T* → cannot modify (except mutable members)
    void set(int);             // non-const → may modify
    int  poll() volatile;      // for volatile objects (memory-mapped I/O) — rare
};
```

```
   const and non-const member functions OVERLOAD (the compiler picks based on the
   object's constness):
     int&       at(size_t i);        // chosen for non-const object
     const int& at(size_t i) const;  // chosen for const object
   A const object can ONLY call const member functions.
```

### ref-qualifiers: `&` / `&&`

```cpp
class Buffer {
    std::vector<char> data_;
public:
    std::vector<char>  get() &&  { return std::move(data_); }  // called on rvalues: steal
    const std::vector<char>& get() const &  { return data_; } // called on lvalues: observe
};
Buffer{}.get();    // ⟶ && overload (rvalue) → moves out
Buffer b; b.get(); // ⟶ const& overload (lvalue) → returns reference
```

```
   ref-qualifiers restrict member functions by the value category of the object:
     f() &   → callable only on lvalues
     f() &&  → callable only on rvalues
   Powerful for: "steal from a temporary" and forbidding assignment to temporaries.
   You may NOT mix ref-qualified and non-ref-qualified overloads of the same name.
```

### `noexcept` specification

```cpp
void swap(T&) noexcept;                 // promise: never throws
void f() noexcept(sizeof(int) == 4);    // conditional
```

### `virtual`, `override`, `final`, pure (`= 0`)

```cpp
struct Base {
    virtual void f();              // overridable
    virtual void g() = 0;          // pure virtual ⇒ Base is abstract
    virtual ~Base() = default;
};
struct D : Base {
    void f() override;             // checked override (error if signature mismatches)
    void g() override final;       // override AND forbid further overriding
};
struct DD final : D { };           // DD cannot be derived from
```

```
   virtual   → dynamic dispatch (vtable, Module 07)
   = 0       → pure virtual; class becomes abstract (can't instantiate)
   override  → compiler verifies it actually overrides a base virtual (USE ALWAYS)
   final     → on a function: no further override; on a class: no derivation
   A pure virtual may still have a DEFINITION (called via Base::g()).
```

### `static` member functions

```cpp
class Math { public: static int add(int a, int b){ return a+b; } };
Math::add(2,3);    // no object, no 'this'
```

### `= default`, `= delete`, trailing return type

```cpp
T(const T&) = default;
T(const T&) = delete;
auto compute() const -> double;     // trailing return type
```

### C++23: explicit object parameter ("deducing this")

```cpp
struct Widget {
    template<class Self>
    auto&& value(this Self&& self) {        // 'this' becomes a real, deducible parameter
        return std::forward<Self>(self).v_; // ONE function handles const/non-const/&/&&!
    }
    int v_;
};
```

```
   C++23 'deducing this' lets you write ONE member template instead of 4 overloads
   (const&, &, const&&, &&). 'self' carries the cv/ref qualification of the object.
   Also enables recursive lambdas and CRTP-free mixins.
```

---

## 5.9 Special member functions & generation rules

The compiler can synthesize **six** special members:

```
   T();                          default constructor
   ~T();                         destructor
   T(const T&);                  copy constructor
   T& operator=(const T&);       copy assignment
   T(T&&) noexcept;              move constructor   (C++11)
   T& operator=(T&&) noexcept;   move assignment    (C++11)
```

Generation rules (the table every expert memorizes):

| You declare... | default ctor | dtor | copy ctor | copy = | move ctor | move = |
|---|:---:|:---:|:---:|:---:|:---:|:---:|
| nothing | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| any constructor | ❌ | ✅ | ✅ | ✅ | ✅ | ✅ |
| **destructor** | ✅ | — | ⚠️dep | ⚠️dep | **❌** | **❌** |
| copy ctor or copy = | ✅ | ✅ | ⚠️/— | ⚠️/— | **❌** | **❌** |
| move ctor or move = | ✅ | ✅ | **=del** | **=del** | —/❌ | ❌/— |

`⚠️dep` = still generated but **deprecated**. `❌` = not provided. `=del` = deleted.

🧠 **The deadliest silent pitfall**: declaring a destructor (e.g. for logging)
**suppresses the move operations**, so every "move" silently becomes a **copy** —
no error, just slow. Cures: Rule of Zero, or explicitly `= default` the moves.

The mnemonic **"Rule of DesDeMovA"** / the Rule of Five (full treatment in
[Module 06](06_copy_move_semantics.md)):

```
   RULE OF ZERO : declare none of the six; let members manage resources. ← PREFER.
   RULE OF FIVE : if you manage a raw resource, declare all five
                  (dtor + copy ctor/assign + move ctor/assign), all consistent.
```

---

## 5.10 static members

```cpp
class Counter {
    static inline int count_ = 0;   // C++17 inline static: define + init in-class, ONE instance
    int id_;
public:
    Counter()  : id_(++count_) {}
    ~Counter() { --count_; }
    static int live() { return count_; }     // static member fn: no 'this'
    static constexpr int kMax = 1000;         // static constexpr (implicitly inline)
};
// pre-C++17 required out-of-line definition:  int Counter::count_ = 0;
```

```
   per-object members        static members
   ┌────────┐ ┌────────┐     ┌───────────────────┐
   │ obj1   │ │ obj2   │     │ count_ (ONE copy, │  shared by ALL objects; lives in
   │ id_    │ │ id_    │     │ static storage)   │  static storage, not in any object.
   └────────┘ └────────┘     └───────────────────┘
```

🧠 A function-local `static` (Meyers singleton) is initialized **once, thread-safe**
on first call (C++11) and solves the static-init-order fiasco (Module 02 §2.10).

---

## 5.11 friend

`friend` grants a function or class access to `private`/`protected` members. It is
an explicit **extension** of the interface, not a violation of encapsulation.

```cpp
class Matrix {
    double* data_;
    friend Matrix operator*(const Matrix&, const Matrix&);   // free function friend
    friend std::ostream& operator<<(std::ostream&, const Matrix&);
    friend class MatrixView;                                  // friend class
    template<class T> friend class Wrapper;                   // friend template
};
```

```
   • friendship is GRANTED, not taken (declared inside the granting class).
   • NOT transitive (a friend's friend isn't a friend), NOT inherited.
   • A friend declared & defined inline in the class is a non-member with ADL
     visibility ("hidden friend" idiom — great for operators; found only via ADL).
```

🧠 **Hidden friends**: defining `operator<<`/`operator==` as inline friends inside
the class makes them findable only by argument-dependent lookup, which speeds
compilation and avoids polluting the namespace. Preferred for operators.

---

## 5.12 Aggregates & aggregate initialization

An **aggregate** (C++20 definition) is an array or a class with: no user-declared/
inherited constructors, no private/protected **non-static** data members, no
virtual functions, and no private/protected/virtual bases.

```cpp
struct Point3 { int x, y, z; };               // aggregate (no ctors, all public)
Point3 a{1, 2, 3};                            // aggregate init (memberwise, in order)
Point3 b{1};                                  // x=1, y=0, z=0 (rest value-initialized)
Point3 c{.x = 1, .z = 3};                     // designated initializers (C++20); y=0
int arr[]{1, 2, 3};                           // arrays are aggregates too

struct Derived : Point3 { int w; };           // C++17: aggregates may have public bases
Derived d{{1,2,3}, 4};                        // base then member
```

```
   AGGREGATE rules:
   • brace-init writes members in DECLARATION order; missing ones are value-init.
   • designated initializers must be in declaration order, non-repeating (C++20).
   • adding ANY user constructor or a private data member ⇒ no longer an aggregate.
   • aggregates skip constructor machinery entirely (no ctor is called).
```

---

## 5.13 RAII — the central idiom

```cpp
class Lock {
    std::mutex& m_;
public:
    explicit Lock(std::mutex& m) : m_(m) { m_.lock(); }   // acquire in ctor
    ~Lock() { m_.unlock(); }                              // release in dtor
    Lock(const Lock&) = delete;                           // non-copyable resource
    Lock& operator=(const Lock&) = delete;
};
```

```
   RAII — Resource Acquisition Is Initialization
   ┌─────────────────────────────────────────────────────────────────┐
   │  acquire in CONSTRUCTOR → use → release in DESTRUCTOR.          │
   │  The destructor runs on EVERY scope exit: normal return, break, │
   │  and EXCEPTION unwinding ⇒ no leaks, no manual cleanup, no      │
   │  'finally'. This is THE C++ resource-management idiom.          │
   └─────────────────────────────────────────────────────────────────┘
   {
       Lock g(mtx);     // ctor locks
       work();          // may throw...
   }                    // ~Lock unlocks — GUARANTEED, even on exception
```

Every resource — memory, files, sockets, locks, GPU handles — should be owned by
an RAII object. `std::unique_ptr`, `std::lock_guard`, `std::fstream` are RAII
wrappers (Modules 11, 15). **If you write manual cleanup on multiple paths, use
RAII instead.**

---

## 5.14 Checklist

- [ ] You can name and write every constructor kind (default, parameterized,
      converting/explicit, copy, move, delegating, inheriting, init-list,
      constexpr, defaulted, deleted, forwarding) and its trap.
- [ ] You know the destructor catalog incl. virtual, pure-virtual-with-body, and noexcept.
- [ ] You can write all assignment forms incl. ref-qualified and copy-and-swap.
- [ ] You initialize members via the init list in DECLARATION order.
- [ ] You know the bases→members→body construction order and its reverse.
- [ ] You can apply cv/ref-qualifiers, `override`/`final`/`= 0`, and C++23 deducing-this.
- [ ] You know the SMF generation table and the destructor-kills-move trap.
- [ ] You use RAII for every resource and prefer the Rule of Zero.

➡ Next: [Module 06 — Copy, Move & the Rules of 0/3/5](06_copy_move_semantics.md)
