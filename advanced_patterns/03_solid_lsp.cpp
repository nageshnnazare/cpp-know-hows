/*
 * LISKOV SUBSTITUTION PRINCIPLE (LSP)
 * Third of the SOLID Principles
 * 
 * DEFINITION:
 * "Objects of a superclass should be replaceable with objects of a subclass
 *  without breaking the application."
 * - Barbara Liskov (1987)
 * 
 * SIMPLIFIED:
 * "Derived classes must be substitutable for their base classes."
 * If S is a subtype of T, then objects of type T can be replaced with 
 * objects of type S without altering program correctness.
 * 
 * RULES:
 * 1. Preconditions cannot be strengthened in subclass
 * 2. Postconditions cannot be weakened in subclass
 * 3. Invariants must be preserved
 * 4. History constraint (immutable properties stay immutable)
 * 
 * BENEFITS:
 * + Code reusability
 * + Proper inheritance hierarchies
 * + Polymorphism works correctly
 * + Fewer surprises and bugs
 * 
 * VIOLATIONS:
 * - Throwing unexpected exceptions in subclass
 * - Changing method behavior drastically
 * - Removing functionality in subclass
 * - Strengthening preconditions
 */

#include <iostream>
#include <string>
#include <memory>
#include <stdexcept>
#include <vector>

using namespace std;

// ============= BAD EXAMPLE 1 - Classic Square/Rectangle Problem =============

class RectangleBad {
protected:
    int width, height;
    
public:
    virtual void setWidth(int w) { width = w; }
    virtual void setHeight(int h) { height = h; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int area() const { return width * height; }
};

class SquareBad : public RectangleBad {
public:
    // Violates LSP: Changes behavior expectations
    void setWidth(int w) override {
        width = w;
        height = w;  // Square must have equal sides
    }
    
    void setHeight(int h) override {
        width = h;   // Square must have equal sides
        height = h;
    }
};

void testRectangle(RectangleBad& rect) {
    rect.setWidth(5);
    rect.setHeight(4);
    // Expected: area = 20
    // With Square: area = 16 (SURPRISE!)
    cout << "Expected area: 20, Actual: " << rect.area() << endl;
}

// ============= GOOD EXAMPLE 1 - Proper Abstraction =============

class Shape {
public:
    virtual ~Shape() = default;
    virtual int area() const = 0;
    virtual string getName() const = 0;
};

class Rectangle : public Shape {
private:
    int width, height;
    
public:
    Rectangle(int w, int h) : width(w), height(h) {}
    
    void setWidth(int w) { width = w; }
    void setHeight(int h) { height = h; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    
    int area() const override {
        return width * height;
    }
    
    string getName() const override {
        return "Rectangle";
    }
};

class Square : public Shape {
private:
    int side;
    
public:
    Square(int s) : side(s) {}
    
    void setSide(int s) { side = s; }
    int getSide() const { return side; }
    
    int area() const override {
        return side * side;
    }
    
    string getName() const override {
        return "Square";
    }
};

// ============= BAD EXAMPLE 2 - Bird Hierarchy =============

class BirdBad {
public:
    virtual ~BirdBad() = default;
    virtual void fly() {
        cout << "Bird is flying" << endl;
    }
};

class SparrowBad : public BirdBad {
public:
    void fly() override {
        cout << "Sparrow is flying" << endl;
    }
};

class PenguinBad : public BirdBad {
public:
    void fly() override {
        // Violates LSP: Penguins can't fly!
        throw runtime_error("Penguins can't fly!");
    }
};

void makeBirdFly(BirdBad& bird) {
    bird.fly();  // Will throw exception for Penguin!
}

// ============= GOOD EXAMPLE 2 - Proper Interface Segregation =============

class Bird {
public:
    virtual ~Bird() = default;
    virtual void eat() {
        cout << "Bird is eating" << endl;
    }
    virtual string getName() const = 0;
};

class FlyingBird : public Bird {
public:
    virtual void fly() = 0;
};

class SwimmingBird : public Bird {
public:
    virtual void swim() = 0;
};

class Sparrow : public FlyingBird {
public:
    void fly() override {
        cout << "Sparrow is flying" << endl;
    }
    
