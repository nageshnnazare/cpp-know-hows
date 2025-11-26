/*
 * POLICY-BASED DESIGN
 * Also known as: Policy Pattern, Strategy at Compile Time
 * 
 * DEFINITION:
 * Use template parameters to define behavior policies at compile time.
 * Combine small, focused policy classes to create flexible designs.
 * 
 * PURPOSE:
 * - Compose behavior at compile time
 * - Zero runtime overhead
 * - Highly flexible and reusable
 * - Type-safe configuration
 * 
 * BENEFITS:
 * + Compile-time polymorphism (faster)
 * + No virtual function overhead
 * + Highly flexible
 * + Can be inlined
 * + Type-safe
 * 
 * POPULARIZED BY:
 * - Andrei Alexandrescu's "Modern C++ Design"
 * - Used extensively in C++ libraries
 */

#include <iostream>
#include <string>
#include <memory>
#include <mutex>
#include <vector>

using namespace std;

// ============= Example 1: Smart Pointer with Policies =============

// Ownership policies
struct RefCounted {
    int count = 1;
    void acquire() { ++count; }
    void release() { --count; }
    bool shouldDelete() { return count == 0; }
};

struct DeepCopy {
    template <typename T>
    static T* clone(const T* ptr) {
        return ptr ? new T(*ptr) : nullptr;
    }
};

struct NoCopy {
    template <typename T>
    static T* clone(const T*) {
        throw runtime_error("Copying not allowed");
    }
};

// Deletion policies
struct DeleteByOperator {
    template <typename T>
    static void destroy(T* ptr) {
        delete ptr;
        cout << "Deleted with operator delete" << endl;
    }
};

struct DeleteByFree {
    template <typename T>
    static void destroy(T* ptr) {
        ptr->~T();
        free(ptr);
        cout << "Deleted with free()" << endl;
    }
};

// Smart pointer with policies
template <typename T,
          typename OwnershipPolicy = RefCounted,
          typename CopyPolicy = DeepCopy,
          typename DeletionPolicy = DeleteByOperator>
class SmartPtr {
private:
    T* ptr;
    OwnershipPolicy ownership;
    
public:
    explicit SmartPtr(T* p = nullptr) : ptr(p) {
        cout << "SmartPtr created" << endl;
    }
    
    SmartPtr(const SmartPtr& other) : ptr(CopyPolicy::clone(other.ptr)) {
        cout << "SmartPtr copied" << endl;
    }
    
    ~SmartPtr() {
        if (ptr) {
            DeletionPolicy::destroy(ptr);
        }
    }
    
    T& operator*() { return *ptr; }
    T* operator->() { return ptr; }
};

// ============= Example 2: Container with Allocation Policy =============

// Allocation policies
class MallocAllocator {
public:
    template <typename T>
    static T* allocate(size_t n) {
        cout << "Allocating with malloc: " << n << " elements" << endl;
        return static_cast<T*>(malloc(n * sizeof(T)));
    }
    
    template <typename T>
    static void deallocate(T* ptr) {
        cout << "Deallocating with free" << endl;
        free(ptr);
    }
};

class NewAllocator {
public:
    template <typename T>
    static T* allocate(size_t n) {
        cout << "Allocating with new: " << n << " elements" << endl;
        return new T[n];
    }
    
    template <typename T>
    static void deallocate(T* ptr) {
        cout << "Deallocating with delete[]" << endl;
        delete[] ptr;
    }
};

// Container with allocation policy
template <typename T, typename Allocator = NewAllocator>
class Vector {
private:
    T* data;
    size_t size;
    size_t capacity;
    
public:
    Vector(size_t cap = 10) : size(0), capacity(cap) {
        data = Allocator::template allocate<T>(capacity);
    }
    
    ~Vector() {
        Allocator::template deallocate<T>(data);
    }
    
    void push_back(const T& value) {
        if (size < capacity) {
            data[size++] = value;
        }
    }
    
    T& operator[](size_t index) { return data[index]; }
    size_t getSize() const { return size; }
};

// ============= Example 3: Thread Safety Policy =============

// Threading policies
class SingleThreaded {
public:
    void lock() { /* no-op */ }
    void unlock() { /* no-op */ }
};

