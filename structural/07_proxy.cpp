/*
 * PROXY PATTERN - Structural Pattern
 * 
 * PURPOSE:
 * Provides a surrogate or placeholder for another object to control access to it.
 * Creates a representative object that controls access to another object.
 * 
 * TYPES OF PROXIES:
 * - Virtual Proxy: Lazy initialization, creates expensive object on demand
 * - Protection Proxy: Controls access based on permissions
 * - Remote Proxy: Represents object in different address space
 * - Caching Proxy: Caches results of expensive operations
 * - Smart Reference: Additional actions when object is accessed
 * 
 * WHEN TO USE:
 * - Need lazy initialization (virtual proxy)
 * - Need access control (protection proxy)
 * - Need local representative of remote object (remote proxy)
 * - Want to add functionality without changing original object
 * 
 * PROS:
 * + Controls access to original object
 * + Can perform operations before/after accessing real object
 * + Works even if real object isn't ready
 * + Open/Closed Principle: new proxies without changing service
 * 
 * CONS:
 * - Response time may increase
 * - Additional layer of abstraction
 * - Code complexity increases
 * 
 * REAL-WORLD EXAMPLES:
 * - Credit card (proxy for bank account)
 * - Smart pointers
 * - Lazy loading images
 * - Network proxies
 */

#include <iostream>
#include <string>
#include <memory>
#include <unordered_map>

using namespace std;

// ============= Example 1: Virtual Proxy (Lazy Loading) =============

// Subject interface
class Image {
public:
    virtual ~Image() = default;
    virtual void display() = 0;
    virtual string getFilename() const = 0;
};

// Real Subject: Heavy object
class RealImage : public Image {
private:
    string filename;
    
    void loadFromDisk() {
        cout << "Loading image from disk: " << filename << " (expensive operation)" << endl;
        // Simulate expensive loading operation
    }
    
public:
    RealImage(const string& file) : filename(file) {
        loadFromDisk();
    }
    
    void display() override {
        cout << "Displaying image: " << filename << endl;
    }
    
    string getFilename() const override {
        return filename;
    }
};

// Proxy: Provides lazy initialization
class ImageProxy : public Image {
private:
    string filename;
    unique_ptr<RealImage> realImage;
    
public:
    ImageProxy(const string& file) : filename(file), realImage(nullptr) {
        cout << "ImageProxy created for: " << filename << " (no loading yet)" << endl;
    }
    
    void display() override {
        // Lazy initialization: create real object only when needed
        if (!realImage) {
            cout << "First access - loading real image..." << endl;
            realImage = make_unique<RealImage>(filename);
        }
        realImage->display();
    }
    
    string getFilename() const override {
        return filename;
    }
};

// ============= Example 2: Protection Proxy (Access Control) =============

// Subject
class Document {
public:
    virtual ~Document() = default;
    virtual void displayContent() = 0;
    virtual void editContent(const string& newContent) = 0;
};

// Real Subject
class SecureDocument : public Document {
private:
    string content;
    
public:
    SecureDocument(const string& initial) : content(initial) {
        cout << "SecureDocument created" << endl;
    }
    
    void displayContent() override {
        cout << "Document content: " << content << endl;
    }
    
    void editContent(const string& newContent) override {
        content = newContent;
        cout << "Document edited successfully" << endl;
    }
};

// Protection Proxy
class DocumentProxy : public Document {
private:
    unique_ptr<SecureDocument> realDocument;
    string userRole;  // "admin", "editor", "viewer"
    
public:
    DocumentProxy(const string& content, const string& role) 
        : realDocument(make_unique<SecureDocument>(content)), userRole(role) {
        cout << "DocumentProxy created for user role: " << role << endl;
    }
    
    void displayContent() override {
        cout << "Checking read permissions..." << endl;
        // All roles can read
        realDocument->displayContent();
    }
    
