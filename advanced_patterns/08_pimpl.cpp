/*
 * PIMPL - Pointer to Implementation (Compilation Firewall)
 * Also known as: Opaque Pointer, d-pointer, Cheshire Cat
 * 
 * DEFINITION:
 * Hide implementation details by moving them to a separate class
 * and accessing through a pointer.
 * 
 * PURPOSE:
 * - Hide implementation details
 * - Reduce compilation dependencies
 * - Maintain binary compatibility
 * - Faster compilation times
 * 
 * BENEFITS:
 * + Binary compatibility (ABI stable)
 * + Faster compilation (fewer includes in header)
 * + Information hiding
 * + Can change implementation without recompiling clients
 * 
 * DRAWBACKS:
 * - Extra indirection (slight performance cost)
 * - More code to write
 * - Dynamic allocation overhead
 */

#include <iostream>
#include <string>
#include <memory>

using namespace std;

// ============= WITHOUT PIMPL (Traditional) =============

// widget_bad.h - Header file exposed to clients
#include <vector>
#include <map>

class WidgetBad {
private:
    // All implementation details exposed in header!
    string name;
    vector<int> data;
    map<string, string> config;
    int internalState;
    
    void internalMethod();
    void helperMethod();
    
public:
    WidgetBad(const string& n);
    ~WidgetBad();
    void doSomething();
    string getName() const;
};

// widget_bad.cpp
WidgetBad::WidgetBad(const string& n) : name(n), internalState(0) {}
WidgetBad::~WidgetBad() = default;

void WidgetBad::internalMethod() {
    cout << "Internal method called" << endl;
}

void WidgetBad::helperMethod() {
    cout << "Helper method called" << endl;
}

void WidgetBad::doSomething() {
    cout << "WidgetBad doing something" << endl;
}

string WidgetBad::getName() const {
    return name;
}

// Problem: Changing any private member requires recompiling ALL clients!

// ============= WITH PIMPL =============

// widget.h - Clean header exposed to clients
class Widget {
public:
    Widget(const string& name);
    ~Widget();
    
    // Rule of 5 (with PIMPL, need to define these)
    Widget(const Widget& other);
    Widget& operator=(const Widget& other);
    Widget(Widget&& other) noexcept;
    Widget& operator=(Widget&& other) noexcept;
    
    void doSomething();
    string getName() const;
    void setConfig(const string& key, const string& value);
    
private:
    class Impl;  // Forward declaration
    unique_ptr<Impl> pImpl;  // Pointer to implementation
};

// widget.cpp - Implementation hidden
class Widget::Impl {
public:
    string name;
    vector<int> data;
    map<string, string> config;
    int internalState;
    
    Impl(const string& n) : name(n), internalState(0) {
        cout << "Widget::Impl constructed" << endl;
    }
    
    ~Impl() {
        cout << "Widget::Impl destroyed" << endl;
    }
    
    void internalMethod() {
        cout << "Internal method (hidden from clients)" << endl;
    }
    
    void helperMethod() {
        cout << "Helper method (hidden from clients)" << endl;
    }
};

// Widget implementation
Widget::Widget(const string& name) 
    : pImpl(make_unique<Impl>(name)) {
    cout << "Widget constructed" << endl;
}

Widget::~Widget() {
    cout << "Widget destroyed" << endl;
}

// Copy constructor
Widget::Widget(const Widget& other)
    : pImpl(make_unique<Impl>(*other.pImpl)) {
}

// Copy assignment
Widget& Widget::operator=(const Widget& other) {
    if (this != &other) {
        *pImpl = *other.pImpl;
    }
    return *this;
}

// Move constructor
Widget::Widget(Widget&& other) noexcept = default;

// Move assignment
Widget& Widget::operator=(Widget&& other) noexcept = default;

void Widget::doSomething() {
    pImpl->internalMethod();
    pImpl->helperMethod();
    cout << "Widget doing something" << endl;
}

string Widget::getName() const {
    return pImpl->name;
}

void Widget::setConfig(const string& key, const string& value) {
    pImpl->config[key] = value;
    cout << "Config set: " << key << " = " << value << endl;
}

// ============= Example 2: Person Class =============

class Person {
public:
    Person(const string& name, int age);
    ~Person();
    
    Person(const Person&) = delete;
    Person& operator=(const Person&) = delete;
    
    void display() const;
    void setEmail(const string& email);
    void addPhone(const string& phone);
    
private:
    struct Impl;
    unique_ptr<Impl> pImpl;
};

struct Person::Impl {
    string name;
    int age;
    string email;
    vector<string> phoneNumbers;
    string address;  // Can add without breaking ABI!
    
    Impl(const string& n, int a) : name(n), age(a) {}
};

Person::Person(const string& name, int age)
    : pImpl(make_unique<Impl>(name, age)) {}

Person::~Person() = default;

