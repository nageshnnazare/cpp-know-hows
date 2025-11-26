/*
 * VISITOR PATTERN - Behavioral Pattern
 * 
 * PURPOSE:
 * Separates algorithms from the objects on which they operate.
 * Lets you add new operations to existing object structures without modifying them.
 * 
 * WHEN TO USE:
 * - Need to perform operations on elements of complex object structure
 * - Want to add new operations without changing element classes
 * - Operations need to work on objects of different types
 * - Object structure rarely changes but operations do
 * 
 * PROS:
 * + Open/Closed Principle: add new operations without modifying classes
 * + Single Responsibility: related operations in one class
 * + Can gather state while traversing structure
 * + Clean separation of concerns
 * 
 * CONS:
 * - Adding new element types is difficult
 * - May break encapsulation
 * - Can become complex
 * 
 * REAL-WORLD EXAMPLES:
 * - Compiler (AST traversal)
 * - Document structure operations (export, render, validate)
 * - Shopping cart (tax calculation, discount application)
 * - File system operations
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>

using namespace std;

// ============= Example 1: Shape Operations =============

// Forward declarations
class Circle;
class Rectangle;
class Triangle;

// Visitor interface
class ShapeVisitor {
public:
    virtual ~ShapeVisitor() = default;
    virtual void visit(Circle* circle) = 0;
    virtual void visit(Rectangle* rectangle) = 0;
    virtual void visit(Triangle* triangle) = 0;
};

// Element interface
class Shape {
public:
    virtual ~Shape() = default;
    virtual void accept(ShapeVisitor* visitor) = 0;
};

// Concrete Elements
class Circle : public Shape {
private:
    double radius;
    
public:
    Circle(double r) : radius(r) {}
    
    double getRadius() const { return radius; }
    
    void accept(ShapeVisitor* visitor) override {
        visitor->visit(this);
    }
};

class Rectangle : public Shape {
private:
    double width, height;
    
public:
    Rectangle(double w, double h) : width(w), height(h) {}
    
    double getWidth() const { return width; }
    double getHeight() const { return height; }
    
    void accept(ShapeVisitor* visitor) override {
        visitor->visit(this);
    }
};

class Triangle : public Shape {
private:
    double base, height;
    
public:
    Triangle(double b, double h) : base(b), height(h) {}
    
    double getBase() const { return base; }
    double getHeight() const { return height; }
    
    void accept(ShapeVisitor* visitor) override {
        visitor->visit(this);
    }
};

// Concrete Visitors
class AreaCalculator : public ShapeVisitor {
private:
    double totalArea;
    
public:
    AreaCalculator() : totalArea(0) {}
    
    void visit(Circle* circle) override {
        double area = 3.14159 * circle->getRadius() * circle->getRadius();
        cout << "Circle area: " << area << endl;
        totalArea += area;
    }
    
    void visit(Rectangle* rectangle) override {
        double area = rectangle->getWidth() * rectangle->getHeight();
        cout << "Rectangle area: " << area << endl;
        totalArea += area;
    }
    
    void visit(Triangle* triangle) override {
        double area = 0.5 * triangle->getBase() * triangle->getHeight();
        cout << "Triangle area: " << area << endl;
        totalArea += area;
    }
    
    double getTotalArea() const { return totalArea; }
};

class PerimeterCalculator : public ShapeVisitor {
public:
    void visit(Circle* circle) override {
        double perimeter = 2 * 3.14159 * circle->getRadius();
        cout << "Circle perimeter: " << perimeter << endl;
    }
    
    void visit(Rectangle* rectangle) override {
        double perimeter = 2 * (rectangle->getWidth() + rectangle->getHeight());
        cout << "Rectangle perimeter: " << perimeter << endl;
    }
    
    void visit(Triangle* triangle) override {
        // Assuming equilateral triangle for simplicity
        double side = triangle->getBase();
        double perimeter = 3 * side;
        cout << "Triangle perimeter (equilateral): " << perimeter << endl;
    }
};

class DrawVisitor : public ShapeVisitor {
public:
    void visit(Circle* circle) override {
        cout << "Drawing Circle with radius " << circle->getRadius() << endl;
    }
    
    void visit(Rectangle* rectangle) override {
        cout << "Drawing Rectangle " << rectangle->getWidth() 
             << "x" << rectangle->getHeight() << endl;
    }
    
    void visit(Triangle* triangle) override {
        cout << "Drawing Triangle with base " << triangle->getBase() 
             << " and height " << triangle->getHeight() << endl;
    }
};

// ============= Example 2: Computer Parts =============

class Keyboard;
class Monitor;
class Mouse;
class Computer;

class ComputerPartVisitor {
public:
    virtual ~ComputerPartVisitor() = default;
    virtual void visit(Keyboard* keyboard) = 0;
    virtual void visit(Monitor* monitor) = 0;
    virtual void visit(Mouse* mouse) = 0;
    virtual void visit(Computer* computer) = 0;
};

class ComputerPart {
public:
    virtual ~ComputerPart() = default;
    virtual void accept(ComputerPartVisitor* visitor) = 0;
};

class Keyboard : public ComputerPart {
public:
    void accept(ComputerPartVisitor* visitor) override {
        visitor->visit(this);
    }
};

class Monitor : public ComputerPart {
public:
    void accept(ComputerPartVisitor* visitor) override {
        visitor->visit(this);
    }
};

class Mouse : public ComputerPart {
public:
    void accept(ComputerPartVisitor* visitor) override {
        visitor->visit(this);
    }
};

class Computer : public ComputerPart {
private:
    vector<shared_ptr<ComputerPart>> parts;
    
public:
    Computer() {
        parts.push_back(make_shared<Keyboard>());
        parts.push_back(make_shared<Monitor>());
        parts.push_back(make_shared<Mouse>());
    }
    
    void accept(ComputerPartVisitor* visitor) override {
        for (auto& part : parts) {
            part->accept(visitor);
        }
        visitor->visit(this);
    }
};

// Concrete Visitors for Computer Parts
class DisplayVisitor : public ComputerPartVisitor {
public:
    void visit(Keyboard* keyboard) override {
        cout << "Displaying Keyboard" << endl;
    }
    
    void visit(Monitor* monitor) override {
        cout << "Displaying Monitor" << endl;
    }
    
    void visit(Mouse* mouse) override {
        cout << "Displaying Mouse" << endl;
    }
    
    void visit(Computer* computer) override {
        cout << "Displaying Computer" << endl;
    }
};

class PriceCalculator : public ComputerPartVisitor {
private:
    double totalPrice;
    
public:
    PriceCalculator() : totalPrice(0) {}
    
    void visit(Keyboard* keyboard) override {
        double price = 50.0;
        cout << "Keyboard: $" << price << endl;
        totalPrice += price;
    }
    
    void visit(Monitor* monitor) override {
        double price = 300.0;
        cout << "Monitor: $" << price << endl;
        totalPrice += price;
    }
    
    void visit(Mouse* mouse) override {
        double price = 30.0;
        cout << "Mouse: $" << price << endl;
        totalPrice += price;
    }
    
    void visit(Computer* computer) override {
        cout << "Computer assembly: $100" << endl;
        totalPrice += 100.0;
    }
    
    double getTotalPrice() const { return totalPrice; }
};

// ============= Example 3: File System =============

class TextFile;
class ImageFile;
class VideoFile;

class FileVisitor {
public:
    virtual ~FileVisitor() = default;
    virtual void visit(TextFile* file) = 0;
    virtual void visit(ImageFile* file) = 0;
    virtual void visit(VideoFile* file) = 0;
};

class File {
protected:
    string name;
    int size;  // in KB
    
public:
    File(const string& n, int s) : name(n), size(s) {}
    virtual ~File() = default;
    virtual void accept(FileVisitor* visitor) = 0;
    
    string getName() const { return name; }
    int getSize() const { return size; }
};

class TextFile : public File {
public:
    TextFile(const string& name, int size) : File(name, size) {}
    
    void accept(FileVisitor* visitor) override {
        visitor->visit(this);
    }
};

class ImageFile : public File {
public:
    ImageFile(const string& name, int size) : File(name, size) {}
    
    void accept(FileVisitor* visitor) override {
        visitor->visit(this);
    }
};

class VideoFile : public File {
public:
    VideoFile(const string& name, int size) : File(name, size) {}
    
    void accept(FileVisitor* visitor) override {
        visitor->visit(this);
    }
};

// Concrete Visitors for Files
class AntivirusScanVisitor : public FileVisitor {
public:
    void visit(TextFile* file) override {
        cout << "Scanning text file: " << file->getName() << " - Safe" << endl;
    }
    
    void visit(ImageFile* file) override {
        cout << "Scanning image file: " << file->getName() << " - Safe" << endl;
    }
    
    void visit(VideoFile* file) override {
        cout << "Deep scanning video file: " << file->getName() << " - Safe" << endl;
    }
};

class CompressionVisitor : public FileVisitor {
private:
    int totalSaved;
    
public:
    CompressionVisitor() : totalSaved(0) {}
    
    void visit(TextFile* file) override {
        int saved = file->getSize() * 0.7;  // 70% compression
        totalSaved += saved;
        cout << "Compressed " << file->getName() << ": saved " << saved << " KB" << endl;
    }
    
    void visit(ImageFile* file) override {
        int saved = file->getSize() * 0.3;  // 30% compression
        totalSaved += saved;
        cout << "Compressed " << file->getName() << ": saved " << saved << " KB" << endl;
    }
    
    void visit(VideoFile* file) override {
        int saved = file->getSize() * 0.5;  // 50% compression
        totalSaved += saved;
        cout << "Compressed " << file->getName() << ": saved " << saved << " KB" << endl;
    }
    
    int getTotalSaved() const { return totalSaved; }
};

int main() {
    cout << "=== VISITOR PATTERN DEMO ===" << endl;
    
    // Example 1: Shape Operations
    cout << "\n1. SHAPE OPERATIONS:" << endl;
    cout << "====================" << endl;
    
    vector<shared_ptr<Shape>> shapes;
    shapes.push_back(make_shared<Circle>(5.0));
    shapes.push_back(make_shared<Rectangle>(4.0, 6.0));
    shapes.push_back(make_shared<Triangle>(3.0, 8.0));
    
    cout << "\n[Calculating Areas]" << endl;
    AreaCalculator areaCalc;
    for (auto& shape : shapes) {
        shape->accept(&areaCalc);
    }
    cout << "Total area: " << areaCalc.getTotalArea() << endl;
    
    cout << "\n[Calculating Perimeters]" << endl;
    PerimeterCalculator perimeterCalc;
    for (auto& shape : shapes) {
        shape->accept(&perimeterCalc);
    }
    
    cout << "\n[Drawing Shapes]" << endl;
    DrawVisitor drawer;
    for (auto& shape : shapes) {
        shape->accept(&drawer);
    }
    
    // Example 2: Computer Parts
    cout << "\n\n2. COMPUTER PARTS:" << endl;
    cout << "==================" << endl;
    
    Computer computer;
    
    cout << "\n[Displaying Components]" << endl;
    DisplayVisitor displayVisitor;
    computer.accept(&displayVisitor);
    
    cout << "\n[Calculating Total Price]" << endl;
    PriceCalculator priceCalc;
    computer.accept(&priceCalc);
    cout << "Total Price: $" << priceCalc.getTotalPrice() << endl;
    
    // Example 3: File System
    cout << "\n\n3. FILE SYSTEM:" << endl;
    cout << "===============" << endl;
    
    vector<shared_ptr<File>> files;
    files.push_back(make_shared<TextFile>("document.txt", 100));
    files.push_back(make_shared<ImageFile>("photo.jpg", 5000));
    files.push_back(make_shared<VideoFile>("movie.mp4", 500000));
    
    cout << "\n[Antivirus Scan]" << endl;
    AntivirusScanVisitor scanner;
    for (auto& file : files) {
        file->accept(&scanner);
    }
    
    cout << "\n[File Compression]" << endl;
    CompressionVisitor compressor;
    for (auto& file : files) {
        file->accept(&compressor);
    }
    cout << "Total space saved: " << compressor.getTotalSaved() << " KB" << endl;
    
    cout << "\n\n=== KEY TAKEAWAYS ===" << endl;
    cout << "1. Visitor SEPARATES algorithms from objects they operate on" << endl;
    cout << "2. Add new operations without modifying element classes" << endl;
    cout << "3. Uses double dispatch (accept + visit methods)" << endl;
    cout << "4. Good when object structure stable but operations change" << endl;
    cout << "5. Groups related operations in one visitor class" << endl;
    cout << "6. Can accumulate state while visiting elements" << endl;
    cout << "7. Difficult to add new element types" << endl;
    cout << "8. May break encapsulation of element classes" << endl;
    
    return 0;
}

