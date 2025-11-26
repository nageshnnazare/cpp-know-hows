/*
 * OPEN/CLOSED PRINCIPLE (OCP)
 * Second of the SOLID Principles
 * 
 * DEFINITION:
 * "Software entities (classes, modules, functions, etc.) should be open for 
 *  extension, but closed for modification."
 * - Bertrand Meyer (1988)
 * 
 * MEANING:
 * - OPEN for extension: Can add new functionality
 * - CLOSED for modification: Don't change existing code
 * - Extend behavior without modifying source code
 * 
 * PURPOSE:
 * Reduce the risk of breaking existing functionality when adding new features.
 * Make the system more maintainable and flexible.
 * 
 * HOW TO ACHIEVE:
 * - Abstraction (interfaces, abstract classes)
 * - Polymorphism (virtual functions, templates)
 * - Design patterns (Strategy, Template Method, etc.)
 * - Dependency Injection
 * 
 * BENEFITS:
 * + Reduced risk when adding features
 * + Existing code remains stable
 * + Easier to maintain and test
 * + Promotes reusability
 * + Supports plugin architectures
 * 
 * VIOLATIONS:
 * - Switch/if-else chains for type checking
 * - Modifying existing classes for new features
 * - Hard-coded dependencies
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <cmath>

using namespace std;

// ============= BAD EXAMPLE - Violates OCP =============

enum class ShapeTypeBad { CIRCLE, SQUARE, TRIANGLE };

class ShapeBad {
public:
    ShapeTypeBad type;
    double dimension1;
    double dimension2;
    
    ShapeBad(ShapeTypeBad t, double d1, double d2 = 0)
        : type(t), dimension1(d1), dimension2(d2) {}
};

class AreaCalculatorBad {
public:
    double calculateArea(const ShapeBad& shape) {
        // Violates OCP: Need to modify this function for every new shape
        switch (shape.type) {
            case ShapeTypeBad::CIRCLE:
                return M_PI * shape.dimension1 * shape.dimension1;
            case ShapeTypeBad::SQUARE:
                return shape.dimension1 * shape.dimension1;
            case ShapeTypeBad::TRIANGLE:
                return 0.5 * shape.dimension1 * shape.dimension2;
            default:
                return 0;
        }
        // Adding a new shape? Must modify this function!
    }
};

// ============= GOOD EXAMPLE - Follows OCP =============

// Abstract base class - closed for modification
class Shape {
public:
    virtual ~Shape() = default;
    virtual double area() const = 0;  // Open for extension
    virtual string getName() const = 0;
};

// Concrete implementations - extend without modifying Shape
class Circle : public Shape {
private:
    double radius;
    
public:
    Circle(double r) : radius(r) {}
    
    double area() const override {
        return M_PI * radius * radius;
    }
    
    string getName() const override {
        return "Circle";
    }
};

class Square : public Shape {
private:
    double side;
    
public:
    Square(double s) : side(s) {}
    
    double area() const override {
        return side * side;
    }
    
    string getName() const override {
        return "Square";
    }
};

class Triangle : public Shape {
private:
    double base, height;
    
public:
    Triangle(double b, double h) : base(b), height(h) {}
    
    double area() const override {
        return 0.5 * base * height;
    }
    
    string getName() const override {
        return "Triangle";
    }
};

// NEW SHAPE - Just add, don't modify existing code
class Rectangle : public Shape {
private:
    double width, height;
    
public:
    Rectangle(double w, double h) : width(w), height(h) {}
    
    double area() const override {
        return width * height;
    }
    
    string getName() const override {
        return "Rectangle";
    }
};

// Calculator doesn't need modification when adding shapes
class AreaCalculator {
public:
    double calculateArea(const Shape& shape) {
        return shape.area();  // Polymorphism!
    }
    
    double calculateTotalArea(const vector<unique_ptr<Shape>>& shapes) {
        double total = 0;
        for (const auto& shape : shapes) {
            total += shape->area();
        }
        return total;
    }
};

// ============= EXAMPLE 2: Discount System =============

// BAD: Violates OCP
class DiscountCalculatorBad {
public:
    double calculateDiscount(const string& customerType, double price) {
        // Must modify this for every new customer type
        if (customerType == "Regular") {
            return price * 0.05;
        } else if (customerType == "Premium") {
            return price * 0.10;
        } else if (customerType == "VIP") {
            return price * 0.20;
        }
        return 0;
        // Adding "Gold" customer? Modify this function!
    }
};

// GOOD: Follows OCP
class DiscountStrategy {
public:
    virtual ~DiscountStrategy() = default;
    virtual double calculateDiscount(double price) const = 0;
    virtual string getType() const = 0;
};

class RegularDiscount : public DiscountStrategy {
public:
    double calculateDiscount(double price) const override {
        return price * 0.05;  // 5% discount
    }
    
    string getType() const override {
        return "Regular";
    }
};

class PremiumDiscount : public DiscountStrategy {
public:
    double calculateDiscount(double price) const override {
        return price * 0.10;  // 10% discount
    }
    
    string getType() const override {
        return "Premium";
    }
};

class VIPDiscount : public DiscountStrategy {
public:
    double calculateDiscount(double price) const override {
        return price * 0.20;  // 20% discount
    }
    
    string getType() const override {
        return "VIP";
    }
};

// NEW: Add without modifying existing code
class GoldDiscount : public DiscountStrategy {
public:
    double calculateDiscount(double price) const override {
        return price * 0.15;  // 15% discount
    }
    
    string getType() const override {
        return "Gold";
    }
};

class Customer {
private:
    string name;
    unique_ptr<DiscountStrategy> discountStrategy;
    
public:
    Customer(const string& n, unique_ptr<DiscountStrategy> strategy)
        : name(n), discountStrategy(move(strategy)) {}
    
    double applyDiscount(double price) const {
        return price - discountStrategy->calculateDiscount(price);
    }
    
    string getName() const { return name; }
    string getCustomerType() const {
        return discountStrategy->getType();
    }
};

// ============= EXAMPLE 3: Notification System =============

// BAD: Violates OCP
class NotificationServiceBad {
public:
    void send(const string& type, const string& message) {
        // Must modify for every new notification type
        if (type == "email") {
            cout << "Sending email: " << message << endl;
        } else if (type == "sms") {
            cout << "Sending SMS: " << message << endl;
        } else if (type == "push") {
            cout << "Sending push notification: " << message << endl;
        }
        // Add Slack notification? Modify this!
    }
};

// GOOD: Follows OCP
class Notification {
public:
    virtual ~Notification() = default;
    virtual void send(const string& message) = 0;
};

class EmailNotification : public Notification {
public:
    void send(const string& message) override {
        cout << "📧 Email: " << message << endl;
    }
};

class SMSNotification : public Notification {
public:
    void send(const string& message) override {
        cout << "📱 SMS: " << message << endl;
    }
};

class PushNotification : public Notification {
public:
    void send(const string& message) override {
        cout << "🔔 Push: " << message << endl;
    }
};

// NEW: Easy to add
class SlackNotification : public Notification {
public:
    void send(const string& message) override {
        cout << "💬 Slack: " << message << endl;
    }
};

class NotificationService {
public:
    void notify(Notification& notification, const string& message) {
        notification.send(message);
    }
    
    void notifyAll(vector<unique_ptr<Notification>>& notifications, 
                   const string& message) {
        for (auto& notification : notifications) {
            notification->send(message);
        }
    }
};

// ============= EXAMPLE 4: Logger System =============

// BAD: Violates OCP
class LoggerBad {
public:
    void log(const string& message, const string& destination) {
        if (destination == "console") {
            cout << "Console: " << message << endl;
        } else if (destination == "file") {
            cout << "File: " << message << endl;
        }
        // Add database logging? Modify this!
    }
};

// GOOD: Follows OCP
class LogWriter {
public:
    virtual ~LogWriter() = default;
    virtual void write(const string& message) = 0;
};

class ConsoleLogWriter : public LogWriter {
public:
    void write(const string& message) override {
        cout << "[Console] " << message << endl;
    }
};

class FileLogWriter : public LogWriter {
public:
    void write(const string& message) override {
        cout << "[File] Writing to log.txt: " << message << endl;
    }
};

class DatabaseLogWriter : public LogWriter {
public:
    void write(const string& message) override {
        cout << "[Database] INSERT INTO logs: " << message << endl;
    }
};

class Logger {
private:
    vector<unique_ptr<LogWriter>> writers;
    
public:
    void addWriter(unique_ptr<LogWriter> writer) {
        writers.push_back(move(writer));
    }
    
    void log(const string& message) {
        for (auto& writer : writers) {
            writer->write(message);
        }
    }
};

int main() {
    cout << "=== OPEN/CLOSED PRINCIPLE (OCP) ===" << endl;
    
    // Example 1: Shape Area Calculator
    cout << "\n1. SHAPE AREA CALCULATOR:" << endl;
    cout << "=========================" << endl;
    
    cout << "\n[BAD - Violates OCP]" << endl;
    ShapeBad badCircle(ShapeTypeBad::CIRCLE, 5);
    ShapeBad badSquare(ShapeTypeBad::SQUARE, 4);
    
    AreaCalculatorBad badCalc;
    cout << "Circle area: " << badCalc.calculateArea(badCircle) << endl;
    cout << "Square area: " << badCalc.calculateArea(badSquare) << endl;
    cout << "Problem: Adding new shape requires modifying AreaCalculatorBad!" << endl;
    
    cout << "\n[GOOD - Follows OCP]" << endl;
    vector<unique_ptr<Shape>> shapes;
    shapes.push_back(make_unique<Circle>(5));
    shapes.push_back(make_unique<Square>(4));
    shapes.push_back(make_unique<Triangle>(3, 6));
    shapes.push_back(make_unique<Rectangle>(4, 7));  // NEW shape, no modification!
    
    AreaCalculator calc;
    for (const auto& shape : shapes) {
        cout << shape->getName() << " area: " << calc.calculateArea(*shape) << endl;
    }
    cout << "Total area: " << calc.calculateTotalArea(shapes) << endl;
    cout << "Benefit: Added Rectangle without modifying existing code!" << endl;
    
    // Example 2: Discount System
    cout << "\n\n2. CUSTOMER DISCOUNT SYSTEM:" << endl;
    cout << "============================" << endl;
    
    vector<Customer> customers;
    customers.emplace_back("Alice", make_unique<RegularDiscount>());
    customers.emplace_back("Bob", make_unique<PremiumDiscount>());
    customers.emplace_back("Charlie", make_unique<VIPDiscount>());
    customers.emplace_back("Diana", make_unique<GoldDiscount>());  // NEW!
    
    double price = 100.0;
    cout << "Original price: $" << price << endl;
    
    for (const auto& customer : customers) {
        double finalPrice = customer.applyDiscount(price);
        cout << customer.getName() << " (" << customer.getCustomerType() 
             << "): $" << finalPrice << endl;
    }
    
    // Example 3: Notification System
    cout << "\n\n3. NOTIFICATION SYSTEM:" << endl;
    cout << "=======================" << endl;
    
    vector<unique_ptr<Notification>> notifications;
    notifications.push_back(make_unique<EmailNotification>());
    notifications.push_back(make_unique<SMSNotification>());
    notifications.push_back(make_unique<PushNotification>());
    notifications.push_back(make_unique<SlackNotification>());  // NEW!
    
    NotificationService notificationService;
    notificationService.notifyAll(notifications, "System update available!");
    
    // Example 4: Logger System
    cout << "\n\n4. MULTI-TARGET LOGGER:" << endl;
    cout << "=======================" << endl;
    
    Logger logger;
    logger.addWriter(make_unique<ConsoleLogWriter>());
    logger.addWriter(make_unique<FileLogWriter>());
    logger.addWriter(make_unique<DatabaseLogWriter>());  // NEW!
    
    logger.log("Application started");
    logger.log("User logged in");
    
    cout << "\n\n=== KEY TAKEAWAYS ===" << endl;
    cout << "1. Classes should be OPEN for extension" << endl;
    cout << "2. Classes should be CLOSED for modification" << endl;
    cout << "3. Use abstraction (interfaces/abstract classes)" << endl;
    cout << "4. Add new features by creating new classes" << endl;
    cout << "5. Don't modify existing, tested code" << endl;
    
    cout << "\nHow to achieve OCP:" << endl;
    cout << "• Use polymorphism (virtual functions)" << endl;
    cout << "• Create abstract base classes/interfaces" << endl;
    cout << "• Apply Strategy pattern" << endl;
    cout << "• Use dependency injection" << endl;
    cout << "• Prefer composition over modification" << endl;
    
    cout << "\nRed Flags (OCP Violations):" << endl;
    cout << "• Switch statements on type" << endl;
    cout << "• if-else chains checking types" << endl;
    cout << "• Modifying existing class for new feature" << endl;
    cout << "• Hard-coded dependencies" << endl;
    
    cout << "\nBenefits:" << endl;
    cout << "✓ Stable, tested code remains unchanged" << endl;
    cout << "✓ New features = new classes (safe)" << endl;
    cout << "✓ Easier to maintain and extend" << endl;
    cout << "✓ Supports plugin architectures" << endl;
    cout << "✓ Reduces regression bugs" << endl;
    
    return 0;
}

