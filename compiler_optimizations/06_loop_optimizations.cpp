/*
 * LOOP OPTIMIZATIONS
 * 
 * DEFINITION:
 * Compiler transformations that improve loop performance through
 * unrolling, vectorization, and other techniques.
 * 
 * TYPES:
 * 1. Loop Unrolling - Execute multiple iterations per loop
 * 2. Loop Vectorization - SIMD instructions for parallel execution
 * 3. Loop Invariant Code Motion - Move invariants outside loop
 * 4. Loop Fusion - Combine multiple loops
 * 5. Loop Interchange - Reorder nested loops
 * 
 * BENEFITS:
 * + 2-10x performance improvement
 * + Better instruction pipeline usage
 * + Reduced branch prediction overhead
 * + SIMD parallelism
 */

#include <iostream>
#include <chrono>
#include <vector>
#include <numeric>
#include <cmath>
#include <cstring>
#include <algorithm>

using namespace std;
using namespace chrono;

// ============= Example 1: Loop Unrolling =============

// Manual unrolling
void sumArrayUnrolled(const int* arr, int size, int& result) {
    result = 0;
    int i = 0;
    
    // Process 4 elements at a time
    for (; i + 3 < size; i += 4) {
        result += arr[i];
        result += arr[i + 1];
        result += arr[i + 2];
        result += arr[i + 3];
    }
    
    // Handle remaining elements
    for (; i < size; ++i) {
        result += arr[i];
    }
}

// Regular loop (compiler may unroll)
void sumArrayRegular(const int* arr, int size, int& result) {
    result = 0;
    for (int i = 0; i < size; ++i) {
        result += arr[i];
    }
}

// ============= Example 2: Loop Vectorization (SIMD) =============

// Vectorizable loop
void addArrays(const float* a, const float* b, float* c, int size) {
    // Compiler can vectorize this with SIMD
    for (int i = 0; i < size; ++i) {
        c[i] = a[i] + b[i];
    }
}

// Not vectorizable (data dependency)
void prefixSum(int* arr, int size) {
    // Can't vectorize - each iteration depends on previous
    for (int i = 1; i < size; ++i) {
        arr[i] += arr[i - 1];
    }
}

// ============= Example 3: Loop Invariant Code Motion =============

void loopInvariantBad(const vector<int>& data) {
    for (size_t i = 0; i < data.size(); ++i) {
        // strlen called every iteration (bad!)
        const char* str = "constant";
        size_t len = strlen(str);  // Loop invariant!
        // Use len...
    }
}

void loopInvariantGood(const vector<int>& data) {
    // Move invariant outside loop
    const char* str = "constant";
    size_t len = strlen(str);  // Computed once
    
    for (size_t i = 0; i < data.size(); ++i) {
        // Use len...
    }
}

// ============= Example 4: Loop Fusion =============

// Two separate loops (bad)
void separateLoops(vector<int>& a, vector<int>& b, int n) {
    for (int i = 0; i < n; ++i) {
        a[i] *= 2;
    }
    
    for (int i = 0; i < n; ++i) {
        b[i] += 5;
    }
}

// Fused loop (better)
void fusedLoop(vector<int>& a, vector<int>& b, int n) {
    for (int i = 0; i < n; ++i) {
        a[i] *= 2;
        b[i] += 5;
    }
}

// ============= Example 5: Cache-Friendly Loop Order =============

constexpr int SIZE = 1000;

// Bad: Column-major access (cache-unfriendly)
void sumColumnMajor(int matrix[][SIZE], int rows, int cols) {
    for (int j = 0; j < cols; ++j) {
        for (int i = 0; i < rows; ++i) {
            matrix[i][j] += 1;  // Jumps in memory
        }
    }
}

// Good: Row-major access (cache-friendly)
void sumRowMajor(int matrix[][SIZE], int rows, int cols) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            matrix[i][j] += 1;  // Sequential memory access
        }
    }
}

// ============= Example 6: Strength Reduction =============

// Expensive operation in loop
void strengthReductionBad(int n) {
    for (int i = 0; i < n; ++i) {
        int square = i * i;  // Multiplication each iteration
        cout << square << " ";
    }
}

// Optimized with strength reduction
void strengthReductionGood(int n) {
    int square = 0;
    int odd = 1;
    for (int i = 0; i < n; ++i) {
        cout << square << " ";
        square += odd;  // Addition instead of multiplication!
        odd += 2;
    }
}

// ============= Performance Tests =============

