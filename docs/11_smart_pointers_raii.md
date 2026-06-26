# Module 11 — Smart Pointers & RAII

> Smart pointers apply RAII (Module 05) to dynamic memory: they own a heap object
> and free it automatically. They encode **ownership** in the type system, which
> is the modern C++ answer to leaks, double-frees, and dangling pointers.
> `[Primer §12.1]`

---

## 11.1 The ownership question

Every pointer in a codebase should answer: **"Who owns this? Who frees it?"**

```
   OWNERSHIP MODEL          USE                         TOOL
   ───────────────────────────────────────────────────────────────────────
   unique ownership         one owner, transferable     std::unique_ptr<T>
   shared ownership         many owners, last frees      std::shared_ptr<T>
   non-owning observation   "I just look, don't free"    raw T* / T& / std::weak_ptr / span
```

🧠 Guideline: **Owning = smart pointer or container. Non-owning = raw pointer/
reference.** A raw pointer in modern C++ means "I observe but do not own." Never
`delete` a raw pointer you received — if you didn't `new` it, it's not yours.

---

## 11.2 `std::unique_ptr` — exclusive ownership, zero overhead

```cpp
#include <memory>
auto p = std::make_unique<Widget>(args);   // ← PREFER make_unique (no raw new)
p->method();
*p;                          // deref
Widget* raw = p.get();       // borrow (non-owning) — do NOT delete it
p.reset();                   // free now, p becomes null
Widget* released = p.release(); // give up ownership; YOU must delete now
// when p goes out of scope, ~unique_ptr deletes the Widget automatically
```

```
   unique_ptr<Widget>            sizeof == sizeof(Widget*)  (with default deleter)
   ┌──────────────┐             ZERO runtime overhead vs raw pointer.
   │ ptr ─────────┼──────▶ [ Widget on heap ]
   └──────────────┘             Move-only: cannot be COPIED (copy is =deleted),
                                only MOVED — which transfers ownership.
```

```cpp
std::unique_ptr<Widget> a = std::make_unique<Widget>();
// std::unique_ptr<Widget> b = a;            // ❌ copy deleted
std::unique_ptr<Widget> b = std::move(a);    // ✅ ownership transferred; a is now null
```

Use cases: factory return types, PIMPL, owning members, the body of a container of
polymorphic objects (`std::vector<std::unique_ptr<Base>>`).

Custom deleter (e.g. for C resources):

```cpp
auto closer = [](std::FILE* f){ if (f) std::fclose(f); };
std::unique_ptr<std::FILE, decltype(closer)> file{std::fopen("x","r"), closer};
// arrays:
auto arr = std::make_unique<int[]>(100);   // unique_ptr<int[]> — uses delete[]
```

---

## 11.3 `std::shared_ptr` — shared ownership via reference counting

```cpp
auto sp = std::make_shared<Widget>(args);   // ← PREFER make_shared
auto sp2 = sp;                               // COPY: refcount now 2
sp.use_count();                              // 2
// last shared_ptr destroyed ⇒ Widget deleted
```

```
   shared_ptr layout (the CONTROL BLOCK is the key):
   sp  ┌──────────┐        ┌──── control block ──────┐
       │ ptr ─────┼──────▶ [ Widget object           ]        
       │ ctrl ────┼──────▶ │ strong count : 2        │
       └──────────┘        │ weak count   : 0        │
   sp2 ┌──────────┐        │ deleter / allocator     │
       │ ptr ─────┼──────▶ └─────────────────────────┘
       │ ctrl ────┼──────▶ (same control block)
       └──────────┘
   • strong count hits 0 → object destroyed
   • weak count + strong both 0 → control block freed
```

🧠 **Why `make_shared`?** It allocates the object **and** the control block in a
**single** allocation (faster, one cache line, better locality). Plain
`shared_ptr<T>(new T)` does two allocations and is less exception-safe.

⚠️ Costs of `shared_ptr`:
- 2× pointer size (object ptr + control-block ptr).
- **Atomic** refcount increment/decrement (thread-safe count, but *not* the
  pointee) → cache-line contention if shared across threads.
- Don't pass `shared_ptr` by value everywhere — each copy is an atomic op. Pass
  `const shared_ptr&`, or better, pass `T&`/`T*` if the callee doesn't keep it.

---

## 11.4 The cycle problem & `std::weak_ptr` 🧠

```cpp
struct Node {
    std::shared_ptr<Node> next;   // ⚠️ if these form a cycle, refcount never hits 0 → LEAK
};
```

```
   CYCLE: A.next → B,  B.next → A
   A(count=1 from B) ⇄ B(count=1 from A)
   Drop external refs → each still has count 1 → NEITHER freed → LEAK.

   FIX: make ONE direction a weak_ptr (a non-owning observer that doesn't bump count)
   parent → child : shared_ptr (owns)
   child  → parent: weak_ptr  (observes)
```

