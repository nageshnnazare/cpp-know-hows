/*
 * SFINAE - Substitution Failure Is Not An Error
 * 
 * DEFINITION:
 * "When substituting template parameters fails, the compiler doesn't
 *  generate an error - it simply removes that overload from consideration."
 * 
 * PURPOSE:
 * - Enable/disable templates based on type properties
 * - Compile-time introspection
 * - Type trait detection
 * - Overload resolution control
 * 
 * MODERN ALTERNATIVES:
 * - C++17: if constexpr
 * - C++20: Concepts
 * 
 * BENEFITS:
 * + Compile-time type checking
 * + No runtime overhead
 * + Type-safe
 * + Powerful metaprogramming
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <type_traits>

using namespace std;

// ============= Example 1: Detect if type has size() method =============

// SFINAE trick to detect size() method
template <typename T>
class has_size_method {
private:
    template <typename U>
    static auto test(int) -> decltype(declval<U>().size(), true_type{});
    
    template <typename>
    static false_type test(...);
    
public:
    static constexpr bool value = decltype(test<T>(0))::value;
};

// Use it
template <typename T>
typename enable_if<has_size_method<T>::value, size_t>::type
getSize(const T& container) {
    return container.size();
}

template <typename T>
typename enable_if<!has_size_method<T>::value, size_t>::type
getSize(const T&) {
    return 1;  // Default size for types without size()
}

// ============= Example 2: Enable function only for integral types =============

template <typename T>
typename enable_if<is_integral<T>::value, T>::type
multiply(T a, T b) {
    cout << "Integral multiplication: ";
    return a * b;
}

template <typename T>
typename enable_if<is_floating_point<T>::value, T>::type
multiply(T a, T b) {
    cout << "Floating-point multiplication: ";
    return a * b;
}

// ============= Example 3: Detect if type is printable =============

template <typename T>
class is_printable {
private:
    template <typename U>
    static auto test(int) -> decltype(cout << declval<U>(), true_type{});
    
    template <typename>
    static false_type test(...);
    
public:
    static constexpr bool value = decltype(test<T>(0))::value;
};

template <typename T>
typename enable_if<is_printable<T>::value>::type
print(const T& value) {
    cout << "Printable: " << value << endl;
}

template <typename T>
typename enable_if<!is_printable<T>::value>::type
print(const T&) {
    cout << "Not printable (no operator<<)" << endl;
}

// ============= Example 4: Detect if type has begin()/end() =============

template <typename T>
class is_iterable {
private:
    template <typename U>
    static auto test(int) -> decltype(
        begin(declval<U>()),
        end(declval<U>()),
        true_type{}
    );
    
    template <typename>
    static false_type test(...);
    
public:
    static constexpr bool value = decltype(test<T>(0))::value;
};

template <typename T>
typename enable_if<is_iterable<T>::value>::type
printContainer(const T& container) {
    cout << "Container elements: ";
    for (const auto& elem : container) {
        cout << elem << " ";
    }
    cout << endl;
}

template <typename T>
typename enable_if<!is_iterable<T>::value>::type
printContainer(const T& value) {
    cout << "Not a container: " << value << endl;
}

// ============= Example 5: Smart pointer detection =============

template <typename T>
class is_smart_pointer {
private:
    template <typename U>
    static auto test(int) -> decltype(*declval<U>(), true_type{});
    
    template <typename>
    static false_type test(...);
    
public:
    static constexpr bool value = decltype(test<T>(0))::value && !is_pointer<T>::value;
};

// ============= Example 6: Modern C++17 approach =============

// Using if constexpr (C++17)
template <typename T>
void modernPrint(const T& value) {
    if constexpr (is_integral<T>::value) {
        cout << "Integer: " << value << endl;
    } else if constexpr (is_floating_point<T>::value) {
        cout << "Float: " << value << endl;
    } else if constexpr (is_same<T, string>::value) {
        cout << "String: " << value << endl;
    } else {
        cout << "Other type" << endl;
    }
}

// ============= Example 7: Type traits composition =============

template <typename T>
struct is_numeric {
    static constexpr bool value = is_integral<T>::value || is_floating_point<T>::value;
};

template <typename T>
typename enable_if<is_numeric<T>::value, T>::type
square(T value) {
    return value * value;
}

// Non-printable type for testing
struct NoPrint {
    int data;
};

int main() {
    cout << "=== SFINAE - SUBSTITUTION FAILURE IS NOT AN ERROR ===" << endl;
    
    // Example 1: has_size_method
    cout << "\n1. DETECT size() METHOD:" << endl;
    cout << "========================" << endl;
    
    vector<int> vec = {1, 2, 3};
    int num = 42;
    
    cout << "vector has size(): " << has_size_method<vector<int>>::value << endl;
    cout << "int has size(): " << has_size_method<int>::value << endl;
    cout << "getSize(vector): " << getSize(vec) << endl;
    cout << "getSize(int): " << getSize(num) << endl;
    
    // Example 2: Type-specific multiplication
    cout << "\n\n2. TYPE-SPECIFIC OPERATIONS:" << endl;
    cout << "============================" << endl;
    
    cout << multiply(5, 3) << endl;
    cout << multiply(5.5, 2.5) << endl;
    
    // Example 3: is_printable
    cout << "\n\n3. DETECT PRINTABLE TYPES:" << endl;
    cout << "==========================" << endl;
    
    cout << "int is printable: " << is_printable<int>::value << endl;
    cout << "string is printable: " << is_printable<string>::value << endl;
    cout << "NoPrint is printable: " << is_printable<NoPrint>::value << endl;
    
    print(42);
    print(string("Hello"));
    print(NoPrint{10});
    
    // Example 4: is_iterable
    cout << "\n\n4. DETECT ITERABLE TYPES:" << endl;
    cout << "=========================" << endl;
    
    vector<int> numbers = {1, 2, 3, 4, 5};
    printContainer(numbers);
    printContainer(42);
    
    // Example 5: Smart pointer detection
    cout << "\n\n5. SMART POINTER DETECTION:" << endl;
    cout << "===========================" << endl;
    
    cout << "int* is smart pointer: " << is_smart_pointer<int*>::value << endl;
    cout << "unique_ptr is smart pointer: " << is_smart_pointer<unique_ptr<int>>::value << endl;
    cout << "shared_ptr is smart pointer: " << is_smart_pointer<shared_ptr<int>>::value << endl;
    
    // Example 6: Modern C++17
    cout << "\n\n6. MODERN C++17 if constexpr:" << endl;
    cout << "=============================" << endl;
    
    modernPrint(42);
    modernPrint(3.14);
    modernPrint(string("Hello"));
    modernPrint('A');
    
    // Example 7: Type traits composition
    cout << "\n\n7. COMPOSITE TYPE TRAITS:" << endl;
    cout << "=========================" << endl;
    
    cout << "int is numeric: " << is_numeric<int>::value << endl;
    cout << "double is numeric: " << is_numeric<double>::value << endl;
    cout << "string is numeric: " << is_numeric<string>::value << endl;
    
    cout << "square(5) = " << square(5) << endl;
    cout << "square(2.5) = " << square(2.5) << endl;
    
    cout << "\n\n=== KEY TAKEAWAYS ===" << endl;
    cout << "1. SFINAE = Substitution Failure Is Not An Error" << endl;
    cout << "2. Failed template substitution removes overload" << endl;
    cout << "3. Enables compile-time type introspection" << endl;
    cout << "4. Used to detect type properties" << endl;
    cout << "5. Foundation of type traits" << endl;
    
    cout << "\nCommon SFINAE Techniques:" << endl;
    cout << "• enable_if for conditional compilation" << endl;
    cout << "• decltype to test expressions" << endl;
    cout << "• declval for hypothetical values" << endl;
    cout << "• true_type/false_type for results" << endl;
    
    cout << "\nModern Alternatives:" << endl;
    cout << "• C++17: if constexpr" << endl;
    cout << "• C++20: Concepts" << endl;
    cout << "• C++20: requires clauses" << endl;
    
    cout << "\nUse Cases:" << endl;
    cout << "• Template constraints" << endl;
    cout << "• Type trait detection" << endl;
    cout << "• Overload resolution control" << endl;
    cout << "• Compile-time reflection" << endl;
    cout << "• Generic programming" << endl;
    
    cout << "\nStandard Library Examples:" << endl;
    cout << "• std::enable_if" << endl;
    cout << "• std::is_integral" << endl;
    cout << "• std::is_same" << endl;
    cout << "• std::decay" << endl;
    cout << "• All type traits in <type_traits>" << endl;
    
    return 0;
}

