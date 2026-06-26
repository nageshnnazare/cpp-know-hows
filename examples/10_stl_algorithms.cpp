// Module 10 — containers, iterators, algorithms, the erase-remove idiom, maps.
#include <algorithm>
#include <iostream>
#include <map>
#include <numeric>
#include <vector>

int main() {
    std::vector<int> v{5, 3, 8, 1, 9, 2, 8, 4};

    std::sort(v.begin(), v.end());
    std::cout << "sorted:";
    for (int x : v) std::cout << ' ' << x;
    std::cout << '\n';

    int sum = std::accumulate(v.begin(), v.end(), 0);
    std::cout << "sum = " << sum << '\n';

    auto evens = std::count_if(v.begin(), v.end(), [](int x) { return x % 2 == 0; });
    std::cout << "evens = " << evens << '\n';

    // erase-remove idiom: remove all 8s
    v.erase(std::remove(v.begin(), v.end(), 8), v.end());
    std::cout << "after removing 8:";
    for (int x : v) std::cout << ' ' << x;
    std::cout << '\n';

    // C++20 std::erase_if
    std::erase_if(v, [](int x) { return x < 3; });
    std::cout << "after erase_if(<3):";
    for (int x : v) std::cout << ' ' << x;
    std::cout << '\n';

    // map: count word frequencies, beware operator[] inserting
    std::map<std::string, int> freq;
    for (const char* w : {"a", "b", "a", "c", "b", "a"}) ++freq[w];
    for (const auto& [word, n] : freq)
        std::cout << word << " -> " << n << '\n';

    if (auto it = freq.find("a"); it != freq.end())
        std::cout << "found a with count " << it->second << '\n';
}
