/*
 * OBJECT POOL PATTERN
 * 
 * DEFINITION:
 * Reuse expensive-to-create objects instead of creating new ones.
 * Maintain a pool of reusable objects.
 * 
 * PURPOSE:
 * - Improve performance by reusing objects
 * - Reduce memory allocation overhead
 * - Control number of instances
 * - Manage expensive resources
 * 
 * BENEFITS:
 * + Better performance (no repeated allocation)
 * + Controlled resource usage
 * + Predictable memory usage
 * + Reduced GC pressure
 * 
 * WHEN TO USE:
 * - Objects are expensive to create
 * - Need many short-lived objects
 * - Want to limit resource usage
 * - Database connections, threads, network sockets
 */

#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <queue>
#include <mutex>
#include <thread>
#include <chrono>

using namespace std;

// ============= Example 1: Simple Object Pool =============

class DatabaseConnection {
private:
    int id;
    bool inUse;
    
public:
    DatabaseConnection(int connId) : id(connId), inUse(false) {
        cout << "Creating expensive DatabaseConnection #" << id << endl;
        this_thread::sleep_for(chrono::milliseconds(100));  // Simulate expensive creation
    }
    
    void connect() {
        cout << "Connection #" << id << " connected" << endl;
    }
    
    void disconnect() {
        cout << "Connection #" << id << " disconnected" << endl;
    }
    
    void executeQuery(const string& query) {
        cout << "Connection #" << id << " executing: " << query << endl;
    }
    
    int getId() const { return id; }
    bool isInUse() const { return inUse; }
    void setInUse(bool use) { inUse = use; }
};

class ConnectionPool {
private:
    vector<unique_ptr<DatabaseConnection>> pool;
    queue<DatabaseConnection*> available;
    mutex mtx;
    size_t maxSize;
    
public:
    ConnectionPool(size_t size) : maxSize(size) {
        cout << "Creating connection pool with " << size << " connections..." << endl;
        for (size_t i = 0; i < size; ++i) {
            auto conn = make_unique<DatabaseConnection>(i + 1);
            available.push(conn.get());
            pool.push_back(move(conn));
        }
        cout << "Connection pool ready!" << endl;
    }
    
    DatabaseConnection* acquire() {
        lock_guard<mutex> lock(mtx);
        if (available.empty()) {
            cout << "No connections available!" << endl;
            return nullptr;
        }
        
        DatabaseConnection* conn = available.front();
        available.pop();
        conn->setInUse(true);
        cout << "Acquired connection #" << conn->getId() << endl;
        return conn;
    }
    
    void release(DatabaseConnection* conn) {
        lock_guard<mutex> lock(mtx);
        conn->setInUse(false);
        available.push(conn);
        cout << "Released connection #" << conn->getId() << " back to pool" << endl;
    }
    
    size_t getAvailableCount() const {
        return available.size();
    }
};

// ============= Example 2: Thread Pool =============

class Worker {
private:
    int id;
    
public:
    Worker(int workerId) : id(workerId) {
        cout << "Worker #" << id << " created" << endl;
    }
    
    void doWork(const string& task) {
        cout << "Worker #" << id << " processing: " << task << endl;
        this_thread::sleep_for(chrono::milliseconds(50));
    }
    
    int getId() const { return id; }
};

class ThreadPool {
private:
    vector<unique_ptr<Worker>> workers;
    queue<Worker*> available;
    mutex mtx;
    
public:
    ThreadPool(size_t size) {
        for (size_t i = 0; i < size; ++i) {
            auto worker = make_unique<Worker>(i + 1);
            available.push(worker.get());
            workers.push_back(move(worker));
        }
        cout << "Thread pool with " << size << " workers ready" << endl;
    }
    
    Worker* getWorker() {
        lock_guard<mutex> lock(mtx);
        if (available.empty()) return nullptr;
        
        Worker* worker = available.front();
        available.pop();
        return worker;
    }
    
    void returnWorker(Worker* worker) {
        lock_guard<mutex> lock(mtx);
        available.push(worker);
    }
};

// ============= Example 3: Memory Buffer Pool =============

class Buffer {
private:
    vector<char> data;
    size_t size;
    
public:
    Buffer(size_t s) : data(s), size(s) {
        cout << "Allocated buffer of " << size << " bytes" << endl;
    }
    
    void write(const string& content) {
        for (size_t i = 0; i < content.length() && i < size; ++i) {
            data[i] = content[i];
        }
    }
    
    void clear() {
        fill(data.begin(), data.end(), 0);
    }
    
    size_t getSize() const { return size; }
};

class BufferPool {
private:
    vector<unique_ptr<Buffer>> buffers;
    queue<Buffer*> available;
    size_t bufferSize;
    
public:
    BufferPool(size_t poolSize, size_t bufSize) : bufferSize(bufSize) {
        for (size_t i = 0; i < poolSize; ++i) {
            auto buffer = make_unique<Buffer>(bufferSize);
            available.push(buffer.get());
            buffers.push_back(move(buffer));
        }
        cout << "Buffer pool with " << poolSize << " buffers ready" << endl;
    }
    
    Buffer* acquire() {
        if (available.empty()) return nullptr;
        Buffer* buffer = available.front();
        available.pop();
        buffer->clear();  // Reset buffer
        return buffer;
    }
    
    void release(Buffer* buffer) {
        available.push(buffer);
    }
};

// ============= Example 4: Particle System (Game Dev) =============

class Particle {
private:
    double x, y;
    double velocityX, velocityY;
    bool active;
    
public:
    Particle() : x(0), y(0), velocityX(0), velocityY(0), active(false) {}
    
