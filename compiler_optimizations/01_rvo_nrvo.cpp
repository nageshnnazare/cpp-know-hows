/*
 * RVO & NRVO - Return Value Optimization & Named Return Value Optimization
 * 
 * DEFINITION:
 * Compiler optimization that eliminates temporary objects and copy operations
 * when returning objects from functions.
 * 
 * RVO: Return Value Optimization (unnamed temporary)
 * NRVO: Named Return Value Optimization (named local variable)
 * 
 * HISTORY:
 * - Allowed since C++98
 * - Mandatory in C++17 for prvalue (RVO)
 * - NRVO still optional but widely implemented
 * 
 * PURPOSE:
 * - Eliminate unnecessary copies
 * - Improve performance
 * - Reduce memory allocations
 * 
 * BENEFITS:
 * + Zero-cost abstraction
 * + No performance penalty for returning objects
 * + Cleaner code (can return by value)
 */

#include <iostream>
#include <string>
#include <vector>

using namespace std;

// ============= Heavy Object for Testing =============

class Heavy {
private:
    vector<int> data;
    string name;
    
public:
    Heavy(const string& n, size_t size) : name(n), data(size, 42) {
        cout << "  [Constructor] Heavy(\"" << name << "\", " << size << ")" << endl;
    }
    
    Heavy(const Heavy& other) : name(other.name + "_copy"), data(other.data) {
        cout << "  [Copy Constructor] Heavy(\"" << name << "\")" << endl;
    }
    
    Heavy(Heavy&& other) noexcept : name(move(other.name)), data(move(other.data)) {
        cout << "  [Move Constructor] Heavy(\"" << name << "\")" << endl;
    }
    
    Heavy& operator=(const Heavy& other) {
        name = other.name + "_assigned";
        data = other.data;
        cout << "  [Copy Assignment] Heavy(\"" << name << "\")" << endl;
        return *this;
    }
    
    Heavy& operator=(Heavy&& other) noexcept {
        name = move(other.name);
        data = move(other.data);
        cout << "  [Move Assignment] Heavy(\"" << name << "\")" << endl;
        return *this;
    }
    
    ~Heavy() {
        cout << "  [Destructor] ~Heavy(\"" << name << "\")" << endl;
    }
    
    string getName() const { return name; }
    size_t getSize() const { return data.size(); }
};

// ============= RVO: Return Value Optimization =============

Heavy createWithRVO() {
    cout << "\nInside createWithRVO():" << endl;
    return Heavy("RVO_Object", 1000);  // Temporary - RVO applies
    // No copy! Object constructed directly in caller's space
}

// ============= NRVO: Named Return Value Optimization =============

Heavy createWithNRVO() {
    cout << "\nInside createWithNRVO():" << endl;
    Heavy obj("NRVO_Object", 1000);  // Named local variable
    return obj;  // NRVO may apply (compiler-dependent)
    // Modern compilers will eliminate the copy
}

// ============= No Optimization: Multiple Return Paths =============

Heavy createWithMultipleReturns(bool condition) {
    cout << "\nInside createWithMultipleReturns():" << endl;
    if (condition) {
        Heavy obj1("Branch1", 500);
        return obj1;  // NRVO might not apply (multiple returns)
    } else {
        Heavy obj2("Branch2", 500);
        return obj2;  // NRVO might not apply (multiple returns)
    }
}

// ============= Guaranteed Copy Elision (C++17) =============

Heavy createGuaranteedElision() {
    cout << "\nInside createGuaranteedElision() [C++17]:" << endl;
    return Heavy("Guaranteed", 1000);  // Mandatory in C++17
}

// ============= Move vs Copy =============

Heavy createWithMove() {
    cout << "\nInside createWithMove():" << endl;
    Heavy obj("Move_Object", 1000);
    return move(obj);  // DON'T DO THIS! Prevents NRVO
    // Compiler warning: moving prevents optimization
}

// ============= Practical Example: Factory Pattern =============

class Widget {
private:
    string type;
    vector<double> data;
    
public:
    Widget(const string& t, size_t size) : type(t), data(size, 3.14) {
        cout << "  Widget(\"" << type << "\") constructed" << endl;
    }
    
    Widget(const Widget& other) : type(other.type + "_copy"), data(other.data) {
        cout << "  Widget(\"" << type << "\") copied" << endl;
    }
    
    Widget(Widget&& other) noexcept : type(move(other.type)), data(move(other.data)) {
        cout << "  Widget(\"" << type << "\") moved" << endl;
    }
    
    ~Widget() {
        cout << "  Widget(\"" << type << "\") destroyed" << endl;
    }
    
    string getType() const { return type; }
};

