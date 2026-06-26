# Module 07 — Inheritance & Polymorphism (OOP Part 3)

> Inheritance models "is-a" relationships and enables **runtime polymorphism** via
> virtual functions. The implementation (vtables, vptrs) is not magic — once you
> see the layout, slicing, the non-virtual-destructor bug, and the cost of virtual
> calls all become obvious. `[Primer §15]`

---

## 7.1 Basic inheritance

```cpp
class Animal {
public:
    explicit Animal(std::string name) : name_(std::move(name)) {}
    const std::string& name() const { return name_; }
    void breathe() const { /* shared behavior */ }
protected:
    std::string name_;     // accessible to derived classes
};

class Dog : public Animal {     // Dog IS-A Animal
public:
    explicit Dog(std::string n) : Animal(std::move(n)) {}  // must init base
    void bark() const { /* Dog-specific */ }
};
```

```
   OBJECT LAYOUT of a Dog
   ┌───────────────────────────────┐
   │  Animal subobject             │ ← base part comes first
   │    name_ : std::string        │
   ├───────────────────────────────┤
   │  Dog-specific members         │
   └───────────────────────────────┘
   A Dog* can be used as an Animal* because the Animal part is at offset 0.
```

### public / protected / private inheritance

```
   class D : public  B   → "is-a"      (public of B stays public)    ← 99% of the time
   class D : protected B → "implemented-in-terms-of" (public→protected)
   class D : private B   → "implemented-in-terms-of" (public→private) ← prefer COMPOSITION instead
```

🧠 Use **public inheritance only for true "is-a"** with the Liskov Substitution
Principle (a `D` must be usable wherever a `B` is). For "has-a"/"uses-a", prefer
**composition** (a member) over private inheritance.

---

## 7.2 Virtual functions & dynamic dispatch

```cpp
class Shape {
public:
    virtual double area() const = 0;     // PURE virtual ⇒ Shape is ABSTRACT
    virtual void   draw() const { /*default*/ }
    virtual ~Shape() = default;          // ⚠️ virtual dtor — see 7.5
};

class Circle : public Shape {
    double r_;
public:
    explicit Circle(double r) : r_(r) {}
    double area() const override { return 3.14159265 * r_ * r_; }  // override!
    void   draw() const override { /* draw circle */ }
};
```

```cpp
std::unique_ptr<Shape> s = std::make_unique<Circle>(2.0);
s->area();   // calls Circle::area() — resolved at RUNTIME via the vtable
```

```
   STATIC dispatch (non-virtual): decided at compile time by the STATIC type
   DYNAMIC dispatch (virtual):    decided at run time by the DYNAMIC type
```

⚠️ Always write `override`. It makes the compiler verify you actually override a
base virtual (catches typos, const mismatches, signature drift). Use `final` to
forbid further overriding/derivation.

```cpp
double area() const override;   // error if no matching base virtual exists
class Circle final : public Shape { ... };   // no one may derive from Circle
void draw() const final;        // no one may override draw further
```

---

## 7.3 How it works: vtable & vptr 🧠

```
   Each polymorphic CLASS has ONE vtable (array of function pointers).
   Each polymorphic OBJECT has a hidden vptr pointing to its class's vtable.

   Circle c;
   ┌──────────────────┐        ┌──────── Circle vtable ─────────┐
   │ vptr ────────────┼───────▶│ [0] &Circle::area              │
   │ Shape members... │        │ [1] &Circle::draw              │
   │ r_ = 2.0         │        │ [2] &Circle::~Circle           │
   └──────────────────┘        └────────────────────────────────┘

   s->area()  compiles to roughly:
       (*(s->vptr[0]))(s)        // index fixed at compile time; target chosen at run time
```

Cost of a virtual call:
- One extra indirection (load vptr → load slot → indirect call).
- Usually **not inlinable** (target unknown at compile time) → this is often the
  bigger cost than the indirection itself.
- One pointer (`vptr`) of space per object; one vtable per class.

🧠 Devirtualization: if the compiler can prove the dynamic type (e.g. `Circle c;
c.area();` on a concrete object, or with `final`), it can call directly and inline.
`final` is a real optimization hint, not just documentation.

---

## 7.4 Object slicing — a silent value-semantics trap ⚠️

```cpp
void render(Shape s);          // ⚠️ by VALUE — takes a Shape, not a Circle
Circle c(2.0);
render(c);                     // SLICES: only the Shape part is copied; area() is wrong
```

```
   Circle c                    copied BY VALUE into a Shape parameter
   ┌──────────────┐            ┌──────────────┐
   │ Shape part   │ ──copy──▶  │ Shape part   │   ← only this survives
   │ r_ = 2.0     │   ✂        └──────────────┘   ← r_ is SLICED OFF; vptr is Shape's
   └──────────────┘
```

