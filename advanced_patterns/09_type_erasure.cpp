/*
 * TYPE ERASURE
 * 
 * DEFINITION:
 * Technique to hide the type of an object behind a generic interface
 * while maintaining value semantics. Allows storing different types
 * in the same container without using inheritance.
 * 
 * PURPOSE:
 * - Store different types in uniform way
 * - No inheritance required from clients
 * - Value semantics (copyable, movable)
 * - Runtime polymorphism without virtual functions in client code
 * 
 * BENEFITS:
 * + Works with any type (even built-in)
 * + No inheritance requirement
 * + Value semantics
 * + Type-safe
 * + Flexible
 * 
 * EXAMPLES IN STL:
 * - std::function
 * - std::any
 * - std::shared_ptr
 */

#include <iostream>
#include <string>
#include <memory>
#include <vector>

using namespace std;

// ============= Example 1: Simple Drawable (Type Erasure) =============

// No base class needed for actual types!
class Circle {
public:
    void draw() const {
        cout << "Drawing Circle" << endl;
    }
};

class Square {
public:
    void draw() const {
        cout << "Drawing Square" << endl;
    }
};

class Triangle {
public:
    void draw() const {
        cout << "Drawing Triangle" << endl;
    }
};

// Type-erased wrapper
class Drawable {
private:
    // Concept (interface)
    struct DrawableConcept {
        virtual ~DrawableConcept() = default;
        virtual void draw() const = 0;
        virtual unique_ptr<DrawableConcept> clone() const = 0;
    };
    
    // Model (wraps concrete types)
    template <typename T>
    struct DrawableModel : DrawableConcept {
        T object;
        
        DrawableModel(T obj) : object(move(obj)) {}
        
        void draw() const override {
            object.draw();  // Calls T::draw()
        }
        
        unique_ptr<DrawableConcept> clone() const override {
            return make_unique<DrawableModel>(object);
        }
    };
    
    unique_ptr<DrawableConcept> pImpl;
    
public:
    // Constructor accepts any type with draw()
    template <typename T>
    Drawable(T obj) : pImpl(make_unique<DrawableModel<T>>(move(obj))) {}
    
    // Copy constructor
    Drawable(const Drawable& other) : pImpl(other.pImpl->clone()) {}
    
    // Copy assignment
    Drawable& operator=(const Drawable& other) {
        if (this != &other) {
            pImpl = other.pImpl->clone();
        }
        return *this;
    }
    
    // Move operations
    Drawable(Drawable&&) = default;
    Drawable& operator=(Drawable&&) = default;
    
    void draw() const {
        pImpl->draw();
    }
};

// ============= Example 2: Callable (like std::function) =============

template <typename Signature>
class Function;

template <typename R, typename... Args>
class Function<R(Args...)> {
private:
    struct Concept {
        virtual ~Concept() = default;
        virtual R invoke(Args... args) = 0;
        virtual unique_ptr<Concept> clone() const = 0;
    };
    
    template <typename F>
    struct Model : Concept {
        F func;
        
        Model(F f) : func(move(f)) {}
        
        R invoke(Args... args) override {
            return func(forward<Args>(args)...);
        }
        
        unique_ptr<Concept> clone() const override {
            return make_unique<Model>(func);
        }
    };
    
    unique_ptr<Concept> pImpl;
    
public:
    template <typename F>
    Function(F f) : pImpl(make_unique<Model<F>>(move(f))) {}
    
    Function(const Function& other) : pImpl(other.pImpl->clone()) {}
    
    Function& operator=(const Function& other) {
        if (this != &other) {
            pImpl = other.pImpl->clone();
        }
        return *this;
    }
    
    Function(Function&&) = default;
    Function& operator=(Function&&) = default;
    
    R operator()(Args... args) {
        return pImpl->invoke(forward<Args>(args)...);
    }
};

// ============= Example 3: Printable =============

class Printable {
private:
    struct Concept {
        virtual ~Concept() = default;
        virtual void print() const = 0;
        virtual unique_ptr<Concept> clone() const = 0;
    };
    
    template <typename T>
    struct Model : Concept {
        T object;
        
        Model(T obj) : object(move(obj)) {}
        
        void print() const override {
            cout << object << endl;  // Works with any << operator
        }
        
        unique_ptr<Concept> clone() const override {
            return make_unique<Model>(object);
        }
    };
    
    unique_ptr<Concept> pImpl;
    
public:
    template <typename T>
    Printable(T obj) : pImpl(make_unique<Model<T>>(move(obj))) {}
    
    Printable(const Printable& other) : pImpl(other.pImpl->clone()) {}
    
    Printable& operator=(const Printable& other) {
        if (this != &other) {
            pImpl = other.pImpl->clone();
        }
        return *this;
    }
    
    Printable(Printable&&) = default;
    Printable& operator=(Printable&&) = default;
    
