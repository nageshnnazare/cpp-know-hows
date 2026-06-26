# Runnable Examples

Each file is self-contained and maps to a module in `../docs/`. They print a trace
so you can *see* the concept (e.g. when copies vs moves happen).

| File | Module | Demonstrates |
|------|--------|--------------|
| `01_types_and_conversions.cpp` | 01 | integer promotion, signed/unsigned wrap, narrowing, `sizeof` non-evaluation |
| `06_move_semantics.cpp` | 06 | Rule of 5, copy vs move trace, copy-and-swap, vector reallocation moves |
| `07_polymorphism.cpp` | 07 | virtual dispatch, abstract base, virtual destructor via `unique_ptr` |
| `09_templates.cpp` | 09 | variadic + fold, `if constexpr`, NTTP class template, CRTP |
| `10_stl_algorithms.cpp` | 10 | sort/accumulate/count_if, erase-remove, `erase_if`, map iteration |
| `11_smart_pointers.cpp` | 11 | `unique_ptr` transfer, `shared_ptr` refcount, `weak_ptr` cycle break |
| `12_lambdas.cpp` | 12 | by-value vs by-ref capture, `mutable`, generic lambda, `std::function` |
| `14_cpp20.cpp` | 14 | concepts, ranges/views pipeline, defaulted `operator<=>` |
| `15_concurrency.cpp` | 15 | mutex vs atomic counter, `std::async`, condition-variable producer/consumer |

## Build & run

```bash
./build.sh                 # build everything into ./bin/
./build.sh 06              # build & run the example starting with "06"
./build.sh --asan 11       # build with AddressSanitizer + UBSan, then run
```

Requires a C++20 compiler (Clang 14+/GCC 11+). `14_cpp20.cpp` needs ranges support.
