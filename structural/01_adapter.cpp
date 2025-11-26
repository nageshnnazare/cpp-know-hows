/*
 * ADAPTER PATTERN - Structural Pattern
 * 
 * PURPOSE:
 * Allows objects with incompatible interfaces to collaborate by converting
 * the interface of one class into another interface that clients expect.
 * 
 * WHEN TO USE:
 * - Want to use existing class but its interface doesn't match what you need
 * - Need to create reusable class that cooperates with unrelated classes
 * - Need to use several existing subclasses but can't adapt each one by subclassing
 * - Working with legacy code or third-party libraries
 * 
 * PROS:
 * + Single Responsibility: separates interface conversion from business logic
 * + Open/Closed Principle: can introduce new adapters without changing existing code
 * + Makes incompatible interfaces work together
 * + Increases reusability of existing code
 * 
 * CONS:
 * - Overall complexity increases (new classes)
 * - Sometimes simpler to just change the service class
 * 
 * REAL-WORLD EXAMPLES:
 * - Power adapters (different plug types)
 * - Database adapters (uniform interface for different databases)
 * - Third-party library integration
 * - Legacy system integration
 */

#include <iostream>
#include <string>
#include <memory>
#include <cmath>

using namespace std;

// ============= Example 1: Media Player =============

// Target interface (what client expects)
class MediaPlayer {
public:
    virtual ~MediaPlayer() = default;
    virtual void play(const string& audioType, const string& fileName) = 0;
};

// Adaptee 1: Advanced Media Player (incompatible interface)
class AdvancedMP3Player {
public:
    void playMP3(const string& fileName) {
        cout << "Playing MP3 file: " << fileName << endl;
    }
};

class AdvancedMP4Player {
public:
    void playMP4(const string& fileName) {
        cout << "Playing MP4 file: " << fileName << endl;
    }
};

class AdvancedVLCPlayer {
public:
    void playVLC(const string& fileName) {
        cout << "Playing VLC file: " << fileName << endl;
    }
};

// Adapter: Makes advanced players compatible with MediaPlayer interface
class MediaAdapter : public MediaPlayer {
private:
    unique_ptr<AdvancedMP3Player> mp3Player;
    unique_ptr<AdvancedMP4Player> mp4Player;
    unique_ptr<AdvancedVLCPlayer> vlcPlayer;
    
public:
    MediaAdapter() {
        mp3Player = make_unique<AdvancedMP3Player>();
        mp4Player = make_unique<AdvancedMP4Player>();
        vlcPlayer = make_unique<AdvancedVLCPlayer>();
    }
    
    void play(const string& audioType, const string& fileName) override {
        if (audioType == "mp3") {
            mp3Player->playMP3(fileName);
        } else if (audioType == "mp4") {
            mp4Player->playMP4(fileName);
        } else if (audioType == "vlc") {
            vlcPlayer->playVLC(fileName);
        } else {
            cout << "Invalid media type: " << audioType << endl;
        }
    }
};

// Client: Audio Player
class AudioPlayer : public MediaPlayer {
private:
    unique_ptr<MediaAdapter> mediaAdapter;
    
public:
    AudioPlayer() {
        mediaAdapter = make_unique<MediaAdapter>();
    }
    
    void play(const string& audioType, const string& fileName) override {
        // Built-in support for MP3
        if (audioType == "mp3") {
            cout << "Playing MP3 file (native): " << fileName << endl;
        }
        // Use adapter for other formats
        else if (audioType == "mp4" || audioType == "vlc") {
            mediaAdapter->play(audioType, fileName);
        }
        else {
            cout << "Invalid media type: " << audioType << endl;
        }
    }
};

// ============= Example 2: Shape Drawing (Class Adapter) =============

// Legacy Rectangle class (Adaptee)
class LegacyRectangle {
public:
    void draw(int x1, int y1, int x2, int y2) {
        cout << "Drawing Legacy Rectangle from (" << x1 << "," << y1 
             << ") to (" << x2 << "," << y2 << ")" << endl;
    }
};

// Target interface
class Shape {
public:
    virtual ~Shape() = default;
    virtual void draw(int x, int y, int width, int height) = 0;
};

// Adapter: Adapts LegacyRectangle to Shape interface
class RectangleAdapter : public Shape, private LegacyRectangle {
public:
    void draw(int x, int y, int width, int height) override {
        // Convert new interface parameters to legacy interface
        int x2 = x + width;
        int y2 = y + height;
        cout << "Adapter converting coordinates..." << endl;
        LegacyRectangle::draw(x, y, x2, y2);
    }
};