// Factory with RVO
Widget createWidget(const string& type) {
    if (type == "fast") {
        return Widget("FastWidget", 100);  // RVO
    } else if (type == "large") {
        return Widget("LargeWidget", 10000);  // RVO
    }
    return Widget("DefaultWidget", 1000);  // RVO
}

// ============= String Optimization Example =============

string createLongString() {
    // RVO applies to standard library types too
    string result;
    for (int i = 0; i < 1000; ++i) {
        result += "data ";
    }
    return result;  // NRVO likely to apply
}

// ============= Vector Optimization Example =============

vector<int> createLargeVector() {
    vector<int> vec;
    vec.reserve(10000);
    for (int i = 0; i < 10000; ++i) {
        vec.push_back(i);
    }
    return vec;  // NRVO likely to apply
}

int main() {
    cout << "=== RVO & NRVO - COMPILER OPTIMIZATIONS ===" << endl;
    
    // Example 1: RVO
    cout << "\n1. RVO (Return Value Optimization):" << endl;
    cout << "===================================" << endl;
    {
        Heavy obj1 = createWithRVO();
        cout << "Result: " << obj1.getName() << ", size: " << obj1.getSize() << endl;
    }
    
    // Example 2: NRVO
    cout << "\n\n2. NRVO (Named Return Value Optimization):" << endl;
    cout << "==========================================" << endl;
    {
        Heavy obj2 = createWithNRVO();
        cout << "Result: " << obj2.getName() << ", size: " << obj2.getSize() << endl;
    }
    
    // Example 3: Multiple Returns
    cout << "\n\n3. Multiple Return Paths (NRVO may not apply):" << endl;
    cout << "===============================================" << endl;
    {
        Heavy obj3 = createWithMultipleReturns(true);
        cout << "Result: " << obj3.getName() << endl;
    }
    
    // Example 4: Guaranteed Elision (C++17)
    cout << "\n\n4. Guaranteed Copy Elision (C++17):" << endl;
    cout << "===================================" << endl;
    {
        Heavy obj4 = createGuaranteedElision();
        cout << "Result: " << obj4.getName() << endl;
    }
    
    // Example 5: Factory Pattern with RVO
    cout << "\n\n5. Factory Pattern with RVO:" << endl;
    cout << "=============================" << endl;
    {
        Widget w1 = createWidget("fast");
        Widget w2 = createWidget("large");
    }
    
    // Example 6: STL Types
    cout << "\n\n6. STL Types Optimization:" << endl;
    cout << "==========================" << endl;
    {
        auto str = createLongString();
        cout << "String length: " << str.length() << " (no copies!)" << endl;
        
        auto vec = createLargeVector();
        cout << "Vector size: " << vec.size() << " (no copies!)" << endl;
    }
    
    cout << "\n\n=== KEY TAKEAWAYS ===" << endl;
    cout << "1. RVO: Eliminates copies for unnamed temporaries" << endl;
    cout << "2. NRVO: Eliminates copies for named local variables" << endl;
    cout << "3. C++17: Mandatory for prvalue returns (RVO)" << endl;
    cout << "4. Return by value is efficient!" << endl;
    cout << "5. Don't use std::move on local returns (breaks RVO)" << endl;
    
    cout << "\nHow to Enable RVO/NRVO:" << endl;
    cout << "• Return by value (not pointer/reference)" << endl;
    cout << "• Single return point helps NRVO" << endl;
    cout << "• Avoid std::move on return" << endl;
    cout << "• Let compiler do its job" << endl;
    
    cout << "\nCompiler Flags:" << endl;
    cout << "• GCC/Clang: Enabled by default at -O1+" << endl;
    cout << "• Disable: -fno-elide-constructors (for testing)" << endl;
    cout << "• C++17: Guaranteed for prvalue" << endl;
    
    cout << "\nVerify Optimization:" << endl;
    cout << "• Watch constructor/destructor calls" << endl;
    cout << "• Use -fno-elide-constructors to see difference" << endl;
    cout << "• Profile code with and without optimization" << endl;
    
    cout << "\nWhen RVO/NRVO Doesn't Apply:" << endl;
    cout << "✗ Multiple return paths with different variables" << endl;
    cout << "✗ Returning function parameter" << endl;
    cout << "✗ Using std::move on local return" << endl;
    cout << "✗ Returning member variable" << endl;
    
    cout << "\nBest Practices:" << endl;
    cout << "✓ Return by value for local objects" << endl;
    cout << "✓ Trust the compiler" << endl;
    cout << "✓ Single return point when possible" << endl;
    cout << "✓ Use modern C++17 or later" << endl;
    cout << "✓ Profile before pessimizing" << endl;
    
    return 0;
}

