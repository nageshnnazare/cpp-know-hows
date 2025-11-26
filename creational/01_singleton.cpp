/*
 * SINGLETON PATTERN - Creational Pattern
 * 
 * PURPOSE:
 * Ensures a class has only ONE instance and provides a global point of access to it.
 * 
 * WHEN TO USE:
 * - Need exactly one instance of a class (Database connection, Logger, Config manager)
 * - Need global access to that instance
 * - Instance should be created only when first needed (lazy initialization)
 * 
 * PROS:
 * + Controlled access to sole instance
 * + Reduced namespace pollution (better than global variables)
 * + Permits refinement of operations and representation
 * + Lazy initialization saves resources
 * 
 * CONS:
 * - Difficult to test (hidden dependencies)
 * - Violates Single Responsibility Principle
 * - Can introduce global state
 * - Thread safety concerns in multithreading
 * 
 * REAL-WORLD EXAMPLES:
 * - Logger systems
 * - Database connection pools
 * - Configuration managers
 * - Cache managers
 */

#include <iostream>
#include <string>
#include <mutex>
#include <memory>

using namespace std;

// Thread-safe Singleton implementation (Meyer's Singleton - C++11)
class Logger {
private:
    // Private constructor prevents external instantiation
    Logger() {
        cout << "Logger initialized!" << endl;
    }
    
    // Delete copy constructor and assignment operator
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
public:
    // Static method to access the single instance
    static Logger& getInstance() {
        // Thread-safe in C++11 and later
        // Static local variable is initialized only once
        static Logger instance;
        return instance;
    }
    
    void log(const string& message) {
        cout << "[LOG] " << message << endl;
    }
};

// Alternative: Thread-safe Singleton with explicit control
class DatabaseConnection {
private:
    static DatabaseConnection* instance;
    static mutex mtx;
    string connectionString;
    
    DatabaseConnection(const string& connStr) : connectionString(connStr) {
        cout << "Database connected: " << connectionString << endl;
    }
    
    DatabaseConnection(const DatabaseConnection&) = delete;
    DatabaseConnection& operator=(const DatabaseConnection&) = delete;
    
public:
    static DatabaseConnection* getInstance(const string& connStr = "localhost:5432") {
        // Double-checked locking pattern
        if (instance == nullptr) {
            lock_guard<mutex> lock(mtx);
            if (instance == nullptr) {
                instance = new DatabaseConnection(connStr);
            }
        }
        return instance;
    }
    
    void executeQuery(const string& query) {
        cout << "[DB] Executing: " << query << endl;
    }
    
    static void destroyInstance() {
        if (instance != nullptr) {
            delete instance;
            instance = nullptr;
        }
    }
};

// Initialize static members
DatabaseConnection* DatabaseConnection::instance = nullptr;
mutex DatabaseConnection::mtx;

// Smart pointer based Singleton (Modern C++ - Alternative approach)
class ConfigManager {
private:
    string appName;
    int maxConnections;
    
    ConfigManager() : appName("MyApp"), maxConnections(100) {
        cout << "ConfigManager initialized!" << endl;
    }
    
public:
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;
    
    // Meyer's Singleton - simplest and safest for modern C++
    static ConfigManager& getInstance() {
        static ConfigManager instance;
        return instance;
    }
    
    void setAppName(const string& name) { appName = name; }
    string getAppName() const { return appName; }
    int getMaxConnections() const { return maxConnections; }
};

// Demonstration
int main() {
    cout << "=== SINGLETON PATTERN DEMO ===" << endl << endl;
    
    // Example 1: Meyer's Singleton (Recommended for C++11+)
    cout << "1. Logger Singleton (Meyer's Singleton):" << endl;
    Logger& logger1 = Logger::getInstance();
    logger1.log("First log message");
    
    Logger& logger2 = Logger::getInstance();
    logger2.log("Second log message");
    
    // Both references point to the same instance
    cout << "logger1 and logger2 are same? " << (&logger1 == &logger2 ? "YES" : "NO") << endl;
    cout << endl;
    
    // Example 2: Classic Singleton with pointer
    cout << "2. Database Singleton (Classic with mutex):" << endl;
    DatabaseConnection* db1 = DatabaseConnection::getInstance("server1:3306");
    db1->executeQuery("SELECT * FROM users");
    
    DatabaseConnection* db2 = DatabaseConnection::getInstance("server2:3306"); // Won't create new
    db2->executeQuery("INSERT INTO users VALUES (1, 'John')");
    
    cout << "db1 and db2 are same? " << (db1 == db2 ? "YES" : "NO") << endl;
    DatabaseConnection::destroyInstance();
    cout << endl;
    
    // Example 3: Modern Singleton with smart pointers
    cout << "3. ConfigManager Singleton (Modern with unique_ptr):" << endl;
    ConfigManager& config1 = ConfigManager::getInstance();
    cout << "App name: " << config1.getAppName() << endl;
    
    config1.setAppName("UpdatedApp");
    
    ConfigManager& config2 = ConfigManager::getInstance();
    cout << "App name from config2: " << config2.getAppName() << endl;
    cout << "Max connections: " << config2.getMaxConnections() << endl;
    
    cout << "config1 and config2 are same? " << (&config1 == &config2 ? "YES" : "NO") << endl;
    
    cout << "\n=== KEY TAKEAWAYS ===" << endl;
    cout << "1. Only ONE instance exists throughout the program" << endl;
    cout << "2. Global access point via getInstance()" << endl;
    cout << "3. Lazy initialization (created when first needed)" << endl;
    cout << "4. Meyer's Singleton is thread-safe and recommended for C++11+" << endl;
    
    return 0;
}

