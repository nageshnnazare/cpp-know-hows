# Module 02 — Control Flow & Functions

> Control flow in C++ is mostly familiar, but the details (switch fallthrough,
> the new `if`/`switch` initializers, range-for desugaring) and the **function
> model** (overload resolution, default arguments, the call stack) are where
> expertise shows. `[Primer §5, §6]`

---

## 2.1 Branching

```cpp
if (cond) { ... } else if (cond2) { ... } else { ... }

// C++17: init-statement scoped to the if/else
if (auto it = m.find(key); it != m.end()) {
    use(it->second);          // it is visible in both branches...
} else {
    insert(key);              // ...and only here, then destroyed.
}
```

```
  if-with-initializer scope
  ┌─────────────────────────────────────────────────┐
  │ if (init; condition) {  then-block }            │
  │         └──────── 'init' & condition vars live  │
  │                   through then AND else, then   │
  │                   go out of scope. Keeps names  │
  │                   tight; avoids leaking 'it'.   │
  └─────────────────────────────────────────────────┘
```

`switch` — integral/enum only, **falls through by default**:

```cpp
switch (c) {
  case 'a':
  case 'b':  doAB();   [[fallthrough]];   // explicit: I MEANT to fall through
  case 'c':  doC();    break;
  default:   doDefault();
}
```

⚠️ Forgetting `break` is the classic bug; `[[fallthrough]]` (C++17) documents
intent and silences `-Wimplicit-fallthrough`. You cannot jump *over* a variable
initialization into its scope — declare such variables inside a `{ }` case block.

🧠 Compilers turn dense `switch`es into **jump tables** (O(1)) and sparse ones into
binary search / if-chains. This is why `switch` can beat `if/else` chains.

---

## 2.2 Loops

```cpp
for (int i = 0; i < n; ++i) { ... }     // classic
while (cond) { ... }
do { ... } while (cond);                // body runs at least once

for (int x : v) { ... }                 // range-based for (C++11)
for (int& x : v) x *= 2;                // by reference to mutate
for (const auto& x : v) read(x);        // PREFERRED for read-only (no copies)
```

Range-`for` **desugars** to this (know it — it explains dangling-range bugs):

```cpp
// for (decl : range) body;   becomes (C++11..C++17):
{
    auto&& __r = range;                 // ⚠️ binds a reference to 'range'
    auto __b = begin(__r);
    auto __e = end(__r);
    for (; __b != __e; ++__b) {
        decl = *__b;
        body;
    }
}
```

⚠️ Dangling range bug:

```cpp
for (char c : getString().substr(1)) { ... }  // temporary OK (lifetime extended)
for (auto& kv : getMap()["key"]) { ... }       // ⚠️ if getMap() returns by value,
                                                //    the temp map dies — UB in older rules
```

🧠 **C++23 fixed** range-`for` temporary lifetime extension for *nested*
temporaries; in C++20 and earlier the inner temporary in `range.member()` is
**not** extended → dangling. Bind to a named variable first.

---

## 2.3 Jump statements & structured cleanup

```cpp
break;        // exit nearest loop/switch
continue;     // next iteration
return v;     // exit function (triggers destructors of locals — see Module 13)
goto label;   // exists; almost never use it. Cannot jump over initializations.
```

🧠 C++ has no `finally`. Cleanup is done with **RAII** (destructors run on every
exit path: normal return, `break`, exception). This is the central C++ resource
idiom — see Modules 05 & 11.

---

## 2.4 The anatomy of a function

```cpp
[[nodiscard]] constexpr inline               // specifiers / attributes
int                                          // return type
add(int a, int b = 0)                        // name, params (b has default)
noexcept                                     // exception spec
{ return a + b; }                            // body
```

```
   add(int a, int b = 0) noexcept
   │   │   │   │      │      └── promises not to throw (Module 13)
   │   │   │   │      └──────── default argument
   │   │   │   └─────────────── parameter list (by value here ⇒ copies)
   │   │   └─────────────────── trailing names
   │   └─────────────────────── return type
   └─────────────────────────── attributes: [[nodiscard]] warns if result ignored
```

### Trailing return type & `auto` return

```cpp
auto multiply(int a, int b) -> int { return a*b; }   // trailing return type
auto deduce(int a, int b)        { return a*b; }      // C++14: deduced return type
template<class T, class U>
auto add(T t, U u) -> decltype(t+u) { return t+u; }   // depend on params
```

---

## 2.5 Parameter passing — the decision table

```
   PASS BY...        signature      copies?   can mutate caller's?   use when
   ───────────────────────────────────────────────────────────────────────────
   value             f(T x)         yes       no                     cheap types (int,ptr),
                                                                      or you need your own copy
   const reference   f(const T& x)  no        no                     read-only, T is "big"
   non-const ref     f(T& x)        no        YES (out-param)        in/out parameters
   rvalue reference  f(T&& x)       no        moves-from             sink/forwarding (Mod 06,12)
   pointer           f(T* x)        no        optional/nullable      C interop, "maybe absent"
```

🧠 Rules of thumb experts use:
- Pass **by value** for `int`, `double`, pointers, and *cheaply movable* sinks.
- Pass **`const T&`** for everything large you only read.
- "**Want a copy? Take by value and `std::move`**" (the *sink* idiom):
  ```cpp
  void setName(std::string n) { name_ = std::move(n); } // one copy OR one move, never two
  ```
- Use **out-params sparingly**; prefer returning by value (RVO makes it free).

---

## 2.6 Overloading & overload resolution

Functions can share a name if their **parameter lists** differ (return type alone
is **not** enough).

```cpp
void print(int);
void print(double);
void print(const char*);
void print(int, int);
// void print(int);  // ❌ redeclaration
// long print(int);  // ❌ differs only by return type
```

