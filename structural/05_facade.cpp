/*
 * FACADE PATTERN - Structural Pattern
 * 
 * PURPOSE:
 * Provides a simplified interface to a complex subsystem.
 * Makes the subsystem easier to use by wrapping complex operations.
 * 
 * WHEN TO USE:
 * - Want to provide simple interface to complex subsystem
 * - Need to decouple subsystem from clients and other subsystems
 * - Want to layer your subsystems
 * - Many dependencies between clients and implementation classes
 * 
 * PROS:
 * + Simplifies interface to complex system
 * + Isolates clients from subsystem components
 * + Promotes weak coupling
 * + Easier to use, understand, and test
 * + Can provide multiple facades for different needs
 * 
 * CONS:
 * - Facade can become god object coupled to all classes
 * - May provide limited functionality
 * - Additional layer of abstraction
 * 
 * REAL-WORLD EXAMPLES:
 * - Computer startup (coordinates CPU, memory, HDD, etc.)
 * - Library APIs (simplify complex operations)
 * - Online shopping checkout
 * - Hotel booking system
 */

#include <iostream>
#include <string>
#include <memory>

using namespace std;

// ============= Example 1: Home Theater System =============

// Complex subsystem classes
class DVDPlayer {
public:
    void on() { cout << "DVD Player: Turning ON" << endl; }
    void off() { cout << "DVD Player: Turning OFF" << endl; }
    void play(const string& movie) { 
        cout << "DVD Player: Playing '" << movie << "'" << endl; 
    }
    void stop() { cout << "DVD Player: Stopping playback" << endl; }
    void eject() { cout << "DVD Player: Ejecting disc" << endl; }
};

class Amplifier {
public:
    void on() { cout << "Amplifier: Turning ON" << endl; }
    void off() { cout << "Amplifier: Turning OFF" << endl; }
    void setVolume(int level) { 
        cout << "Amplifier: Setting volume to " << level << endl; 
    }
    void setSurroundSound() { cout << "Amplifier: Enabling surround sound" << endl; }
};

class Projector {
public:
    void on() { cout << "Projector: Turning ON" << endl; }
    void off() { cout << "Projector: Turning OFF" << endl; }
    void wideScreenMode() { cout << "Projector: Setting wide screen mode" << endl; }
};

class Lights {
public:
    void dim(int level) { 
        cout << "Lights: Dimming to " << level << "%" << endl; 
    }
    void on() { cout << "Lights: Turning ON (100%)" << endl; }
};

class Screen {
public:
    void down() { cout << "Screen: Moving down" << endl; }
    void up() { cout << "Screen: Moving up" << endl; }
};

class PopcornMaker {
public:
    void on() { cout << "Popcorn Maker: Turning ON" << endl; }
    void off() { cout << "Popcorn Maker: Turning OFF" << endl; }
    void pop() { cout << "Popcorn Maker: Popping popcorn!" << endl; }
};

// Facade: Simplifies the complex subsystem
class HomeTheaterFacade {
private:
    unique_ptr<DVDPlayer> dvd;
    unique_ptr<Amplifier> amp;
    unique_ptr<Projector> projector;
    unique_ptr<Lights> lights;
    unique_ptr<Screen> screen;
    unique_ptr<PopcornMaker> popcorn;
    
public:
    HomeTheaterFacade() {
        dvd = make_unique<DVDPlayer>();
        amp = make_unique<Amplifier>();
        projector = make_unique<Projector>();
        lights = make_unique<Lights>();
        screen = make_unique<Screen>();
        popcorn = make_unique<PopcornMaker>();
    }
    
    void watchMovie(const string& movie) {
        cout << "\n=== Getting ready to watch '" << movie << "' ===" << endl;
        popcorn->on();
        popcorn->pop();
        lights->dim(10);
        screen->down();
        projector->on();
        projector->wideScreenMode();
        amp->on();
        amp->setSurroundSound();
        amp->setVolume(5);
        dvd->on();
        dvd->play(movie);
        cout << "=== Enjoy your movie! ===" << endl;
    }
    
    void endMovie() {
        cout << "\n=== Shutting down home theater ===" << endl;
        popcorn->off();
        lights->on();
        screen->up();
        projector->off();
        amp->off();
        dvd->stop();
        dvd->eject();
        dvd->off();
        cout << "=== Movie time is over ===" << endl;
    }
};

// ============= Example 2: Computer System =============

// Subsystem classes
class CPU {
public:
    void freeze() { cout << "CPU: Freezing..." << endl; }
    void jump(long position) { 
        cout << "CPU: Jumping to position " << position << endl; 
    }
    void execute() { cout << "CPU: Executing..." << endl; }
};

class Memory {
public:
    void load(long position, const string& data) {
        cout << "Memory: Loading data at position " << position << endl;
    }
};

class HardDrive {
public:
    string read(long lba, int size) {
        cout << "HardDrive: Reading " << size << " bytes from sector " << lba << endl;
        return "boot_data";
    }
};

// Facade
class ComputerFacade {
private:
    unique_ptr<CPU> cpu;
    unique_ptr<Memory> memory;
    unique_ptr<HardDrive> hardDrive;
    
public:
    ComputerFacade() {
        cpu = make_unique<CPU>();
        memory = make_unique<Memory>();
        hardDrive = make_unique<HardDrive>();
    }
    
