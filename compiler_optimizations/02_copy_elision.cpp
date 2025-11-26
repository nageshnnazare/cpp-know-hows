/*
 * COPY ELISION - Compiler Optimization
 * 
 * DEFINITION:
 * Compiler optimization that eliminates copying/moving of objects
 * even when copy/move constructors have side effects.
 * 
 * TYPES:
 * 1. RVO/NRVO (Return Value Optimization)
 * 2. Copy initialization from temporary
 * 3. Pass-by-value optimization
 * 4. Exception handling optimization
 * 
 * C++17 RULES:
 * - Mandatory in certain cases (guaranteed copy elision)
 * - Copy/move constructors don't need to exist for prvalues
 * 
 * PURPOSE:
 * - Zero-cost abstractions
 * - Efficient value semantics
 * - Clean interfaces
 */

#include <iostream>
#include <string>

using namespace std;

// ============= Tracer Class =============

class Tracer {
private:
    string name;
    int id;
    static int counter;
    
public:
    Tracer(const string& n) : name(n), id(++counter) {
        cout << "  [" << id << "] Constructor: " << name << endl;
    }
    
    Tracer(const Tracer& other) : name(other.name + "_copy"), id(++counter) {
        cout << "  [" << id << "] Copy Constructor from [" << other.id << "]: " << name << endl;
    }
    
    Tracer(Tracer&& other) noexcept : name(move(other.name)), id(++counter) {
        cout << "  [" << id << "] Move Constructor from [" << other.id << "]: " << name << endl;
    }
    
    ~Tracer() {
        cout << "  [" << id << "] Destructor: " << name << endl;
    }
    
    string getName() const { return name; }
    int getId() const { return id; }
};

int Tracer::counter = 0;

// ============= Example 1: Direct Initialization =============

Tracer createTracer() {
    return Tracer("DirectInit");  // Guaranteed elision in C++17
}

void example1_directInit() {
    cout << "\n1. Direct Initialization (Guaranteed Elision):" << endl;
    Tracer t = createTracer();  // No copy/move in C++17!
    cout << "Final object: " << t.getName() << endl;
}

// ============= Example 2: Temporary Materialization =============

void processByValue(Tracer t) {
    cout << "  Processing: " << t.getName() << endl;
}

void example2_temporary() {
    cout << "\n2. Passing Temporary (Copy Elision):" << endl;
    processByValue(Tracer("Temporary"));  // May be elided
}

// ============= Example 3: Initializer List =============

void example3_initializerList() {
    cout << "\n3. Initializer List (Guaranteed Elision):" << endl;
    Tracer t = Tracer("InitList");  // Guaranteed elision
    cout << "Object created: " << t.getName() << endl;
}

// ============= Example 4: Function Return Chain =============

Tracer processAndReturn(Tracer t) {
    cout << "  Inside processAndReturn" << endl;
    return t;  // RVO on parameter (not guaranteed)
}

void example4_returnChain() {
    cout << "\n4. Return Chain:" << endl;
    Tracer t = processAndReturn(Tracer("ChainInput"));
}

// ============= Example 5: Conditional Return =============

Tracer conditionalCreate(bool flag) {
    if (flag) {
        Tracer t1("Path1");
        return t1;  // NRVO may not apply (multiple returns)
    } else {
        Tracer t2("Path2");
        return t2;  // NRVO may not apply
    }
}

void example5_conditional() {
    cout << "\n5. Conditional Return (NRVO Limited):" << endl;
    Tracer t = conditionalCreate(true);
}

// ============= Example 6: Non-Copyable Class =============

class NonCopyable {
private:
    string data;
    
public:
    NonCopyable(const string& d) : data(d) {
        cout << "  NonCopyable constructor: " << data << endl;
    }
    
    // Delete copy operations
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
    
    // Allow move
    NonCopyable(NonCopyable&& other) noexcept : data(move(other.data)) {
        cout << "  NonCopyable move constructor" << endl;
    }
    
    ~NonCopyable() {
        cout << "  NonCopyable destructor: " << data << endl;
    }
    
    string getData() const { return data; }
};

