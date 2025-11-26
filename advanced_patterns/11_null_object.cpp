/*
 * NULL OBJECT PATTERN
 * 
 * DEFINITION:
 * Provide a default "do-nothing" object instead of null/nullptr.
 * Eliminates the need for null checks throughout the code.
 * 
 * PURPOSE:
 * - Eliminate null checks
 * - Provide default behavior
 * - Simplify client code
 * - Follow "Tell, Don't Ask" principle
 * 
 * BENEFITS:
 * + No null pointer checks needed
 * + Cleaner code
 * + Prevents NullPointerException crashes
 * + Polymorphic with real objects
 * + Default behavior built-in
 * 
 * WHEN TO USE:
 * - Many null checks in code
 * - Default "do nothing" behavior makes sense
 * - Want to eliminate special cases
 */

#include <iostream>
#include <string>
#include <memory>
#include <vector>

using namespace std;

// ============= Example 1: Logger =============

// Abstract logger interface
class Logger {
public:
    virtual ~Logger() = default;
    virtual void log(const string& message) = 0;
    virtual void error(const string& message) = 0;
    virtual void warning(const string& message) = 0;
};

// Real logger
class ConsoleLogger : public Logger {
public:
    void log(const string& message) override {
        cout << "[LOG] " << message << endl;
    }
    
    void error(const string& message) override {
        cout << "[ERROR] " << message << endl;
    }
    
    void warning(const string& message) override {
        cout << "[WARNING] " << message << endl;
    }
};

// Null Object - Does nothing (no-op)
class NullLogger : public Logger {
public:
    void log(const string&) override {
        // Do nothing
    }
    
    void error(const string&) override {
        // Do nothing
    }
    
    void warning(const string&) override {
        // Do nothing
    }
};

// Client code
class Application {
private:
    shared_ptr<Logger> logger;
    
public:
    Application(shared_ptr<Logger> log) : logger(log) {}
    
    void run() {
        // No null checks needed!
        logger->log("Application started");
        logger->warning("Low memory");
        logger->error("Failed to connect");
        logger->log("Application finished");
    }
};

// ============= Example 2: Customer =============

class Customer {
public:
    virtual ~Customer() = default;
    virtual string getName() const = 0;
    virtual double getDiscount() const = 0;
    virtual void addOrder(double amount) = 0;
    virtual bool isNull() const { return false; }
};

class RealCustomer : public Customer {
private:
    string name;
    double totalSpent;
    
public:
    RealCustomer(const string& n) : name(n), totalSpent(0) {}
    
    string getName() const override {
        return name;
    }
    
    double getDiscount() const override {
        if (totalSpent > 1000) return 0.1;
        if (totalSpent > 500) return 0.05;
        return 0.0;
    }
    
    void addOrder(double amount) override {
        totalSpent += amount;
        cout << name << " ordered $" << amount << endl;
    }
};

class NullCustomer : public Customer {
public:
    string getName() const override {
        return "Guest";
    }
    
    double getDiscount() const override {
        return 0.0;
    }
    
    void addOrder(double) override {
        // Do nothing
    }
    
    bool isNull() const override {
        return true;
    }
};

// Factory
class CustomerFactory {
private:
    static inline shared_ptr<Customer> nullCustomer = make_shared<NullCustomer>();
    
public:
    static shared_ptr<Customer> getCustomer(const string& name) {
        if (name == "Alice") return make_shared<RealCustomer>("Alice");
        if (name == "Bob") return make_shared<RealCustomer>("Bob");
        return nullCustomer;  // Return null object instead of nullptr
    }
};

// ============= Example 3: Command Pattern with Null Command =============

class Command {
public:
    virtual ~Command() = default;
    virtual void execute() = 0;
};

class SaveCommand : public Command {
public:
    void execute() override {
        cout << "Saving file..." << endl;
    }
};

class PrintCommand : public Command {
public:
    void execute() override {
        cout << "Printing document..." << endl;
    }
};

class NullCommand : public Command {
public:
    void execute() override {
        // Do nothing
    }
};

class CommandProcessor {
private:
    shared_ptr<Command> lastCommand;
    static inline shared_ptr<Command> nullCommand = make_shared<NullCommand>();
    
public:
    CommandProcessor() : lastCommand(nullCommand) {}
    
    void setCommand(shared_ptr<Command> cmd) {
        lastCommand = cmd ? cmd : nullCommand;
    }
    
    void executeLastCommand() {
        // No null check needed!
        lastCommand->execute();
    }
};

// ============= Example 4: Animal Sound =============

class Animal {
public:
    virtual ~Animal() = default;
    virtual void makeSound() = 0;
    virtual string getName() const = 0;
};

class Dog : public Animal {
public:
    void makeSound() override {
        cout << "Woof!" << endl;
    }
    
    string getName() const override {
        return "Dog";
    }
};

class Cat : public Animal {
public:
    void makeSound() override {
        cout << "Meow!" << endl;
    }
    
    string getName() const override {
        return "Cat";
    }
};

class NullAnimal : public Animal {
public:
    void makeSound() override {
        // Silent
    }
    
    string getName() const override {
        return "No Animal";
    }
};

// ============= Example 5: Database Connection =============

class DatabaseConnection {
public:
    virtual ~DatabaseConnection() = default;
    virtual void connect() = 0;
    virtual void disconnect() = 0;
    virtual void executeQuery(const string& query) = 0;
};

