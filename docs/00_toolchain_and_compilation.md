# Module 00 — The Toolchain & Compilation Model

> Before a single line of C++ makes sense, you must understand *how* a `.cpp`
> file becomes a running program. Most mysterious C++ errors (linker errors,
> ODR violations, "multiple definition", header guard problems) are explained
> entirely by this model. `[Primer §2.6.3, §6.1.2, §19.7]`

---

## 0.1 The four phases: preprocess → compile → assemble → link

C++ uses a **separate compilation** model inherited from C. Each `.cpp` file is
compiled *independently* into an object file. The linker stitches them together.

```
 source files                                            executable
 ┌──────────┐   preprocess   ┌──────────────┐  compile   ┌─────────┐  assemble ┌──────────┐
 │ main.cpp │ ─────────────▶ │ main.ii      │ ─────────▶ │ main.s  │ ────────▶ │ main.o   │┐
 └──────────┘  (cpp/-E)      │(translation  │ (front+    │ (asm)   │ (as)      │ (object) ││
 ┌──────────┐                │  unit, TU)   │  backend)  └─────────┘           └──────────┘│  link
 │ math.cpp │ ─────────────▶ │ math.ii      │ ─────────▶  ...      ────────▶   math.o      │├────────▶ ./a.out
 └──────────┘                └──────────────┘                                  libfoo.a    │  (ld)
 ┌──────────┐                                                                  libc++.so   │
 │ *.h      │  (textually pasted in during preprocess; headers are NOT compiled separately)│
 └──────────┘                                                                              ┘
```

| Phase | Tool (clang) | Input → Output | What happens |
|-------|--------------|----------------|--------------|
| Preprocess | `clang -E` | `.cpp` → `.ii` | `#include` pasted, macros expanded, `#if` resolved, comments stripped |
| Compile | `clang -S` | `.ii` → `.s` | parse, semantic analysis, template instantiation, optimization, emit asm |
| Assemble | `clang -c` | `.s` → `.o` | assembly → machine code in an object file (ELF/Mach-O/COFF) |
| Link | `ld`/`clang` | `.o` + libs → exe | resolve symbols across TUs, lay out sections, relocate |

Try it yourself:

```bash
clang++ -std=c++20 -E main.cpp -o main.ii   # stop after preprocess
clang++ -std=c++20 -S main.cpp -o main.s    # stop after compile (read the asm!)
clang++ -std=c++20 -c main.cpp -o main.o    # stop after assemble
clang++ -std=c++20 main.o math.o -o app     # link
```

🧠 **A header is never "compiled".** It is *textually pasted* into every `.cpp`
that includes it. If two `.cpp` files include the same header, the header's text
is compiled twice — once in each translation unit. This single fact explains
header guards, `inline`, and most ODR rules below.

---

## 0.2 The Translation Unit (TU)

A **translation unit** is one `.cpp` file *plus everything it `#include`s*, after
preprocessing. It is the atomic unit the compiler sees.

```
  main.cpp                          Translation Unit "main"
  ┌──────────────────┐               ┌─────────────────────────────────┐
  │ #include <vector>│  preprocess   │ ...50,000 lines of <vector>...  │
  │ #include "foo.h" │ ───────────▶  │ ...contents of foo.h...         │
  │ int main(){...}  │               │ int main(){...}                 │
  └──────────────────┘               └─────────────────────────────────┘
                                     compiled as ONE indivisible chunk
```

Two TUs know *nothing* about each other except through **declarations** and the
**linker**. This is why you split code into:

- **Declarations** in headers (`.h`/`.hpp`) — "this thing exists, here's its type".
- **Definitions** in sources (`.cpp`) — "here's the actual body/storage".

---

## 0.3 Declaration vs Definition (the distinction experts live by)

```cpp
// ---- foo.h (declarations) ----
extern int g_counter;     // DECLARATION: promises g_counter exists somewhere
int add(int a, int b);    // DECLARATION: promises add() exists somewhere
struct Point;             // DECLARATION (incomplete type / forward decl)

// ---- foo.cpp (definitions) ----
int g_counter = 0;            // DEFINITION: allocates storage
int add(int a, int b){        // DEFINITION: provides the body
    return a + b;
}
struct Point { int x, y; };   // DEFINITION: full type
```