void testUnrolling() {
    cout << "\n=== Loop Unrolling Performance ===" << endl;
    
    const int size = 10000000;
    vector<int> arr(size);
    for (int i = 0; i < size; ++i) {
        arr[i] = i;
    }
    
    int result1, result2;
    
    // Regular loop
    auto start1 = high_resolution_clock::now();
    sumArrayRegular(arr.data(), size, result1);
    auto end1 = high_resolution_clock::now();
    auto duration1 = duration_cast<microseconds>(end1 - start1).count();
    
    // Unrolled loop
    auto start2 = high_resolution_clock::now();
    sumArrayUnrolled(arr.data(), size, result2);
    auto end2 = high_resolution_clock::now();
    auto duration2 = duration_cast<microseconds>(end2 - start2).count();
    
    cout << "Regular loop: " << duration1 << " μs" << endl;
    cout << "Unrolled loop: " << duration2 << " μs" << endl;
    cout << "Speedup: " << (double)duration1 / duration2 << "x" << endl;
}

void testVectorization() {
    cout << "\n=== Vectorization Performance ===" << endl;
    
    const int size = 10000000;
    vector<float> a(size, 1.0f);
    vector<float> b(size, 2.0f);
    vector<float> c(size);
    
    auto start = high_resolution_clock::now();
    addArrays(a.data(), b.data(), c.data(), size);
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start).count();
    
    cout << "Array addition (vectorized): " << duration << " μs" << endl;
    cout << "Note: Compiler uses SIMD instructions (SSE/AVX)" << endl;
}

void testLoopFusion() {
    cout << "\n=== Loop Fusion Performance ===" << endl;
    
    const int size = 1000000;
    vector<int> a(size, 1);
    vector<int> b(size, 2);
    
    // Separate loops
    auto start1 = high_resolution_clock::now();
    separateLoops(a, b, size);
    auto end1 = high_resolution_clock::now();
    auto duration1 = duration_cast<microseconds>(end1 - start1).count();
    
    // Reset
    fill(a.begin(), a.end(), 1);
    fill(b.begin(), b.end(), 2);
    
    // Fused loop
    auto start2 = high_resolution_clock::now();
    fusedLoop(a, b, size);
    auto end2 = high_resolution_clock::now();
    auto duration2 = duration_cast<microseconds>(end2 - start2).count();
    
    cout << "Separate loops: " << duration1 << " μs" << endl;
    cout << "Fused loop: " << duration2 << " μs" << endl;
    cout << "Speedup: " << (double)duration1 / duration2 << "x" << endl;
}

int main() {
    cout << "=== LOOP OPTIMIZATIONS ===" << endl;
    
    testUnrolling();
    testVectorization();
    testLoopFusion();
    
    cout << "\n\n=== KEY TAKEAWAYS ===" << endl;
    cout << "1. Loop optimizations can provide 2-10x speedup" << endl;
    cout << "2. Compiler automatically applies many optimizations" << endl;
    cout << "3. Write simple, clear loops - let compiler optimize" << endl;
    cout << "4. Cache-friendly access patterns are crucial" << endl;
    cout << "5. Vectorization provides massive parallelism" << endl;
    
    cout << "\nLoop Optimization Types:" << endl;
    cout << "• Unrolling - Reduce loop overhead" << endl;
    cout << "• Vectorization - SIMD parallelism" << endl;
    cout << "• Invariant motion - Move constants outside" << endl;
    cout << "• Fusion - Combine loops" << endl;
    cout << "• Interchange - Reorder for cache" << endl;
    cout << "• Strength reduction - Replace expensive ops" << endl;
    
    cout << "\nVectorization Requirements:" << endl;
    cout << "✓ No data dependencies between iterations" << endl;
    cout << "✓ Contiguous memory access" << endl;
    cout << "✓ Simple arithmetic operations" << endl;
    cout << "✓ Compile with -O2 or -O3" << endl;
    cout << "✓ May need -march=native for best SIMD" << endl;
    
    cout << "\nCache-Friendly Loops:" << endl;
    cout << "✓ Access memory sequentially" << endl;
    cout << "✓ Row-major for C/C++ arrays" << endl;
    cout << "✓ Process data in cache-line chunks" << endl;
    cout << "✓ Avoid stride access" << endl;
    
    cout << "\nCompiler Flags:" << endl;
    cout << "• -O2: Enable loop optimizations" << endl;
    cout << "• -O3: More aggressive unrolling" << endl;
    cout << "• -march=native: Enable SIMD for CPU" << endl;
    cout << "• -ftree-vectorize: Force vectorization" << endl;
    cout << "• -fopt-info-vec: Show vectorization info" << endl;
    
    cout << "\nManual Hints:" << endl;
    cout << "• #pragma GCC unroll n" << endl;
    cout << "• #pragma GCC ivdep (ignore dependencies)" << endl;
    cout << "• #pragma omp simd (OpenMP SIMD)" << endl;
    cout << "• __restrict__ keyword for pointers" << endl;
    
    cout << "\nBest Practices:" << endl;
    cout << "✓ Write simple, clear loops" << endl;
    cout << "✓ Use contiguous containers (vector, array)" << endl;
    cout << "✓ Avoid data dependencies" << endl;
    cout << "✓ Process data sequentially" << endl;
    cout << "✓ Let compiler do the work" << endl;
    cout << "✓ Profile before manual optimization" << endl;
    
    return 0;
}

