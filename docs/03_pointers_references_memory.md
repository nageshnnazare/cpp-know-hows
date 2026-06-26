# Module 03 — Pointers, References & the Memory Model

> Pointers and references are where C++ rewards understanding and punishes
> hand-waving. Master the *memory model* (where objects live, how long, and what
> a pointer really is) and 80% of "weird" C++ behavior becomes obvious.
> `[Primer §2.3, §12.1, §12.2]`

---

## 3.1 What a variable really is

A variable is a **named region of storage** with a type. The type says how many
bytes and how to interpret them.

```
   int x = 65;
   ┌──────── address 0x7ffd...c4 ─────────┐
   │  01000001 00000000 00000000 00000000 │  ← 4 bytes (little-endian shown)
   └──────────────────────────────────────┘
        name: x    value: 65    &x == 0x7ffd...c4
```

- `&x` → the *address* (a pointer to x).
- `*p` → the *object* a pointer p points to (dereference).

---

## 3.2 Pointers

```cpp
int  x = 10;
int* p = &x;     // p holds the address of x
*p = 20;         // write THROUGH p ⇒ x is now 20
int** pp = &p;   // pointer to pointer
```

```
   p ──────────▶ x
   ┌───────┐      ┌──────┐
   │0x...c4│      │  20  │
   └───────┘      └──────┘
   p stores       x stores
   x's address    the value
```

Pointer arithmetic moves in **units of the pointed-to type**:

```cpp
int a[4] = {10,20,30,40};
int* q = a;          // a decays to &a[0]
q + 1;               // address increases by sizeof(int) (4 bytes), points to a[1]
*(q + 2);            // == a[2] == 30      (and a[i] is literally *(a+i))
q[2];                // identical to *(q+2)  ⇒ 2[a] == a[2] (!)
```

```
  a:  ┌────┬────┬────┬────┐
      │ 10 │ 20 │ 30 │ 40 │
      └────┴────┴────┴────┘
       ▲    ▲              ▲
       q    q+1            q+4 == "one past the end" (legal to FORM, not to deref)
```

🧠 You may form a pointer **one past the end** of an array (used by `end()`), but
dereferencing it is UB. Pointer subtraction yields `std::ptrdiff_t`. Comparing
pointers into *different* arrays is UB (except `==`/`!=`).

### `nullptr` vs `NULL` vs `0`

```cpp
int* p = nullptr;     // ✅ typed null (std::nullptr_t) — use this
int* q = NULL;        // legacy macro, usually 0 — overload-resolution hazard
int* r = 0;           // works but obscures intent
```

⚠️ `void f(int); void f(char*);` — calling `f(NULL)` may pick `f(int)` because
`NULL` is `0`. `f(nullptr)` correctly picks the pointer overload.

### `void*` and `const` placement

```cpp
void* v = p;                 // can point at anything; can't deref without a cast
const int* pc;     // pointer to const int   : can't change *pc, can change pc
int* const cp = &x;// const pointer to int   : can change *cp, can't change cp
const int* const x;// const pointer to const int

// Read RIGHT to LEFT:  cp is a const pointer to int.
```

```
   const int*  p;   →  *p is read-only,  p can be repointed
   int* const  p;   →  *p is writable,   p is fixed
```

---

## 3.3 References — aliases, not objects

A reference is **another name** for an existing object. It is not an object, has
no address of its own (conceptually), and **must be initialized** and **can't be
rebound**.

```cpp
int x = 10;
int& r = x;      // r IS x (an alias)
r = 20;          // x is now 20
int& bad;        // ❌ must initialize
// references cannot be "reseated" to refer to a different object afterwards
```

```
   POINTER                         REFERENCE
   ┌──────┐    ┌──────┐            r ───is─── x  (no separate storage in the model)
   │  p   │───▶│  x   │            ┌──────┐
   └──────┘    └──────┘            │  x   │  (r and x are the same box)
   p is its own object             └──────┘
   (can be null, reseated)         (always bound, never null, never reseated)
```

| | Pointer | Reference |
|---|---------|-----------|
| Can be null? | yes | no (a null reference is UB to create) |
| Must init? | no | **yes** |
| Reseatable? | yes | **no** |
| Own address/storage? | yes | no (optimized away usually) |
| Arithmetic? | yes | no |
| Use for | optional/owning/iteration | aliasing, parameters, return |

🧠 References are usually implemented as pointers under the hood, but the language
treats them as aliases. Prefer references for parameters/returns where "must
exist, never rebinds" is the right contract; use pointers (or `optional`,
`unique_ptr`) for "maybe absent" / "owning" / "reseatable".

### lvalue ref `&` vs rvalue ref `&&`

```cpp
int  x = 1;
int& lr = x;            // lvalue ref — binds to lvalues (named objects)
int&& rr = 5;           // rvalue ref — binds to rvalues (temporaries)
const int& cr = 5;      // const lvalue ref CAN bind a temporary (lifetime extended!)
```

🧠 Binding a temporary to a `const&` (or `&&`) **extends its lifetime** to the
reference's lifetime — a crucial idiom. (More in Module 06.)

---

## 3.4 Storage durations & where objects live