- You may **declare** a name as many times as you like.
- You must **define** it according to the **One Definition Rule**.

`[Primer §2.2.2]`

---

## 0.4 The One Definition Rule (ODR) — the law that governs everything

The ODR has three parts. Memorize all three; violating it is **undefined
behavior with no diagnostic required** (the nastiest kind of bug).

```
┌─────────────────────────────────────────────────────────────────────────┐
│ ODR-1: In ONE translation unit, a variable/function/class/template/     │
│        enum may be DEFINED at most once.                                │
│                                                                         │
│ ODR-2: In the WHOLE program, every non-inline function or variable that │
│        is ODR-used must be DEFINED exactly once (across all TUs).       │
│                                                                         │
│ ODR-3: Some entities (classes, inline functions, templates, inline      │
│        variables) MAY be defined in multiple TUs, BUT every definition  │
│        must be TOKEN-FOR-TOKEN IDENTICAL and mean the same thing.       │
└─────────────────────────────────────────────────────────────────────────┘
```

🧠 ODR-3 is why class definitions and template definitions go in headers: they
are *allowed* to repeat in every TU, as long as the text is identical. It is also
why a subtle difference (e.g. compiling two TUs with different `-DNDEBUG`,
different struct layouts, or different `#pragma pack`) is a silent ODR violation
that "works on my machine" and crashes in production.

### Why you get "multiple definition" linker errors

```cpp
// util.h
int square(int x){ return x*x; }   // ⚠️ NON-inline definition in a header
```

If `a.cpp` and `b.cpp` both include `util.h`, the linker sees **two** definitions
of `square` (one per object file) → `duplicate symbol`. Fixes:

```cpp
inline int square(int x){ return x*x; }   // ✅ inline ⇒ ODR-3 lets it repeat
// OR put the definition in exactly one .cpp and only declare it in the header.
```

---

## 0.5 Linkage: internal, external, none

**Linkage** decides whether a name in one TU can refer to the same entity as a
name in another TU.

```
                      ┌──────────────── linkage ──────────────────┐
  name at namespace   │  external : visible to the linker, can    │
  scope               │             be referenced from other TUs  │
                      │  internal : private to its own TU         │
                      └───────────────────────────────────────────┘
```

| Construct | Linkage |
|-----------|---------|
| non-`const` global variable | external |
| free function | external |
| `static` global / function | **internal** (TU-private) |
| name in an **anonymous namespace** | **internal** (preferred over `static`) |
| `const`/`constexpr` global (no `extern`) | **internal** by default (C++ quirk!) |
| local variable | none |
| `inline` variable/function | external but ODR-mergeable |

```cpp
// Preferred way to make TU-private helpers in modern C++:
namespace {                 // anonymous namespace
    int helper() { return 42; }   // internal linkage, no name clash across TUs
}
```

🧠 **`const` at namespace scope is implicitly `internal`** in C++ (unlike C).
That's why `const int N = 10;` in a header doesn't cause multiple-definition
errors — each TU gets its own private copy. To share one, use
`inline constexpr int N = 10;` (C++17) or `extern const int N;` + one definition.

---

## 0.6 Name mangling & `extern "C"`

C++ encodes types into linker symbol names ("name mangling") so overloaded
functions get distinct symbols.

```bash
$ nm -C app | grep add        # -C demangles
0000000100003f10 T int add(int, int)
$ nm    app | grep add        # raw mangled name (Itanium ABI)
0000000100003f10 T __Z3addii   #  _Z 3add ii  =>  add(int,int)
```

```
  source name        mangled (Itanium ABI)        meaning
  add(int,int)   →   _Z3addii                  3=len "add", i=int, i=int
  add(double)    →   _Z3addd                   different symbol ⇒ overloading works
```

To call C functions (no mangling), wrap declarations:

```cpp
extern "C" int c_style_function(int);   // linker symbol is exactly "c_style_function"
```