class RealDatabaseConnection : public DatabaseConnection {
public:
    void connect() override {
        cout << "Connected to database" << endl;
    }
    
    void disconnect() override {
        cout << "Disconnected from database" << endl;
    }
    
    void executeQuery(const string& query) override {
        cout << "Executing: " << query << endl;
    }
};

class NullDatabaseConnection : public DatabaseConnection {
public:
    void connect() override {
        // Do nothing - already "connected" to nothing
    }
    
    void disconnect() override {
        // Do nothing
    }
    
    void executeQuery(const string&) override {
        // Do nothing - offline mode
    }
};

int main() {
    cout << "=== NULL OBJECT PATTERN ===" << endl;
    
    // Example 1: Logger
    cout << "\n1. LOGGER EXAMPLE:" << endl;
    cout << "==================" << endl;
    
    cout << "\n[With Console Logger]" << endl;
    Application app1(make_shared<ConsoleLogger>());
    app1.run();
    
    cout << "\n[With Null Logger - Silent mode]" << endl;
    Application app2(make_shared<NullLogger>());
    app2.run();
    cout << "No output - logging disabled!" << endl;
    
    // Example 2: Customer
    cout << "\n\n2. CUSTOMER EXAMPLE:" << endl;
    cout << "====================" << endl;
    
    auto customer1 = CustomerFactory::getCustomer("Alice");
    cout << "Customer: " << customer1->getName() 
         << ", Discount: " << customer1->getDiscount() * 100 << "%" << endl;
    customer1->addOrder(600);
    
    auto customer2 = CustomerFactory::getCustomer("Unknown");
    cout << "Customer: " << customer2->getName() 
         << ", Discount: " << customer2->getDiscount() * 100 << "%" << endl;
    customer2->addOrder(100);  // Does nothing
    
    // Example 3: Command
    cout << "\n\n3. COMMAND EXAMPLE:" << endl;
    cout << "===================" << endl;
    
    CommandProcessor processor;
    processor.executeLastCommand();  // Null command - does nothing
    
    processor.setCommand(make_shared<SaveCommand>());
    processor.executeLastCommand();
    
    processor.setCommand(make_shared<PrintCommand>());
    processor.executeLastCommand();
    
    processor.setCommand(nullptr);  // Set to null - gets null object
    processor.executeLastCommand();  // Safe - does nothing
    
    // Example 4: Animal
    cout << "\n\n4. ANIMAL EXAMPLE:" << endl;
    cout << "==================" << endl;
    
    vector<shared_ptr<Animal>> animals;
    animals.push_back(make_shared<Dog>());
    animals.push_back(make_shared<Cat>());
    animals.push_back(make_shared<NullAnimal>());
    animals.push_back(make_shared<Dog>());
    
    cout << "All animals making sounds:" << endl;
    for (const auto& animal : animals) {
        cout << animal->getName() << ": ";
        animal->makeSound();  // No null checks needed!
    }
    
    // Example 5: Database
    cout << "\n\n5. DATABASE EXAMPLE:" << endl;
    cout << "====================" << endl;
    
    cout << "[Online mode]" << endl;
    shared_ptr<DatabaseConnection> db1 = make_shared<RealDatabaseConnection>();
    db1->connect();
    db1->executeQuery("SELECT * FROM users");
    db1->disconnect();
    
    cout << "\n[Offline mode]" << endl;
    shared_ptr<DatabaseConnection> db2 = make_shared<NullDatabaseConnection>();
    db2->connect();  // Does nothing
    db2->executeQuery("SELECT * FROM users");  // Does nothing
    db2->disconnect();  // Does nothing
    cout << "Operations silently ignored in offline mode" << endl;
    
    cout << "\n\n=== KEY TAKEAWAYS ===" << endl;
    cout << "1. Null Object provides default 'do-nothing' behavior" << endl;
    cout << "2. Eliminates null checks throughout code" << endl;
    cout << "3. Implements same interface as real objects" << endl;
    cout << "4. Returns null object instead of nullptr" << endl;
    cout << "5. Makes code cleaner and safer" << endl;
    
    cout << "\nStructure:" << endl;
    cout << "• Abstract interface" << endl;
    cout << "• Real implementation(s)" << endl;
    cout << "• Null Object implementation (no-op)" << endl;
    cout << "• Factory returns null object instead of nullptr" << endl;
    
    cout << "\nBenefits:" << endl;
    cout << "✓ No null pointer exceptions" << endl;
    cout << "✓ Cleaner code (no if-null checks)" << endl;
    cout << "✓ Default behavior built-in" << endl;
    cout << "✓ Follows polymorphism" << endl;
    cout << "✓ Easy to test" << endl;
    
    cout << "\nWhen to use:" << endl;
    cout << "• Many null checks in code" << endl;
    cout << "• Default behavior makes sense" << endl;
    cout << "• Want optional functionality" << endl;
    cout << "• Testing with mock objects" << endl;
    
    cout << "\nReal-world examples:" << endl;
    cout << "• Logging systems (disable logging)" << endl;
    cout << "• Missing customers/users" << endl;
    cout << "• Offline mode" << endl;
    cout << "• Default commands" << endl;
    cout << "• Silent handlers" << endl;
    
    return 0;
}