    void start() {
        cout << "\n=== Starting Computer ===" << endl;
        cpu->freeze();
        string bootData = hardDrive->read(0, 1024);
        memory->load(0, bootData);
        cpu->jump(0);
        cpu->execute();
        cout << "=== Computer started successfully ===" << endl;
    }
};

// ============= Example 3: Online Shopping =============

// Subsystem classes
class Inventory {
public:
    bool checkAvailability(const string& product) {
        cout << "Inventory: Checking availability of " << product << endl;
        return true;
    }
    
    void reserve(const string& product) {
        cout << "Inventory: Reserving " << product << endl;
    }
};

class PaymentProcessor {
public:
    bool processPayment(double amount) {
        cout << "Payment: Processing payment of $" << amount << endl;
        return true;
    }
};

class ShippingService {
public:
    void shipProduct(const string& product, const string& address) {
        cout << "Shipping: Shipping " << product << " to " << address << endl;
    }
};

class NotificationService {
public:
    void sendConfirmation(const string& email) {
        cout << "Notification: Sending confirmation email to " << email << endl;
    }
};

// Facade
class OnlineShoppingFacade {
private:
    unique_ptr<Inventory> inventory;
    unique_ptr<PaymentProcessor> payment;
    unique_ptr<ShippingService> shipping;
    unique_ptr<NotificationService> notification;
    
public:
    OnlineShoppingFacade() {
        inventory = make_unique<Inventory>();
        payment = make_unique<PaymentProcessor>();
        shipping = make_unique<ShippingService>();
        notification = make_unique<NotificationService>();
    }
    
    bool placeOrder(const string& product, double price, 
                    const string& address, const string& email) {
        cout << "\n=== Processing Order ===" << endl;
        
        // Check availability
        if (!inventory->checkAvailability(product)) {
            cout << "Order failed: Product not available" << endl;
            return false;
        }
        
        // Process payment
        if (!payment->processPayment(price)) {
            cout << "Order failed: Payment declined" << endl;
            return false;
        }
        
        // Reserve and ship
        inventory->reserve(product);
        shipping->shipProduct(product, address);
        notification->sendConfirmation(email);
        
        cout << "=== Order completed successfully! ===" << endl;
        return true;
    }
};

// ============= Example 4: Banking System =============

class Account {
public:
    bool hasEnoughBalance(double amount) {
        cout << "Account: Checking balance for $" << amount << endl;
        return true;
    }
    
    void debit(double amount) {
        cout << "Account: Debiting $" << amount << endl;
    }
};

class SecurityCheck {
public:
    bool authenticate(const string& pin) {
        cout << "Security: Authenticating PIN" << endl;
        return pin == "1234";
    }
};

class TransactionLog {
public:
    void log(const string& transaction) {
        cout << "Log: Recording - " << transaction << endl;
    }
};

// Facade
class BankingFacade {
private:
    unique_ptr<Account> account;
    unique_ptr<SecurityCheck> security;
    unique_ptr<TransactionLog> log;
    
public:
    BankingFacade() {
        account = make_unique<Account>();
        security = make_unique<SecurityCheck>();
        log = make_unique<TransactionLog>();
    }
    
    bool withdraw(double amount, const string& pin) {
        cout << "\n=== Processing Withdrawal ===" << endl;
        
        if (!security->authenticate(pin)) {
            cout << "Withdrawal failed: Invalid PIN" << endl;
            return false;
        }
        
        if (!account->hasEnoughBalance(amount)) {
            cout << "Withdrawal failed: Insufficient funds" << endl;
            return false;
        }
        
        account->debit(amount);
        log->log("Withdrew $" + to_string(amount));
        cout << "=== Withdrawal successful! ===" << endl;
        return true;
    }
};

int main() {
    cout << "=== FACADE PATTERN DEMO ===" << endl;
    
    // Example 1: Home Theater
    cout << "\n1. HOME THEATER SYSTEM:" << endl;
    cout << "=======================" << endl;
    HomeTheaterFacade homeTheater;
    homeTheater.watchMovie("Inception");
    homeTheater.endMovie();
    
    // Example 2: Computer System
    cout << "\n\n2. COMPUTER SYSTEM:" << endl;
    cout << "===================" << endl;
    ComputerFacade computer;
    computer.start();
    
    // Example 3: Online Shopping
    cout << "\n\n3. ONLINE SHOPPING:" << endl;
    cout << "===================" << endl;
    OnlineShoppingFacade shop;
    shop.placeOrder("Laptop", 999.99, "123 Main St", "customer@email.com");
    
    // Example 4: Banking
    cout << "\n\n4. BANKING SYSTEM:" << endl;
    cout << "==================" << endl;
    BankingFacade bank;
    bank.withdraw(100.0, "1234");
    
    cout << "\n\n=== KEY TAKEAWAYS ===" << endl;
    cout << "1. Facade provides SIMPLIFIED interface to complex subsystem" << endl;
    cout << "2. Hides complexity from clients" << endl;
    cout << "3. Coordinates multiple subsystem components" << endl;
    cout << "4. Clients interact only with facade, not subsystem directly" << endl;
    cout << "5. Makes subsystem easier to use and understand" << endl;
    cout << "6. Promotes loose coupling between clients and subsystems" << endl;
    cout << "7. Common pattern in real-world applications and libraries" << endl;
    
    return 0;
}