    string getName() const override {
        return "Sparrow";
    }
};

class Penguin : public SwimmingBird {
public:
    void swim() override {
        cout << "Penguin is swimming" << endl;
    }
    
    string getName() const override {
        return "Penguin";
    }
};

class Duck : public FlyingBird, public SwimmingBird {
public:
    void fly() override {
        cout << "Duck is flying" << endl;
    }
    
    void swim() override {
        cout << "Duck is swimming" << endl;
    }
    
    string getName() const override {
        return "Duck";
    }
};

// ============= BAD EXAMPLE 3 - Account Hierarchy =============

class AccountBad {
protected:
    double balance;
    
public:
    AccountBad(double bal) : balance(bal) {}
    virtual ~AccountBad() = default;
    
    virtual void withdraw(double amount) {
        if (amount <= balance) {
            balance -= amount;
            cout << "Withdrew $" << amount << endl;
        } else {
            cout << "Insufficient funds" << endl;
        }
    }
    
    double getBalance() const { return balance; }
};

class FixedDepositAccountBad : public AccountBad {
public:
    FixedDepositAccountBad(double bal) : AccountBad(bal) {}
    
    // Violates LSP: Can't withdraw from fixed deposit!
    void withdraw(double amount) override {
        throw runtime_error("Cannot withdraw from fixed deposit!");
    }
};

// ============= GOOD EXAMPLE 3 - Proper Hierarchy =============

class BankAccount {
protected:
    double balance;
    string accountNumber;
    
public:
    BankAccount(const string& accNum, double bal) 
        : accountNumber(accNum), balance(bal) {}
    virtual ~BankAccount() = default;
    
    virtual void deposit(double amount) {
        balance += amount;
        cout << "Deposited $" << amount << endl;
    }
    
    double getBalance() const { return balance; }
    string getAccountNumber() const { return accountNumber; }
};

class WithdrawableAccount : public BankAccount {
public:
    WithdrawableAccount(const string& accNum, double bal) 
        : BankAccount(accNum, bal) {}
    
    virtual void withdraw(double amount) {
        if (amount <= balance) {
            balance -= amount;
            cout << "Withdrew $" << amount << endl;
        } else {
            cout << "Insufficient funds" << endl;
        }
    }
};

class SavingsAccount : public WithdrawableAccount {
public:
    SavingsAccount(const string& accNum, double bal) 
        : WithdrawableAccount(accNum, bal) {}
};

class FixedDepositAccount : public BankAccount {
private:
    int maturityMonths;
    
public:
    FixedDepositAccount(const string& accNum, double bal, int months)
        : BankAccount(accNum, bal), maturityMonths(months) {}
    
    int getMaturityMonths() const { return maturityMonths; }
    
    void breakDeposit() {
        cout << "Breaking FD with penalty" << endl;
        // Calculate penalty and allow withdrawal
    }
};

// ============= EXAMPLE 4 - Collection Hierarchy =============

class ReadOnlyCollection {
protected:
    vector<int> items;
    
public:
    virtual ~ReadOnlyCollection() = default;
    
    virtual int get(size_t index) const {
        return items.at(index);
    }
    
    virtual size_t size() const {
        return items.size();
    }
};

class MutableCollection : public ReadOnlyCollection {
public:
    virtual void add(int item) {
        items.push_back(item);
    }
    