```
   MEMORY LAYOUT OF A TYPICAL PROCESS
   high ┌─────────────────────────┐
        │        STACK            │  automatic storage: locals, params.
        │   (grows downward ↓)    │  LIFO; freed automatically at scope exit.
        ├─────────────────────────┤
        │           │             │
        │           ▼             │
        │      (unused gap)       │
        │           ▲             │
        │           │             │
        ├─────────────────────────┤
        │         HEAP            │  dynamic storage: new/malloc.
        │    (grows upward ↑)     │  YOU manage lifetime (or smart pointers do).
        ├─────────────────────────┤
        │  BSS (zero-init globals)│  static storage duration
        │  DATA (init globals)    │  lives for whole program
        ├─────────────────────────┤
        │  TEXT / RODATA          │  code + string literals + const data
   low  └─────────────────────────┘
```

| Storage duration | Created | Destroyed | Examples |
|------------------|---------|-----------|----------|
| **automatic** | entering scope | leaving scope | locals, params |
| **static** | before first use | program end | globals, `static` locals |
| **thread** | thread start | thread end | `thread_local` |
| **dynamic** | `new` | `delete` | heap objects |

---

## 3.5 `new` / `delete` and the heap

```cpp
int*  p  = new int(42);        // allocate + construct one int
delete p;                      // destruct + free

int*  a  = new int[100]{};     // array new (zero-initialized)
delete[] a;                    // MUST use delete[] for array new ⚠️

Widget* w = new Widget(args);  // calls operator new, then Widget ctor
delete w;                      // calls ~Widget, then operator delete
```

```
   new T(args)                       delete p
   ┌───────────────────────────┐     ┌───────────────────────────┐
   │ 1. operator new(sizeof T) │     │ 1. p->~T()  (run dtor)    │
   │    → raw memory           │     │ 2. operator delete(p)     │
   │ 2. construct T in place   │     │    → free memory          │
   │ 3. return T*              │     └───────────────────────────┘
   └───────────────────────────┘
```

⚠️ The deadly mismatches (each is UB):
- `new[]` paired with `delete` (not `delete[]`).
- `new` paired with `free`, or `malloc` paired with `delete`.
- Double `delete`.
- Using a pointer after `delete` (dangling / use-after-free).

🧠 **In modern C++ you almost never write raw `new`/`delete`.** Use
`std::make_unique`, `std::make_shared`, and containers (Modules 10–11). Raw
owning pointers are a code smell. `new`/`delete` knowledge is for understanding
the machinery and for writing those abstractions.

### Placement new & `operator new` overloading (expert)

```cpp
#include <new>
alignas(Widget) std::byte buf[sizeof(Widget)];
Widget* w = new (buf) Widget(args);   // construct INTO existing storage (no allocation)
w->~Widget();                          // must call dtor manually; no delete!
```

This underlies `std::vector`'s separation of *allocation* from *construction* and
`std::optional`/`std::variant`'s in-place storage.

---

## 3.6 Object lifetime, alignment & `std::launder`

Every object has a precise lifetime: it begins when storage is obtained **and**
the constructor completes; it ends when the destructor starts (or storage is
reused/freed). Accessing an object outside its lifetime is UB.

```
   alignment: an object of type T must live at an address divisible by alignof(T)
   alignof(char)==1   alignof(int)==4   alignof(double)==8   alignas(64) for cache lines

   struct S { char c; int i; };   // padding inserted so 'i' is 4-aligned:
   ┌────┬───┬───┬───┬─────────────┐
   │ c  │pad│pad│pad│    i (4B)   │   sizeof(S)==8, not 5
   └────┴───┴───┴───┴─────────────┘
```

🧠 Reorder members largest-first to minimize padding. `alignas`/`alignof`,
`std::aligned_storage` (deprecated in C++23), and `std::launder` are tools for
when you manage raw storage yourself.

---

## 3.7 Dangling, leaks, and the bug taxonomy

```
   BUG                  WHAT HAPPENED                          DETECTOR
   ───────────────────────────────────────────────────────────────────────
   dangling pointer     points to freed/expired object         ASan, UBSan
   use-after-free       deref a freed pointer                   ASan
   double free          delete the same pointer twice           ASan
   memory leak          new without delete; lost last pointer   LeakSanitizer/valgrind
   buffer overflow      index/pointer past array bounds         ASan
   uninitialized read   read a variable before writing it       MSan, valgrind
   null deref           *nullptr                                UBSan, crash
```

```cpp
int* leak = new int[1000];      // ⚠️ never deleted ⇒ leak
int* p = new int(1); delete p;
*p = 5;                          // ⚠️ use-after-free
delete p;                        // ⚠️ double free
```

🧠 Compile your tests with `-fsanitize=address,undefined`. ASan instruments every
allocation and access; it will pinpoint these with stack traces. This habit alone
catches most memory bugs before they ship.

---

## 3.8 `const`-correctness through pointers/refs (preview)

```cpp
void read(const std::string& s);   // promises NOT to modify s; can take temporaries
void write(std::string& s);        // may modify; needs an lvalue
const std::string& cref = make();  // lifetime-extends the temporary

const int  ci = 1;
const int& r  = ci;     // fine
int& bad      = ci;     // ❌ can't bind non-const ref to const object
```

Deep dive in Module 04.

---

## 3.9 Checklist

- [ ] You can draw the box-and-arrow diagram for any pointer/reference snippet.
- [ ] You read `const` placement right-to-left and know top-level vs low-level const.
- [ ] You match `new`/`delete` and `new[]`/`delete[]`, but prefer smart pointers.
- [ ] You can name every entry in the bug taxonomy and the sanitizer that finds it.
- [ ] You understand storage durations and never return refs/pointers to locals.

➡ Next: [Module 04 — const/constexpr, Arrays, Strings, Enums, Unions](04_const_arrays_strings_enums.md)
