/*
 * FUNCTION INLINING
 * 
 * DEFINITION:
 * Compiler replaces a function call with the function's body,
 * eliminating call overhead.
 * 
 * PURPOSE:
 * - Eliminate function call overhead
 * - Enable further optimizations
 * - Improve instruction cache usage
 * 
 * TYPES:
 * 1. Inline keyword (hint to compiler)
 * 2. Automatic inlining (compiler decision)
 * 3. Force inline (compiler-specific)
 * 
 * BENEFITS:
 * + No function call overhead
 * + Better optimization opportunities
 * + Faster for small functions
 * 
 * DRAWBACKS:
 * - Increased code size
 * - Can hurt instruction cache
 * - Longer compilation time
 */

#include <iostream>
#include <chrono>
#include <cmath>

using namespace std;
using namespace chrono;

// ============= Example 1: Simple Inline Functions =============

// Inline function - compiler hint
inline int add(int a, int b) {
    return a + b;
}

// Non-inline function
int addNoInline(int a, int b) {
    return a + b;
}

// Force inline (GCC/Clang)
__attribute__((always_inline))
inline int addForceInline(int a, int b) {
    return a + b;
}

// Prevent inline (GCC/Clang)
__attribute__((noinline))
int addNeverInline(int a, int b) {
    return a + b;
}

// ============= Example 2: Class Member Functions =============

class Calculator {
public:
    // Implicitly inline (defined in class)
    int multiply(int a, int b) {
        return a * b;
    }
    
    // Explicitly inline
    inline int divide(int a, int b);
    
    // Large function - unlikely to inline
    int complexCalculation(int n) {
        int result = 0;
        for (int i = 0; i < n; ++i) {
            result += i * i;
            result += sqrt(result);
            result %= 1000000;
        }
        return result;
    }
};

inline int Calculator::divide(int a, int b) {
    return b != 0 ? a / b : 0;
}

// ============= Example 3: Performance Comparison =============

// Small function - good candidate for inlining
inline int square(int x) {
    return x * x;
}

__attribute__((noinline))
int squareNoInline(int x) {
    return x * x;
}

void performanceTest() {
    cout << "\n=== Performance Test ===" << endl;
    const int iterations = 10000000;
    
    // Test inlined function
    auto start1 = high_resolution_clock::now();
    volatile int result1 = 0;
    for (int i = 0; i < iterations; ++i) {
        result1 += square(i);
    }
    auto end1 = high_resolution_clock::now();
    auto duration1 = duration_cast<microseconds>(end1 - start1).count();
    
    // Test non-inlined function
    auto start2 = high_resolution_clock::now();
    volatile int result2 = 0;
    for (int i = 0; i < iterations; ++i) {
        result2 += squareNoInline(i);
    }
    auto end2 = high_resolution_clock::now();
    auto duration2 = duration_cast<microseconds>(end2 - start2).count();
    
    cout << "Inlined:     " << duration1 << " μs" << endl;
    cout << "Non-inlined: " << duration2 << " μs" << endl;
    cout << "Speedup:     " << (double)duration2 / duration1 << "x" << endl;
}

// ============= Example 4: When NOT to Inline =============

// Too large - compiler won't inline
inline void largeFunction() {
    int data[1000];
    for (int i = 0; i < 1000; ++i) {
        data[i] = i * i;
    }
    // Many more operations...
    for (int i = 0; i < 999; ++i) {
        data[i] += data[i + 1];
    }
}

// Recursive - can't fully inline
inline int factorial(int n) {
    return n <= 1 ? 1 : n * factorial(n - 1);
}

// Virtual function - can't inline through pointer
class Base {
public:
    virtual ~Base() = default;
    virtual int getValue() { return 42; }  // Can't inline via pointer
};

// ============= Example 5: Inline with Templates =============

template <typename T>
inline T maximum(T a, T b) {
    return a > b ? a : b;
}

// Template functions are implicitly inline
template <typename T>
T minimum(T a, T b) {
    return a < b ? a : b;
}

// ============= Example 6: constexpr (Guaranteed Compile-Time) =============

constexpr int power(int base, int exp) {
    int result = 1;
    for (int i = 0; i < exp; ++i) {
        result *= base;
    }
    return result;
}

// ============= Example 7: Lambda Inlining =============

