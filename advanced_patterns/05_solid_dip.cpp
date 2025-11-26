/*
 * DEPENDENCY INVERSION PRINCIPLE (DIP)
 * Fifth of the SOLID Principles
 * 
 * DEFINITION:
 * "High-level modules should not depend on low-level modules.
 *  Both should depend on abstractions.
 *  Abstractions should not depend on details.
 *  Details should depend on abstractions."
 * - Robert C. Martin
 * 
 * SIMPLIFIED:
 * "Depend on abstractions, not on concretions."
 * 
 * PURPOSE:
 * - Decouple high-level and low-level modules
 * - Make code more flexible and testable
 * - Enable dependency injection
 * - Invert the traditional dependency direction
 * 
 * BENEFITS:
 * + Loose coupling
 * + Easy to test (mock dependencies)
 * + Easy to extend
 * + Flexible system architecture
 * 
 * VIOLATIONS:
 * - Creating dependencies with 'new' inside class
 * - Depending on concrete classes instead of interfaces
 * - Hard-coded dependencies
 */

#include <iostream>
#include <string>
#include <memory>
#include <vector>

using namespace std;

// ============= BAD EXAMPLE - Tight Coupling =============

class MySQLDatabaseBad {
public:
    void connect() {
        cout << "Connected to MySQL" << endl;
    }
    
    void executeQuery(const string& query) {
        cout << "Executing on MySQL: " << query << endl;
    }
};

class UserRepositoryBad {
private:
    MySQLDatabaseBad database;  // Tight coupling!
    
public:
    void save(const string& user) {
        database.connect();
        database.executeQuery("INSERT INTO users VALUES ('" + user + "')");
    }
    // Can't switch to PostgreSQL without changing this class!
};

// ============= GOOD EXAMPLE - Dependency Inversion =============

// Abstraction
class Database {
public:
    virtual ~Database() = default;
    virtual void connect() = 0;
    virtual void executeQuery(const string& query) = 0;
};

// Low-level modules depend on abstraction
class MySQLDatabase : public Database {
public:
    void connect() override {
        cout << "Connected to MySQL" << endl;
    }
    
    void executeQuery(const string& query) override {
        cout << "[MySQL] " << query << endl;
    }
};

class PostgreSQLDatabase : public Database {
public:
    void connect() override {
        cout << "Connected to PostgreSQL" << endl;
    }
    
    void executeQuery(const string& query) override {
        cout << "[PostgreSQL] " << query << endl;
    }
};

class MongoDBDatabase : public Database {
public:
    void connect() override {
        cout << "Connected to MongoDB" << endl;
    }
    
    void executeQuery(const string& query) override {
        cout << "[MongoDB] " << query << endl;
    }
};

// High-level module depends on abstraction
class UserRepository {
private:
    shared_ptr<Database> database;
    
public:
    // Dependency Injection through constructor
    UserRepository(shared_ptr<Database> db) : database(db) {}
    
    void save(const string& user) {
        database->connect();
        database->executeQuery("INSERT INTO users VALUES ('" + user + "')");
    }
    
    // Can switch databases easily!
};

// ============= EXAMPLE 2: Notification System =============

// BAD
class EmailSenderBad {
public:
    void sendEmail(const string& message) {
        cout << "Sending email: " << message << endl;
    }
};

class NotificationServiceBad {
private:
    EmailSenderBad emailSender;  // Tight coupling!
    
public:
    void notify(const string& message) {
        emailSender.sendEmail(message);
    }
    // Can't add SMS without changing this class!
};

// GOOD
class MessageSender {
public:
    virtual ~MessageSender() = default;
    virtual void send(const string& message) = 0;
};

class EmailSender : public MessageSender {
public:
    void send(const string& message) override {
        cout << "📧 Email: " << message << endl;
    }
};

class SMSSender : public MessageSender {
public:
    void send(const string& message) override {
        cout << "📱 SMS: " << message << endl;
    }
};

class PushNotificationSender : public MessageSender {
public:
    void send(const string& message) override {
        cout << "🔔 Push: " << message << endl;
    }
};

class NotificationService {
private:
    vector<shared_ptr<MessageSender>> senders;
    
public:
    void addSender(shared_ptr<MessageSender> sender) {
        senders.push_back(sender);
    }
    
    void notify(const string& message) {
        for (auto& sender : senders) {
            sender->send(message);
        }
    }
};

// ============= EXAMPLE 3: Payment Processing =============

// BAD
class CreditCardProcessorBad {
public:
    void processPayment(double amount) {
        cout << "Processing $" << amount << " via credit card" << endl;
    }
};

class OrderServiceBad {
private:
    CreditCardProcessorBad processor;  // Tight coupling!
    
public:
    void checkout(double amount) {
        processor.processPayment(amount);
    }
};

// GOOD
class PaymentProcessor {
public:
    virtual ~PaymentProcessor() = default;
    virtual void processPayment(double amount) = 0;
};

class CreditCardProcessor : public PaymentProcessor {
public:
    void processPayment(double amount) override {
        cout << "💳 Credit Card: $" << amount << endl;
    }
};

class PayPalProcessor : public PaymentProcessor {
public:
    void processPayment(double amount) override {
        cout << "🅿️ PayPal: $" << amount << endl;
    }
};