void Person::display() const {
    cout << "Name: " << pImpl->name << ", Age: " << pImpl->age << endl;
    if (!pImpl->email.empty()) {
        cout << "Email: " << pImpl->email << endl;
    }
    for (const auto& phone : pImpl->phoneNumbers) {
        cout << "Phone: " << phone << endl;
    }
}

void Person::setEmail(const string& email) {
    pImpl->email = email;
}

void Person::addPhone(const string& phone) {
    pImpl->phoneNumbers.push_back(phone);
}

// ============= Example 3: Database Connection =============

class DatabaseConnection {
public:
    DatabaseConnection(const string& connectionString);
    ~DatabaseConnection();
    
    DatabaseConnection(const DatabaseConnection&) = delete;
    DatabaseConnection& operator=(const DatabaseConnection&) = delete;
    
    void connect();
    void disconnect();
    void executeQuery(const string& query);
    bool isConnected() const;
    
private:
    class Impl;
    unique_ptr<Impl> pImpl;
};

class DatabaseConnection::Impl {
public:
    string connectionString;
    bool connected;
    void* nativeHandle;  // Could be actual DB handle
    
    Impl(const string& connStr) 
        : connectionString(connStr), connected(false), nativeHandle(nullptr) {}
    
    void connect() {
        cout << "Connecting to: " << connectionString << endl;
        connected = true;
    }
    
    void disconnect() {
        if (connected) {
            cout << "Disconnecting..." << endl;
            connected = false;
        }
    }
};

DatabaseConnection::DatabaseConnection(const string& connectionString)
    : pImpl(make_unique<Impl>(connectionString)) {}

DatabaseConnection::~DatabaseConnection() = default;

void DatabaseConnection::connect() {
    pImpl->connect();
}

void DatabaseConnection::disconnect() {
    pImpl->disconnect();
}

void DatabaseConnection::executeQuery(const string& query) {
    if (pImpl->connected) {
        cout << "Executing: " << query << endl;
    }
}

bool DatabaseConnection::isConnected() const {
    return pImpl->connected;
}

int main() {
    cout << "=== PIMPL - POINTER TO IMPLEMENTATION ===" << endl;
    
    // Example 1: Widget
    cout << "\n1. WIDGET WITH PIMPL:" << endl;
    cout << "=====================" << endl;
    {
        Widget widget("MyWidget");
        widget.setConfig("color", "blue");
        widget.setConfig("size", "large");
        widget.doSomething();
        cout << "Widget name: " << widget.getName() << endl;
    }
    
    // Example 2: Person
    cout << "\n\n2. PERSON WITH PIMPL:" << endl;
    cout << "=====================" << endl;
    {
        Person person("Alice", 30);
        person.setEmail("alice@example.com");
        person.addPhone("555-1234");
        person.addPhone("555-5678");
        person.display();
    }
    
    // Example 3: Database Connection
    cout << "\n\n3. DATABASE CONNECTION WITH PIMPL:" << endl;
    cout << "==================================" << endl;
    {
        DatabaseConnection db("localhost:3306/mydb");
        db.connect();
        cout << "Connected: " << (db.isConnected() ? "Yes" : "No") << endl;
        db.executeQuery("SELECT * FROM users");
        db.disconnect();
    }
    
    cout << "\n\n=== KEY TAKEAWAYS ===" << endl;
    cout << "1. PIMPL hides implementation details" << endl;
    cout << "2. Pointer to implementation in header" << endl;
    cout << "3. Implementation class defined in .cpp" << endl;
    cout << "4. Enables binary compatibility (ABI stable)" << endl;
    cout << "5. Reduces compilation dependencies" << endl;
    
    cout << "\nBenefits:" << endl;
    cout << "✓ Faster compilation (fewer #includes)" << endl;
    cout << "✓ Binary compatibility" << endl;
    cout << "✓ Information hiding" << endl;
    cout << "✓ Can change implementation freely" << endl;
    cout << "✓ Reduced header dependencies" << endl;
    
    cout << "\nDrawbacks:" << endl;
    cout << "✗ Extra indirection (small perf cost)" << endl;
    cout << "✗ More code to write" << endl;
    cout << "✗ Need to define Rule of 5" << endl;
    cout << "✗ Dynamic allocation overhead" << endl;
    
    cout << "\nWhen to use PIMPL:" << endl;
    cout << "• Library interfaces (ABI stability)" << endl;
    cout << "• Large classes with many dependencies" << endl;
    cout << "• When compilation time is critical" << endl;
    cout << "• Need to hide proprietary code" << endl;
    
    cout << "\nReal-world users of PIMPL:" << endl;
    cout << "• Qt (d-pointer pattern)" << endl;
    cout << "• Large commercial libraries" << endl;
    cout << "• Plugin systems" << endl;
    cout << "• Shared libraries" << endl;
    
    return 0;
}