🧠 **Polymorphism only works through pointers or references**, never by value.
Pass `const Shape&` or `Shape*` (or a smart pointer). Storing polymorphic objects
in a `std::vector<Shape>` slices them — store `std::vector<std::unique_ptr<Shape>>`.

---

## 7.5 The non-virtual destructor bug ⚠️

```cpp
class Base { public: ~Base(){} };          // ⚠️ NON-virtual destructor
class Derived : public Base { Big data_; };
Base* p = new Derived;
delete p;                                  // ⚠️ UB: only ~Base runs, ~Derived skipped → leak
```

```
   RULE: If a class is meant to be deleted through a base pointer,
         its destructor MUST be virtual.
   Guideline (Core Guidelines C.35): a base class destructor should be
   either PUBLIC + VIRTUAL, or PROTECTED + NON-virtual.
```

```cpp
class Base { public: virtual ~Base() = default; };   // ✅ deletes through Base* correctly
```

---

## 7.6 Calling base versions & name hiding

```cpp
class D : public B {
public:
    void f() override {
        B::f();          // explicitly call the base version
        /* extra */
    }
};
```

⚠️ **Name hiding**: a derived declaration of a name hides *all* base overloads of
that name, even with different signatures:

```cpp
struct B { void g(int); void g(double); };
struct D : B { void g(int); };   // hides BOTH B::g — D{}.g(3.14) calls D::g(int)!
struct D2: B { using B::g; void g(int); };  // ✅ 'using' un-hides the base overloads
```

---

## 7.7 Abstract classes & interfaces

```cpp
class Drawable {                       // a pure interface
public:
    virtual void draw() const = 0;     // pure virtual
    virtual ~Drawable() = default;
};
// Cannot instantiate Drawable directly; must derive and implement draw().
```

C++ has no `interface` keyword — an interface is just an abstract class with only
pure virtuals (and a virtual dtor). A pure virtual *may* still have a body
(`void Drawable::draw() const {}`) to provide a callable default via `Base::draw()`.

---

## 7.8 Multiple inheritance & the diamond

```cpp
struct A { int a; };
struct B : A {};
struct C : A {};
struct D : B, C {};      // ⚠️ D has TWO A subobjects (one via B, one via C)
```

```
   THE DIAMOND PROBLEM
        A           Without 'virtual': D contains B::A and C::A — d.a is AMBIGUOUS.
       / \
      B   C         With virtual inheritance: ONE shared A.
       \ /
        D

   struct B : virtual A {};
   struct C : virtual A {};
   struct D : B, C {};      // ✅ exactly one A subobject; d.a is unambiguous
```

🧠 Virtual inheritance has costs (extra pointers/offsets to find the shared base,
trickier construction — the *most derived* class initializes the virtual base).
Most designs avoid it; the common, clean use is **multiple interface inheritance**
(several abstract bases with no data).

---

## 7.9 RTTI: `dynamic_cast` & `typeid`

```cpp
Shape* s = getShape();
if (auto* c = dynamic_cast<Circle*>(s)) {   // returns nullptr if not a Circle
    use(c->radius());
}
Shape& r = *s;
try { auto& c = dynamic_cast<Circle&>(r); } // throws std::bad_cast on failure (refs)
catch (const std::bad_cast&) { }

#include <typeinfo>
if (typeid(*s) == typeid(Circle)) { ... }   // exact dynamic type query
```

```
   dynamic_cast<Derived*>(basePtr)   needs a POLYMORPHIC type (>=1 virtual fn)
     success → Derived*    failure → nullptr (pointers) / throws (references)
```

🧠 Frequent `dynamic_cast` in hot paths is a design smell — prefer a virtual
function that does the type-specific work. RTTI also has a (small) runtime cost
and can be disabled with `-fno-rtti`.

---

## 7.10 Static (compile-time) polymorphism preview: CRTP

Runtime polymorphism (virtual) has indirection cost. **CRTP** (Module 09) gives
polymorphism resolved at compile time, zero overhead:

```cpp
template<class Derived>
struct Shape {
    double area() const { return static_cast<const Derived&>(*this).area_impl(); }
};
struct Circle : Shape<Circle> {
    double r_;
    double area_impl() const { return 3.14159 * r_ * r_; }  // no vtable, inlinable
};
```

---

## 7.11 Checklist

- [ ] You use public inheritance only for genuine "is-a" (LSP); else compose.
- [ ] You always write `override`; you use `final` where appropriate.
- [ ] You can draw the vtable/vptr diagram and explain virtual-call cost.
- [ ] You give polymorphic base classes a virtual (or protected) destructor.
- [ ] You never pass polymorphic objects by value (slicing).
- [ ] You understand the diamond and when virtual inheritance is warranted.

➡ Next: [Module 08 — Operator Overloading](08_operator_overloading.md)