    void editContent(const string& newContent) override {
        cout << "Checking write permissions..." << endl;
        if (userRole == "admin" || userRole == "editor") {
            realDocument->editContent(newContent);
        } else {
            cout << "Access denied: " << userRole << " cannot edit documents" << endl;
        }
    }
};

// ============= Example 3: Caching Proxy =============

// Subject
class DatabaseQuery {
public:
    virtual ~DatabaseQuery() = default;
    virtual string executeQuery(const string& query) = 0;
};

// Real Subject
class RealDatabase : public DatabaseQuery {
public:
    string executeQuery(const string& query) override {
        cout << "Executing query on database (slow operation): " << query << endl;
        // Simulate slow database query
        return "Result for: " + query;
    }
};

// Caching Proxy
class CachingDatabaseProxy : public DatabaseQuery {
private:
    unique_ptr<RealDatabase> realDatabase;
    unordered_map<string, string> cache;
    
public:
    CachingDatabaseProxy() : realDatabase(make_unique<RealDatabase>()) {}
    
    string executeQuery(const string& query) override {
        // Check cache first
        if (cache.find(query) != cache.end()) {
            cout << "Returning cached result for: " << query << endl;
            return cache[query];
        }
        
        // Cache miss - execute real query
        cout << "Cache miss - executing real query" << endl;
        string result = realDatabase->executeQuery(query);
        cache[query] = result;
        return result;
    }
    
    void clearCache() {
        cache.clear();
        cout << "Cache cleared" << endl;
    }
};

// ============= Example 4: Smart Reference Proxy =============

// Subject
class NetworkResource {
public:
    virtual ~NetworkResource() = default;
    virtual void access() = 0;
};

// Real Subject
class RealNetworkResource : public NetworkResource {
private:
    string url;
    
public:
    RealNetworkResource(const string& url) : url(url) {
        cout << "Connected to: " << url << endl;
    }
    
    void access() override {
        cout << "Accessing resource at: " << url << endl;
    }
};

// Smart Reference Proxy (tracks usage, manages lifecycle)
class NetworkResourceProxy : public NetworkResource {
private:
    unique_ptr<RealNetworkResource> resource;
    string url;
    int accessCount;
    
public:
    NetworkResourceProxy(const string& url) 
        : url(url), accessCount(0), resource(nullptr) {}
    
    void access() override {
        // Create resource on first access
        if (!resource) {
            cout << "Establishing connection..." << endl;
            resource = make_unique<RealNetworkResource>(url);
        }
        
        // Log access
        accessCount++;
        cout << "[Proxy] Access #" << accessCount << endl;
        
        // Perform actual access
        resource->access();
        
        // Additional functionality
        if (accessCount >= 5) {
            cout << "[Proxy] Warning: High access count detected!" << endl;
        }
    }
    
    int getAccessCount() const {
        return accessCount;
    }
    
    ~NetworkResourceProxy() {
        cout << "[Proxy] Total accesses: " << accessCount << endl;
        cout << "[Proxy] Cleaning up connection" << endl;
    }
};

// ============= Example 5: Remote Proxy (Simulated) =============

// Subject
class RemoteService {
public:
    virtual ~RemoteService() = default;
    virtual string processRequest(const string& request) = 0;
};

// Real Subject (on remote server)
class RealRemoteService : public RemoteService {
public:
    string processRequest(const string& request) override {
        return "Server response for: " + request;
    }
};

// Remote Proxy (client side)
class RemoteServiceProxy : public RemoteService {
private:
    unique_ptr<RealRemoteService> remoteService;
    string serverAddress;
    
public:
    RemoteServiceProxy(const string& address) 
        : serverAddress(address), remoteService(make_unique<RealRemoteService>()) {}
    
    string processRequest(const string& request) override {
        cout << "Connecting to server: " << serverAddress << endl;
        cout << "Sending request over network..." << endl;
        
        // Simulate network communication
        string response = remoteService->processRequest(request);
        
        cout << "Receiving response..." << endl;
        return response;
    }
};

