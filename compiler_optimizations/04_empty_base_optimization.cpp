/*
 * EMPTY BASE OPTIMIZATION (EBO)
 * 
 * DEFINITION:
 * Compiler optimization that allows empty base classes to occupy zero bytes,
 * preventing unnecessary padding and size increase.
 * 
 * PURPOSE:
 * - Save memory in class hierarchies
 * - Enable zero-cost policy classes
 * - Used in STL (allocators, comparators)
 * 
 * BENEFITS:
 * + Zero overhead for empty base classes
 * + Enables policy-based design
 * + Memory efficient
 */

#include <iostream>
#include <memory>

using namespace std;

// Empty classes
struct Empty1 {};
struct Empty2 {};
struct Empty3 {};

// Without EBO - composition
struct WithoutEBO {
    Empty1 e1;  // Takes 1 byte (minimum size)
    Empty2 e2;  // Takes 1 byte + padding
    int data;
};

// With EBO - inheritance
struct WithEBO : Empty1, Empty2 {
    int data;
};

// Policy-based design example
template <typename T, typename Allocator = allocator<T>>
class Container : private Allocator {  // EBO applied
    T* data;
    size_t size;
public:
    Container() : data(nullptr), size(0) {}
    // Allocator takes zero space!
};

int main() {
    cout << "=== EMPTY BASE OPTIMIZATION (EBO) ===" << endl;
    cout << "\nSize without EBO (composition): " << sizeof(WithoutEBO) << " bytes" << endl;
    cout << "Size with EBO (inheritance):    " << sizeof(WithEBO) << " bytes" << endl;
    cout << "Savings: " << (sizeof(WithoutEBO) - sizeof(WithEBO)) << " bytes" << endl;
    
    cout << "\nEmpty class sizes:" << endl;
    cout << "sizeof(Empty1): " << sizeof(Empty1) << " byte(s)" << endl;
    cout << "sizeof(Empty2): " << sizeof(Empty2) << " byte(s)" << endl;
    
    cout << "\nWhen EBO matters:" << endl;
    cout << "✓ Policy-based design" << endl;
    cout << "✓ STL allocators" << endl;
    cout << "✓ Comparators" << endl;
    cout << "✓ Tag dispatch" << endl;
    
    return 0;
}
