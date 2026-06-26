# Module 13 — Exceptions & Error Handling

> Exceptions separate the "happy path" from error handling and guarantee cleanup
> via stack unwinding + RAII. Used well they are powerful; used poorly they hide
> control flow and leak. This module covers the mechanism, the **exception-safety
> guarantees**, and the modern alternatives. `[Primer §5.6, §18.1]`

---

## 13.1 The mechanism: throw → unwind → catch

```cpp
double divide(int a, int b) {
    if (b == 0) throw std::invalid_argument("divide by zero");
    return double(a) / b;
}
try {
    auto r = divide(10, 0);
} catch (const std::invalid_argument& e) {   // catch BY const REFERENCE
    std::cerr << e.what() << '\n';
} catch (const std::exception& e) {           // base class catches derived
    std::cerr << "other: " << e.what() << '\n';
} catch (...) {                               // catch-all (last resort)
    std::cerr << "unknown\n";
}
```

```
   throw X;
     │  1. construct the exception object X
     ▼
   STACK UNWINDING: walk OUT of each enclosing scope toward a matching handler,
     │              running DESTRUCTORS of all fully-constructed locals on the way.
     ▼            (This is why RAII works under exceptions.)
   catch (matching type)  ── handler runs; exception object destroyed at handler end
     │
     ▼  (no handler anywhere?) → std::terminate() → abort
```

🧠 Handlers are matched **top to bottom by type** (no "best match" like
overloading). Order from **most-derived to least-derived**; a `catch(Base&)`
placed before `catch(Derived&)` shadows the latter (compilers warn).

---

## 13.2 The standard exception hierarchy

```
   std::exception                       (what() → const char*)
   ├── std::bad_alloc                    (new failed)
   ├── std::bad_cast                     (failed dynamic_cast<ref>)
   ├── std::logic_error                  (preventable bugs — precondition violations)
   │   ├── std::invalid_argument
   │   ├── std::out_of_range             (vector::at, map::at)
   │   ├── std::length_error
   │   └── std::domain_error
   └── std::runtime_error                (only detectable at run time)
       ├── std::range_error
       ├── std::overflow_error
       └── std::system_error             (errno-based, has .code())
```

```cpp
class ConfigError : public std::runtime_error {   // derive your own
public:
    explicit ConfigError(const std::string& m) : std::runtime_error(m) {}
};
```

🧠 **Throw by value, catch by (const) reference.** Throwing a pointer leaks/ownership-
ambiguous; catching by value *slices* (Module 07). Derive your exceptions from
`std::exception` so generic `catch(const std::exception&)` works.

---

## 13.3 The three exception-safety guarantees 🧠

This is the vocabulary experts use to reason about correctness under exceptions.

```
   ┌──────────────────────────────────────────────────────────────────────────┐
   │ NO-THROW (failure transparency): operation never throws. (swap, dtors,   │
   │   move ops should aim for this; mark noexcept.)                          │
   │                                                                          │
   │ STRONG (commit-or-rollback): if it throws, program state is UNCHANGED    │
   │   (as if the operation never started). e.g. vector::push_back, copy-swap.│
   │                                                                          │
   │ BASIC (no-leak / invariant-preserving): if it throws, no resources leak  │
   │   and all objects are in some VALID (if unspecified) state.              │
   │                                                                          │
   │ NONE: any throw may corrupt/leak. ← avoid; not acceptable in good code.  │
   └──────────────────────────────────────────────────────────────────────────┘
```

The **copy-and-swap** idiom (Module 06) gives the *strong* guarantee for
assignment: build a complete copy first (may throw, but the original is untouched),
then `swap` (no-throw) to commit.

```cpp
T& operator=(T o) {        // 'o' is a copy (may throw here — original safe)
    swap(*this, o);        // no-throw commit
    return *this;          // old state destroyed via o's destructor
}
```

---

## 13.4 `noexcept` — promise and query

```cpp
void f() noexcept;                 // promises not to throw
void g() noexcept(false);          // may throw (the default for normal functions)
template<class T>
void h() noexcept(std::is_nothrow_move_constructible_v<T>);  // conditional
constexpr bool b = noexcept(f());  // QUERY: is this expression non-throwing?
```

```
   If a noexcept function DOES throw:
   throw ─▶ std::terminate()  immediately (NO unwinding past the noexcept boundary)
```

🧠 Why `noexcept` matters beyond documentation:
- **`std::vector` and friends choose move vs copy based on `noexcept` move ctors**
  (Module 06). This is the highest-impact use.
- Enables optimizations (no unwinding tables/cleanup code on that path).
- Destructors, `swap`, and move operations are implicitly `noexcept`-friendly;
  make them `noexcept` explicitly.

