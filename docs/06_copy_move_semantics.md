# Module 06 — Copy, Move & the Rules of 0 / 3 / 5 (OOP Part 2)

> Move semantics (C++11) is arguably the most important addition to the language.
> To understand it you must first understand **value categories**. This module is
> the heart of modern, efficient C++. `[Primer §13]`

---

## 6.1 Value categories: lvalue, prvalue, xvalue

Every expression has a **type** and a **value category**.

```
                        expression
                       ┌───────────┐
                       │  glvalue  │           rvalue
                  ┌────┴────┐                ┌────┴────┐
               lvalue     xvalue ────────────┘      prvalue
               (has        (expiring: about to       (pure: a temporary,
                identity,    be moved-from)            no identity)
                a name)
```

| Category | Has identity? | Can be moved from? | Examples |
|----------|---------------|--------------------|----------|
| **lvalue** | yes | no | `x`, `arr[i]`, `*p`, `++x`, a named variable |
| **prvalue** | no | yes | `42`, `a+b`, `f()` returning by value, `Widget{}` |
| **xvalue** | yes | yes | `std::move(x)`, `f()` returning `T&&` |

```cpp
int x = 5;
int& lr = x;          // x is an lvalue
int&& rr = 10;        // 10 is a prvalue ⇒ binds to rvalue ref
int&& mr = std::move(x);  // std::move(x) is an xvalue
```

🧠 Mnemonic: **lvalue** = "locator value", something with a stable address/name.
**rvalue** = "right-hand value", a temporary you may steal from. `std::move`
doesn't move anything — it's a **cast to rvalue reference** that *enables* a move.

---

## 6.2 Why move matters — the copy that wasn't needed

```cpp
std::vector<std::string> v;
std::string s = "a very long string................";
v.push_back(s);             // COPIES s (s is an lvalue, still needed)
v.push_back(std::move(s));  // MOVES from s (we promise we're done with s)
v.push_back(makeString());  // MOVES from the temporary automatically
```

```
   COPY                              MOVE
   ┌──────┐   allocate + copy        ┌──────┐   steal the pointer
   │ src  │──── all N bytes ───▶ dst │ src  │───┐  (O(1), no allocation)
   └──────┘                          └──────┘   ▼
   src unchanged                     src left EMPTY but VALID (e.g. ptr=nullptr)
```

A move **transfers ownership** of the internals (pointer, buffer, handle) instead
of duplicating them. For a `std::string`/`vector` that's O(1) vs O(n).

---

## 6.3 Writing copy and move members — the Rule of 5

Recall `IntBuffer` from Module 05. Here it is, done correctly:

```cpp
class IntBuffer {
    int*        data_ = nullptr;
    std::size_t size_ = 0;
public:
    IntBuffer() = default;
    explicit IntBuffer(std::size_t n) : data_(new int[n]{}), size_(n) {}

    // 1. destructor
    ~IntBuffer() { delete[] data_; }

    // 2. copy constructor — DEEP copy
    IntBuffer(const IntBuffer& o) : data_(new int[o.size_]), size_(o.size_) {
        std::copy(o.data_, o.data_ + size_, data_);
    }

    // 3. copy assignment — copy-and-swap (strong exception safety)
    IntBuffer& operator=(const IntBuffer& o) {
        IntBuffer tmp(o);     // copy
        swap(tmp);            // swap our guts with the copy
        return *this;         // tmp (our old guts) is destroyed here
    }

    // 4. move constructor — STEAL, leave source empty-but-valid; noexcept!
    IntBuffer(IntBuffer&& o) noexcept
        : data_(o.data_), size_(o.size_) {
        o.data_ = nullptr;    // ← critical: source must be safely destructible
        o.size_ = 0;
    }

    // 5. move assignment
    IntBuffer& operator=(IntBuffer&& o) noexcept {
        IntBuffer tmp(std::move(o));
        swap(tmp);
        return *this;
    }

    void swap(IntBuffer& o) noexcept {
        std::swap(data_, o.data_);
        std::swap(size_, o.size_);
    }
};
```

```
   THE RULE OF FIVE
   If you write ANY of:  destructor, copy ctor, copy assign,
                         move ctor, move assign
   you probably need ALL FIVE (resource ownership is involved).
```

⚠️ **Self-assignment** (`x = x`) and **self-move** must not corrupt the object.
The copy-and-swap idiom above handles self-assignment automatically (it copies
first). A naive `delete[] data_; data_ = new...; copy...;` breaks on self-assign.

---

## 6.4 The Rule of Zero (what you should aim for) 🧠

```
   THE RULE OF ZERO
   Design classes that need NONE of the five. Let members (std::string,
   std::vector, std::unique_ptr) manage resources. The compiler-generated
   copy/move/destroy then "just work" by composing the members'.
```