// ============= Example 3: Payment System =============

// Adaptee: Third-party payment processor
class PayPalPayment {
public:
    void sendPaymentViaPayPal(const string& email, double amount) {
        cout << "Processing $" << amount << " payment to " << email 
             << " via PayPal" << endl;
    }
};

class StripePayment {
public:
    void processStripePayment(const string& token, double amount) {
        cout << "Processing $" << amount << " payment with token " << token 
             << " via Stripe" << endl;
    }
};

// Target interface
class PaymentProcessor {
public:
    virtual ~PaymentProcessor() = default;
    virtual void processPayment(const string& identifier, double amount) = 0;
};

// Adapters
class PayPalAdapter : public PaymentProcessor {
private:
    unique_ptr<PayPalPayment> paypal;
    
public:
    PayPalAdapter() {
        paypal = make_unique<PayPalPayment>();
    }
    
    void processPayment(const string& identifier, double amount) override {
        paypal->sendPaymentViaPayPal(identifier, amount);
    }
};

class StripeAdapter : public PaymentProcessor {
private:
    unique_ptr<StripePayment> stripe;
    
public:
    StripeAdapter() {
        stripe = make_unique<StripePayment>();
    }
    
    void processPayment(const string& identifier, double amount) override {
        stripe->processStripePayment(identifier, amount);
    }
};

// ============= Example 4: Temperature Conversion =============

// Adaptee: Celsius sensor (legacy)
class CelsiusSensor {
public:
    double getTemperatureInCelsius() {
        return 25.0; // Example reading
    }
};

// Target interface: Fahrenheit
class TemperatureSensor {
public:
    virtual ~TemperatureSensor() = default;
    virtual double getTemperature() = 0; // Returns Fahrenheit
};

// Adapter
class CelsiusToFahrenheitAdapter : public TemperatureSensor {
private:
    unique_ptr<CelsiusSensor> celsiusSensor;
    
public:
    CelsiusToFahrenheitAdapter() {
        celsiusSensor = make_unique<CelsiusSensor>();
    }
    
    double getTemperature() override {
        double celsius = celsiusSensor->getTemperatureInCelsius();
        double fahrenheit = (celsius * 9.0 / 5.0) + 32.0;
        return fahrenheit;
    }
};

int main() {
    cout << "=== ADAPTER PATTERN DEMO ===" << endl;
    
    // Example 1: Media Player
    cout << "\n1. MEDIA PLAYER ADAPTER:" << endl;
    cout << "========================" << endl;
    unique_ptr<AudioPlayer> player = make_unique<AudioPlayer>();
    player->play("mp3", "song.mp3");
    player->play("mp4", "video.mp4");
    player->play("vlc", "movie.vlc");
    player->play("avi", "video.avi");
    
    // Example 2: Shape Drawing
    cout << "\n2. SHAPE DRAWING ADAPTER:" << endl;
    cout << "=========================" << endl;
    unique_ptr<Shape> rect = make_unique<RectangleAdapter>();
    rect->draw(10, 20, 100, 50);
    
    // Example 3: Payment Processing
    cout << "\n3. PAYMENT PROCESSOR ADAPTER:" << endl;
    cout << "=============================" << endl;
    unique_ptr<PaymentProcessor> paypal = make_unique<PayPalAdapter>();
    paypal->processPayment("user@email.com", 99.99);
    
    unique_ptr<PaymentProcessor> stripe = make_unique<StripeAdapter>();
    stripe->processPayment("tok_visa_1234", 149.99);
    
    // Example 4: Temperature Sensor
    cout << "\n4. TEMPERATURE SENSOR ADAPTER:" << endl;
    cout << "==============================" << endl;
    unique_ptr<TemperatureSensor> sensor = make_unique<CelsiusToFahrenheitAdapter>();
    cout << "Temperature: " << sensor->getTemperature() << "°F" << endl;
    
    cout << "\n\n=== KEY TAKEAWAYS ===" << endl;
    cout << "1. Adapter makes incompatible interfaces work together" << endl;
    cout << "2. Converts one interface to another that clients expect" << endl;
    cout << "3. Useful for integrating third-party libraries or legacy code" << endl;
    cout << "4. Two types: Object Adapter (composition) and Class Adapter (inheritance)" << endl;
    cout << "5. Object Adapter is more flexible (used in most examples)" << endl;
    cout << "6. Client remains unaware of adaptation happening" << endl;
    
    return 0;
}