void lambdaInlining() {
    cout << "\n=== Lambda Inlining ===" << endl;
    
    // Lambdas are typically inlined
    auto multiply = [](int a, int b) { return a * b; };
    
    int result = multiply(5, 3);
    cout << "Result: " << result << endl;
    cout << "Lambdas are excellent candidates for inlining!" << endl;
}

// ============= Example 8: Accessor Functions =============

class Point {
private:
    int x, y;
    
public:
    Point(int x_, int y_) : x(x_), y(y_) {}
    
    // Getters - perfect for inlining
    int getX() const { return x; }
    int getY() const { return y; }
    
    // Setters - also inline
    void setX(int x_) { x = x_; }
    void setY(int y_) { y = y_; }
};

int main() {
    cout << "=== FUNCTION INLINING ===" << endl;
    
    // Example 1: Basic usage
    cout << "\n1. Basic Inline Functions:" << endl;
    cout << "=========================" << endl;
    cout << "add(5, 3) = " << add(5, 3) << endl;
    cout << "addForceInline(5, 3) = " << addForceInline(5, 3) << endl;
    cout << "Note: Function body replaces the call" << endl;
    
    // Example 2: Class members
    cout << "\n2. Class Member Functions:" << endl;
    cout << "==========================" << endl;
    Calculator calc;
    cout << "multiply(4, 5) = " << calc.multiply(4, 5) << endl;
    cout << "divide(20, 4) = " << calc.divide(20, 4) << endl;
    
    // Example 3: Performance
    performanceTest();
    
    // Example 4: Templates
    cout << "\n4. Template Functions:" << endl;
    cout << "======================" << endl;
    cout << "maximum(10, 20) = " << maximum(10, 20) << endl;
    cout << "minimum(10, 20) = " << minimum(10, 20) << endl;
    cout << "Templates are always inline" << endl;
    
    // Example 5: constexpr
    cout << "\n5. constexpr Functions:" << endl;
    cout << "=======================" << endl;
    constexpr int result = power(2, 10);  // Computed at compile time!
    cout << "2^10 = " << result << " (computed at compile time)" << endl;
    
    // Example 6: Lambdas
    lambdaInlining();
    
    // Example 7: Accessors
    cout << "\n7. Accessor Functions:" << endl;
    cout << "======================" << endl;
    Point p(10, 20);
    cout << "Point(" << p.getX() << ", " << p.getY() << ")" << endl;
    cout << "Getters/setters are inlined" << endl;
    
    cout << "\n\n=== KEY TAKEAWAYS ===" << endl;
    cout << "1. Inlining eliminates function call overhead" << endl;
    cout << "2. Compiler makes inlining decisions automatically" << endl;
    cout << "3. 'inline' keyword is a hint, not a command" << endl;
    cout << "4. Small functions benefit most from inlining" << endl;
    cout << "5. Can improve performance 2-10x for tiny functions" << endl;
    
    cout << "\nGood Candidates for Inlining:" << endl;
    cout << "✓ Small functions (1-3 lines)" << endl;
    cout << "✓ Getters and setters" << endl;
    cout << "✓ Simple arithmetic operations" << endl;
    cout << "✓ Template functions" << endl;
    cout << "✓ Lambdas" << endl;
    cout << "✓ constexpr functions" << endl;
    
    cout << "\nBad Candidates for Inlining:" << endl;
    cout << "✗ Large functions (>10 lines)" << endl;
    cout << "✗ Recursive functions" << endl;
    cout << "✗ Virtual functions (via pointer)" << endl;
    cout << "✗ Functions with loops" << endl;
    cout << "✗ Functions called rarely" << endl;
    
    cout << "\nCompiler Decisions:" << endl;
    cout << "• Ignores 'inline' if function is too large" << endl;
    cout << "• Inlines without 'inline' if beneficial" << endl;
    cout << "• Uses heuristics (size, complexity, call frequency)" << endl;
    cout << "• Optimization level matters (-O2, -O3)" << endl;
    
    cout << "\nForce Inlining:" << endl;
    cout << "• GCC/Clang: __attribute__((always_inline))" << endl;
    cout << "• MSVC: __forceinline" << endl;
    cout << "• Use sparingly - trust the compiler!" << endl;
    
    cout << "\nPrevent Inlining:" << endl;
    cout << "• GCC/Clang: __attribute__((noinline))" << endl;
    cout << "• MSVC: __declspec(noinline)" << endl;
    cout << "• Useful for debugging and profiling" << endl;
    
    return 0;
}