⚠️ Mixing two C++ standard-library/ABI versions, or linking objects built with
different compilers, can produce "undefined symbol" errors caused by *different
mangling/ABI*. This is the real-world manifestation of ABI incompatibility.

---

## 0.7 Header hygiene: guards, `#pragma once`, and what belongs where

```cpp
// point.h
#ifndef POINT_H          //  ┐ traditional include guard:
#define POINT_H          //  │ prevents the body being pasted twice
                         //  │ into ONE translation unit.
struct Point { int x, y; };
#endif // POINT_H        //  ┘

// — or, simpler, non-standard but universally supported:
#pragma once
```

What goes in a header (must obey ODR-3 → identical everywhere):

```
HEADER (.h/.hpp)                          SOURCE (.cpp)
├─ class/struct definitions               ├─ non-inline function bodies
├─ templates (decl + definition)          ├─ non-const global variable definitions
├─ inline functions/variables             ├─ static (TU-local) helpers
├─ constexpr functions/variables          └─ the one definition of each external symbol
├─ enum definitions
└─ declarations (extern var, fn protos)
```

---

## 0.8 Static vs dynamic libraries

```
  STATIC LIBRARY (.a / .lib)              DYNAMIC LIBRARY (.so / .dylib / .dll)
  ┌───────────────────────────┐           ┌───────────────────────────┐
  │ archive of .o files       │           │ shared image loaded at    │
  │ COPIED into the exe at    │           │ RUNTIME; exe stores only  │
  │ link time                 │           │ a reference + symbols     │
  │ + no runtime dependency   │           │ + smaller exe, shareable  │
  │ + faster startup          │           │ + update lib w/o relink   │
  │ - bigger exe              │           │ - needs lib present/found │
  │ - relink to update        │           │ - ABI must stay stable    │
  └───────────────────────────┘           └───────────────────────────┘
```

```bash
# static
clang++ -c math.cpp -o math.o && ar rcs libmath.a math.o
clang++ main.cpp -L. -lmath -o app

# dynamic
clang++ -dynamiclib math.cpp -o libmath.dylib    # (.so on Linux with -shared)
clang++ main.cpp -L. -lmath -o app               # loader finds libmath at runtime
```

---

## 0.9 Build systems (you will use CMake)

A trivial `CMakeLists.txt` you can actually grow into a real project:

```cmake
cmake_minimum_required(VERSION 3.20)
project(cpp_mastery CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)            # -std=c++20, not gnu++20

add_executable(app main.cpp math.cpp)
target_compile_options(app PRIVATE -Wall -Wextra -Wpedantic)
```

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build -j
```

---

## 0.10 Compiler flags every expert keeps in muscle memory

| Flag | Why |
|------|-----|
| `-std=c++20` | pick the language standard explicitly |
| `-Wall -Wextra -Wpedantic` | turn on the warnings that catch real bugs |
| `-Werror` | treat warnings as errors in CI |
| `-O0 -g` | no optimization, full debug info (debugging) |
| `-O2` / `-O3` | release optimization |
| `-fsanitize=address` | AddressSanitizer: use-after-free, overflow |
| `-fsanitize=undefined` | UBSan: catches UB at runtime |
| `-fsanitize=thread` | data-race detector |
| `-fno-exceptions -fno-rtti` | embedded/perf builds |
| `-march=native` | optimize for this CPU's instruction set |

🧠 **Run your tests under `-fsanitize=address,undefined` at least once.** It is
the single highest-value habit for catching the UB that this guide warns about in
Module 16.

---

## 0.11 Mental model summary

```
   You write:            The compiler thinks in:        The linker thinks in:
   ┌──────────┐          ┌───────────────────┐           ┌───────────────────┐
   │ files,   │          │ ONE translation   │           │ symbols with      │
   │ #includes│   ───▶   │ unit at a time;   │   ───▶    │ external linkage; │
   │ classes  │          │ knows nothing of  │           │ one definition    │
   │          │          │ other .cpp files  │           │ each (ODR)        │
   └──────────┘          └───────────────────┘           └───────────────────┘
```

➡ Next: [Module 01 — Types, Variables, Literals & Operators](01_types_variables_operators.md)
