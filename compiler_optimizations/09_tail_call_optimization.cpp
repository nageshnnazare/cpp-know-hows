/*
 * TAIL CALL OPTIMIZATION (TCO)
 * Also known as: Tail Call Elimination
 * 
 * DEFINITION:
 * When a function's last operation is calling another function,
 * the compiler can reuse the current stack frame instead of creating a new one.
 * 
 * PURPOSE:
 * - Prevent stack overflow in recursive functions
 * - Convert recursion to iteration
 * - Enable functional programming patterns
 * 
 * REQUIREMENTS:
 * - Function call must be in tail position
 * - No operations after the call
 * - Return the result directly
 * 
 * BENEFITS:
 * + Constant stack space (O(1) instead of O(n))
 * + Prevents stack overflow
 * + Similar performance to iteration
 */

#include <iostream>
#include <chrono>

using namespace std;
using namespace chrono;

// ============= Example 1: Factorial - Bad (Not Tail Recursive) =============

// NOT tail recursive - multiplication happens after recursive call
int factorialNonTail(int n) {
    if (n <= 1) return 1;
    return n * factorialNonTail(n - 1);  // Operation AFTER recursive call
    // Stack: fact(5) waits for fact(4) waits for fact(3)...
}

// Tail recursive - accumulator pattern
int factorialTail(int n, int accumulator = 1) {
    if (n <= 1) return accumulator;
    return factorialTail(n - 1, n * accumulator);  // Last operation is call
    // Compiler can reuse stack frame!
}

// Iterative equivalent (what TCO generates)
int factorialIterative(int n) {
    int result = 1;
    while (n > 1) {
        result *= n;
        n--;
    }
    return result;
}

// ============= Example 2: Fibonacci - Tail Recursive =============

// Non-tail recursive (two recursive calls)
int fibonacciNonTail(int n) {
    if (n <= 1) return n;
    return fibonacciNonTail(n - 1) + fibonacciNonTail(n - 2);
    // NOT tail recursive - addition after calls
}

// Tail recursive version
int fibonacciTail(int n, int a = 0, int b = 1) {
    if (n == 0) return a;
    if (n == 1) return b;
    return fibonacciTail(n - 1, b, a + b);  // Tail call!
}

// ============= Example 3: Sum List - Tail Recursive =============

struct Node {
    int value;
    Node* next;
    Node(int v, Node* n = nullptr) : value(v), next(n) {}
};

// Non-tail recursive
int sumListNonTail(Node* node) {
    if (!node) return 0;
    return node->value + sumListNonTail(node->next);  // NOT tail recursive
}

// Tail recursive
int sumListTail(Node* node, int accumulator = 0) {
    if (!node) return accumulator;
    return sumListTail(node->next, accumulator + node->value);  // Tail call!
}

// ============= Example 4: GCD - Naturally Tail Recursive =============

int gcdNonOptimized(int a, int b) {
    if (b == 0) return a;
    return gcdNonOptimized(b, a % b);  // Perfect tail call!
}

// ============= Example 5: Power Function =============

// Non-tail recursive
double powerNonTail(double base, int exp) {
    if (exp == 0) return 1;
    return base * powerNonTail(base, exp - 1);  // NOT tail
}

// Tail recursive
double powerTail(double base, int exp, double accumulator = 1) {
    if (exp == 0) return accumulator;
    return powerTail(base, exp - 1, base * accumulator);  // Tail call!
}

// ============= Example 6: Mutual Recursion =============

bool isEven(int n);
bool isOdd(int n);

bool isEven(int n) {
    if (n == 0) return true;
    return isOdd(n - 1);  // Tail call to isOdd
}

bool isOdd(int n) {
    if (n == 0) return false;
    return isEven(n - 1);  // Tail call to isEven
}

// ============= Performance Test =============

void performanceTest() {
    cout << "\n=== Performance Comparison ===" << endl;
    
    const int n = 10000;
    
    // Tail recursive (optimized to iteration)
    auto start1 = high_resolution_clock::now();
    int result1 = factorialTail(n);
    auto end1 = high_resolution_clock::now();
    auto duration1 = duration_cast<microseconds>(end1 - start1).count();
    
    // Iterative
    auto start2 = high_resolution_clock::now();
    int result2 = factorialIterative(n);
    auto end2 = high_resolution_clock::now();
    auto duration2 = duration_cast<microseconds>(end2 - start2).count();
    
    cout << "Tail recursive (with TCO): " << duration1 << " μs" << endl;
    cout << "Iterative:                 " << duration2 << " μs" << endl;
    cout << "Performance similar - TCO converts recursion to iteration!" << endl;
    
    // Note: Cannot test non-tail for large n - would stack overflow!
}

// ============= Example 7: When TCO Doesn't Apply =============

int notTailRecursive1(int n) {
    if (n <= 0) return 0;
    int result = notTailRecursive1(n - 1);
    return result + 1;  // Operation AFTER recursive call
}

int notTailRecursive2(int n) {
    if (n <= 0) return 0;
    notTailRecursive2(n - 1);
    cout << n << " ";  // Statement after call
    return n;
}

// ============= Example 8: Verification =============

