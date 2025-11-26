/*
 * INTERFACE SEGREGATION PRINCIPLE (ISP)
 * Fourth of the SOLID Principles
 * 
 * DEFINITION:
 * "No client should be forced to depend on methods it does not use."
 * - Robert C. Martin
 * 
 * SIMPLIFIED:
 * "Many specific interfaces are better than one general-purpose interface."
 * Split large interfaces into smaller, more specific ones.
 * 
 * PURPOSE:
 * - Reduce coupling between classes
 * - Avoid "fat" interfaces
 * - Make interfaces cohesive
 * - Clients only depend on what they need
 * 
 * BENEFITS:
 * + Reduces unnecessary dependencies
 * + Makes code more flexible
 * + Easier to implement
 * + Better separation of concerns
 * + Prevents interface pollution
 * 
 * VIOLATIONS:
 * - Fat interfaces with many methods
 * - Forcing clients to implement unused methods
 * - God interfaces
 */

#include <iostream>
#include <string>

using namespace std;

// ============= BAD EXAMPLE - Fat Interface =============

class WorkerBad {
public:
    virtual ~WorkerBad() = default;
    virtual void work() = 0;
    virtual void eat() = 0;
    virtual void sleep() = 0;
    virtual void getSalary() = 0;
};

class HumanWorkerBad : public WorkerBad {
public:
    void work() override {
        cout << "Human working" << endl;
    }
    
    void eat() override {
        cout << "Human eating lunch" << endl;
    }
    
    void sleep() override {
        cout << "Human sleeping" << endl;
    }
    
    void getSalary() override {
        cout << "Human getting salary" << endl;
    }
};

class RobotWorkerBad : public WorkerBad {
public:
    void work() override {
        cout << "Robot working" << endl;
    }
    
    // Forced to implement methods it doesn't need!
    void eat() override {
        // Robots don't eat!
        throw runtime_error("Robots don't eat!");
    }
    
    void sleep() override {
        // Robots don't sleep!
        throw runtime_error("Robots don't sleep!");
    }
    
    void getSalary() override {
        // Robots don't get salaries!
        throw runtime_error("Robots don't get salaries!");
    }
};

// ============= GOOD EXAMPLE - Segregated Interfaces =============

class Workable {
public:
    virtual ~Workable() = default;
    virtual void work() = 0;
};

class Eatable {
public:
    virtual ~Eatable() = default;
    virtual void eat() = 0;
};

class Sleepable {
public:
    virtual ~Sleepable() = default;
    virtual void sleep() = 0;
};

class Payable {
public:
    virtual ~Payable() = default;
    virtual void getSalary() = 0;
};

class HumanWorker : public Workable, public Eatable, 
                    public Sleepable, public Payable {
public:
    void work() override {
        cout << "Human working" << endl;
    }
    
    void eat() override {
        cout << "Human eating lunch" << endl;
    }
    
    void sleep() override {
        cout << "Human sleeping" << endl;
    }
    
    void getSalary() override {
        cout << "Human getting salary" << endl;
    }
};

class RobotWorker : public Workable {
public:
    void work() override {
        cout << "Robot working 24/7" << endl;
    }
    // Only implements what it needs!
};

// ============= EXAMPLE 2: Printer Interface =============

// BAD: Fat interface
class PrinterBad {
public:
    virtual ~PrinterBad() = default;
    virtual void print() = 0;
    virtual void scan() = 0;
    virtual void fax() = 0;
    virtual void photocopy() = 0;
};

class SimplePrinterBad : public PrinterBad {
public:
    void print() override {
        cout << "Printing..." << endl;
    }
    
    // Forced to implement features it doesn't have!
    void scan() override {
        throw runtime_error("Scan not supported!");
    }
    
    void fax() override {
        throw runtime_error("Fax not supported!");
    }
    
    void photocopy() override {
        throw runtime_error("Photocopy not supported!");
    }
};

// GOOD: Segregated interfaces
class Printer {
public:
    virtual ~Printer() = default;
    virtual void print() = 0;
};

class Scanner {
public:
    virtual ~Scanner() = default;
    virtual void scan() = 0;
};

class Fax {
public:
    virtual ~Fax() = default;
    virtual void fax() = 0;
};

class SimplePrinter : public Printer {
public:
    void print() override {
        cout << "Simple printer: Printing document" << endl;
    }
};

class MultiFunctionPrinter : public Printer, public Scanner, public Fax {
public:
    void print() override {
        cout << "MFP: Printing" << endl;
    }
    
    void scan() override {
        cout << "MFP: Scanning" << endl;
    }
    
    void fax() override {
        cout << "MFP: Sending fax" << endl;
    }
};

// ============= EXAMPLE 3: Database Operations =============

// BAD: God interface
class DatabaseBad {
public:
    virtual ~DatabaseBad() = default;
    virtual void connect() = 0;
    virtual void disconnect() = 0;
    virtual void executeQuery() = 0;
    virtual void backup() = 0;
    virtual void restore() = 0;
    virtual void optimize() = 0;
    virtual void migrate() = 0;
};

// GOOD: Segregated interfaces
class DatabaseConnection {
public:
    virtual ~DatabaseConnection() = default;
    virtual void connect() = 0;
    virtual void disconnect() = 0;
};

class QueryExecutor {
public:
    virtual ~QueryExecutor() = default;
    virtual void executeQuery() = 0;
};

class DatabaseBackup {
public:
    virtual ~DatabaseBackup() = default;
    virtual void backup() = 0;
    virtual void restore() = 0;
};

