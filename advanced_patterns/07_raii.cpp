/*
 * RAII - Resource Acquisition Is Initialization
 * 
 * DEFINITION:
 * "Resource acquisition is initialization" - resources are acquired in
 * constructor and released in destructor automatically.
 * 
 * CORE PRINCIPLE:
 * Tie resource lifetime to object lifetime.
 * Constructor acquires, destructor releases.
 * 
 * PURPOSE:
 * - Automatic resource management
 * - Exception-safe code
 * - No resource leaks
 * - Deterministic cleanup
 * 
 * BENEFITS:
 * + No manual cleanup needed
 * + Exception-safe
 * + No resource leaks
 * + Clear ownership
 * + Compiler enforced
 * 
 * EXAMPLES:
 * - Smart pointers (unique_ptr, shared_ptr)
 * - File handles (fstream)
 * - Mutexes (lock_guard)
 * - Database connections
 */

#include <iostream>
#include <fstream>
#include <memory>
#include <mutex>
#include <string>

using namespace std;

// ============= BAD - Manual Resource Management =============

class FileBad {
private:
    FILE* file;
    
public:
    FileBad(const string& filename) {
        file = fopen(filename.c_str(), "w");
        if (!file) {
            throw runtime_error("Cannot open file");
        }
    }
    
    void write(const string& data) {
        if (file) {
            fprintf(file, "%s", data.c_str());
        }
    }
    
    // User must remember to call this!
    void close() {
        if (file) {
            fclose(file);
            file = nullptr;
        }
    }
    
    // If exception thrown, file never closed!
};

// ============= GOOD - RAII =============

class File {
private:
    FILE* file;
    
public:
    File(const string& filename) {
        file = fopen(filename.c_str(), "w");
        if (!file) {
            throw runtime_error("Cannot open file");
        }
        cout << "File opened: " << filename << endl;
    }
    
    ~File() {
        if (file) {
            fclose(file);
            file = nullptr;
            cout << "File closed automatically" << endl;
        }
    }
    
    // Delete copy operations
    File(const File&) = delete;
    File& operator=(const File&) = delete;
    
    void write(const string& data) {
        if (file) {
            fprintf(file, "%s", data.c_str());
        }
    }
};

// ============= Example: Database Connection =============

class DatabaseConnection {
private:
    string connectionString;
    bool connected;
    
public:
    DatabaseConnection(const string& connStr) 
        : connectionString(connStr), connected(false) {
        // Acquire resource
        cout << "Connecting to database: " << connectionString << endl;
        connected = true;
    }
    
    ~DatabaseConnection() {
        // Release resource
        if (connected) {
            cout << "Disconnecting from database" << endl;
            connected = false;
        }
    }
    
    DatabaseConnection(const DatabaseConnection&) = delete;
    DatabaseConnection& operator=(const DatabaseConnection&) = delete;
    
    void executeQuery(const string& query) {
        if (connected) {
            cout << "Executing: " << query << endl;
        }
    }
};

// ============= Example: Mutex Lock Guard =============

class MutexLock {
private:
    mutex& mtx;
    
public:
    explicit MutexLock(mutex& m) : mtx(m) {
        mtx.lock();
        cout << "Mutex locked" << endl;
    }
    
    ~MutexLock() {
        mtx.unlock();
        cout << "Mutex unlocked" << endl;
    }
    
    MutexLock(const MutexLock&) = delete;
    MutexLock& operator=(const MutexLock&) = delete;
};

// ============= Example: Timer/Profiler =============

class ScopedTimer {
private:
    string name;
    
public:
    ScopedTimer(const string& n) : name(n) {
        cout << "⏱️  [" << name << "] Started" << endl;
    }
    
    ~ScopedTimer() {
        cout << "⏱️  [" << name << "] Completed" << endl;
    }
};

// ============= Example: Memory Manager =============

class MemoryBlock {
private:
    int* data;
    size_t size;
    
public:
    MemoryBlock(size_t s) : size(s) {
        data = new int[size];
        cout << "Allocated " << size << " integers" << endl;
    }
    
    ~MemoryBlock() {
        delete[] data;
        cout << "Deallocated " << size << " integers" << endl;
    }
    
    MemoryBlock(const MemoryBlock&) = delete;
    MemoryBlock& operator=(const MemoryBlock&) = delete;
    