void verifyTCO() {
    cout << "\n\n8. VERIFICATION OF TCO:" << endl;
    cout << "=======================" << endl;
    
    cout << "\nTo verify if TCO is applied:" << endl;
    cout << "1. Compile: g++ -S -O2 file.cpp" << endl;
    cout << "2. Check assembly for tail calls" << endl;
    cout << "3. Look for 'jmp' instead of 'call'" << endl;
    cout << "4. Should see no new stack frame" << endl;
    
    cout << "\nWithout TCO: call instruction (new stack frame)" << endl;
    cout << "With TCO: jmp instruction (reuse stack frame)" << endl;
}

int main() {
    cout << "=== TAIL CALL OPTIMIZATION (TCO) ===" << endl;
    
    // Example 1: Factorial
    cout << "\n1. FACTORIAL:" << endl;
    cout << "=============" << endl;
    cout << "5! (non-tail): " << factorialNonTail(5) << endl;
    cout << "5! (tail):     " << factorialTail(5) << endl;
    cout << "5! (iterative): " << factorialIterative(5) << endl;
    
    cout << "\nTail recursive version uses constant stack space!" << endl;
    cout << "Can compute factorial(10000) without stack overflow" << endl;
    
    // Example 2: Fibonacci
    cout << "\n\n2. FIBONACCI:" << endl;
    cout << "=============" << endl;
    cout << "fib(10) (tail): " << fibonacciTail(10) << endl;
    cout << "Note: Non-tail fib(10) would be extremely slow!" << endl;
    
    // Example 3: Sum List
    cout << "\n\n3. SUM LINKED LIST:" << endl;
    cout << "===================" << endl;
    Node* list = new Node(1, new Node(2, new Node(3, new Node(4, new Node(5)))));
    cout << "Sum (tail recursive): " << sumListTail(list) << endl;
    
    // Cleanup
    while (list) {
        Node* temp = list;
        list = list->next;
        delete temp;
    }
    
    // Example 4: GCD
    cout << "\n\n4. GREATEST COMMON DIVISOR:" << endl;
    cout << "============================" << endl;
    cout << "GCD(48, 18) = " << gcdNonOptimized(48, 18) << endl;
    cout << "Naturally tail recursive - perfect for TCO!" << endl;
    
    // Example 5: Power
    cout << "\n\n5. POWER FUNCTION:" << endl;
    cout << "==================" << endl;
    cout << "2^10 (tail): " << powerTail(2, 10) << endl;
    
    // Example 6: Mutual Recursion
    cout << "\n\n6. MUTUAL RECURSION:" << endl;
    cout << "====================" << endl;
    cout << "isEven(4) = " << (isEven(4) ? "true" : "false") << endl;
    cout << "isOdd(7) = " << (isOdd(7) ? "true" : "false") << endl;
    cout << "Both functions tail-call each other - optimized!" << endl;
    
    // Performance
    performanceTest();
    
    // Verification
    verifyTCO();
    
    cout << "\n\n=== KEY TAKEAWAYS ===" << endl;
    cout << "1. TCO converts tail recursion to iteration" << endl;
    cout << "2. Prevents stack overflow for deep recursion" << endl;
    cout << "3. Requires call to be in tail position" << endl;
    cout << "4. Use accumulator pattern for tail recursion" << endl;
    cout << "5. Performance equivalent to loops" << endl;
    
    cout << "\nTail Call Requirements:" << endl;
    cout << "✓ Recursive call is last operation" << endl;
    cout << "✓ Return result directly (no operations after)" << endl;
    cout << "✓ No pending computations" << endl;
    cout << "✓ Single recursive path (not multiple calls)" << endl;
    
    cout << "\nNOT Tail Recursive:" << endl;
    cout << "✗ return n * factorial(n-1)  // Multiplication after" << endl;
    cout << "✗ int x = recurse(); return x+1;  // Addition after" << endl;
    cout << "✗ recurse(); doSomething();  // Code after call" << endl;
    
    cout << "\nTail Recursive:" << endl;
    cout << "✓ return factorial(n-1, n*acc)  // Direct return" << endl;
    cout << "✓ return gcd(b, a%b)  // Direct return" << endl;
    cout << "✓ return process(list->next, acc+val)  // Direct return" << endl;
    
    cout << "\nAccumulator Pattern:" << endl;
    cout << "• Pass intermediate result as parameter" << endl;
    cout << "• Build result through parameters" << endl;
    cout << "• Return accumulator at base case" << endl;
    cout << "• Enables tail recursion" << endl;
    
    cout << "\nCompiler Support:" << endl;
    cout << "• GCC: Enabled with -O2, -O3" << endl;
    cout << "• Clang: Enabled with -O2, -O3" << endl;
    cout << "• MSVC: Limited support" << endl;
    cout << "• Check assembly to verify" << endl;
    
    cout << "\nVerification:" << endl;
    cout << "• Compile with -O2: g++ -O2 -S file.cpp" << endl;
    cout << "• Check assembly: no new call instructions" << endl;
    cout << "• Should see jump instead of call" << endl;
    
    cout << "\nWhen to Use:" << endl;
    cout << "✓ Functional programming style" << endl;
    cout << "✓ Deep recursion needed" << endl;
    cout << "✓ Cleaner than iterative solution" << endl;
    cout << "✓ Natural recursive algorithms (GCD, tree traversal)" << endl;
    
    cout << "\nAlternatives:" << endl;
    cout << "• Convert to iterative (manual)" << endl;
    cout << "• Use stack data structure explicitly" << endl;
    cout << "• Trampoline pattern (manual TCO)" << endl;
    cout << "• Increase stack size (not recommended)" << endl;
    
    return 0;
}

