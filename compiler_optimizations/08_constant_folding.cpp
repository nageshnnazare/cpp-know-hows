/*
 * CONSTANT FOLDING & COMPILE-TIME OPTIMIZATIONS
 * 
 * DEFINITION:
 * Compiler evaluates constant expressions at compile time
 * rather than runtime, eliminating computation overhead.
 * 
 * TYPES:
 * 1. Constant Folding - Evaluate constant expressions
 * 2. Constant Propagation - Replace variables with constants
 * 3. Dead Code Elimination - Remove unreachable code
 * 4. constexpr - Guaranteed compile-time evaluation
 * 
 * BENEFITS:
 * + Zero runtime cost
 * + Smaller binaries
 * + Faster execution
 * + Better optimization opportunities
 */

#include <iostream>
#include <array>
#include <cmath>

using namespace std;

// ============= Example 1: Basic Constant Folding =============

void constantFoldingBasic() {
    cout << "\n=== Basic Constant Folding ===" << endl;
    
    // Computed at compile time
    int a = 2 + 3;           // Becomes: int a = 5;
    int b = 10 * 20;         // Becomes: int b = 200;
    int c = (5 + 3) * 2;     // Becomes: int c = 16;
    
    cout << "a = " << a << " (computed at compile time)" << endl;
    cout << "b = " << b << " (computed at compile time)" << endl;
    cout << "c = " << c << " (computed at compile time)" << endl;
}

// ============= Example 2: Constant Propagation =============

void constantPropagation() {
    cout << "\n=== Constant Propagation ===" << endl;
    
    const int x = 10;
    const int y = 20;
    
    // Compiler propagates constants and folds
    int result = x * y + x - y;  // Becomes: int result = 190;
    
    cout << "Result: " << result << " (all compile-time)" << endl;
}

// ============= Example 3: constexpr Functions =============

constexpr int factorial(int n) {
    return n <= 1 ? 1 : n * factorial(n - 1);
}

constexpr double power(double base, int exp) {
    double result = 1.0;
    for (int i = 0; i < exp; ++i) {
        result *= base;
    }
    return result;
}

constexpr int fibonacci(int n) {
    if (n <= 1) return n;
    int a = 0, b = 1;
    for (int i = 2; i <= n; ++i) {
        int temp = a + b;
        a = b;
        b = temp;
    }
    return b;
}

void constexprFunctions() {
    cout << "\n=== constexpr Functions ===" << endl;
    
    // Computed at compile time!
    constexpr int fact5 = factorial(5);
    constexpr double pow2_10 = power(2, 10);
    constexpr int fib10 = fibonacci(10);
    
    cout << "5! = " << fact5 << " (compile-time)" << endl;
    cout << "2^10 = " << pow2_10 << " (compile-time)" << endl;
    cout << "fib(10) = " << fib10 << " (compile-time)" << endl;
}

// ============= Example 4: constexpr Arrays =============

constexpr array<int, 10> generateSquares() {
    array<int, 10> result{};
    for (int i = 0; i < 10; ++i) {
        result[i] = i * i;
    }
    return result;
}

void constexprArrays() {
    cout << "\n=== constexpr Arrays ===" << endl;
    
    // Array computed at compile time!
    constexpr auto squares = generateSquares();
    
    cout << "Squares (computed at compile time): ";
    for (int sq : squares) {
        cout << sq << " ";
    }
    cout << endl;
}

// ============= Example 5: Dead Code Elimination =============

void deadCodeElimination() {
    cout << "\n=== Dead Code Elimination ===" << endl;
    
    const bool debug = false;
    
    if (debug) {
        // This entire block is eliminated at compile time!
        cout << "Debug info..." << endl;
        cout << "More debug..." << endl;
        // Expensive debug operations...
    }
    
    if constexpr (false) {  // C++17
        // Guaranteed to be eliminated
        cout << "Never compiled!" << endl;
    }
    
    cout << "Debug code removed by compiler" << endl;
}

// ============= Example 6: Template Metaprogramming =============

template <int N>
struct FactorialTemplate {
    static constexpr int value = N * FactorialTemplate<N - 1>::value;
};

template <>
struct FactorialTemplate<0> {
    static constexpr int value = 1;
};

void templateMetaprogramming() {
    cout << "\n=== Template Metaprogramming ===" << endl;
    
    // Computed at compile time via templates
    constexpr int fact = FactorialTemplate<5>::value;
    cout << "5! = " << fact << " (template metaprogramming)" << endl;
}

// ============= Example 7: if constexpr (C++17) =============

template <typename T>
void processValue(T value) {
    if constexpr (is_integral_v<T>) {
        // Only compiled for integral types
        cout << "Integer: " << value << endl;
    } else if constexpr (is_floating_point_v<T>) {
        // Only compiled for floating-point types
        cout << "Float: " << value << endl;
    } else {
        // Only compiled for other types
        cout << "Other type" << endl;
    }
}

void ifConstexpr() {
    cout << "\n=== if constexpr ===" << endl;
    
    processValue(42);     // Only integer branch compiled
    processValue(3.14);   // Only float branch compiled
    processValue("hi");   // Only other branch compiled
}

// ============= Example 8: Compile-Time String Processing =============