    void init(double posX, double posY, double velX, double velY) {
        x = posX;
        y = posY;
        velocityX = velX;
        velocityY = velY;
        active = true;
    }
    
    void update() {
        if (active) {
            x += velocityX;
            y += velocityY;
        }
    }
    
    void deactivate() {
        active = false;
    }
    
    bool isActive() const { return active; }
    double getX() const { return x; }
    double getY() const { return y; }
};

class ParticlePool {
private:
    vector<Particle> particles;
    size_t nextAvailable;
    
public:
    ParticlePool(size_t size) : particles(size), nextAvailable(0) {
        cout << "Particle pool with " << size << " particles created" << endl;
    }
    
    Particle* spawn(double x, double y, double velX, double velY) {
        // Find inactive particle
        for (size_t i = 0; i < particles.size(); ++i) {
            if (!particles[i].isActive()) {
                particles[i].init(x, y, velX, velY);
                cout << "Spawned particle at (" << x << ", " << y << ")" << endl;
                return &particles[i];
            }
        }
        cout << "Pool exhausted! No available particles" << endl;
        return nullptr;
    }
    
    void updateAll() {
        for (auto& particle : particles) {
            if (particle.isActive()) {
                particle.update();
            }
        }
    }
    
    size_t getActiveCount() const {
        size_t count = 0;
        for (const auto& particle : particles) {
            if (particle.isActive()) ++count;
        }
        return count;
    }
};

int main() {
    cout << "=== OBJECT POOL PATTERN ===" << endl;
    
    // Example 1: Database Connection Pool
    cout << "\n1. DATABASE CONNECTION POOL:" << endl;
    cout << "============================" << endl;
    
    ConnectionPool dbPool(3);
    
    cout << "\nAcquiring connections..." << endl;
    auto conn1 = dbPool.acquire();
    auto conn2 = dbPool.acquire();
    
    if (conn1) {
        conn1->connect();
        conn1->executeQuery("SELECT * FROM users");
        conn1->disconnect();
    }
    
    if (conn2) {
        conn2->executeQuery("INSERT INTO logs VALUES (1)");
    }
    
    cout << "\nReleasing connections..." << endl;
    dbPool.release(conn1);
    dbPool.release(conn2);
    
    cout << "Available connections: " << dbPool.getAvailableCount() << endl;
    
    // Example 2: Thread Pool
    cout << "\n\n2. THREAD POOL:" << endl;
    cout << "===============" << endl;
    
    ThreadPool threadPool(2);
    
    auto worker1 = threadPool.getWorker();
    auto worker2 = threadPool.getWorker();
    
    if (worker1) worker1->doWork("Task 1");
    if (worker2) worker2->doWork("Task 2");
    
    threadPool.returnWorker(worker1);
    threadPool.returnWorker(worker2);
    
    // Example 3: Buffer Pool
    cout << "\n\n3. BUFFER POOL:" << endl;
    cout << "===============" << endl;
    
    BufferPool bufferPool(2, 1024);
    
    auto buffer1 = bufferPool.acquire();
    if (buffer1) {
        buffer1->write("Hello from buffer 1");
        cout << "Using buffer of " << buffer1->getSize() << " bytes" << endl;
    }
    
    bufferPool.release(buffer1);
    
    // Example 4: Particle System
    cout << "\n\n4. PARTICLE SYSTEM (Game Dev):" << endl;
    cout << "===============================" << endl;
    
    ParticlePool particlePool(100);
    
    cout << "\nSpawning particles..." << endl;
    particlePool.spawn(0, 0, 1, 1);
    particlePool.spawn(10, 10, -1, 1);
    particlePool.spawn(20, 20, 0, -1);
    
    cout << "Active particles: " << particlePool.getActiveCount() << endl;
    
    cout << "\nUpdating particles..." << endl;
    particlePool.updateAll();
    
    cout << "\n\n=== KEY TAKEAWAYS ===" << endl;
    cout << "1. Object Pool reuses expensive objects" << endl;
    cout << "2. Pre-allocate fixed number of objects" << endl;
    cout << "3. Acquire from pool, use, return to pool" << endl;
    cout << "4. Improves performance and memory usage" << endl;
    cout << "5. Essential for game development and servers" << endl;
    
    cout << "\nStructure:" << endl;
    cout << "• Pool holds reusable objects" << endl;
    cout << "• acquire() gets object from pool" << endl;
    cout << "• release() returns object to pool" << endl;
    cout << "• Objects are reset when returned" << endl;
    
    cout << "\nBenefits:" << endl;
    cout << "✓ Reduced allocation/deallocation overhead" << endl;
    cout << "✓ Predictable memory usage" << endl;
    cout << "✓ Better performance" << endl;
    cout << "✓ Controlled resource usage" << endl;
    cout << "✓ Reduced garbage collection pressure" << endl;
    
    cout << "\nWhen to use:" << endl;
    cout << "• Objects are expensive to create" << endl;
    cout << "• Need many short-lived objects" << endl;
    cout << "• Performance is critical" << endl;
    cout << "• Want to limit resource count" << endl;
    
    cout << "\nReal-world examples:" << endl;
    cout << "• Database connection pools" << endl;
    cout << "• Thread pools" << endl;
    cout << "• Memory buffer pools" << endl;
    cout << "• Game object pools (particles, bullets)" << endl;
    cout << "• Network socket pools" << endl;
    cout << "• String/StringBuilder pools" << endl;
    
    cout << "\nConsiderations:" << endl;
    cout << "• Pool size (too small = exhaustion, too large = waste)" << endl;
    cout << "• Thread safety (use mutexes)" << endl;
    cout << "• Object initialization/reset" << endl;
    cout << "• Growth strategy (fixed vs dynamic)" << endl;
    
    return 0;
}

