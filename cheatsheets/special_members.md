# Cheatsheet — Special Member Functions (SMFs) & Rules 0/3/5

## The six special members
```cpp
struct T {
    T();                            // default constructor
    ~T();                           // destructor
    T(const T&);                    // copy constructor
    T& operator=(const T&);         // copy assignment
    T(T&&) noexcept;                // move constructor   (C++11)
    T& operator=(T&&) noexcept;     // move assignment    (C++11)
};
```

## What the compiler generates (the "rule of generation")
| You declare... | default ctor | dtor | copy ctor | copy = | move ctor | move = |
|---|:---:|:---:|:---:|:---:|:---:|:---:|
| nothing | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| any constructor | ❌ | ✅ | ✅ | ✅ | ✅ | ✅ |
| destructor | ✅ | — | ⚠️dep | ⚠️dep | **❌** | **❌** |
| copy ctor or copy = | ✅ | ✅ | — | — | **❌** | **❌** |
| move ctor or move = | ✅ | ✅ | **❌(=del)** | **❌(=del)** | — | — |

`⚠️dep` = generated but **deprecated**. **❌** = not provided (silently).

🧠 **Biggest trap:** declaring a destructor (e.g. for logging) silently **suppresses
the move operations** → every "move" becomes a copy. Fix with `= default` or Rule of 0.

## The rules
```
   RULE OF ZERO  : design so you need NONE of the six. Let members
                   (string/vector/unique_ptr) manage resources. ← PREFER THIS.
   RULE OF THREE : (pre-C++11) if you need dtor/copy-ctor/copy-assign, you need all 3.
   RULE OF FIVE  : if you manage a raw resource, define all five
                   (dtor + copy ctor/assign + move ctor/assign).
```

## Explicit control
```cpp
T() = default;                 // ask for the implicit one
T(const T&) = delete;          // forbid copying (move-only type)
T(T&&) = default;              // re-enable a move that a dtor suppressed
```

## Always do
- Mark move ctor / move assign / `swap` / destructor **`noexcept`**.
- Implement copy assignment via **copy-and-swap** for the strong exception guarantee.
- Leave moved-from objects **valid but unspecified**.
