/*
 * COMPOSITE PATTERN - Structural Pattern
 * 
 * PURPOSE:
 * Composes objects into tree structures to represent part-whole hierarchies.
 * Lets clients treat individual objects and compositions uniformly.
 * 
 * WHEN TO USE:
 * - Need to represent part-whole hierarchies of objects
 * - Want clients to treat individual objects and compositions uniformly
 * - Need tree structures (file systems, GUI components, organizational charts)
 * 
 * PROS:
 * + Simplifies client code (treats all objects uniformly)
 * + Easy to add new component types
 * + Follows Open/Closed Principle
 * + Tree structure is natural and intuitive
 * 
 * CONS:
 * - Can make design overly general
 * - Difficult to restrict component types
 * - Type safety can be compromised
 * 
 * REAL-WORLD EXAMPLES:
 * - File systems (files and directories)
 * - GUI components (containers and widgets)
 * - Organizational charts (employees and departments)
 * - Graphics (shapes and groups)
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>

using namespace std;

// ============= Example 1: File System =============

// Component interface
class FileSystemComponent {
protected:
    string name;
    
public:
    FileSystemComponent(const string& n) : name(n) {}
    virtual ~FileSystemComponent() = default;
    
    virtual void display(int depth = 0) const = 0;
    virtual int getSize() const = 0;
    virtual string getName() const { return name; }
    
    // Optional: add/remove methods (may not make sense for leaf)
    virtual void add(shared_ptr<FileSystemComponent> component) {
        throw runtime_error("Cannot add to a file");
    }
    
    virtual void remove(shared_ptr<FileSystemComponent> component) {
        throw runtime_error("Cannot remove from a file");
    }
};

// Leaf: File
class File : public FileSystemComponent {
private:
    int size;
    
public:
    File(const string& name, int size) : FileSystemComponent(name), size(size) {}
    
    void display(int depth = 0) const override {
        cout << string(depth, ' ') << "📄 " << name << " (" << size << " KB)" << endl;
    }
    
    int getSize() const override {
        return size;
    }
};

// Composite: Directory
class Directory : public FileSystemComponent {
private:
    vector<shared_ptr<FileSystemComponent>> children;
    
public:
    Directory(const string& name) : FileSystemComponent(name) {}
    
    void add(shared_ptr<FileSystemComponent> component) override {
        children.push_back(component);
    }
    
    void remove(shared_ptr<FileSystemComponent> component) override {
        auto it = find(children.begin(), children.end(), component);
        if (it != children.end()) {
            children.erase(it);
        }
    }
    
    void display(int depth = 0) const override {
        cout << string(depth, ' ') << "📁 " << name << "/" << endl;
        for (const auto& child : children) {
            child->display(depth + 2);
        }
    }
    
    int getSize() const override {
        int totalSize = 0;
        for (const auto& child : children) {
            totalSize += child->getSize();
        }
        return totalSize;
    }
};

// ============= Example 2: Graphics System =============

// Component
class Graphic {
public:
    virtual ~Graphic() = default;
    virtual void draw() const = 0;
    virtual void move(int x, int y) = 0;
};

// Leaf: Circle
class GraphicCircle : public Graphic {
private:
    int x, y, radius;
    
public:
    GraphicCircle(int x, int y, int r) : x(x), y(y), radius(r) {}
    
    void draw() const override {
        cout << "  Circle at (" << x << "," << y << ") with radius " << radius << endl;
    }
    
    void move(int dx, int dy) override {
        x += dx;
        y += dy;
    }
};

// Leaf: Rectangle
class GraphicRectangle : public Graphic {
private:
    int x, y, width, height;
    
public:
    GraphicRectangle(int x, int y, int w, int h) : x(x), y(y), width(w), height(h) {}
    
    void draw() const override {
        cout << "  Rectangle at (" << x << "," << y << ") with size " 
             << width << "x" << height << endl;
    }
    
    void move(int dx, int dy) override {
        x += dx;
        y += dy;
    }
};

// Composite: Group
class GraphicGroup : public Graphic {
private:
    vector<shared_ptr<Graphic>> graphics;
    string groupName;
    
public:
    GraphicGroup(const string& name) : groupName(name) {}
    
    void add(shared_ptr<Graphic> graphic) {
        graphics.push_back(graphic);
    }
    
    void remove(shared_ptr<Graphic> graphic) {
        auto it = find(graphics.begin(), graphics.end(), graphic);
        if (it != graphics.end()) {
            graphics.erase(it);
        }
    }
    
    void draw() const override {
        cout << "Group: " << groupName << endl;
        for (const auto& graphic : graphics) {
            graphic->draw();
        }
    }
    
    void move(int dx, int dy) override {
        for (auto& graphic : graphics) {
            graphic->move(dx, dy);
        }
    }
};

// ============= Example 3: Company Organization =============

// Component
class Employee {
protected:
    string name;
    string position;
    int salary;
    
public:
    Employee(const string& n, const string& pos, int sal) 
        : name(n), position(pos), salary(sal) {}
    virtual ~Employee() = default;
    
    virtual void showDetails(int depth = 0) const = 0;
    virtual int getTotalSalary() const = 0;
    
    virtual void addSubordinate(shared_ptr<Employee> emp) {
        throw runtime_error("Cannot add subordinate to individual contributor");
    }
    
    virtual void removeSubordinate(shared_ptr<Employee> emp) {
        throw runtime_error("Cannot remove subordinate from individual contributor");
    }
};

// Leaf: Individual Contributor
class IndividualContributor : public Employee {
public:
    IndividualContributor(const string& n, const string& pos, int sal)
        : Employee(n, pos, sal) {}
    
    void showDetails(int depth = 0) const override {
        cout << string(depth, ' ') << "👤 " << name << " - " << position 
             << " (Salary: $" << salary << ")" << endl;
    }
    
    int getTotalSalary() const override {
        return salary;
    }
};

// Composite: Manager
class Manager : public Employee {
private:
    vector<shared_ptr<Employee>> subordinates;
    
public:
    Manager(const string& n, const string& pos, int sal)
        : Employee(n, pos, sal) {}
    
    void addSubordinate(shared_ptr<Employee> emp) override {
        subordinates.push_back(emp);
    }
    
    void removeSubordinate(shared_ptr<Employee> emp) override {
        auto it = find(subordinates.begin(), subordinates.end(), emp);
        if (it != subordinates.end()) {
            subordinates.erase(it);
        }
    }
    
    void showDetails(int depth = 0) const override {
        cout << string(depth, ' ') << "👔 " << name << " - " << position 
             << " (Salary: $" << salary << ")" << endl;
        for (const auto& sub : subordinates) {
            sub->showDetails(depth + 2);
        }
    }
    
    int getTotalSalary() const override {
        int total = salary;
        for (const auto& sub : subordinates) {
            total += sub->getTotalSalary();
        }
        return total;
    }
};

int main() {
    cout << "=== COMPOSITE PATTERN DEMO ===" << endl;
    
    // Example 1: File System
    cout << "\n1. FILE SYSTEM:" << endl;
    cout << "===============" << endl;
    
    // Create files
    auto file1 = make_shared<File>("document.txt", 100);
    auto file2 = make_shared<File>("image.jpg", 500);
    auto file3 = make_shared<File>("video.mp4", 2000);
    auto file4 = make_shared<File>("readme.md", 50);
    auto file5 = make_shared<File>("config.json", 20);
    
    // Create directories
    auto documents = make_shared<Directory>("Documents");
    documents->add(file1);
    documents->add(file4);
    
    auto media = make_shared<Directory>("Media");
    media->add(file2);
    media->add(file3);
    
    auto root = make_shared<Directory>("Root");
    root->add(documents);
    root->add(media);
    root->add(file5);
    
    // Display structure
    cout << "\nFile System Structure:" << endl;
    root->display();
    cout << "\nTotal Size: " << root->getSize() << " KB" << endl;
    
    // Example 2: Graphics System
    cout << "\n\n2. GRAPHICS SYSTEM:" << endl;
    cout << "===================" << endl;
    
    // Create individual shapes
    auto circle1 = make_shared<GraphicCircle>(10, 10, 5);
    auto circle2 = make_shared<GraphicCircle>(50, 50, 8);
    auto rect1 = make_shared<GraphicRectangle>(20, 20, 30, 40);
    auto rect2 = make_shared<GraphicRectangle>(100, 100, 50, 60);
    
    // Create groups
    auto group1 = make_shared<GraphicGroup>("Group 1");
    group1->add(circle1);
    group1->add(rect1);
    
    auto group2 = make_shared<GraphicGroup>("Group 2");
    group2->add(circle2);
    group2->add(rect2);
    
    auto mainGroup = make_shared<GraphicGroup>("Main Group");
    mainGroup->add(group1);
    mainGroup->add(group2);
    
    cout << "\nDrawing all graphics:" << endl;
    mainGroup->draw();
    
    cout << "\nMoving entire group by (10, 10):" << endl;
    mainGroup->move(10, 10);
    mainGroup->draw();
    
    // Example 3: Company Organization
    cout << "\n\n3. COMPANY ORGANIZATION:" << endl;
    cout << "========================" << endl;
    
    // CEO
    auto ceo = make_shared<Manager>("John Smith", "CEO", 200000);
    
    // VPs reporting to CEO
    auto vpEng = make_shared<Manager>("Alice Johnson", "VP Engineering", 150000);
    auto vpSales = make_shared<Manager>("Bob Williams", "VP Sales", 150000);
    
    // Engineering managers
    auto engMgr1 = make_shared<Manager>("Charlie Brown", "Engineering Manager", 120000);
    auto engMgr2 = make_shared<Manager>("Diana Prince", "Engineering Manager", 120000);
    
    // Engineers
    auto dev1 = make_shared<IndividualContributor>("Eve Davis", "Senior Developer", 100000);
    auto dev2 = make_shared<IndividualContributor>("Frank Miller", "Developer", 80000);
    auto dev3 = make_shared<IndividualContributor>("Grace Lee", "Developer", 80000);
    auto dev4 = make_shared<IndividualContributor>("Henry Wilson", "Junior Developer", 60000);
    
    // Sales team
    auto sales1 = make_shared<IndividualContributor>("Ivy Chen", "Sales Executive", 90000);
    auto sales2 = make_shared<IndividualContributor>("Jack Taylor", "Sales Executive", 90000);
    
    // Build hierarchy
    engMgr1->addSubordinate(dev1);
    engMgr1->addSubordinate(dev2);
    
    engMgr2->addSubordinate(dev3);
    engMgr2->addSubordinate(dev4);
    
    vpEng->addSubordinate(engMgr1);
    vpEng->addSubordinate(engMgr2);
    
    vpSales->addSubordinate(sales1);
    vpSales->addSubordinate(sales2);
    
    ceo->addSubordinate(vpEng);
    ceo->addSubordinate(vpSales);
    
    // Display organization
    cout << "\nCompany Organization Chart:" << endl;
    ceo->showDetails();
    cout << "\nTotal Company Salary Budget: $" << ceo->getTotalSalary() << endl;
    
    cout << "\n\n=== KEY TAKEAWAYS ===" << endl;
    cout << "1. Composite creates TREE structures (part-whole hierarchies)" << endl;
    cout << "2. Treats individual objects and compositions UNIFORMLY" << endl;
    cout << "3. Client code doesn't need to distinguish between leaf and composite" << endl;
    cout << "4. Easy to add new component types" << endl;
    cout << "5. Recursive composition allows unlimited nesting" << endl;
    cout << "6. Operations on composite automatically propagate to children" << endl;
    
    return 0;
}