class CryptoProcessor : public PaymentProcessor {
public:
    void processPayment(double amount) override {
        cout << "₿ Crypto: $" << amount << endl;
    }
};

class OrderService {
private:
    shared_ptr<PaymentProcessor> processor;
    
public:
    OrderService(shared_ptr<PaymentProcessor> proc) : processor(proc) {}
    
    void checkout(double amount) {
        cout << "Processing order..." << endl;
        processor->processPayment(amount);
        cout << "Order completed!" << endl;
    }
};

// ============= EXAMPLE 4: Logger System =============

// BAD
class FileLoggerBad {
public:
    void log(const string& message) {
        cout << "Logging to file: " << message << endl;
    }
};

class ApplicationBad {
private:
    FileLoggerBad logger;  // Tight coupling!
    
public:
    void run() {
        logger.log("Application started");
    }
};

// GOOD
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const string& message) = 0;
};

class ConsoleLogger : public ILogger {
public:
    void log(const string& message) override {
        cout << "[Console] " << message << endl;
    }
};

class FileLogger : public ILogger {
public:
    void log(const string& message) override {
        cout << "[File] " << message << endl;
    }
};

class DatabaseLogger : public ILogger {
public:
    void log(const string& message) override {
        cout << "[Database] " << message << endl;
    }
};

class Application {
private:
    shared_ptr<ILogger> logger;
    
public:
    Application(shared_ptr<ILogger> log) : logger(log) {}
    
    void run() {
        logger->log("Application started");
        logger->log("Processing...");
        logger->log("Application finished");
    }
};

int main() {
    cout << "=== DEPENDENCY INVERSION PRINCIPLE (DIP) ===" << endl;
    
    // Example 1: Database Repository
    cout << "\n1. DATABASE REPOSITORY:" << endl;
    cout << "=======================" << endl;
    
    cout << "\n[Using MySQL]" << endl;
    auto mysqlDb = make_shared<MySQLDatabase>();
    UserRepository repo1(mysqlDb);
    repo1.save("Alice");
    
    cout << "\n[Switched to PostgreSQL - no code change!]" << endl;
    auto postgresDb = make_shared<PostgreSQLDatabase>();
    UserRepository repo2(postgresDb);
    repo2.save("Bob");
    
    cout << "\n[Switched to MongoDB - still no code change!]" << endl;
    auto mongoDb = make_shared<MongoDBDatabase>();
    UserRepository repo3(mongoDb);
    repo3.save("Charlie");
    
    // Example 2: Notification System
    cout << "\n\n2. NOTIFICATION SYSTEM:" << endl;
    cout << "=======================" << endl;
    
    NotificationService notificationService;
    notificationService.addSender(make_shared<EmailSender>());
    notificationService.addSender(make_shared<SMSSender>());
    notificationService.addSender(make_shared<PushNotificationSender>());
    
    notificationService.notify("Server is down!");
    
    // Example 3: Payment Processing
    cout << "\n\n3. PAYMENT PROCESSING:" << endl;
    cout << "======================" << endl;
    
    cout << "\n[Credit Card Payment]" << endl;
    OrderService order1(make_shared<CreditCardProcessor>());
    order1.checkout(99.99);
    
    cout << "\n[PayPal Payment]" << endl;
    OrderService order2(make_shared<PayPalProcessor>());
    order2.checkout(149.99);
    
    cout << "\n[Crypto Payment]" << endl;
    OrderService order3(make_shared<CryptoProcessor>());
    order3.checkout(249.99);
    
    // Example 4: Logger System
    cout << "\n\n4. LOGGER SYSTEM:" << endl;
    cout << "=================" << endl;
    
    cout << "\n[Console Logger]" << endl;
    Application app1(make_shared<ConsoleLogger>());
    app1.run();
    
    cout << "\n[File Logger]" << endl;
    Application app2(make_shared<FileLogger>());
    app2.run();
    
    cout << "\n\n=== KEY TAKEAWAYS ===" << endl;
    cout << "1. Depend on abstractions, not concrete classes" << endl;
    cout << "2. High-level modules should not depend on low-level modules" << endl;
    cout << "3. Both should depend on abstractions" << endl;
    cout << "4. Use dependency injection (constructor, setter, interface)" << endl;
    cout << "5. Enables testing with mocks" << endl;
    
    cout << "\nRed Flags (DIP Violations):" << endl;
    cout << "✗ 'new' keyword in high-level classes" << endl;
    cout << "✗ Direct instantiation of dependencies" << endl;
    cout << "✗ Depending on concrete classes" << endl;
    cout << "✗ Hard-coded dependencies" << endl;
    
    cout << "\nHow to follow DIP:" << endl;
    cout << "✓ Create interfaces/abstract classes" << endl;
    cout << "✓ Use dependency injection" << endl;
    cout << "✓ Factory patterns for object creation" << endl;
    cout << "✓ Inversion of Control (IoC) containers" << endl;
    cout << "✓ Think about abstractions first" << endl;
    
    cout << "\nBenefits:" << endl;
    cout << "✓ Easy to test (use mocks)" << endl;
    cout << "✓ Easy to extend (new implementations)" << endl;
    cout << "✓ Loose coupling" << endl;
    cout << "✓ Flexible architecture" << endl;
    cout << "✓ Reusable components" << endl;
    
    return 0;
}