constexpr size_t constStrlen(const char* str) {
    size_t len = 0;
    while (str[len] != '\0') {
        ++len;
    }
    return len;
}

constexpr bool constStrEqual(const char* a, const char* b) {
    while (*a && *b) {
        if (*a != *b) return false;
        ++a;
        ++b;
    }
    return *a == *b;
}

void compileTimeStrings() {
    cout << "\n=== Compile-Time String Processing ===" << endl;
    
    constexpr size_t len = constStrlen("Hello, World!");
    constexpr bool equal = constStrEqual("test", "test");
    
    cout << "Length: " << len << " (compile-time)" << endl;
    cout << "Equal: " << (equal ? "yes" : "no") << " (compile-time)" << endl;
}

// ============= Example 9: Strength Reduction =============

void strengthReduction() {
    cout << "\n=== Strength Reduction ===" << endl;
    
    // Compiler replaces expensive operations with cheaper ones
    
    int x = 100;
    
    // x * 2 becomes x + x or x << 1
    int a = x * 2;
    
    // x / 2 becomes x >> 1 (for unsigned)
    unsigned int b = x / 2;
    
    // x * 8 becomes x << 3
    int c = x * 8;
    
    cout << "All optimized by compiler (multiplication → shifts)" << endl;
}

// ============= Example 10: Algebraic Simplification =============

void algebraicSimplification() {
    cout << "\n=== Algebraic Simplification ===" << endl;
    
    int x = 42;
    
    // x + 0 → x
    int a = x + 0;  // Becomes: int a = x;
    
    // x * 1 → x
    int b = x * 1;  // Becomes: int b = x;
    
    // x * 0 → 0
    int c = x * 0;  // Becomes: int c = 0;
    
    // x - x → 0
    int d = x - x;  // Becomes: int d = 0;
    
    cout << "Compiler simplifies algebraic expressions" << endl;
}

int main() {
    cout << "=== CONSTANT FOLDING & COMPILE-TIME OPTIMIZATIONS ===" << endl;
    
    constantFoldingBasic();
    constantPropagation();
    constexprFunctions();
    constexprArrays();
    deadCodeElimination();
    templateMetaprogramming();
    ifConstexpr();
    compileTimeStrings();
    strengthReduction();
    algebraicSimplification();
    
    cout << "\n\n=== KEY TAKEAWAYS ===" << endl;
    cout << "1. Compiler evaluates constants at compile time" << endl;
    cout << "2. constexpr guarantees compile-time evaluation" << endl;
    cout << "3. Zero runtime cost for compile-time computations" << endl;
    cout << "4. Dead code is completely eliminated" << endl;
    cout << "5. Enables smaller, faster binaries" << endl;
    
    cout << "\nOptimization Types:" << endl;
    cout << "• Constant Folding - Evaluate constant expressions" << endl;
    cout << "• Constant Propagation - Replace vars with constants" << endl;
    cout << "• Dead Code Elimination - Remove unreachable code" << endl;
    cout << "• Strength Reduction - Replace expensive ops" << endl;
    cout << "• Algebraic Simplification - Simplify math" << endl;
    
    cout << "\nconstexpr Benefits:" << endl;
    cout << "✓ Guaranteed compile-time evaluation" << endl;
    cout << "✓ No runtime cost" << endl;
    cout << "✓ Can use in constant expressions" << endl;
    cout << "✓ Can use in template arguments" << endl;
    cout << "✓ Can initialize arrays" << endl;
    
    cout << "\nCompile-Time Computation:" << endl;
    cout << "• constexpr functions" << endl;
    cout << "• constexpr variables" << endl;
    cout << "• Template metaprogramming" << endl;
    cout << "• if constexpr (C++17)" << endl;
    cout << "• consteval (C++20 - always compile-time)" << endl;
    
    cout << "\nWhen to Use constexpr:" << endl;
    cout << "✓ Mathematical constants" << endl;
    cout << "✓ Lookup tables" << endl;
    cout << "✓ Configuration values" << endl;
    cout << "✓ String hashing" << endl;
    cout << "✓ Unit conversions" << endl;
    
    cout << "\nCompiler Optimization Levels:" << endl;
    cout << "• -O0: No optimization (debugging)" << endl;
    cout << "• -O1: Basic optimizations" << endl;
    cout << "• -O2: Recommended (constant folding enabled)" << endl;
    cout << "• -O3: Aggressive (may increase code size)" << endl;
    cout << "• -Os: Optimize for size" << endl;
    
    cout << "\nVerification:" << endl;
    cout << "• Check assembly: g++ -S -O2 file.cpp" << endl;
    cout << "• Use Compiler Explorer (godbolt.org)" << endl;
    cout << "• Static assert: static_assert(constexpr_expr)" << endl;
    cout << "• constexpr variables must be compile-time" << endl;
    
    cout << "\nBest Practices:" << endl;
    cout << "✓ Use constexpr for constants and pure functions" << endl;
    cout << "✓ Use const for runtime constants" << endl;
    cout << "✓ Use if constexpr for type-dependent code" << endl;
    cout << "✓ Let compiler do algebraic simplification" << endl;
    cout << "✓ Enable optimization flags (-O2, -O3)" << endl;
    
    return 0;
}

