# Appendix C — Name Lookup, ADL & Namespaces: The Complete Reference

> Before overload resolution (Appendix B) can rank functions, the compiler must
> *find* the candidate names. Name lookup is a precise, multi-stage process, and
> **Argument-Dependent Lookup (ADL)** is the rule that makes `std::cout << x` and
> `swap(a, b)` work — and occasionally surprises you. `[Primer §16.4, §18.2;
> standard [basic.lookup]]`

---

## C.1 Two kinds of lookup

```
   QUALIFIED lookup     a::b , obj.member , ptr->member
     → look only in the named scope (and base classes for members).
   UNQUALIFIED lookup   plain  name
     → ORDINARY lookup (lexical scopes outward) + (for function calls) ADL.
```

---

## C.2 Unqualified ordinary lookup — the scope chain

```
   For an unqualified name, search OUTWARD until found, then STOP:
   ┌─────────────────────────────────────────────────────────────┐
   │ 1. local block scope (innermost first)                      │
   │ 2. enclosing blocks                                         │
   │ 3. function parameter scope                                 │
   │ 4. class scope (members) + base classes                     │
   │ 5. enclosing class(es)                                      │
   │ 6. enclosing namespace(s), innermost to outermost           │
   │ 7. global namespace                                         │
   │ 8. using-directives bring in names at the relevant scope    │
   └─────────────────────────────────────────────────────────────┘
   ⚠️ Lookup stops at the FIRST scope that has the name — even if that name is the
      "wrong" overload set. (Name HIDING / shadowing.)
```

```cpp
int x = 1;
void f() {
    int x = 2;          // hides the global x
    { int x = 3;        // hides the outer local x
      use(x);           // 3
    }
    use(x);             // 2
    use(::x);           // 1 (qualified — the global)
}
```

---

## C.3 Argument-Dependent Lookup (ADL / Koenig lookup) 🧠

For a **function call** `f(args)`, in addition to ordinary lookup, the compiler
also searches the namespaces **associated with the argument types**.

```cpp
namespace lib {
    struct Widget {};
    void draw(Widget);          // not in scope, but...
}
lib::Widget w;
draw(w);                        // ✅ found via ADL: Widget's namespace 'lib' is searched
```

```
   ASSOCIATED NAMESPACES/CLASSES of an argument include:
   • the namespace the type is declared in
   • the type itself and its base classes
   • for a class template specialization: the template's namespace + the namespaces
     of its template type arguments
   • for enums: the enclosing namespace
   • for pointers/arrays: the associated set of the pointed-to/element type
```

Why ADL exists: it lets operators and free functions be found in the type's
namespace without explicit qualification. **This is why these work:**

```cpp
std::cout << x;          // operator<<(std::ostream&, ...) found in std via ADL
std::string a, b;
a + b;                   // std::operator+ found via ADL
using std::swap; swap(a, b);  // the std::swap-or-custom-swap idiom relies on ADL
std::ranges::sort(v);    // niebloids deliberately DISABLE ADL (see C.6)
```

---

## C.4 The `swap` two-step idiom (ADL done right)

```cpp
template<class T>
void relocate(T& a, T& b) {
    using std::swap;     // make std::swap a candidate...
    swap(a, b);          // ...then UNQUALIFIED call lets ADL find a better T-specific swap
}
```

```
   ❌ std::swap(a, b);   always uses std::swap (misses a custom, faster swap)
   ✅ using std::swap; swap(a, b);
        → if T's namespace has a custom swap, ADL finds it (preferred);
          otherwise std::swap is the fallback. Same pattern for begin/end, etc.
```

🧠 This "using-declaration + unqualified call" is the canonical customization
point pattern in pre-C++20 generic code. C++20 ranges replace many of these with
*customization point objects* (C.6).

---

## C.5 Namespaces — organization & control

```cpp
namespace app {                         // named namespace
    namespace net { class Socket; } }   // nested
namespace app::net { class Socket; }    // C++17 nested shorthand

namespace fs = std::filesystem;         // namespace ALIAS

using app::net::Socket;                 // using-DECLARATION: brings ONE name in
using namespace std::literals;          // using-DIRECTIVE: brings ALL names in (use sparingly)

namespace {                             // ANONYMOUS namespace → internal linkage
    int helper();                       //   (TU-private; preferred over 'static')
}

inline namespace v2 { void api(); }     // INLINE namespace → members act as if in the
                                        //   enclosing namespace (used for ABI versioning)
```

```
   using-DECLARATION  using ns::name;     adds ONE name to the current scope (controlled).
   using-DIRECTIVE    using namespace ns;  makes ALL of ns visible (⚠️ in headers = pollution).
   ANONYMOUS namespace                     internal linkage, TU-local (replaces file 'static').
   INLINE namespace                        transparent versioning: std::literals uses this.
```

⚠️ **Never put `using namespace` in a header** — it leaks into every TU that
includes it and can change overload resolution program-wide.

---

## C.6 Niebloids / customization point objects (C++20)

```cpp
std::ranges::swap(a, b);     // a function OBJECT, not a function
std::ranges::begin(r);
std::ranges::sort(v);
```

```
   These names are OBJECTS (not functions), which:
   • DISABLE ADL on the name itself (you can't accidentally call a wrong overload),
   • but internally perform the 'using std::swap; swap(...)' dance for you,
   • and are constrained by concepts.
   ⇒ You get the customization-point benefits of ADL without its hazards.
```

---

## C.7 Two-phase lookup in templates (recap)

```
   Phase 1 (template DEFINITION):  non-dependent names looked up & bound now.
   Phase 2 (template INSTANTIATION): dependent names looked up via ordinary lookup
                                     + ADL at the point of instantiation.
   ⇒ Names from a dependent base need this-> or qualification (Module 09 §9.18).
   ⇒ A function found ONLY by ADL at instantiation can be defined AFTER the template.
```

```cpp
template<class T> void call(T x) { helper(x); }   // 'helper' is dependent → phase 2
struct Foo {};
void helper(Foo);                                  // defined AFTER the template — still found
// call(Foo{}); → helper(Foo) found by ADL at instantiation. ✅
```

---

## C.8 Name hiding across classes (recap & detail)

```cpp
struct B { void f(int); void f(double); };
struct D : B { void f(const char*); };   // ⚠️ hides BOTH B::f overloads
D d;
// d.f(3);          // ❌ B::f(int) is hidden; only D::f(const char*) visible
d.f("x");           // ✅
struct D2 : B { using B::f; void f(const char*); };  // ✅ 'using' un-hides base overloads
D2{}.f(3);          // ✅ now B::f(int) is visible
```

```
   A name in a derived scope HIDES all same-named entities in bases (regardless of
   signature). Re-expose base overloads with 'using Base::name;'.
   Same hiding happens with namespaces, blocks, and template params.
```

---

## C.9 Lookup quick reference

```
   x            ordinary unqualified lookup (+ ADL if it's a function call)
   ::x          global namespace only
   ns::x        namespace ns (qualified — no ADL)
   obj.x / p->x member lookup in the (dynamic? no, STATIC) class + bases
   T::x         qualified; 'typename T::x' if x is a dependent type
   this->x      forces dependent lookup (phase 2) for base members in templates
```

➡ Related: [Appendix B — Overload Resolution](B1_overload_resolution_reference.md), [Module 09 §9.18](09_templates_generic_programming.md), [Module 07 §7.6](07_inheritance_polymorphism.md)