```cpp
class Document {                 // Rule of Zero — no SMFs needed!
    std::string            title_;
    std::vector<std::string> lines_;
    std::unique_ptr<Index> index_;   // moves correctly, copy is auto-deleted
    // compiler generates correct dtor, move ctor/assign; copy is deleted because
    // unique_ptr is move-only. Nothing to write. Nothing to leak.
};
```

This is the *preferred* modern style. Reserve the Rule of 5 for the rare
low-level class that genuinely owns a raw resource (and even then, prefer wrapping
it in a tiny RAII handle).

---

## 6.5 When the compiler generates / suppresses SMFs

```
   You declare...          compiler still gives you...
   ─────────────────────────────────────────────────────────────────
   nothing                 all 5 (+ default ctor)
   a destructor            copy ops (DEPRECATED), but NO move ops ⚠️
   a copy ctor/assign      NO move ops; other copy op deprecated-generated
   a move ctor/assign      copy ops are DELETED
   = default for one       requests that specific one
   = delete for one        forbids it (and may affect overload resolution)
```

🧠 **The deadliest silent pitfall**: add a destructor (e.g. for logging) to a
class that holds a `vector`, and the compiler **stops generating move operations**.
Every "move" silently becomes a **copy** → massive performance regression with no
error. Cure: follow Rule of 0, or explicitly `=default` the moves.

```cpp
class Slow {
    std::vector<int> data_;
public:
    ~Slow() { log("destroyed"); }   // ⚠️ this killed move semantics!
    // Fix: also add
    Slow(Slow&&) = default;
    Slow& operator=(Slow&&) = default;
    Slow(const Slow&) = default;
    Slow& operator=(const Slow&) = default;
};
```

---

## 6.6 `noexcept` on move operations — not optional 🧠

```cpp
IntBuffer(IntBuffer&&) noexcept;   // ← the noexcept matters A LOT
```

`std::vector` reallocation uses moves **only if the move constructor is
`noexcept`**; otherwise it falls back to *copies* to preserve the strong exception
guarantee.

```
   vector grows; must relocate elements:
   move ctor is noexcept  →  MOVE each element  (fast, O(1) per element)
   move ctor may throw    →  COPY each element  (slow, O(n) per element) ⚠️
```

`std::move_if_noexcept` encodes this rule. **Always mark your move ops `noexcept`.**

---

## 6.7 Forwarding references & `std::forward` (preview of Module 09/12)

```cpp
template<class T>
void wrapper(T&& arg) {                 // T&& here is a FORWARDING (universal) reference
    callee(std::forward<T>(arg));        // preserves lvalue/rvalue-ness of arg
}
```

```
   T&& in a deduced template context is a FORWARDING reference (not an rvalue ref):
     wrapper(x);         T deduced as int& , arg type int&   → forwards as lvalue
     wrapper(make());    T deduced as int  , arg type int&&  → forwards as rvalue
   std::forward<T> casts back to the ORIGINAL category. std::move ALWAYS casts to rvalue.
```

🧠 Use `std::move` when you have a concrete rvalue ref / are done with a named
object; use `std::forward<T>` **only** in templates with `T&&` forwarding refs.

---

## 6.8 Copy elision, RVO & NRVO

```cpp
IntBuffer make() {
    return IntBuffer(100);   // prvalue — NO copy, NO move: constructed directly in caller
}
IntBuffer b = make();        // guaranteed elision (C++17): zero copies/moves
```

```
   RVO (Return Value Optimization): a returned temporary is constructed
   DIRECTLY in the caller's storage — the copy/move is ELIDED.
   • C++17 makes elision MANDATORY for returning prvalues.
   • NRVO (named return) is allowed but NOT guaranteed:
        IntBuffer f(){ IntBuffer x(100); return x; }  // usually elided, not required
```

⚠️ Don't write `return std::move(x);` to "help" — it **disables** NRVO and forces
a move. Just `return x;`. (`std::move` on return is only needed for returning a
*member* or by some non-prvalue paths.)

---

## 6.9 The empty-but-valid invariant 🧠

After a move, the source object must be in a **valid but unspecified** state: you
may destroy it or assign to it, but you shouldn't assume its value.

```cpp
std::string s = "hello";
std::string t = std::move(s);
// s is valid: you may do s.clear(), s = "new", s.size() (some value).
// You may NOT assume s == "hello" or s.empty(). Don't read its value blindly.
```

---

## 6.10 Checklist

- [ ] You can classify any expression as lvalue / prvalue / xvalue.
- [ ] You know `std::move` is a cast, not a move; `std::forward` preserves category.
- [ ] You default to the Rule of 0; you implement Rule of 5 with copy-and-swap when needed.
- [ ] You always mark move operations `noexcept` and know why vector needs it.
- [ ] You don't `return std::move(local)`; you rely on (N)RVO.
- [ ] You know that declaring a destructor silently disables move generation.

➡ Next: [Module 07 — Inheritance & Polymorphism](07_inheritance_polymorphism.md)