```cpp
struct Tree {
    std::vector<std::shared_ptr<Tree>> children;
    std::weak_ptr<Tree> parent;        // ✅ breaks the cycle
};

std::weak_ptr<Widget> w = sp;          // does NOT increase strong count
if (auto locked = w.lock()) {          // promote to shared_ptr IF still alive
    locked->use();                     // safe: object guaranteed alive in this block
} else {
    // object already destroyed
}
w.expired();                            // true if the object is gone
```

`weak_ptr` is also the standard tool for **caches** and **observer** patterns
where you must not keep the object alive but must detect its death.

---

## 11.5 `enable_shared_from_this`

To hand out a `shared_ptr` to `this` from inside a member (without creating a
*second*, independent control block — which would double-free):

```cpp
class Session : public std::enable_shared_from_this<Session> {
public:
    void start() {
        auto self = shared_from_this();   // ✅ shares the EXISTING control block
        asyncOp([self]{ self->onDone(); });// keep alive during async op
    }
};
auto s = std::make_shared<Session>();     // ⚠️ object MUST already be owned by a shared_ptr
s->start();
```

⚠️ Calling `shared_from_this()` on an object **not** owned by a `shared_ptr` is UB
(throws `std::bad_weak_ptr` in practice).

---

## 11.6 Passing smart pointers — the parameter guidelines 🧠

```
   FUNCTION INTENT                         PARAMETER TYPE
   ──────────────────────────────────────────────────────────────────────
   "I just use the object" (no ownership)  Widget&  or  const Widget&   (or Widget*)
   "I take ownership (a sink)"             std::unique_ptr<Widget>      (by value)
   "I may share ownership / keep it"       std::shared_ptr<Widget>      (by value)
   "I observe shared ownership"            const std::shared_ptr<Widget>&  (avoid copy)
   "I conditionally extend lifetime"       std::weak_ptr<Widget>
```

⚠️ Anti-pattern: taking `const std::unique_ptr<T>&` or `const std::shared_ptr<T>&`
just to call methods. If you don't manage ownership, take `T&`/`T*` — it decouples
the callee from the ownership scheme.

---

## 11.7 RAII beyond memory

Smart pointers are one instance of RAII. The same pattern wraps every resource:

```cpp
{
    std::lock_guard<std::mutex> lk(m);   // acquires mutex; releases on scope exit
    std::ifstream file("data.txt");      // closes on scope exit
    std::unique_ptr<Conn> c = connect(); // disconnects on scope exit
    // ... use them ...
}   // released in REVERSE order, even if an exception is thrown
```

Write your own scope-guard for ad-hoc cleanup:

```cpp
template<class F>
struct ScopeGuard {
    F f; bool active = true;
    ~ScopeGuard(){ if (active) f(); }
    void dismiss(){ active = false; }
};
auto guard = ScopeGuard{[&]{ rollback(); }};   // runs rollback unless we dismiss()
commit();
guard.dismiss();   // success: don't roll back
```

(`std::experimental::scope_exit` / Boost.ScopeExit / `gsl::finally` formalize this.)

---

## 11.8 Decision diagram

```
   Do you need a heap object with managed lifetime?
     no  → use a value/stack object or a container (best)
     yes → is ownership shared among independent parties with no clear single owner?
             no  → std::unique_ptr   (default; cheap, clear)
             yes → std::shared_ptr   (+ std::weak_ptr to break cycles / observe)
   Need to OBSERVE without owning? → raw pointer / reference / span / weak_ptr
```

---

## 11.9 Common mistakes

```
   ⚠️ new/delete by hand                → use make_unique/make_shared
   ⚠️ shared_ptr where unique_ptr fits  → unnecessary atomic overhead
   ⚠️ two shared_ptr from one raw new   → two control blocks → double free
        shared_ptr<T> a(raw); shared_ptr<T> b(raw);   // ❌
   ⚠️ shared_ptr cycles                 → use weak_ptr for back-references
   ⚠️ passing shared_ptr by value in hot paths → atomic refcount churn
   ⚠️ get()/release() then forgetting who deletes
```

---

## 11.10 Checklist

- [ ] Every owning pointer is a smart pointer or container; raw = non-owning.
- [ ] You default to `unique_ptr`; you use `shared_ptr` only for true shared ownership.
- [ ] You always use `make_unique`/`make_shared`.
- [ ] You break shared_ptr cycles with `weak_ptr`.
- [ ] You pass `T&`/`T*` when a function only *uses* an object.
- [ ] You wrap every resource (locks, files, handles) in RAII.

➡ Next: [Module 12 — Lambdas & Functional C++](12_lambdas_functional.md)
