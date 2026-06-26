# Appendix B — Overload Resolution & Conversions: The Complete Reference

> When you call `f(args)` and several `f`s exist, the compiler runs a precise,
> three-stage algorithm to pick one. Understanding it explains "ambiguous call"
> errors, why a template lost to a non-template, implicit conversions, and the
> C++20 rewritten/reversed comparison candidates. `[Primer §6.4, §6.6, §14.9.2;
> standard [over.match]]`

---

## B.1 The three stages

```
   f(args)
     │
     ▼
   1. NAME LOOKUP        gather every 'f' visible by ordinary lookup + ADL (Appendix C).
     │                   → the "candidate functions".
     ▼
   2. VIABILITY filter   keep candidates that (a) have the right arity (after defaults &
     │                   ellipsis) AND (b) have an implicit conversion for EACH argument.
     │                   → the "viable functions".
     ▼
   3. BEST MATCH ranking compare viable functions by the conversion cost of each argument.
                         one must be ≥ all others in every arg and > at least one,
                         else the call is AMBIGUOUS.
```

---

## B.2 The conversion-sequence ranking (the core table)

Each argument→parameter conversion is graded. **Better rank = lower cost.**

```
   RANK 0  EXACT MATCH
     • identity (T → T)
     • lvalue-to-rvalue, array-to-pointer, function-to-pointer ("lvalue transformations")
     • qualification adjustment (T* → const T*)        [these are all "exact"]
   RANK 1  PROMOTION
     • integral promotion: char/short/bool/enum/bitfield → int (or unsigned int)
     • floating-point promotion: float → double
   RANK 2  CONVERSION
     • integral conversions (int → long, long → short, signed↔unsigned)
     • floating conversions (double → float), floating-integral (int ↔ double)
     • pointer conversions (Derived* → Base*, T* → void*, nullptr → T*)
     • boolean conversions (anything → bool)
     • pointer-to-member conversions
   ── (a STANDARD conversion sequence = at most one of each, in the order above) ──
   RANK 3  USER-DEFINED CONVERSION  (exactly ONE: a converting ctor or operator T())
            [std-seq] → [user conversion] → [std-seq]
   RANK 4  ELLIPSIS  (...)   — worst
```

```
   Better standard conversion sequences (within the same "rank bucket"):
   EXACT  >  PROMOTION  >  CONVERSION
   And a sub-rule: if two conversions are otherwise equal, the one that converts
   LESS (e.g. Derived*→Base* beats Derived*→void*; more-derived target wins) is better.
```

```cpp
void f(int);      void f(double);
f('a');           // 'a'→int is PROMOTION(1); 'a'→double is CONVERSION(2) ⇒ f(int)
f(3.14f);         // float→double is PROMOTION(1) ⇒ f(double)
f(3.14);          // exact ⇒ f(double)
```

---

## B.3 Tie-breakers (when conversion ranks are equal)

Applied in order until one candidate wins:

```
   1. A non-template function BEATS a template specialization (equal conversions).
   2. A MORE SPECIALIZED template beats a less specialized one (partial ordering).
   3. (C++20) A MORE CONSTRAINED candidate beats a less constrained one (concepts).
   4. For conversions to references: binding to a less cv-qualified ref is better.
   5. Non-ellipsis beats ellipsis; non-user-conversion beats user-conversion.
   If none breaks the tie → the call is AMBIGUOUS (hard error).
```

```cpp
template<class T> void g(T);     // template
void g(int);                     // non-template
g(5);     // both exact-match ⇒ NON-TEMPLATE wins (rule 1)

template<class T> void h(T);     // (a)
template<class T> void h(T*);    // (b) more specialized
int* p; h(p);                    // (b) wins (rule 2)
```

---

## B.4 Classic ambiguities ⚠️

```cpp
void f(long);  void f(unsigned);
f(42);          // ❌ ambiguous: int→long and int→unsigned are BOTH rank-2 conversions

void g(int);   void g(char*);
g(NULL);        // ⚠️ NULL is 0 ⇒ may pick g(int); g(nullptr) picks g(char*)

void h(double); void h(float);
h(1);           // ❌ ambiguous (int→double and int→float both conversions, equal rank)

struct A{ A(int); };  struct B{ B(int); };
void k(A); void k(B);
k(5);           // ❌ ambiguous: two equally-good USER-defined conversions
```

🧠 To resolve deliberately: cast the argument (`f(42L)`), add an exact-match
overload, or `=delete` the unwanted one.

---

## B.5 Implicit conversions catalog (what stage 2 can use)

