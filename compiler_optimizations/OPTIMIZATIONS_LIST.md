# Compiler Optimizations Complete List

## ✅ All 8 Compiler Optimization Techniques

### 1. **RVO/NRVO** - Return Value Optimization
- **File**: `01_rvo_nrvo.cpp`
- **What**: Eliminates copies when returning objects
- **Impact**: 3-5x performance improvement
- **C++17**: Mandatory for prvalue returns
- **Example**: `return Object();` → no copy!

### 2. **Copy Elision**
- **File**: `02_copy_elision.cpp`
- **What**: Eliminates unnecessary copy/move operations
- **Impact**: Zero-cost value semantics
- **Types**: Direct init, return value, exception handling
- **Example**: `T t = createT();` → no copy!

### 3. **Small String Optimization (SSO)**
- **File**: `03_small_string_optimization.cpp`
- **What**: Stores small strings inline (no heap)
- **Threshold**: 15-23 bytes (implementation-dependent)
- **Impact**: 2-10x faster for small strings
- **Example**: `string s = "Hi";` → no allocation!

### 4. **Empty Base Optimization (EBO)**
- **File**: `04_empty_base_optimization.cpp`
- **What**: Empty base classes occupy zero bytes
- **Use Case**: Policy-based design
- **Impact**: Memory savings in class hierarchies
- **Example**: Policy classes in templates

### 5. **Function Inlining**
- **File**: `05_function_inlining.cpp`
- **What**: Replaces function call with function body
- **Impact**: 2-10x faster for tiny functions
- **Candidates**: Getters, setters, simple arithmetic
- **Example**: Small functions automatically inlined

### 6. **Loop Optimizations**
- **File**: `06_loop_optimizations.cpp`
- **Techniques**:
  - Loop unrolling
  - SIMD vectorization
  - Loop invariant code motion
  - Loop fusion
  - Strength reduction
- **Impact**: 2-10x speedup
- **Example**: SIMD for array operations

### 7. **Devirtualization**
- **File**: `07_devirtualization.cpp`
- **What**: Replaces virtual calls with direct calls
- **Impact**: 2-5x faster than virtual calls
- **When**: Type known at compile time, final keyword
- **Example**: Calling final methods directly

### 8. **Constant Folding & Compile-Time Evaluation**
- **File**: `08_constant_folding.cpp`
- **What**: Evaluates constant expressions at compile time
- **Impact**: Zero runtime cost
- **Techniques**:
  - Constant folding
  - Constant propagation
  - Dead code elimination
  - constexpr functions
- **Example**: `constexpr int x = 2 + 3;` → `5` at compile time

## 📊 Performance Impact Summary

| Optimization | Typical Speedup | Memory Impact |
|--------------|-----------------|---------------|
| RVO/NRVO | 3-5x | Large reduction |
| Copy Elision | 3-5x | Large reduction |
| SSO | 2-10x | None |
| EBO | N/A | 1-16 bytes saved |
| Inlining | 2-10x | Code size increase |
| Loop Opts | 2-10x | None/decrease |
| Devirtualization | 2-5x | None |
| Constant Folding | ∞ (zero cost) | Code size decrease |

## 🎯 When to Use Each

### Always Beneficial
- RVO/NRVO - automatic
- Copy Elision - automatic
- SSO - automatic (standard library)
- Constant Folding - use constexpr liberally

### Use When Appropriate
- EBO - policy-based design
- Inlining - small functions (automatic)
- Loop Opts - enable with -O2/-O3
- Devirtualization - use `final` keyword

## 🔧 Compiler Flags

```bash
# Enable all optimizations
g++ -std=c++17 -O2 -march=native file.cpp

# Aggressive optimization
g++ -std=c++17 -O3 -march=native -flto file.cpp

# Show optimization info
g++ -O2 -fopt-info-vec -fopt-info-inline file.cpp

# Disable specific optimizations (testing)
g++ -O2 -fno-elide-constructors  # Disable copy elision
g++ -O2 -fno-inline              # Disable inlining
```

## 📈 Measuring Performance

### Before Optimization
```cpp
// Profile first!
g++ -pg program.cpp
./a.out
gprof a.out gprof.out
```

### After Optimization
```cpp
// Compare assembly
g++ -S -O0 file.cpp -o no_opt.s
g++ -S -O2 file.cpp -o opt.s
diff no_opt.s opt.s
```

### Use Compiler Explorer
- Visit: https://godbolt.org/
- Compare different optimization levels
- See what transformations compiler applies

## ✅ Best Practices

1. **Trust the Compiler**
   - Write clear, simple code
   - Enable -O2 or -O3
   - Let compiler do its job

2. **Use Modern C++**
   - C++17: Guaranteed copy elision
   - constexpr for compile-time
   - final keyword for devirtualization

3. **Profile Before Optimizing**
   - Measure actual performance
   - Find real bottlenecks
   - Don't guess!

4. **Enable Optimizations**
   - Always use -O2 in release builds
   - Consider -O3 for performance-critical code
   - Use -march=native for SIMD

5. **Write Optimizer-Friendly Code**
   - Simple, clear logic
   - Const correctness
   - No unnecessary complexity
   - Cache-friendly data structures

## 🎓 Learning Resources

### Documentation
- Each .cpp file has extensive comments
- Real-world examples included
- Performance comparisons provided

### External Resources
- Compiler Explorer: https://godbolt.org/
- C++ Reference: https://cppreference.com/
- Optimization guides for GCC, Clang, MSVC

### Books
- "Optimized C++" - Kurt Guntheroth
- "Effective Modern C++" - Scott Meyers
- "C++ Move Semantics" - Nicolai Josuttis

## 🚀 Quick Reference

```cpp
// RVO/NRVO
Object create() { return Object(); }  // No copy

// Copy Elision
Object o = create();  // No copy in C++17

// SSO
string s = "small";  // No allocation

// EBO
template <typename T, typename Policy>
class MyClass : private Policy {  // Zero size

// Inlining
inline int add(int a, int b) { return a + b; }

// constexpr
constexpr int factorial(int n) {
    return n <= 1 ? 1 : n * factorial(n - 1);
}

// Devirtualization
class Derived final : public Base {
    int method() override final { return 42; }
};

// Loop optimization
for (int i = 0; i < n; ++i) {
    c[i] = a[i] + b[i];  // Vectorized by compiler
}
```

---

**Total Optimizations**: 8  
**Total Examples**: ~2,000 lines  
**Status**: ✅ All working and tested  
**Performance Impact**: 2-10x typical improvements
