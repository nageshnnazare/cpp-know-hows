// Module 12 — captures, mutable, generic lambdas, closure factories, std::function.
#include <functional>
#include <iostream>
#include <vector>

// returning a closure: capture BY VALUE so it stays valid after we return
auto multiplier(int factor) {
    return [factor](int x) { return x * factor; };
}

int main() {
    int base = 100;

    auto byValue = [base](int x) { return base + x; };  // snapshot of base
    auto byRef   = [&base](int x) { return base + x; };
    base = 1;                                            // change after capture
    std::cout << "byValue(10) = " << byValue(10) << " (saw base=100)\n";
    std::cout << "byRef(10)   = " << byRef(10)   << " (sees base=1)\n";

    // mutable: a stateful counter living inside the closure
    auto counter = [n = 0]() mutable { return ++n; };
    std::cout << "counter: " << counter() << ' ' << counter() << ' ' << counter() << '\n';

    // generic lambda
    auto add = [](auto a, auto b) { return a + b; };
    std::cout << "add(2,3)=" << add(2, 3) << "  add(1.5,2.5)=" << add(1.5, 2.5) << '\n';

    // closure factory
    auto triple = multiplier(3);
    std::cout << "triple(7) = " << triple(7) << '\n';

    // std::function: store heterogeneous callables in one container
    std::vector<std::function<int(int)>> ops{
        [](int x) { return x + 1; },
        [](int x) { return x * x; },
        triple,
    };
    for (auto& op : ops) std::cout << "op(5) = " << op(5) << '\n';
}