```
   STANDARD CONVERSIONS (compiler built-in, no user code):
   ┌─────────────────────────────────────────────────────────────────────────┐
   │ lvalue→rvalue, array→pointer, function→pointer                          │
   │ integral promotions / conversions ; floating promotions / conversions   │
   │ floating↔integral ; pointer conversions ; pointer-to-member conversions │
   │ boolean conversions (T → bool) ; qualification (T* → const T*)          │
   │ nullptr_t → any pointer / pointer-to-member                             │
   └─────────────────────────────────────────────────────────────────────────┘

   USER-DEFINED CONVERSIONS (at most ONE in a sequence):
   • a non-explicit converting CONSTRUCTOR:  struct S{ S(int); };  → int converts to S
   • a CONVERSION OPERATOR:                  struct S{ operator int() const; }; → S→int
   'explicit' removes a constructor/operator from implicit-conversion consideration.
```

⚠️ Only **one** user-defined conversion is allowed in a chain. `const char*` →
`std::string` → `MyType` won't happen implicitly (two user conversions).

---

## B.6 Reference binding & overload resolution

```cpp
void f(int&);          void f(const int&);    void f(int&&);
int x = 1; const int cx = 2;
f(x);     // f(int&)        — lvalue prefers non-const lvalue ref
f(cx);    // f(const int&)  — const lvalue
f(5);     // f(int&&)       — rvalue prefers rvalue ref over const lvalue ref
```

```
   For an RVALUE argument:   T&&  >  const T&        (move overload preferred)
   For an LVALUE argument:   T&   >  const T&        (and T&& not viable)
   For a const lvalue:       only const T& (and by-value) viable.
   This ranking is exactly what makes std::move route to the move constructor.
```

---

## B.7 C++20 rewritten & reversed candidates (comparisons)

For `==`, `!=`, `<`, `<=`, `>`, `>=`, `<=>`, the compiler builds extra candidates:

```
   For  a @ b  where @ is a relational/equality operator, candidates include:
   • the operator as written:                    operator@(a, b)
   • REWRITTEN from <=>:    a < b → (a <=> b) < 0 ;  a != b → !(a == b)
   • REVERSED operands:     if a == b not found, try b == a ; 0 < (b <=> a) ; etc.
   ⇒ Defining operator<=> and operator== (even as members) yields all six
     comparisons in BOTH argument orders, including heterogeneous (V vs int).
```

```cpp
struct V {
    int v;
    friend auto operator<=>(const V& a, int b){ return a.v <=> b; }
    friend bool operator==(const V& a, int b){ return a.v == b; }
};
V x{5};
x < 10;     // operator<=> rewrite
10 < x;     // REVERSED candidate: 0 < (x <=> 10)  ✅ works without writing it
10 == x;    // reversed ==
```

⚠️ Pre-C++20 you had to write `operator==(int, V)` and `operator==(V, int)`
separately. The reversed-candidate rule removed that boilerplate — but it can
occasionally introduce **new ambiguities** when upgrading old code that defined
both directions.

---

## B.8 Special situations

```
   • DELETED functions ARE candidates: if a deleted overload is the best match,
     the program is ILL-FORMED (that's the point of '= delete').
   • DEFAULT ARGUMENTS affect viability (arity) but not ranking.
   • Member vs free: a member f participates as if it had an implicit object param
     of type "ref to the class"; the object argument is ranked like any other.
   • const/ref-qualified members: the object's cv/value-category selects among
     overloaded members (Module 05 §5.8).
   • Conversion to/from the SAME class: the copy/move ctor competes with conversions.
```

```cpp
void p(int);
void p(char) = delete;
p('a');    // ❌ best match is the DELETED p(char) → hard error (intentional ban)
```

---

## B.9 How to debug an overload error

```
   1. "ambiguous call": two candidates tie. → cast the arg to force a rank, add an
      exact overload, or delete the unwanted one.
   2. "no matching function": no candidate is viable. → check arg types/const,
      missing #include, ADL not finding it (Appendix C), or a failed constraint.
   3. "called deleted function": the best match is '= delete'. → you hit a banned
      overload (e.g. passing the wrong type on purpose-blocked API).
   4. Use 'static_cast' to a parameter type to see which overload you intended.
   5. Compiler note lines list every candidate + WHY each was rejected — read them.
```

➡ Related: [Module 02 §2.6](02_control_flow_and_functions.md), [Appendix C — Name Lookup & ADL](C1_name_lookup_adl_reference.md), [Appendix D — Type Deduction](D1_type_deduction_reference.md)