class MultiThreaded {
private:
    mutex mtx;
public:
    void lock() { mtx.lock(); }
    void unlock() { mtx.unlock(); }
};

// Counter with threading policy
template <typename ThreadingPolicy = SingleThreaded>
class Counter {
private:
    int value;
    ThreadingPolicy threadPolicy;
    
public:
    Counter() : value(0) {}
    
    void increment() {
        threadPolicy.lock();
        ++value;
        threadPolicy.unlock();
    }
    
    int get() {
        threadPolicy.lock();
        int v = value;
        threadPolicy.unlock();
        return v;
    }
};

// ============= Example 4: Logging Policy =============

// Logging policies
class NoLogging {
public:
    void log(const string&) { /* no-op */ }
};

class ConsoleLogging {
public:
    void log(const string& message) {
        cout << "[LOG] " << message << endl;
    }
};

class FileLogging {
public:
    void log(const string& message) {
        cout << "[FILE] " << message << endl;
        // In real code: write to file
    }
};

// Application with logging policy
template <typename LoggingPolicy = ConsoleLogging>
class Application {
private:
    LoggingPolicy logger;
    
public:
    void run() {
        logger.log("Application started");
        logger.log("Processing...");
        logger.log("Application finished");
    }
};

// ============= Example 5: Validation Policy =============

// Validation policies
class NoValidation {
public:
    template <typename T>
    static bool validate(const T&) {
        return true;
    }
};

class RangeValidation {
public:
    static bool validate(int value) {
        bool valid = value >= 0 && value <= 100;
        if (!valid) cout << "Validation failed: " << value << endl;
        return valid;
    }
};

// Data class with validation policy
template <typename T, typename ValidationPolicy = NoValidation>
class ValidatedData {
private:
    T value;
    
public:
    void setValue(const T& v) {
        if (ValidationPolicy::validate(v)) {
            value = v;
            cout << "Value set to: " << value << endl;
        } else {
            cout << "Invalid value rejected" << endl;
        }
    }
    
    T getValue() const { return value; }
};

// ============= Example 6: Sorting Strategy Policy =============

// Sorting policies
class BubbleSort {
public:
    template <typename T>
    static void sort(T* arr, size_t n) {
        cout << "Using Bubble Sort" << endl;
        for (size_t i = 0; i < n - 1; ++i) {
            for (size_t j = 0; j < n - i - 1; ++j) {
                if (arr[j] > arr[j + 1]) {
                    swap(arr[j], arr[j + 1]);
                }
            }
        }
    }
};

class QuickSort {
public:
    template <typename T>
    static void sort(T* arr, size_t n) {
        cout << "Using Quick Sort" << endl;
        // Simplified - just for demonstration
        for (size_t i = 0; i < n - 1; ++i) {
            for (size_t j = i + 1; j < n; ++j) {
                if (arr[i] > arr[j]) {
                    swap(arr[i], arr[j]);
                }
            }
        }
    }
};

// Sorter with sorting policy
template <typename SortPolicy = QuickSort>
class Sorter {
public:
    template <typename T>
    static void sortArray(T* arr, size_t n) {
        SortPolicy::template sort(arr, n);
    }
};

