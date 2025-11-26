/*
 * PROTOTYPE PATTERN - Creational Pattern
 * 
 * PURPOSE:
 * Creates new objects by copying existing objects (prototypes) rather than
 * creating new instances from scratch.
 * 
 * WHEN TO USE:
 * - Object creation is expensive (database queries, network calls, complex initialization)
 * - Want to avoid subclasses of an object creator
 * - Instances have only few different combinations of state
 * - Need to create objects that are similar to existing objects
 * 
 * PROS:
 * + Avoids expensive initialization
 * + Reduces subclassing
 * + Can add/remove objects at runtime
 * + Can specify new objects by varying values
 * + Reduces number of classes
 * 
 * CONS:
 * - Cloning complex objects with circular references can be tricky
 * - Deep copy vs shallow copy considerations
 * - Must implement clone() for all classes
 * 
 * REAL-WORLD EXAMPLES:
 * - Graphic editors (copying shapes/objects)
 * - Document templates
 * - Game objects (cloning enemies, items)
 * - Database record duplication
 */

#include <iostream>
#include <string>
#include <memory>
#include <unordered_map>

using namespace std;

// ============= Example 1: Shape Cloning =============

// Prototype interface
class Shape {
protected:
    string type;
    int x, y;
    string color;
    
public:
    Shape() : x(0), y(0), color("white") {}
    Shape(const Shape& source) {
        type = source.type;
        x = source.x;
        y = source.y;
        color = source.color;
    }
    
    virtual ~Shape() = default;
    
    // Pure virtual clone method
    virtual unique_ptr<Shape> clone() const = 0;
    virtual void draw() const = 0;
    
    void setPosition(int px, int py) { x = px; y = py; }
    void setColor(const string& c) { color = c; }
    
    string getType() const { return type; }
};

// Concrete Prototype 1: Circle
class Circle : public Shape {
private:
    int radius;
    
public:
    Circle(int r) : radius(r) {
        type = "Circle";
        cout << "Creating new Circle (expensive operation)..." << endl;
    }
    
    // Copy constructor
    Circle(const Circle& source) : Shape(source) {
        radius = source.radius;
        cout << "Cloning Circle (fast operation)..." << endl;
    }
    
    unique_ptr<Shape> clone() const override {
        return make_unique<Circle>(*this);
    }
    
    void draw() const override {
        cout << "Drawing " << color << " Circle at (" << x << "," << y 
             << ") with radius " << radius << endl;
    }
    
    void setRadius(int r) { radius = r; }
};

// Concrete Prototype 2: Rectangle
class Rectangle : public Shape {
private:
    int width, height;
    
public:
    Rectangle(int w, int h) : width(w), height(h) {
        type = "Rectangle";
        cout << "Creating new Rectangle (expensive operation)..." << endl;
    }
    
    Rectangle(const Rectangle& source) : Shape(source) {
        width = source.width;
        height = source.height;
        cout << "Cloning Rectangle (fast operation)..." << endl;
    }
    
    unique_ptr<Shape> clone() const override {
        return make_unique<Rectangle>(*this);
    }
    
    void draw() const override {
        cout << "Drawing " << color << " Rectangle at (" << x << "," << y 
             << ") with size " << width << "x" << height << endl;
    }
    
    void setDimensions(int w, int h) { width = w; height = h; }
};

// Concrete Prototype 3: Triangle
class Triangle : public Shape {
private:
    int base, height;
    
public:
    Triangle(int b, int h) : base(b), height(h) {
        type = "Triangle";
        cout << "Creating new Triangle (expensive operation)..." << endl;
    }
    
    Triangle(const Triangle& source) : Shape(source) {
        base = source.base;
        height = source.height;
        cout << "Cloning Triangle (fast operation)..." << endl;
    }
    
    unique_ptr<Shape> clone() const override {
        return make_unique<Triangle>(*this);
    }
    
    void draw() const override {
        cout << "Drawing " << color << " Triangle at (" << x << "," << y 
             << ") with base " << base << " and height " << height << endl;
    }
};

// Prototype Registry/Manager
class ShapeRegistry {
private:
    unordered_map<string, unique_ptr<Shape>> prototypes;
    
public:
    void registerPrototype(const string& key, unique_ptr<Shape> prototype) {
        prototypes[key] = move(prototype);
    }
    
    unique_ptr<Shape> createShape(const string& key) {
        auto it = prototypes.find(key);
        if (it != prototypes.end()) {
            return it->second->clone();
        }
        return nullptr;
    }
};

// ============= Example 2: Document Templates =============

class Document {
protected:
    string title;
    string content;
    string header;
    string footer;
    string fontFamily;
    int fontSize;
    
public:
    Document() : fontSize(12), fontFamily("Arial") {}
    
