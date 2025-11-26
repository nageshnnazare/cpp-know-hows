/*
 * STRATEGY PATTERN - Behavioral Pattern
 * 
 * PURPOSE:
 * Defines a family of algorithms, encapsulates each one, and makes them
 * interchangeable. Lets the algorithm vary independently from clients that use it.
 * 
 * WHEN TO USE:
 * - Many related classes differ only in behavior
 * - Need different variants of an algorithm
 * - Algorithm uses data clients shouldn't know about
 * - Class has massive conditional selecting different behaviors
 * 
 * PROS:
 * + Can switch algorithms at runtime
 * + Isolates implementation details
 * + Replaces inheritance with composition
 * + Open/Closed Principle: add new strategies without changing context
 * + Eliminates conditional statements
 * 
 * CONS:
 * - Clients must be aware of different strategies
 * - Increases number of objects
 * - Overkill for few algorithms that rarely change
 * 
 * REAL-WORLD EXAMPLES:
 * - Sorting algorithms (quick sort, merge sort, bubble sort)
 * - Payment methods (credit card, PayPal, crypto)
 * - Compression algorithms (ZIP, RAR, 7Z)
 * - Route planning (fastest, shortest, scenic)
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>

using namespace std;

// ============= Example 1: Sorting Strategies =============

// Strategy interface
class SortStrategy {
public:
    virtual ~SortStrategy() = default;
    virtual void sort(vector<int>& data) = 0;
    virtual string getName() const = 0;
};

// Concrete Strategies
class BubbleSort : public SortStrategy {
public:
    void sort(vector<int>& data) override {
        cout << "Using Bubble Sort" << endl;
        int n = data.size();
        for (int i = 0; i < n-1; i++) {
            for (int j = 0; j < n-i-1; j++) {
                if (data[j] > data[j+1]) {
                    swap(data[j], data[j+1]);
                }
            }
        }
    }
    
    string getName() const override { return "Bubble Sort"; }
};

class QuickSort : public SortStrategy {
private:
    void quickSort(vector<int>& data, int low, int high) {
        if (low < high) {
            int pi = partition(data, low, high);
            quickSort(data, low, pi - 1);
            quickSort(data, pi + 1, high);
        }
    }
    
    int partition(vector<int>& data, int low, int high) {
        int pivot = data[high];
        int i = low - 1;
        for (int j = low; j < high; j++) {
            if (data[j] < pivot) {
                i++;
                swap(data[i], data[j]);
            }
        }
        swap(data[i + 1], data[high]);
        return i + 1;
    }
    
public:
    void sort(vector<int>& data) override {
        cout << "Using Quick Sort" << endl;
        quickSort(data, 0, data.size() - 1);
    }
    
    string getName() const override { return "Quick Sort"; }
};

class STLSort : public SortStrategy {
public:
    void sort(vector<int>& data) override {
        cout << "Using STL Sort" << endl;
        std::sort(data.begin(), data.end());
    }
    
    string getName() const override { return "STL Sort"; }
};

// Context
class DataSorter {
private:
    unique_ptr<SortStrategy> strategy;
    
public:
    void setStrategy(unique_ptr<SortStrategy> newStrategy) {
        strategy = move(newStrategy);
        cout << "Strategy set to: " << strategy->getName() << endl;
    }
    
    void sortData(vector<int>& data) {
        if (strategy) {
            strategy->sort(data);
        } else {
            cout << "No sorting strategy set!" << endl;
        }
    }
};

// ============= Example 2: Payment Methods =============

class PaymentStrategy {
public:
    virtual ~PaymentStrategy() = default;
    virtual void pay(double amount) = 0;
    virtual string getPaymentType() const = 0;
};

class CreditCardPayment : public PaymentStrategy {
private:
    string cardNumber;
    string cvv;
    
public:
    CreditCardPayment(const string& number, const string& cvv) 
        : cardNumber(number), cvv(cvv) {}
    
    void pay(double amount) override {
        cout << "Processing credit card payment of $" << amount << endl;
        cout << "Card: ****-****-****-" << cardNumber.substr(cardNumber.length() - 4) << endl;
    }
    
    string getPaymentType() const override { return "Credit Card"; }
};

class PayPalPayment : public PaymentStrategy {
private:
    string email;
    
public:
    PayPalPayment(const string& email) : email(email) {}
    
    void pay(double amount) override {
        cout << "Processing PayPal payment of $" << amount << endl;
        cout << "PayPal account: " << email << endl;
    }
    
    string getPaymentType() const override { return "PayPal"; }
};

class CryptoPayment : public PaymentStrategy {
private:
    string walletAddress;
    
public:
    CryptoPayment(const string& address) : walletAddress(address) {}
    
    void pay(double amount) override {
        cout << "Processing cryptocurrency payment of $" << amount << endl;
        cout << "Wallet: " << walletAddress.substr(0, 10) << "..." << endl;
    }
    
    string getPaymentType() const override { return "Cryptocurrency"; }
};

class ShoppingCart {
private:
    double totalAmount;
    unique_ptr<PaymentStrategy> paymentMethod;
    
public:
    ShoppingCart() : totalAmount(0) {}
    
    void addItem(double price) {
        totalAmount += price;
        cout << "Item added. Total: $" << totalAmount << endl;
    }
    
    void setPaymentMethod(unique_ptr<PaymentStrategy> method) {
        paymentMethod = move(method);
        cout << "Payment method set to: " << paymentMethod->getPaymentType() << endl;
    }
    
    void checkout() {
        if (paymentMethod) {
            cout << "\nProcessing checkout..." << endl;
            paymentMethod->pay(totalAmount);
            cout << "Payment successful!" << endl;
            totalAmount = 0;
        } else {
            cout << "Please select a payment method!" << endl;
        }
    }
};

// ============= Example 3: Navigation Routes =============

class RouteStrategy {
public:
    virtual ~RouteStrategy() = default;
    virtual void buildRoute(const string& from, const string& to) = 0;
    virtual string getRouteName() const = 0;
};

class FastestRoute : public RouteStrategy {
public:
    void buildRoute(const string& from, const string& to) override {
        cout << "Building FASTEST route from " << from << " to " << to << endl;
        cout << "  Route: Highway -> Express lanes" << endl;
        cout << "  Distance: 45 km, Time: 30 minutes" << endl;
    }
    
    string getRouteName() const override { return "Fastest Route"; }
};

class ShortestRoute : public RouteStrategy {
public:
    void buildRoute(const string& from, const string& to) override {
        cout << "Building SHORTEST route from " << from << " to " << to << endl;
        cout << "  Route: Direct path through city" << endl;
        cout << "  Distance: 35 km, Time: 45 minutes" << endl;
    }
    
    string getRouteName() const override { return "Shortest Route"; }
};

class ScenicRoute : public RouteStrategy {
public:
    void buildRoute(const string& from, const string& to) override {
        cout << "Building SCENIC route from " << from << " to " << to << endl;
        cout << "  Route: Coastal road -> Mountain pass" << endl;
        cout << "  Distance: 60 km, Time: 90 minutes" << endl;
    }
    
    string getRouteName() const override { return "Scenic Route"; }
};

class Navigator {
private:
    unique_ptr<RouteStrategy> routeStrategy;
    
public:
    void setRouteStrategy(unique_ptr<RouteStrategy> strategy) {
        routeStrategy = move(strategy);
        cout << "Route strategy: " << routeStrategy->getRouteName() << endl;
    }
    
    void navigate(const string& from, const string& to) {
        if (routeStrategy) {
            routeStrategy->buildRoute(from, to);
        } else {
            cout << "No route strategy selected!" << endl;
        }
    }
};

// ============= Example 4: Compression =============

class CompressionStrategy {
public:
    virtual ~CompressionStrategy() = default;
    virtual void compress(const string& filename) = 0;
    virtual string getCompressionType() const = 0;
};

class ZipCompression : public CompressionStrategy {
public:
    void compress(const string& filename) override {
        cout << "Compressing " << filename << " using ZIP" << endl;
        cout << "  Compression ratio: 50%" << endl;
    }
    
    string getCompressionType() const override { return "ZIP"; }
};

class RarCompression : public CompressionStrategy {
public:
    void compress(const string& filename) override {
        cout << "Compressing " << filename << " using RAR" << endl;
        cout << "  Compression ratio: 60%" << endl;
    }
    
    string getCompressionType() const override { return "RAR"; }
};

class SevenZipCompression : public CompressionStrategy {
public:
    void compress(const string& filename) override {
        cout << "Compressing " << filename << " using 7-Zip" << endl;
        cout << "  Compression ratio: 70%" << endl;
    }
    
    string getCompressionType() const override { return "7-Zip"; }
};

class FileCompressor {
private:
    unique_ptr<CompressionStrategy> strategy;
    
public:
    void setCompressionStrategy(unique_ptr<CompressionStrategy> newStrategy) {
        strategy = move(newStrategy);
        cout << "Compression type: " << strategy->getCompressionType() << endl;
    }
    
    void compressFile(const string& filename) {
        if (strategy) {
            strategy->compress(filename);
        } else {
            cout << "No compression strategy set!" << endl;
        }
    }
};

void printVector(const vector<int>& data) {
    for (int num : data) {
        cout << num << " ";
    }
    cout << endl;
}

int main() {
    cout << "=== STRATEGY PATTERN DEMO ===" << endl;
    
    // Example 1: Sorting
    cout << "\n1. SORTING ALGORITHMS:" << endl;
    cout << "======================" << endl;
    
    DataSorter sorter;
    vector<int> data = {64, 34, 25, 12, 22, 11, 90};
    
    cout << "Original data: ";
    printVector(data);
    
    cout << "\n[Using Bubble Sort]" << endl;
    vector<int> data1 = data;
    sorter.setStrategy(make_unique<BubbleSort>());
    sorter.sortData(data1);
    cout << "Sorted: ";
    printVector(data1);
    
    cout << "\n[Using Quick Sort]" << endl;
    vector<int> data2 = data;
    sorter.setStrategy(make_unique<QuickSort>());
    sorter.sortData(data2);
    cout << "Sorted: ";
    printVector(data2);
    
    // Example 2: Payment Methods
    cout << "\n\n2. PAYMENT PROCESSING:" << endl;
    cout << "======================" << endl;
    
    ShoppingCart cart;
    cart.addItem(29.99);
    cart.addItem(49.99);
    cart.addItem(19.99);
    
    cout << "\n[Paying with Credit Card]" << endl;
    cart.setPaymentMethod(make_unique<CreditCardPayment>("1234567890123456", "123"));
    cart.checkout();
    
    cart.addItem(99.99);
    cout << "\n[Paying with PayPal]" << endl;
    cart.setPaymentMethod(make_unique<PayPalPayment>("user@example.com"));
    cart.checkout();
    
    cart.addItem(199.99);
    cout << "\n[Paying with Crypto]" << endl;
    cart.setPaymentMethod(make_unique<CryptoPayment>("1A1zP1eP5QGefi2DMPTfTL5SLmv7DivfNa"));
    cart.checkout();
    
    // Example 3: Navigation
    cout << "\n\n3. NAVIGATION ROUTES:" << endl;
    cout << "=====================" << endl;
    
    Navigator nav;
    string from = "New York";
    string to = "Boston";
    
    cout << "\n[Fastest Route]" << endl;
    nav.setRouteStrategy(make_unique<FastestRoute>());
    nav.navigate(from, to);
    
    cout << "\n[Shortest Route]" << endl;
    nav.setRouteStrategy(make_unique<ShortestRoute>());
    nav.navigate(from, to);
    
    cout << "\n[Scenic Route]" << endl;
    nav.setRouteStrategy(make_unique<ScenicRoute>());
    nav.navigate(from, to);
    
    // Example 4: Compression
    cout << "\n\n4. FILE COMPRESSION:" << endl;
    cout << "====================" << endl;
    
    FileCompressor compressor;
    string filename = "document.txt";
    
    cout << "\n[ZIP Compression]" << endl;
    compressor.setCompressionStrategy(make_unique<ZipCompression>());
    compressor.compressFile(filename);
    
    cout << "\n[RAR Compression]" << endl;
    compressor.setCompressionStrategy(make_unique<RarCompression>());
    compressor.compressFile(filename);
    
    cout << "\n[7-Zip Compression]" << endl;
    compressor.setCompressionStrategy(make_unique<SevenZipCompression>());
    compressor.compressFile(filename);
    
    cout << "\n\n=== KEY TAKEAWAYS ===" << endl;
    cout << "1. Strategy defines FAMILY OF ALGORITHMS" << endl;
    cout << "2. Algorithms are interchangeable at runtime" << endl;
    cout << "3. Separates algorithm from client code" << endl;
    cout << "4. Uses composition instead of inheritance" << endl;
    cout << "5. Eliminates conditional statements for algorithm selection" << endl;
    cout << "6. Easy to add new strategies without modifying context" << endl;
    cout << "7. Client chooses which strategy to use" << endl;
    
    return 0;
}

