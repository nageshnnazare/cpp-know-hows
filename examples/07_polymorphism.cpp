// Module 07 — virtual dispatch, abstract base, virtual destructor, slicing demo.
#include <iostream>
#include <memory>
#include <vector>

struct Shape {
    virtual double area() const = 0;          // pure virtual ⇒ abstract
    virtual const char* name() const = 0;
    virtual ~Shape() = default;               // virtual dtor: safe delete via Shape*
};

struct Circle : Shape {
    double r;
    explicit Circle(double r) : r(r) {}
    double area() const override { return 3.14159265 * r * r; }
    const char* name() const override { return "Circle"; }
};

struct Rectangle : Shape {
    double w, h;
    Rectangle(double w, double h) : w(w), h(h) {}
    double area() const override { return w * h; }
    const char* name() const override { return "Rectangle"; }
};

// polymorphism works through references/pointers, NOT by value
void describe(const Shape& s) {
    std::cout << s.name() << " area = " << s.area() << '\n';
}

int main() {
    std::vector<std::unique_ptr<Shape>> shapes;
    shapes.push_back(std::make_unique<Circle>(2.0));
    shapes.push_back(std::make_unique<Rectangle>(3.0, 4.0));

    double total = 0;
    for (const auto& s : shapes) {
        describe(*s);            // dynamic dispatch via vtable
        total += s->area();
    }
    std::cout << "total area = " << total << '\n';
    // all unique_ptrs delete correctly because ~Shape is virtual
}