    Document(const Document& source) {
        title = source.title;
        content = source.content;
        header = source.header;
        footer = source.footer;
        fontFamily = source.fontFamily;
        fontSize = source.fontSize;
    }
    
    virtual ~Document() = default;
    virtual unique_ptr<Document> clone() const = 0;
    
    void setTitle(const string& t) { title = t; }
    void setContent(const string& c) { content = c; }
    
    virtual void display() const {
        cout << "\n--- Document ---" << endl;
        cout << "Header: " << header << endl;
        cout << "Title: " << title << endl;
        cout << "Font: " << fontFamily << ", Size: " << fontSize << endl;
        cout << "Content: " << content << endl;
        cout << "Footer: " << footer << endl;
    }
};

class BusinessLetter : public Document {
public:
    BusinessLetter() {
        header = "Company Letterhead";
        footer = "Company Address | Phone: xxx-xxx-xxxx";
        fontFamily = "Times New Roman";
        fontSize = 12;
        cout << "Creating Business Letter template..." << endl;
    }
    
    BusinessLetter(const BusinessLetter& source) : Document(source) {
        cout << "Cloning Business Letter..." << endl;
    }
    
    unique_ptr<Document> clone() const override {
        return make_unique<BusinessLetter>(*this);
    }
};

class Resume : public Document {
public:
    Resume() {
        header = "Professional Resume";
        footer = "References available upon request";
        fontFamily = "Calibri";
        fontSize = 11;
        cout << "Creating Resume template..." << endl;
    }
    
    Resume(const Resume& source) : Document(source) {
        cout << "Cloning Resume..." << endl;
    }
    
    unique_ptr<Document> clone() const override {
        return make_unique<Resume>(*this);
    }
};

int main() {
    cout << "=== PROTOTYPE PATTERN DEMO ===" << endl;
    
    // Example 1: Shape Cloning with Registry
    cout << "\n1. SHAPE CLONING SYSTEM:" << endl;
    cout << "========================" << endl;
    
    // Create registry and register prototypes
    ShapeRegistry registry;
    
    cout << "\n[Registering Prototypes]" << endl;
    auto circleProto = make_unique<Circle>(10);
    circleProto->setColor("red");
    registry.registerPrototype("basic_circle", move(circleProto));
    
    auto rectProto = make_unique<Rectangle>(20, 15);
    rectProto->setColor("blue");
    registry.registerPrototype("basic_rectangle", move(rectProto));
    
    auto triangleProto = make_unique<Triangle>(15, 20);
    triangleProto->setColor("green");
    registry.registerPrototype("basic_triangle", move(triangleProto));
    
    // Clone shapes from registry
    cout << "\n[Cloning Shapes from Registry]" << endl;
    auto circle1 = registry.createShape("basic_circle");
    circle1->setPosition(10, 20);
    circle1->draw();
    
    auto circle2 = registry.createShape("basic_circle");
    circle2->setPosition(50, 60);
    circle2->setColor("yellow");
    circle2->draw();
    
    auto rect1 = registry.createShape("basic_rectangle");
    rect1->setPosition(100, 100);
    rect1->draw();
    
    auto triangle1 = registry.createShape("basic_triangle");
    triangle1->setPosition(200, 150);
    triangle1->draw();
    
    // Example 2: Document Templates
    cout << "\n\n2. DOCUMENT TEMPLATE SYSTEM:" << endl;
    cout << "============================" << endl;
    
    cout << "\n[Creating Original Templates]" << endl;
    unique_ptr<Document> letterTemplate = make_unique<BusinessLetter>();
    unique_ptr<Document> resumeTemplate = make_unique<Resume>();
    
    cout << "\n[Creating Documents from Templates]" << endl;
    
    // Clone business letter for different purposes
    auto letter1 = letterTemplate->clone();
    letter1->setTitle("Job Application");
    letter1->setContent("I am writing to apply for the position...");
    letter1->display();
    
    auto letter2 = letterTemplate->clone();
    letter2->setTitle("Thank You Letter");
    letter2->setContent("Thank you for the opportunity to interview...");
    letter2->display();
    
    // Clone resume
    auto resume1 = resumeTemplate->clone();
    resume1->setTitle("Software Engineer Resume");
    resume1->setContent("Skills: C++, Python, Design Patterns...");
    resume1->display();
    
    cout << "\n\n=== KEY TAKEAWAYS ===" << endl;
    cout << "1. Prototype creates new objects by COPYING existing ones" << endl;
    cout << "2. Avoids expensive initialization (clone is faster)" << endl;
    cout << "3. Useful when object creation is complex or costly" << endl;
    cout << "4. Registry pattern manages and provides access to prototypes" << endl;
    cout << "5. Each concrete class implements its own clone() method" << endl;
    cout << "6. Be careful with deep vs shallow copying!" << endl;
    
    return 0;
}

