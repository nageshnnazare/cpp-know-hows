// Module 01 — types, integer promotion, signed/unsigned, narrowing.
// Build:  ../examples/build.sh 01    (or: clang++ -std=c++20 -Wall 01_*.cpp)
#include <cstdint>
#include <iostream>
#include <type_traits>

int main() {
    // 1) char + char promotes to int
    char a = 1, b = 2;
    auto c = a + b;
    static_assert(std::is_same_v<decltype(c), int>, "char+char is int");
    std::cout << "char + char has type int? "
              << std::boolalpha << std::is_same_v<decltype(c), int> << '\n';

    // 2) the signed/unsigned trap
    unsigned u = 10;
    int s = -42;
    std::cout << "u + s (unsigned wrap!) = " << (u + s) << '\n';

    // 3) unsigned wraps (defined), signed overflow would be UB (don't do it)
    unsigned umax = UINT32_MAX;
    std::cout << "UINT32_MAX + 1 wraps to " << (umax + 1u) << '\n';

    // 4) narrowing is rejected by braces (this line is intentionally commented):
    // int narrow{3.14}; // ❌ compile error — that's the point
    double pi = 3.14;
    int truncated = pi; // silently truncates (use {} to forbid this)
    std::cout << "int truncated = " << truncated << '\n';

    // 5) sizeof never evaluates its operand
    auto noisy = []{ std::cout << "(should NOT print)\n"; return 0; };
    std::cout << "sizeof(noisy()) = " << sizeof(noisy()) << " (lambda not called)\n";

    // 6) fixed-width types
    std::cout << "sizeof(int)=" << sizeof(int)
              << " sizeof(long)=" << sizeof(long)
              << " sizeof(void*)=" << sizeof(void*) << '\n';
}