class DatabaseMaintenance {
public:
    virtual ~DatabaseMaintenance() = default;
    virtual void optimize() = 0;
    virtual void migrate() = 0;
};

class BasicDatabase : public DatabaseConnection, public QueryExecutor {
public:
    void connect() override {
        cout << "Connected to database" << endl;
    }
    
    void disconnect() override {
        cout << "Disconnected from database" << endl;
    }
    
    void executeQuery() override {
        cout << "Executing query" << endl;
    }
};

class AdvancedDatabase : public DatabaseConnection, public QueryExecutor,
                         public DatabaseBackup, public DatabaseMaintenance {
public:
    void connect() override {
        cout << "Advanced DB: Connected" << endl;
    }
    
    void disconnect() override {
        cout << "Advanced DB: Disconnected" << endl;
    }
    
    void executeQuery() override {
        cout << "Advanced DB: Query executed" << endl;
    }
    
    void backup() override {
        cout << "Advanced DB: Backup created" << endl;
    }
    
    void restore() override {
        cout << "Advanced DB: Restored from backup" << endl;
    }
    
    void optimize() override {
        cout << "Advanced DB: Optimized" << endl;
    }
    
    void migrate() override {
        cout << "Advanced DB: Migration completed" << endl;
    }
};

// ============= EXAMPLE 4: Vehicle Interface =============

// BAD
class VehicleBad {
public:
    virtual ~VehicleBad() = default;
    virtual void drive() = 0;
    virtual void fly() = 0;
    virtual void sail() = 0;
};

// GOOD
class Drivable {
public:
    virtual ~Drivable() = default;
    virtual void drive() = 0;
};

class Flyable {
public:
    virtual ~Flyable() = default;
    virtual void fly() = 0;
};

class Sailable {
public:
    virtual ~Sailable() = default;
    virtual void sail() = 0;
};

class Car : public Drivable {
public:
    void drive() override {
        cout << "Car driving on road" << endl;
    }
};

class Airplane : public Flyable {
public:
    void fly() override {
        cout << "Airplane flying in sky" << endl;
    }
};

class Boat : public Sailable {
public:
    void sail() override {
        cout << "Boat sailing on water" << endl;
    }
};

class AmphibiousVehicle : public Drivable, public Sailable {
public:
    void drive() override {
        cout << "Amphibious vehicle driving on land" << endl;
    }
    
    void sail() override {
        cout << "Amphibious vehicle sailing on water" << endl;
    }
};

int main() {
    cout << "=== INTERFACE SEGREGATION PRINCIPLE (ISP) ===" << endl;
    
    // Example 1: Worker Interface
    cout << "\n1. WORKER INTERFACES:" << endl;
    cout << "=====================" << endl;
    
    cout << "\n[GOOD - Segregated Interfaces]" << endl;
    HumanWorker human;
    RobotWorker robot;
    
    cout << "Human worker:" << endl;
    human.work();
    human.eat();
    human.sleep();
    human.getSalary();
    
    cout << "\nRobot worker:" << endl;
    robot.work();
    // Robot only implements what it needs!
    
    // Example 2: Printer Interface
    cout << "\n\n2. PRINTER INTERFACES:" << endl;
    cout << "======================" << endl;
    
    SimplePrinter simplePrinter;
    MultiFunctionPrinter mfp;
    
    cout << "Simple Printer:" << endl;
    simplePrinter.print();
    
    cout << "\nMulti-Function Printer:" << endl;
    mfp.print();
    mfp.scan();
    mfp.fax();
    
    // Example 3: Database Interface
    cout << "\n\n3. DATABASE INTERFACES:" << endl;
    cout << "=======================" << endl;
    
    BasicDatabase basicDb;
    AdvancedDatabase advancedDb;
    
    cout << "Basic Database:" << endl;
    basicDb.connect();
    basicDb.executeQuery();
    basicDb.disconnect();
    
    cout << "\nAdvanced Database:" << endl;
    advancedDb.connect();
    advancedDb.executeQuery();
    advancedDb.backup();
    advancedDb.optimize();
    advancedDb.disconnect();
    
    // Example 4: Vehicle Interface
    cout << "\n\n4. VEHICLE INTERFACES:" << endl;
    cout << "======================" << endl;
    
    Car car;
    Airplane plane;
    Boat boat;
    AmphibiousVehicle amphibious;
    
    car.drive();
    plane.fly();
    boat.sail();
    
    cout << "\nAmphibious vehicle:" << endl;
    amphibious.drive();
    amphibious.sail();
    
    cout << "\n\n=== KEY TAKEAWAYS ===" << endl;
    cout << "1. Many specific interfaces > one general interface" << endl;
    cout << "2. Clients shouldn't depend on unused methods" << endl;
    cout << "3. Split fat interfaces into smaller ones" << endl;
    cout << "4. Each interface should be cohesive" << endl;
    cout << "5. Reduces coupling and increases flexibility" << endl;
    
    cout << "\nRed Flags (ISP Violations):" << endl;
    cout << "✗ Empty/throwing method implementations" << endl;
    cout << "✗ Methods that don't make sense for class" << endl;
    cout << "✗ Interfaces with many unrelated methods" << endl;
    cout << "✗ Clients implementing unused methods" << endl;
    
    cout << "\nHow to follow ISP:" << endl;
    cout << "✓ Keep interfaces small and focused" << endl;
    cout << "✓ Use multiple inheritance when needed" << endl;
    cout << "✓ Split large interfaces" << endl;
    cout << "✓ Design role-based interfaces" << endl;
    cout << "✓ Think about clients, not implementations" << endl;
    
    return 0;
}