    void print() const {
        pImpl->print();
    }
};

// ============= Example 4: Any (like std::any) =============

class Any {
private:
    struct Concept {
        virtual ~Concept() = default;
        virtual unique_ptr<Concept> clone() const = 0;
    };
    
    template <typename T>
    struct Model : Concept {
        T value;
        
        Model(T val) : value(move(val)) {}
        
        unique_ptr<Concept> clone() const override {
            return make_unique<Model>(value);
        }
    };
    
    unique_ptr<Concept> pImpl;
    
public:
    template <typename T>
    Any(T value) : pImpl(make_unique<Model<T>>(move(value))) {}
    
    Any(const Any& other) : pImpl(other.pImpl ? other.pImpl->clone() : nullptr) {}
    
    Any& operator=(const Any& other) {
        if (this != &other) {
            pImpl = other.pImpl ? other.pImpl->clone() : nullptr;
        }
        return *this;
    }
    
    Any(Any&&) = default;
    Any& operator=(Any&&) = default;
    
    template <typename T>
    T* cast() {
        if (auto model = dynamic_cast<Model<T>*>(pImpl.get())) {
            return &model->value;
        }
        return nullptr;
    }
};

int main() {
    cout << "=== TYPE ERASURE ===" << endl;
    
    // Example 1: Drawable
    cout << "\n1. DRAWABLE TYPE ERASURE:" << endl;
    cout << "=========================" << endl;
    
    vector<Drawable> shapes;
    shapes.push_back(Circle{});
    shapes.push_back(Square{});
    shapes.push_back(Triangle{});
    
    cout << "Drawing all shapes:" << endl;
    for (const auto& shape : shapes) {
        shape.draw();
    }
    
    // Example 2: Function
    cout << "\n\n2. FUNCTION TYPE ERASURE:" << endl;
    cout << "=========================" << endl;
    
    // Lambda
    Function<int(int, int)> add = [](int a, int b) { return a + b; };
    cout << "add(5, 3) = " << add(5, 3) << endl;
    
    // Function pointer
    auto multiply = [](int a, int b) { return a * b; };
    Function<int(int, int)> mul = multiply;
    cout << "multiply(5, 3) = " << mul(5, 3) << endl;
    
    // Example 3: Printable
    cout << "\n\n3. PRINTABLE TYPE ERASURE:" << endl;
    cout << "==========================" << endl;
    
    vector<Printable> items;
    items.push_back(42);
    items.push_back(string("Hello, Type Erasure!"));
    items.push_back(3.14159);
    
    cout << "Printing all items:" << endl;
    for (const auto& item : items) {
        item.print();
    }
    
    // Example 4: Any
    cout << "\n\n4. ANY TYPE ERASURE:" << endl;
    cout << "====================" << endl;
    
    Any value1 = 42;
    Any value2 = string("Hello");
    Any value3 = 3.14;
    
    if (int* p = value1.cast<int>()) {
        cout << "value1 is int: " << *p << endl;
    }
    
    if (string* p = value2.cast<string>()) {
        cout << "value2 is string: " << *p << endl;
    }
    
    if (double* p = value3.cast<double>()) {
        cout << "value3 is double: " << *p << endl;
    }
    
    cout << "\n\n=== KEY TAKEAWAYS ===" << endl;
    cout << "1. Type erasure hides concrete types" << endl;
    cout << "2. Uses concept/model pattern internally" << endl;
    cout << "3. No inheritance required from clients" << endl;
    cout << "4. Maintains value semantics" << endl;
    cout << "5. Enables heterogeneous containers" << endl;
    
    cout << "\nPattern Structure:" << endl;
    cout << "• Wrapper class (public interface)" << endl;
    cout << "• Concept interface (abstract)" << endl;
    cout << "• Model template (wraps concrete types)" << endl;
    cout << "• Uses virtual functions internally" << endl;
    
    cout << "\nExamples in C++ Standard Library:" << endl;
    cout << "• std::function - callable type erasure" << endl;
    cout << "• std::any - any type storage" << endl;
    cout << "• std::shared_ptr - reference counting" << endl;
    cout << "• std::packaged_task - task type erasure" << endl;
    
    cout << "\nBenefits:" << endl;
    cout << "✓ Works with any type (no base class)" << endl;
    cout << "✓ Value semantics (copy/move)" << endl;
    cout << "✓ Type-safe" << endl;
    cout << "✓ Flexible and reusable" << endl;
    cout << "✓ Decouples interface from implementation" << endl;
    
    cout << "\nWhen to use Type Erasure:" << endl;
    cout << "• Need polymorphism without inheritance" << endl;
    cout << "• Want to store different types uniformly" << endl;
    cout << "• Can't modify existing types" << endl;
    cout << "• Need value semantics with polymorphism" << endl;
    
    return 0;
}

