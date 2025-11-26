/*
 * DEVIRTUALIZATION
 * 
 * DEFINITION:
 * Optimization where the compiler replaces virtual function calls
 * with direct function calls when the type is known at compile time.
 * 
 * PURPOSE:
 * - Eliminate virtual function overhead
 * - Enable inlining of virtual functions
 * - Improve branch prediction
 * 
 * REQUIREMENTS:
 * - Type must be known at compile time
 * - Final classes/methods
 * - Link-time optimization (LTO)
 * 
 * BENEFITS:
 * + 2-5x faster than virtual calls
 * + Enables inlining
 * + Better CPU pipeline usage
 */

#include <iostream>
#include <chrono>
#include <memory>
#include <vector>

using namespace std;
using namespace chrono;

// ============= Example 1: Virtual vs Non-Virtual =============

class VirtualBase {
public:
    virtual ~VirtualBase() = default;
    virtual int compute(int x) {
        return x * 2;
    }
};

class VirtualDerived : public VirtualBase {
public:
    int compute(int x) override {
        return x * 3;
    }
};

class NonVirtualBase {
public:
    int compute(int x) {
        return x * 2;
    }
};

// ============= Example 2: Final Keyword =============

class Base {
public:
    virtual ~Base() = default;
    virtual int getValue() { return 42; }
};

class Derived final : public Base {  // final class
public:
    int getValue() override final {  // final method
        return 100;
    }
};

// Compiler knows Derived is final - can devirtualize

// ============= Example 3: Known Type at Compile Time =============

void processKnownType() {
    // Type is known - can be devirtualized
    Derived obj;
    int val = obj.getValue();  // Direct call, not virtual!
    cout << "Value: " << val << endl;
}

void processUnknownType(Base* obj) {
    // Type unknown - must use virtual call
    int val = obj->getValue();  // Virtual call through vtable
    cout << "Value: " << val << endl;
}

// ============= Example 4: Template Devirtualization =============

template <typename T>
int processObject(T& obj) {
    // T is known at compile time - devirtualized!
    return obj.getValue();
}

// ============= Example 5: Performance Comparison =============

class Shape {
public:
    virtual ~Shape() = default;
    virtual double area() const = 0;
};

class Circle : public Shape {
private:
    double radius;
public:
    Circle(double r) : radius(r) {}
    double area() const override {
        return 3.14159 * radius * radius;
    }
};

class CircleNonVirtual {
private:
    double radius;
public:
    CircleNonVirtual(double r) : radius(r) {}
    double area() const {  // Non-virtual
        return 3.14159 * radius * radius;
    }
};

void testVirtualCall() {
    cout << "\n=== Virtual Call Performance ===" << endl;
    
    const int iterations = 10000000;
    vector<unique_ptr<Shape>> shapes;
    for (int i = 0; i < 100; ++i) {
        shapes.push_back(make_unique<Circle>(5.0));
    }
    
    // Virtual calls
    auto start1 = high_resolution_clock::now();
    volatile double sum1 = 0;
    for (int i = 0; i < iterations; ++i) {
        sum1 += shapes[i % 100]->area();  // Virtual call
    }
    auto end1 = high_resolution_clock::now();
    auto duration1 = duration_cast<microseconds>(end1 - start1).count();
    
    // Non-virtual (known type)
    vector<CircleNonVirtual> circles;
    for (int i = 0; i < 100; ++i) {
        circles.emplace_back(5.0);
    }
    
    auto start2 = high_resolution_clock::now();
    volatile double sum2 = 0;
    for (int i = 0; i < iterations; ++i) {
        sum2 += circles[i % 100].area();  // Direct call, can inline
    }
    auto end2 = high_resolution_clock::now();
    auto duration2 = duration_cast<microseconds>(end2 - start2).count();
    
    cout << "Virtual calls:     " << duration1 << " μs" << endl;
    cout << "Non-virtual calls: " << duration2 << " μs" << endl;
    cout << "Speedup: " << (double)duration1 / duration2 << "x" << endl;
}

// ============= Example 6: CRTP Alternative =============

// Compile-time polymorphism (no virtual functions)
template <typename Derived>
class ShapeCRTP {
public:
    double area() const {
        return static_cast<const Derived*>(this)->areaImpl();
    }
};

class CircleCRTP : public ShapeCRTP<CircleCRTP> {
private:
    double radius;
public:
    CircleCRTP(double r) : radius(r) {}
    double areaImpl() const {
        return 3.14159 * radius * radius;
    }
};