    virtual void remove(size_t index) {
        if (index < items.size()) {
            items.erase(items.begin() + index);
        }
    }
};

int main() {
    cout << "=== LISKOV SUBSTITUTION PRINCIPLE (LSP) ===" << endl;
    
    // Example 1: Rectangle/Square Problem
    cout << "\n1. RECTANGLE/SQUARE PROBLEM:" << endl;
    cout << "=============================" << endl;
    
    cout << "\n[BAD - Violates LSP]" << endl;
    RectangleBad rect1;
    SquareBad square1;
    
    cout << "Testing Rectangle:" << endl;
    testRectangle(rect1);
    
    cout << "\nTesting Square (passed as Rectangle):" << endl;
    testRectangle(square1);  // Breaks expectation!
    
    cout << "\n[GOOD - Follows LSP]" << endl;
    Rectangle rect2(5, 4);
    Square square2(5);
    
    cout << "Rectangle area: " << rect2.area() << endl;
    cout << "Square area: " << square2.area() << endl;
    cout << "Both correctly implement Shape interface" << endl;
    
    // Example 2: Bird Hierarchy
    cout << "\n\n2. BIRD HIERARCHY:" << endl;
    cout << "==================" << endl;
    
    cout << "\n[BAD - Violates LSP]" << endl;
    SparrowBad sparrowBad;
    PenguinBad penguinBad;
    
    try {
        makeBirdFly(sparrowBad);
        makeBirdFly(penguinBad);  // EXCEPTION!
    } catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
    }
    
    cout << "\n[GOOD - Follows LSP]" << endl;
    Sparrow sparrow;
    Penguin penguin;
    Duck duck;
    
    sparrow.fly();
    penguin.swim();  // Each bird does what it can
    duck.fly();
    duck.swim();
    
    // Example 3: Account Hierarchy  
    cout << "\n\n3. BANK ACCOUNT HIERARCHY:" << endl;
    cout << "==========================" << endl;
    
    cout << "\n[GOOD - Follows LSP]" << endl;
    SavingsAccount savings("SAV001", 1000);
    FixedDepositAccount fixedDeposit("FD001", 5000, 12);
    
    cout << "Savings account balance: $" << savings.getBalance() << endl;
    savings.withdraw(200);
    cout << "After withdrawal: $" << savings.getBalance() << endl;
    
    cout << "\nFixed deposit balance: $" << fixedDeposit.getBalance() << endl;
    cout << "Maturity: " << fixedDeposit.getMaturityMonths() << " months" << endl;
    // No withdraw method - compile-time safety!
    
    // Example 4: Collection Hierarchy
    cout << "\n\n4. COLLECTION HIERARCHY:" << endl;
    cout << "========================" << endl;
    
    MutableCollection collection;
    collection.add(10);
    collection.add(20);
    collection.add(30);
    
    cout << "Collection size: " << collection.size() << endl;
    cout << "First element: " << collection.get(0) << endl;
    
    ReadOnlyCollection& readOnly = collection;
    cout << "Through read-only interface: " << readOnly.get(1) << endl;
    // readOnly.add(40);  // Compile error - good!
    
    cout << "\n\n=== KEY TAKEAWAYS ===" << endl;
    cout << "1. Subtypes must be substitutable for base types" << endl;
    cout << "2. Subclass shouldn't break parent's contract" << endl;
    cout << "3. Don't strengthen preconditions" << endl;
    cout << "4. Don't weaken postconditions" << endl;
    cout << "5. Preserve invariants" << endl;
    
    cout << "\nCommon LSP Violations:" << endl;
    cout << "✗ Throwing new exceptions in subclass" << endl;
    cout << "✗ Changing return types unexpectedly" << endl;
    cout << "✗ Removing functionality (Square.setWidth/Height)" << endl;
    cout << "✗ Changing behavior drastically" << endl;
    cout << "✗ Type checking (instanceof)" << endl;
    
    cout << "\nHow to follow LSP:" << endl;
    cout << "✓ Design by contract" << endl;
    cout << "✓ Use proper abstractions" << endl;
    cout << "✓ Favor composition over inheritance" << endl;
    cout << "✓ Make interfaces cohesive" << endl;
    cout << "✓ Test substitutability" << endl;
    
    return 0;
}