Overload resolution picks the *best viable* function:

```
  CANDIDATE → VIABLE (right arity & convertible args) → BEST MATCH
  Ranking of conversions (best → worst):
    1. exact match (incl. T→const T, array→ptr, T&→T)
    2. promotion (char→int, float→double)
    3. standard conversion (int→double, Derived*→Base*, →bool)
    4. user-defined conversion (one ctor / operator)
    5. ellipsis (...)
  If two candidates tie ⇒ AMBIGUOUS (compile error).
```

```cpp
void f(int);  void f(double);
f('a');   // 'a'→int is a PROMOTION (rank 2), 'a'→double is a CONVERSION (rank 3)
          // ⇒ calls f(int)
f(3.14f); // float→double promotion ⇒ f(double)
```

⚠️ Ambiguity example:

```cpp
void g(long);  void g(unsigned);
g(42);    // ❌ ambiguous: int→long and int→unsigned are both "conversions" of equal rank
```

🧠 Overloading interacts with templates (a non-template exact match beats a
template), with implicit conversions, and with `=delete` (you can *ban* an
overload):

```cpp
void process(int);
void process(char) = delete;   // calling process('x') is now a compile error
```

---

## 2.7 Default arguments — rules & traps

```cpp
void f(int a, int b = 10, int c = 20);   // defaults must be trailing
f(1);        // f(1,10,20)
f(1, 2);     // f(1,2,20)
```

⚠️ Traps:
- Defaults are evaluated **at the call site**, in the caller's scope.
- A default value is **fixed by the declaration the caller sees** — putting
  different defaults in a header vs a cpp, or in base vs derived, is a bug
  source:
  ```cpp
  struct B { virtual void f(int x = 1); };
  struct D : B { void f(int x = 2) override; };
  B* p = new D;
  p->f();   // ⚠️ calls D::f but with B's default x==1 (defaults are STATIC, vcall is dynamic)
  ```
- Don't use a mutable global as a default; it's evaluated fresh each call but is
  still a surprise.

---

## 2.8 `inline`, `constexpr`, and `consteval` functions

```cpp
inline int sq(int x){ return x*x; }       // ODR-mergeable; "may" be inlined (a hint at best)
constexpr int cube(int x){ return x*x*x; }// usable at compile time IF args are constant
consteval int must(int x){ return x+1; }  // C++20: MUST run at compile time
```

```
   constexpr fn   ── can run at compile time OR run time (compiler's choice/needs)
   consteval fn   ── compile time ONLY (immediate function); calling at runtime = error
   inline kw      ── about linkage/ODR, NOT really about inlining anymore
```

```cpp
constexpr int n = cube(3);     // computed at compile time ⇒ n is a constant 27
int r = cube(runtimeVal());    // same function, computed at run time — fine
constexpr int bad = must(runtimeVal()); // ❌ consteval needs a constant
```

🧠 Modern compilers inline based on cost models and `-O` level regardless of the
`inline` keyword. `inline`'s real job today is **allowing a definition in a
header** (ODR-3, Module 00).

---

## 2.9 The call stack & recursion

```
  high addr ┌──────────────────┐
            │ caller's frame   │
            ├──────────────────┤  ← each call pushes a "stack frame":
            │ return address   │     • return address
            │ saved registers  │     • saved frame/base pointer
            │ parameters       │     • function parameters
            │ local variables  │     • local (automatic) variables
            ├──────────────────┤  ← stack pointer (grows DOWNWARD on x86/ARM)
  low addr  │   (free)         │
            └──────────────────┘
```

```cpp
long fact(int n){ return n <= 1 ? 1 : n * fact(n-1); } // each call = one frame
```

⚠️ Deep/infinite recursion overflows the stack (typically 1–8 MB) → crash, not an
exception. **Tail-call optimization is allowed but not guaranteed** in C++ —
never rely on it.

🧠 Locals live in their frame and are destroyed (destructors run) when the
function returns. **Returning a pointer/reference to a local is UB** (dangling):

```cpp
int& bad(){ int x = 5; return x; }   // ⚠️ x dies when bad() returns
```

---

## 2.10 `main`, program startup, and exit

```cpp
int main() { ... }                     // returns 0 implicitly if you don't
int main(int argc, char** argv) { ... }// command-line args
```

```
  PROGRAM LIFECYCLE
  ┌───────────────────────────────────────────────────────────────────┐
  │ 1. dynamic init of namespace-scope objects (unspecified order     │
  │    ACROSS translation units — the "static init order fiasco" ⚠️)  │
  │ 2. main() runs                                                    │
  │ 3. return / exit() ⇒ destructors of static objects run in REVERSE │
  │    order of construction; atexit handlers run                     │
  │ 4. std::abort()/std::terminate() ⇒ NO cleanup, immediate stop     │
  └───────────────────────────────────────────────────────────────────┘
```

🧠 *Static initialization order fiasco*: globals in different TUs have no defined
construction order relative to each other. Fix with the **Meyers singleton**
(function-local static, initialized on first use, thread-safe since C++11):

```cpp
Config& config() { static Config c; return c; }  // constructed on first call, once
```

---

## 2.11 Checklist

- [ ] You use `if (init; cond)` / `switch (init; …)` to scope helper variables.
- [ ] You know range-`for` desugaring and the temporary-lifetime trap.
- [ ] You can rank overload candidates and explain an ambiguity error.
- [ ] You pass parameters by the right mechanism and use the sink idiom.
- [ ] You never return references/pointers to locals; you use RAII for cleanup.

➡ Next: [Module 03 — Pointers, References & the Memory Model](03_pointers_references_memory.md)