// ============= Example 7: Link-Time Optimization (LTO) =============

// With LTO, compiler can see across translation units
// and devirtualize even when type seems unknown

class Interface {
public:
    virtual ~Interface() = default;
    virtual void process() = 0;
};

class Implementation : public Interface {
public:
    void process() override {
        // Do work
    }
};

// ============= Example 8: Avoiding Virtual When Possible =============

// Bad: Unnecessary virtual
class LoggerBad {
public:
    virtual ~LoggerBad() = default;
    virtual void log(const string& msg) {  // Why virtual?
        cout << msg << endl;
    }
};

// Good: Only virtual if needed for polymorphism
class LoggerGood {
public:
    void log(const string& msg) {  // Non-virtual
        cout << msg << endl;
    }
};

int main() {
    cout << "=== DEVIRTUALIZATION ===" << endl;
    
    // Example 1: Known vs Unknown Type
    cout << "\n1. Known vs Unknown Type:" << endl;
    cout << "=========================" << endl;
    
    Derived known;
    processKnownType();  // Can devirtualize
    
    Base* unknown = new Derived();
    processUnknownType(unknown);  // Must use virtual call
    delete unknown;
    
    // Example 2: Template Devirtualization
    cout << "\n2. Template Devirtualization:" << endl;
    cout << "==============================" << endl;
    
    Derived obj;
    int result = processObject(obj);  // Devirtualized!
    cout << "Result: " << result << endl;
    
    // Example 3: Performance Test
    testVirtualCall();
    
    // Example 4: CRTP Alternative
    cout << "\n3. CRTP (Zero Overhead):" << endl;
    cout << "========================" << endl;
    
    CircleCRTP crtp_circle(5.0);
    cout << "CRTP Circle area: " << crtp_circle.area() << endl;
    cout << "No virtual functions - compile-time polymorphism!" << endl;
    
    cout << "\n\n=== KEY TAKEAWAYS ===" << endl;
    cout << "1. Devirtualization eliminates virtual call overhead" << endl;
    cout << "2. Requires type to be known at compile time" << endl;
    cout << "3. 'final' keyword helps compiler devirtualize" << endl;
    cout << "4. Can provide 2-5x speedup" << endl;
    cout << "5. Enables inlining of virtual functions" << endl;
    
    cout << "\nWhen Devirtualization Occurs:" << endl;
    cout << "✓ Object type known at compile time" << endl;
    cout << "✓ Final classes/methods" << endl;
    cout << "✓ Link-time optimization (LTO)" << endl;
    cout << "✓ Template instantiation" << endl;
    cout << "✓ Whole program optimization" << endl;
    
    cout << "\nVirtual Function Cost:" << endl;
    cout << "• Indirect call through vtable (~3-5 CPU cycles)" << endl;
    cout << "• Prevents inlining" << endl;
    cout << "• Branch misprediction possible" << endl;
    cout << "• 8 bytes vtable pointer per object" << endl;
    
    cout << "\nDevirtualization Techniques:" << endl;
    cout << "• Use 'final' keyword" << endl;
    cout << "• Avoid virtual when not needed" << endl;
    cout << "• Use templates (CRTP)" << endl;
    cout << "• Enable LTO (-flto)" << endl;
    cout << "• Profile-guided optimization (PGO)" << endl;
    
    cout << "\nAlternatives to Virtual Functions:" << endl;
    cout << "• CRTP (Curiously Recurring Template Pattern)" << endl;
    cout << "• std::variant + std::visit" << endl;
    cout << "• Type erasure" << endl;
    cout << "• Function pointers/std::function" << endl;
    cout << "• Strategy pattern with templates" << endl;
    
    cout << "\nCompiler Flags:" << endl;
    cout << "• -O2, -O3: Enable devirtualization" << endl;
    cout << "• -flto: Link-time optimization" << endl;
    cout << "• -fdevirtualize: Explicit devirtualization" << endl;
    cout << "• -fwhole-program: Whole program analysis" << endl;
    
    cout << "\nBest Practices:" << endl;
    cout << "✓ Use 'final' when inheritance not needed" << endl;
    cout << "✓ Avoid virtual for non-polymorphic classes" << endl;
    cout << "✓ Consider CRTP for performance-critical code" << endl;
    cout << "✓ Enable LTO in release builds" << endl;
    cout << "✓ Profile to identify hot virtual calls" << endl;
    
    return 0;
}

