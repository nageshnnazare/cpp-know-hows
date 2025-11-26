/*
 * BUILDER PATTERN - Creational Pattern
 * 
 * PURPOSE:
 * Separates the construction of a complex object from its representation,
 * allowing the same construction process to create different representations.
 * 
 * WHEN TO USE:
 * - Object construction is complex with many parameters
 * - Want to create different representations of the same object
 * - Need step-by-step construction process
 * - Want to avoid "telescoping constructor" anti-pattern
 * 
 * PROS:
 * + Construct objects step-by-step
 * + Reuse construction code for different representations
 * + Single Responsibility: isolate complex construction code
 * + More readable than constructors with many parameters
 * + Can defer construction steps or run them recursively
 * 
 * CONS:
 * - Overall complexity increases (new classes needed)
 * - More code than simple constructors
 * 
 * REAL-WORLD EXAMPLES:
 * - Building complex documents (HTML, XML, etc.)
 * - Constructing meals (burgers, pizzas with different options)
 * - Creating complex queries (SQL builders)
 * - Building game characters with many attributes
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>

using namespace std;

// ============= Example 1: House Builder =============

// Product
class House {
private:
    string foundation;
    string structure;
    string roof;
    int windows;
    int doors;
    bool hasGarage;
    bool hasSwimmingPool;
    bool hasGarden;
    
public:
    House() : windows(0), doors(0), hasGarage(false), 
              hasSwimmingPool(false), hasGarden(false) {}
    
    // Setters
    void setFoundation(const string& f) { foundation = f; }
    void setStructure(const string& s) { structure = s; }
    void setRoof(const string& r) { roof = r; }
    void setWindows(int w) { windows = w; }
    void setDoors(int d) { doors = d; }
    void setGarage(bool g) { hasGarage = g; }
    void setSwimmingPool(bool s) { hasSwimmingPool = s; }
    void setGarden(bool g) { hasGarden = g; }
    
    void showSpecifications() const {
        cout << "\n=== House Specifications ===" << endl;
        cout << "Foundation: " << foundation << endl;
        cout << "Structure: " << structure << endl;
        cout << "Roof: " << roof << endl;
        cout << "Windows: " << windows << endl;
        cout << "Doors: " << doors << endl;
        cout << "Garage: " << (hasGarage ? "Yes" : "No") << endl;
        cout << "Swimming Pool: " << (hasSwimmingPool ? "Yes" : "No") << endl;
        cout << "Garden: " << (hasGarden ? "Yes" : "No") << endl;
    }
};

// Builder Interface
class HouseBuilder {
protected:
    unique_ptr<House> house;
    
public:
    HouseBuilder() { house = make_unique<House>(); }
    virtual ~HouseBuilder() = default;
    
    virtual void buildFoundation() = 0;
    virtual void buildStructure() = 0;
    virtual void buildRoof() = 0;
    virtual void buildWindows() = 0;
    virtual void buildDoors() = 0;
    virtual void buildGarage() = 0;
    virtual void buildSwimmingPool() = 0;
    virtual void buildGarden() = 0;
    
    unique_ptr<House> getHouse() {
        return move(house);
    }
};

// Concrete Builder 1: Luxury House
class LuxuryHouseBuilder : public HouseBuilder {
public:
    void buildFoundation() override {
        house->setFoundation("Reinforced concrete with basement");
        cout << "Building luxury foundation..." << endl;
    }
    
    void buildStructure() override {
        house->setStructure("Premium wood and steel structure");
        cout << "Building luxury structure..." << endl;
    }
    
    void buildRoof() override {
        house->setRoof("Slate roof with solar panels");
        cout << "Building luxury roof..." << endl;
    }
    
    void buildWindows() override {
        house->setWindows(20);
        cout << "Installing 20 double-glazed windows..." << endl;
    }
    
    void buildDoors() override {
        house->setDoors(5);
        cout << "Installing 5 mahogany doors..." << endl;
    }
    
    void buildGarage() override {
        house->setGarage(true);
        cout << "Building 3-car garage..." << endl;
    }
    
    void buildSwimmingPool() override {
        house->setSwimmingPool(true);
        cout << "Building Olympic-sized swimming pool..." << endl;
    }
    
    void buildGarden() override {
        house->setGarden(true);
        cout << "Creating landscaped garden..." << endl;
    }
};

// Concrete Builder 2: Simple House
class SimpleHouseBuilder : public HouseBuilder {
public:
    void buildFoundation() override {
        house->setFoundation("Standard concrete foundation");
        cout << "Building simple foundation..." << endl;
    }
    
    void buildStructure() override {
        house->setStructure("Standard brick structure");
        cout << "Building simple structure..." << endl;
    }
    
    void buildRoof() override {
        house->setRoof("Asphalt shingle roof");
        cout << "Building simple roof..." << endl;
    }
    
    void buildWindows() override {
        house->setWindows(6);
        cout << "Installing 6 standard windows..." << endl;
    }
    
    void buildDoors() override {
        house->setDoors(2);
        cout << "Installing 2 standard doors..." << endl;
    }
    
    void buildGarage() override {
        house->setGarage(false);
        cout << "No garage..." << endl;
    }
    
    void buildSwimmingPool() override {
        house->setSwimmingPool(false);
        cout << "No swimming pool..." << endl;
    }
    
    void buildGarden() override {
        house->setGarden(true);
        cout << "Creating small garden..." << endl;
    }
};

// Director (optional but useful for defining construction order)
class ConstructionDirector {
private:
    HouseBuilder* builder;
    
public:
    void setBuilder(HouseBuilder* b) {
        builder = b;
    }
    
    void constructMinimalHouse() {
        cout << "\n[Constructing Minimal House]" << endl;
        builder->buildFoundation();
        builder->buildStructure();
        builder->buildRoof();
        builder->buildWindows();
        builder->buildDoors();
    }
    
    void constructFullFeaturedHouse() {
        cout << "\n[Constructing Full-Featured House]" << endl;
        builder->buildFoundation();
        builder->buildStructure();
        builder->buildRoof();
        builder->buildWindows();
        builder->buildDoors();
        builder->buildGarage();
        builder->buildSwimmingPool();
        builder->buildGarden();
    }
};

// ============= Example 2: Fluent Builder (Modern C++ style) =============

class Pizza {
private:
    string dough;
    string sauce;
    vector<string> toppings;
    string size;
    bool cheeseFilled;
    
public:
    void setDough(const string& d) { dough = d; }
    void setSauce(const string& s) { sauce = s; }
    void addTopping(const string& t) { toppings.push_back(t); }
    void setSize(const string& s) { size = s; }
    void setCheeseFilled(bool c) { cheeseFilled = c; }
    
    void describe() const {
        cout << "\n=== Pizza Order ===" << endl;
        cout << "Size: " << size << endl;
        cout << "Dough: " << dough << endl;
        cout << "Sauce: " << sauce << endl;
        cout << "Cheese-filled crust: " << (cheeseFilled ? "Yes" : "No") << endl;
        cout << "Toppings: ";
        for (size_t i = 0; i < toppings.size(); ++i) {
            cout << toppings[i];
            if (i < toppings.size() - 1) cout << ", ";
        }
        cout << endl;
    }
};

// Fluent Builder
class PizzaBuilder {
private:
    unique_ptr<Pizza> pizza;
    
public:
    PizzaBuilder() { pizza = make_unique<Pizza>(); }
    
    // Fluent interface: each method returns reference to builder
    PizzaBuilder& setSize(const string& size) {
        pizza->setSize(size);
        return *this;
    }
    
    PizzaBuilder& setDough(const string& dough) {
        pizza->setSauce(dough);
        return *this;
    }
    
    PizzaBuilder& setSauce(const string& sauce) {
        pizza->setSauce(sauce);
        return *this;
    }
    
    PizzaBuilder& addTopping(const string& topping) {
        pizza->addTopping(topping);
        return *this;
    }
    
    PizzaBuilder& setCheeseFilled(bool filled) {
        pizza->setCheeseFilled(filled);
        return *this;
    }
    
    unique_ptr<Pizza> build() {
        return move(pizza);
    }
};

int main() {
    cout << "=== BUILDER PATTERN DEMO ===" << endl;
    
    // Example 1: House construction with Director
    cout << "\n1. HOUSE CONSTRUCTION (with Director):" << endl;
    cout << "======================================" << endl;
    
    ConstructionDirector director;
    
    // Build luxury house
    cout << "\nBuilding Luxury House:" << endl;
    LuxuryHouseBuilder luxuryBuilder;
    director.setBuilder(&luxuryBuilder);
    director.constructFullFeaturedHouse();
    unique_ptr<House> luxuryHouse = luxuryBuilder.getHouse();
    luxuryHouse->showSpecifications();
    
    // Build simple house
    cout << "\n\nBuilding Simple House:" << endl;
    SimpleHouseBuilder simpleBuilder;
    director.setBuilder(&simpleBuilder);
    director.constructMinimalHouse();
    unique_ptr<House> simpleHouse = simpleBuilder.getHouse();
    simpleHouse->showSpecifications();
    
    // Example 2: Pizza with Fluent Builder
    cout << "\n\n2. PIZZA BUILDER (Fluent Interface):" << endl;
    cout << "====================================" << endl;
    
    // Build Margherita Pizza
    cout << "\nOrdering Margherita Pizza:" << endl;
    unique_ptr<Pizza> margherita = PizzaBuilder()
        .setSize("Medium")
        .setDough("Thin crust")
        .setSauce("Tomato")
        .addTopping("Mozzarella")
        .addTopping("Basil")
        .setCheeseFilled(false)
        .build();
    margherita->describe();
    
    // Build Meat Lovers Pizza
    cout << "\n\nOrdering Meat Lovers Pizza:" << endl;
    unique_ptr<Pizza> meatLovers = PizzaBuilder()
        .setSize("Large")
        .setDough("Thick crust")
        .setSauce("BBQ")
        .addTopping("Pepperoni")
        .addTopping("Sausage")
        .addTopping("Bacon")
        .addTopping("Ham")
        .setCheeseFilled(true)
        .build();
    meatLovers->describe();
    
    cout << "\n\n=== KEY TAKEAWAYS ===" << endl;
    cout << "1. Builder separates construction from representation" << endl;
    cout << "2. Same construction process can create different representations" << endl;
    cout << "3. Step-by-step construction allows fine control" << endl;
    cout << "4. Fluent interface makes code more readable (method chaining)" << endl;
    cout << "5. Director is optional but helps define construction sequences" << endl;
    cout << "6. Solves 'telescoping constructor' problem elegantly" << endl;
    
    return 0;
}