int main() {
    cout << "=== POLICY-BASED DESIGN ===" << endl;
    
    // Example 1: Smart Pointer
    cout << "\n1. SMART POINTER WITH POLICIES:" << endl;
    cout << "===============================" << endl;
    {
        SmartPtr<int> ptr(new int(42));
        cout << "Value: " << *ptr << endl;
    }
    
    // Example 2: Container with Allocator
    cout << "\n\n2. CONTAINER WITH ALLOCATION POLICY:" << endl;
    cout << "====================================" << endl;
    
    cout << "[Using New Allocator]" << endl;
    Vector<int, NewAllocator> vec1(5);
    vec1.push_back(1);
    vec1.push_back(2);
    cout << "Vector size: " << vec1.getSize() << endl;
    
    cout << "\n[Using Malloc Allocator]" << endl;
    Vector<int, MallocAllocator> vec2(5);
    vec2.push_back(10);
    vec2.push_back(20);
    cout << "Vector size: " << vec2.getSize() << endl;
    
    // Example 3: Threading Policy
    cout << "\n\n3. COUNTER WITH THREADING POLICY:" << endl;
    cout << "==================================" << endl;
    
    Counter<SingleThreaded> singleThreadCounter;
    singleThreadCounter.increment();
    singleThreadCounter.increment();
    cout << "Single-threaded counter: " << singleThreadCounter.get() << endl;
    
    Counter<MultiThreaded> multiThreadCounter;
    multiThreadCounter.increment();
    multiThreadCounter.increment();
    cout << "Multi-threaded counter: " << multiThreadCounter.get() << endl;
    
    // Example 4: Logging Policy
    cout << "\n\n4. APPLICATION WITH LOGGING POLICY:" << endl;
    cout << "===================================" << endl;
    
    cout << "[Console Logging]" << endl;
    Application<ConsoleLogging> app1;
    app1.run();
    
    cout << "\n[No Logging]" << endl;
    Application<NoLogging> app2;
    app2.run();
    cout << "(No output - logging disabled)" << endl;
    
    // Example 5: Validation Policy
    cout << "\n\n5. DATA WITH VALIDATION POLICY:" << endl;
    cout << "===============================" << endl;
    
    cout << "[No Validation]" << endl;
    ValidatedData<int, NoValidation> data1;
    data1.setValue(150);  // Accepted
    
    cout << "\n[Range Validation]" << endl;
    ValidatedData<int, RangeValidation> data2;
    data2.setValue(50);   // Valid
    data2.setValue(150);  // Invalid
    
    // Example 6: Sorting Policy
    cout << "\n\n6. SORTER WITH SORTING POLICY:" << endl;
    cout << "==============================" << endl;
    
    int arr1[] = {5, 2, 8, 1, 9};
    Sorter<BubbleSort>::sortArray(arr1, 5);
    cout << "Sorted: ";
    for (int i = 0; i < 5; ++i) cout << arr1[i] << " ";
    cout << endl;
    
    int arr2[] = {7, 3, 6, 2, 4};
    Sorter<QuickSort>::sortArray(arr2, 5);
    cout << "Sorted: ";
    for (int i = 0; i < 5; ++i) cout << arr2[i] << " ";
    cout << endl;
    
    cout << "\n\n=== KEY TAKEAWAYS ===" << endl;
    cout << "1. Policy-based design uses templates for behavior" << endl;
    cout << "2. Policies are small, focused classes" << endl;
    cout << "3. Combine policies to create flexible designs" << endl;
    cout << "4. Zero runtime overhead (compile-time)" << endl;
    cout << "5. Highly reusable and type-safe" << endl;
    
    cout << "\nStructure:" << endl;
    cout << "• Host class takes policy template parameters" << endl;
    cout << "• Policies are small, focused classes" << endl;
    cout << "• Policies implement specific interfaces" << endl;
    cout << "• Mix and match policies as needed" << endl;
    
    cout << "\nBenefits:" << endl;
    cout << "✓ No virtual function overhead" << endl;
    cout << "✓ Compile-time polymorphism" << endl;
    cout << "✓ Highly flexible" << endl;
    cout << "✓ Type-safe" << endl;
    cout << "✓ Can be inlined" << endl;
    cout << "✓ Reusable policies" << endl;
    
    cout << "\nWhen to use:" << endl;
    cout << "• Need flexibility without runtime cost" << endl;
    cout << "• Performance is critical" << endl;
    cout << "• Want to configure behavior at compile time" << endl;
    cout << "• Building libraries/frameworks" << endl;
    
    cout << "\nReal-world examples:" << endl;
    cout << "• STL allocators (std::allocator)" << endl;
    cout << "• std::char_traits" << endl;
    cout << "• Boost libraries" << endl;
    cout << "• Loki library (Andrei Alexandrescu)" << endl;
    cout << "• Game engine component systems" << endl;
    
    cout << "\nCommon policies:" << endl;
    cout << "• Allocation (new, malloc, pool)" << endl;
    cout << "• Threading (single, multi)" << endl;
    cout << "• Logging (none, console, file)" << endl;
    cout << "• Validation (none, range, regex)" << endl;
    cout << "• Storage (value, reference, pointer)" << endl;
    cout << "• Error handling (exceptions, codes)" << endl;
    
    return 0;
}