⚠️ Don't slap `noexcept` on functions that *can* throw (e.g. ones that allocate)
unless you truly handle/absorb it — a stray throw becomes an instant crash.

---

## 13.5 Exceptions during construction & destruction

```cpp
class Resource {
    A* a_; B* b_;
public:
    Resource() : a_(new A), b_(new B) {}   // ⚠️ if `new B` throws, a_ leaks
                                            //    (Resource's dtor does NOT run —
                                            //     the object was never fully constructed)
    ~Resource() { delete b_; delete a_; }
};
// FIX: members manage themselves (Rule of 0) — each fully-constructed member's
//      destructor runs during unwinding even if a later member throws:
class ResourceOk {
    std::unique_ptr<A> a_ = std::make_unique<A>();
    std::unique_ptr<B> b_ = std::make_unique<B>();  // if this throws, a_ is cleaned up
};
```

```
   KEY RULE: if a constructor throws, the object is NOT constructed, so its
   DESTRUCTOR will NOT run — but destructors of already-constructed MEMBERS and
   BASES DO run. ⇒ Let members own resources (smart pointers) for safety.
```

⚠️ **Destructors must not throw.** If a destructor throws during stack unwinding
(while another exception is in flight), `std::terminate` is called. Destructors
are implicitly `noexcept` since C++11.

`function-try-block` (rarely needed) catches exceptions from member init lists:

```cpp
Resource::Resource() try : a_(new A), b_(new B) { }
catch (...) { /* members already destroyed; rethrows automatically */ }
```

---

## 13.6 `throw;`, rethrow, and `std::exception_ptr`

```cpp
try { risky(); }
catch (const std::exception& e) {
    log(e.what());
    throw;                 // RETHROW the SAME exception (preserves dynamic type)
    // throw e;            // ⚠️ would COPY & SLICE to std::exception — don't
}

std::exception_ptr ep;
try { risky(); } catch (...) { ep = std::current_exception(); }  // capture & store
if (ep) std::rethrow_exception(ep);   // rethrow later / on another thread
```

🧠 `exception_ptr` lets you transport an exception across threads/`std::future`
(Module 15) — how `std::async` propagates exceptions to the caller.

---

## 13.7 When NOT to use exceptions — modern alternatives

Exceptions are for **exceptional, rare** errors. For expected/recoverable
outcomes, value-based error handling is often clearer and faster:

```cpp
// std::optional<T> : "a value, or nothing" (no error detail)
std::optional<int> parseInt(std::string_view s);
if (auto v = parseInt(s)) use(*v); else handleMissing();

// std::expected<T,E> (C++23) : "a value, or an error with detail"
std::expected<Config, ParseError> loadConfig(std::string_view path);
auto r = loadConfig("a.cfg");
if (r) use(*r); else report(r.error());

// error codes (std::error_code) : for system/library boundaries, no throw
std::error_code ec;
auto sz = std::filesystem::file_size(p, ec);   // no-throw overload
if (ec) { ... }
```

```
   CHOOSE...
   exceptions      : truly exceptional, rare; deep call stacks; constructors
   std::optional   : "absent" is normal and needs no explanation
   std::expected   : recoverable failure WITH error detail (C++23)
   error_code      : crossing C/OS boundaries; hot paths; no-throw guarantee
   assert/contracts: programming bugs (precondition violations) → fail fast
```

🧠 Costs: exceptions are "zero-cost" on the **happy path** (modern table-based
unwinding adds no per-call overhead), but **throwing is expensive** (unwinding,
RTTI lookup) — never use them for ordinary control flow or in tight loops.
Some domains (embedded, games) compile with `-fno-exceptions`.

---

## 13.8 Best practices

```
   ✅ Throw by value, catch by const reference.
   ✅ Derive exceptions from std::exception; give a useful what().
   ✅ Use RAII so unwinding cleans up automatically — no manual try/finally.
   ✅ Mark move ops, swap, and dtors noexcept.
   ✅ Aim for the strong guarantee on mutating ops (copy-and-swap).
   ✅ Catch only what you can handle; let the rest propagate.
   ⚠️ Don't throw from destructors. Don't use exceptions for normal flow.
   ⚠️ Don't catch(...) and swallow silently. Don't throw raw pointers.
```

---

## 13.9 Checklist

- [ ] You can describe stack unwinding and why RAII makes it safe.
- [ ] You know the standard hierarchy and throw-by-value/catch-by-ref.
- [ ] You can state and apply the no-throw / strong / basic guarantees.
- [ ] You mark moves/swap/dtors `noexcept` and know vector depends on it.
- [ ] You handle constructor-throw with member ownership (Rule of 0).
- [ ] You choose optional/expected/error_code vs exceptions appropriately.

➡ Next: [Module 14 — C++20: Concepts, Ranges, Coroutines, Modules, `<=>`](14_cpp20_features.md)