int main() {
    cout << "=== PROXY PATTERN DEMO ===" << endl;
    
    // Example 1: Virtual Proxy (Lazy Loading)
    cout << "\n1. VIRTUAL PROXY (Lazy Loading):" << endl;
    cout << "=================================" << endl;
    
    // Create proxies (images not loaded yet)
    cout << "\n[Creating image proxies]" << endl;
    unique_ptr<Image> image1 = make_unique<ImageProxy>("photo1.jpg");
    unique_ptr<Image> image2 = make_unique<ImageProxy>("photo2.jpg");
    
    // Display images (loaded on first access)
    cout << "\n[Displaying images]" << endl;
    image1->display();  // Loads from disk
    image1->display();  // Already loaded
    image2->display();  // Loads from disk
    
    // Example 2: Protection Proxy (Access Control)
    cout << "\n\n2. PROTECTION PROXY (Access Control):" << endl;
    cout << "======================================" << endl;
    
    cout << "\n[Admin user]" << endl;
    unique_ptr<Document> adminDoc = make_unique<DocumentProxy>(
        "Confidential Data", "admin"
    );
    adminDoc->displayContent();
    adminDoc->editContent("Updated Data");
    
    cout << "\n[Viewer user]" << endl;
    unique_ptr<Document> viewerDoc = make_unique<DocumentProxy>(
        "Public Data", "viewer"
    );
    viewerDoc->displayContent();
    viewerDoc->editContent("Trying to edit");  // Should be denied
    
    // Example 3: Caching Proxy
    cout << "\n\n3. CACHING PROXY:" << endl;
    cout << "=================" << endl;
    
    unique_ptr<CachingDatabaseProxy> db = make_unique<CachingDatabaseProxy>();
    
    cout << "\n[First query]" << endl;
    cout << db->executeQuery("SELECT * FROM users") << endl;
    
    cout << "\n[Same query again - from cache]" << endl;
    cout << db->executeQuery("SELECT * FROM users") << endl;
    
    cout << "\n[Different query]" << endl;
    cout << db->executeQuery("SELECT * FROM products") << endl;
    
    cout << "\n[First query again - still cached]" << endl;
    cout << db->executeQuery("SELECT * FROM users") << endl;
    
    // Example 4: Smart Reference Proxy
    cout << "\n\n4. SMART REFERENCE PROXY:" << endl;
    cout << "=========================" << endl;
    
    {
        NetworkResourceProxy api("https://api.example.com");
        
        cout << "\n[Multiple accesses]" << endl;
        for (int i = 0; i < 6; i++) {
            api.access();
        }
        
        cout << "\n[Total access count: " << api.getAccessCount() << "]" << endl;
    }  // Destructor called here
    
    // Example 5: Remote Proxy
    cout << "\n\n5. REMOTE PROXY:" << endl;
    cout << "================" << endl;
    
    unique_ptr<RemoteService> service = make_unique<RemoteServiceProxy>(
        "192.168.1.100:8080"
    );
    
    cout << "\n[Making remote call]" << endl;
    string response = service->processRequest("GET /data");
    cout << "Response: " << response << endl;
    
    cout << "\n\n=== KEY TAKEAWAYS ===" << endl;
    cout << "1. Proxy CONTROLS ACCESS to another object" << endl;
    cout << "2. Proxy and real object implement same interface" << endl;
    cout << "3. Types: Virtual, Protection, Caching, Smart Reference, Remote" << endl;
    cout << "4. Virtual Proxy: Delays expensive object creation (lazy loading)" << endl;
    cout << "5. Protection Proxy: Controls access based on permissions" << endl;
    cout << "6. Caching Proxy: Stores results to avoid repeated expensive operations" << endl;
    cout << "7. Smart Reference: Adds extra functionality (logging, counting, etc.)" << endl;
    cout << "8. Client code doesn't know if it's using proxy or real object" << endl;
    
    return 0;
}

