// Module 14 — concepts, ranges/views pipeline, defaulted spaceship operator.
// Needs a C++20 compiler with ranges (Apple Clang 14+/recent libc++).
#include <compare>
#include <concepts>
#include <iostream>
#include <ranges>
#include <vector>

// a named concept and a constrained function
template <class T>
concept Numeric = std::integral<T> || std::floating_point<T>;

Numeric auto square(Numeric auto x) { return x * x; }

// defaulted three-way comparison ⇒ all six comparison operators
struct Version {
    int major, minor, patch;
    auto operator<=>(const Version&) const = default;
};

int main() {
    std::cout << "square(6)=" << square(6) << "  square(2.5)=" << square(2.5) << '\n';

    Version a{1, 2, 0}, b{1, 3, 0};
    std::cout << std::boolalpha
              << "a < b? " << (a < b)
              << "  a == b? " << (a == b) << '\n';

    namespace rv = std::views;
    std::vector<int> v{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    std::cout << "even squares (first 3): ";
    for (int x : v | rv::filter([](int n) { return n % 2 == 0; })
                   | rv::transform([](int n) { return n * n; })
                   | rv::take(3))
        std::cout << x << ' ';
    std::cout << '\n';

    std::cout << "iota 1..5 reversed: ";
    for (int x : rv::iota(1, 6) | rv::reverse) std::cout << x << ' ';
    std::cout << '\n';
}
