// Module 09 — function/class templates, variadics + fold, if constexpr, CRTP.
#include <iostream>
#include <string>
#include <type_traits>

// variadic + C++17 fold expression
template <class... Ts>
auto sum(const Ts&... xs) { return (xs + ...); }

// if constexpr: compile-time branch
template <class T>
std::string describe(const T& x) {
    if constexpr (std::is_integral_v<T>)
        return "integer:" + std::to_string(x);
    else if constexpr (std::is_floating_point_v<T>)
        return "float:" + std::to_string(x);
    else
        return std::string("other");
}

// class template with a non-type parameter
template <class T, std::size_t N>
struct FixedArray {
    T data[N]{};
    constexpr std::size_t size() const { return N; }
    T& operator[](std::size_t i) { return data[i]; }
};

// CRTP: static polymorphism, zero overhead
template <class Derived>
struct AreaMixin {
    double scaled(double k) const {
        return k * static_cast<const Derived&>(*this).area();
    }
};
struct Square : AreaMixin<Square> {
    double s;
    explicit Square(double s) : s(s) {}
    double area() const { return s * s; }
};

int main() {
    std::cout << "sum = " << sum(1, 2, 3, 4, 5) << '\n';
    std::cout << "sum = " << sum(1.5, 2.5) << '\n';
    std::cout << describe(42) << '\n';
    std::cout << describe(3.14) << '\n';
    std::cout << describe(std::string("hi")) << '\n';

    FixedArray<int, 3> a;
    a[0] = 10; a[1] = 20;
    std::cout << "FixedArray size=" << a.size() << " a[0]=" << a[0] << '\n';

    Square sq(3.0);
    std::cout << "Square area=" << sq.area() << " scaled x2=" << sq.scaled(2.0) << '\n';
}