    // Move semantics
    MemoryBlock(MemoryBlock&& other) noexcept 
        : data(other.data), size(other.size) {
        other.data = nullptr;
        other.size = 0;
    }
    
    int& operator[](size_t index) {
        return data[index];
    }
};

// ============= Demonstration Functions =============

void demonstrateFileRAII() {
    cout << "\n=== File RAII ===" << endl;
    try {
        File file("test.txt");
        file.write("Hello RAII!");
        file.write(" This is safe.");
        // File automatically closed when scope ends
    } catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
    }
    cout << "File guaranteed to be closed" << endl;
}

void demonstrateDatabaseRAII() {
    cout << "\n=== Database RAII ===" << endl;
    {
        DatabaseConnection db("server:3306/mydb");
        db.executeQuery("SELECT * FROM users");
        db.executeQuery("INSERT INTO logs VALUES (1, 'test')");
        // Connection automatically closed
    }
    cout << "Database connection guaranteed to be closed" << endl;
}

void demonstrateMutexRAII() {
    cout << "\n=== Mutex RAII ===" << endl;
    mutex mtx;
    {
        MutexLock lock(mtx);
        cout << "Critical section - thread safe" << endl;
        // Mutex automatically unlocked
    }
    cout << "Mutex guaranteed to be unlocked" << endl;
}

void demonstrateTimerRAII() {
    cout << "\n=== Timer RAII ===" << endl;
    {
        ScopedTimer timer("Database Operation");
        cout << "Performing database operation..." << endl;
        cout << "Processing data..." << endl;
        // Timer automatically reports on destruction
    }
}

void demonstrateMemoryRAII() {
    cout << "\n=== Memory RAII ===" << endl;
    {
        MemoryBlock mem(100);
        mem[0] = 42;
        mem[1] = 100;
        cout << "mem[0] = " << mem[0] << endl;
        // Memory automatically freed
    }
    cout << "Memory guaranteed to be freed" << endl;
}

void demonstrateSmartPointers() {
    cout << "\n=== Smart Pointers (Built-in RAII) ===" << endl;
    
    // unique_ptr
    {
        unique_ptr<int> ptr = make_unique<int>(42);
        cout << "unique_ptr value: " << *ptr << endl;
        // Automatically deleted
    }
    cout << "unique_ptr memory freed" << endl;
    
    // shared_ptr
    {
        shared_ptr<string> str1 = make_shared<string>("RAII");
        {
            shared_ptr<string> str2 = str1;
            cout << "shared_ptr count: " << str1.use_count() << endl;
        }
        cout << "shared_ptr count: " << str1.use_count() << endl;
        // Deleted when last shared_ptr goes out of scope
    }
    cout << "shared_ptr memory freed" << endl;
}

int main() {
    cout << "=== RAII - RESOURCE ACQUISITION IS INITIALIZATION ===" << endl;
    
    demonstrateFileRAII();
    demonstrateDatabaseRAII();
    demonstrateMutexRAII();
    demonstrateTimerRAII();
    demonstrateMemoryRAII();
    demonstrateSmartPointers();
    
    cout << "\n\n=== KEY TAKEAWAYS ===" << endl;
    cout << "1. RAII ties resource lifetime to object lifetime" << endl;
    cout << "2. Constructor acquires, destructor releases" << endl;
    cout << "3. Automatic cleanup - no manual release needed" << endl;
    cout << "4. Exception-safe by design" << endl;
    cout << "5. Compiler-enforced resource management" << endl;
    
    cout << "\nRAII Examples in C++ Standard Library:" << endl;
    cout << "• unique_ptr, shared_ptr - memory" << endl;
    cout << "• lock_guard, unique_lock - mutexes" << endl;
    cout << "• fstream - files" << endl;
    cout << "• vector, string - dynamic arrays" << endl;
    
    cout << "\nBenefits:" << endl;
    cout << "✓ No resource leaks" << endl;
    cout << "✓ Exception-safe" << endl;
    cout << "✓ Clear ownership" << endl;
    cout << "✓ Deterministic cleanup" << endl;
    cout << "✓ Less error-prone" << endl;
    
    cout << "\nRAII Guidelines:" << endl;
    cout << "• Acquire in constructor" << endl;
    cout << "• Release in destructor" << endl;
    cout << "• Delete copy operations (or implement properly)" << endl;
    cout << "• Consider move semantics" << endl;
    cout << "• Make RAII classes own ONE resource" << endl;
    
    return 0;
}