NonCopyable createNonCopyable() {
    return NonCopyable("ElidedNonCopyable");  // Works! Elision guaranteed
}

void example6_nonCopyable() {
    cout << "\n6. Non-Copyable Class (Guaranteed Elision):" << endl;
    NonCopyable nc = createNonCopyable();
    cout << "Data: " << nc.getData() << endl;
}

// ============= Example 7: Aggregate Initialization =============

struct Point {
    int x, y;
    string label;
    
    Point(int x_, int y_, const string& l) : x(x_), y(y_), label(l) {
        cout << "  Point(" << x << ", " << y << ", \"" << label << "\") constructed" << endl;
    }
    
    ~Point() {
        cout << "  Point(" << x << ", " << y << ") destroyed" << endl;
    }
};

Point createPoint() {
    return Point(10, 20, "Origin");  // Elided
}

void example7_aggregate() {
    cout << "\n7. Aggregate Initialization:" << endl;
    Point p = createPoint();
}

// ============= Example 8: Exception Handling =============

Tracer mayThrow(bool shouldThrow) {
    if (shouldThrow) {
        throw runtime_error("Error occurred");
    }
    return Tracer("NoException");
}

void example8_exception() {
    cout << "\n8. Exception Handling:" << endl;
    try {
        Tracer t = mayThrow(false);
        cout << "Success: " << t.getName() << endl;
    } catch (const exception& e) {
        cout << "Caught: " << e.what() << endl;
    }
}

// ============= Comparison: With vs Without Elision =============

void demonstrateElisionBenefit() {
    cout << "\n9. Demonstrating Elision Benefit:" << endl;
    cout << "===================================" << endl;
    cout << "\nWithout copy elision, you would see:" << endl;
    cout << "  - Constructor in function" << endl;
    cout << "  - Copy/Move to return value" << endl;
    cout << "  - Destructor of temporary" << endl;
    cout << "  - Copy/Move to caller's variable" << endl;
    cout << "  - Destructor of return value" << endl;
    cout << "  - Final destructor" << endl;
    cout << "\nWith copy elision (current):" << endl;
    cout << "  - Constructor directly in caller's space" << endl;
    cout << "  - Final destructor" << endl;
    cout << "\nResult: 3-5x fewer operations!" << endl;
}

int main() {
    cout << "=== COPY ELISION - COMPILER OPTIMIZATION ===" << endl;
    
    example1_directInit();
    example2_temporary();
    example3_initializerList();
    example4_returnChain();
    example5_conditional();
    example6_nonCopyable();
    example7_aggregate();
    example8_exception();
    demonstrateElisionBenefit();
    
    cout << "\n\n=== KEY TAKEAWAYS ===" << endl;
    cout << "1. Copy elision eliminates unnecessary copies" << endl;
    cout << "2. C++17: Guaranteed for prvalue temporaries" << endl;
    cout << "3. Works even if copy/move constructors are deleted" << endl;
    cout << "4. RVO/NRVO are forms of copy elision" << endl;
    cout << "5. Zero-cost abstraction in modern C++" << endl;
    
    cout << "\nWhen Elision is Guaranteed (C++17+):" << endl;
    cout << "✓ Return temporary: return T(...)" << endl;
    cout << "✓ Direct initialization: T t = T(...)" << endl;
    cout << "✓ Throw temporary: throw T(...)" << endl;
    cout << "✓ Catch by value: catch(T t)" << endl;
    
    cout << "\nWhen Elision May Occur:" << endl;
    cout << "• Named return value (NRVO)" << endl;
    cout << "• Pass temporary by value" << endl;
    cout << "• Single return path helps" << endl;
    
    cout << "\nTo Test Elision:" << endl;
    cout << "• Compile with: g++ -std=c++17 -O2" << endl;
    cout << "• Disable: -fno-elide-constructors" << endl;
    cout << "• Compare constructor/destructor counts" << endl;
    
    cout << "\nPerformance Impact:" << endl;
    cout << "• 3-5x fewer operations typical" << endl;
    cout << "• No memory allocation overhead" << endl;
    cout << "• Enables passing large objects by value" << endl;
    cout << "• Makes value semantics efficient" << endl;
    
    return 0;
}

