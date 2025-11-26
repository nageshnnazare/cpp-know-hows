/*
 * BRIDGE PATTERN - Structural Pattern
 * 
 * PURPOSE:
 * Separates an abstraction from its implementation so that the two can vary
 * independently. Uses composition over inheritance.
 * 
 * WHEN TO USE:
 * - Want to avoid permanent binding between abstraction and implementation
 * - Both abstractions and implementations should be extensible by subclassing
 * - Changes in implementation shouldn't affect clients
 * - Want to hide implementation from clients completely
 * - Have proliferation of classes (cartesian product problem)
 * 
 * PROS:
 * + Decouples interface from implementation
 * + Improved extensibility (extend abstraction and implementation independently)
 * + Hides implementation details from clients
 * + Single Responsibility Principle
 * + Open/Closed Principle
 * 
 * CONS:
 * - Increased complexity
 * - May be overkill for simple scenarios
 * 
 * REAL-WORLD EXAMPLES:
 * - GUI frameworks (separate abstraction from platform-specific implementation)
 * - Device drivers
 * - Database drivers (JDBC in Java)
 * - Graphics rendering (separate shape from rendering method)
 */

#include <iostream>
#include <string>
#include <memory>

using namespace std;

// ============= Example 1: Remote Control and Devices =============

// Implementation interface
class Device {
public:
    virtual ~Device() = default;
    virtual void turnOn() = 0;
    virtual void turnOff() = 0;
    virtual void setVolume(int volume) = 0;
    virtual int getVolume() = 0;
    virtual void setChannel(int channel) = 0;
    virtual int getChannel() = 0;
};

// Concrete Implementation 1: TV
class TV : public Device {
private:
    bool on;
    int volume;
    int channel;
    
public:
    TV() : on(false), volume(30), channel(1) {}
    
    void turnOn() override {
        on = true;
        cout << "TV: Turning ON" << endl;
    }
    
    void turnOff() override {
        on = false;
        cout << "TV: Turning OFF" << endl;
    }
    
    void setVolume(int vol) override {
        volume = vol;
        cout << "TV: Setting volume to " << volume << endl;
    }
    
    int getVolume() override {
        return volume;
    }
    
    void setChannel(int ch) override {
        channel = ch;
        cout << "TV: Setting channel to " << channel << endl;
    }
    
    int getChannel() override {
        return channel;
    }
};

// Concrete Implementation 2: Radio
class Radio : public Device {
private:
    bool on;
    int volume;
    int channel;
    
public:
    Radio() : on(false), volume(20), channel(88) {}
    
    void turnOn() override {
        on = true;
        cout << "Radio: Turning ON" << endl;
    }
    
    void turnOff() override {
        on = false;
        cout << "Radio: Turning OFF" << endl;
    }
    
    void setVolume(int vol) override {
        volume = vol;
        cout << "Radio: Setting volume to " << volume << endl;
    }
    
    int getVolume() override {
        return volume;
    }
    
    void setChannel(int ch) override {
        channel = ch;
        cout << "Radio: Setting frequency to " << channel << " FM" << endl;
    }
    
    int getChannel() override {
        return channel;
    }
};

// Abstraction: Remote Control
class RemoteControl {
protected:
    shared_ptr<Device> device;
    
public:
    RemoteControl(shared_ptr<Device> dev) : device(dev) {}
    virtual ~RemoteControl() = default;
    
    virtual void togglePower() {
        cout << "Remote: Toggling power" << endl;
        // Simple toggle logic
        device->turnOn();
    }
    
    virtual void volumeUp() {
        int vol = device->getVolume();
        device->setVolume(vol + 10);
    }
    
    virtual void volumeDown() {
        int vol = device->getVolume();
        device->setVolume(vol - 10);
    }
    
    virtual void channelUp() {
        int ch = device->getChannel();
        device->setChannel(ch + 1);
    }
    
    virtual void channelDown() {
        int ch = device->getChannel();
        device->setChannel(ch - 1);
    }
};

// Refined Abstraction: Advanced Remote
class AdvancedRemote : public RemoteControl {
public:
    AdvancedRemote(shared_ptr<Device> dev) : RemoteControl(dev) {}
    
    void mute() {
        cout << "Remote: Muting device" << endl;
        device->setVolume(0);
    }
    
    void setChannelDirect(int channel) {
        cout << "Remote: Setting channel directly" << endl;
        device->setChannel(channel);
    }
};

// ============= Example 2: Drawing Shapes =============

// Implementation interface
class Renderer {
public:
    virtual ~Renderer() = default;
    virtual void renderCircle(double radius) = 0;
    virtual void renderSquare(double side) = 0;
};

// Concrete Implementation: Vector Renderer
class VectorRenderer : public Renderer {
public:
    void renderCircle(double radius) override {
        cout << "Drawing circle with radius " << radius << " as VECTORS" << endl;
    }
    
    void renderSquare(double side) override {
        cout << "Drawing square with side " << side << " as VECTORS" << endl;
    }
};

// Concrete Implementation: Raster Renderer
class RasterRenderer : public Renderer {
public:
    void renderCircle(double radius) override {
        cout << "Drawing circle with radius " << radius << " as PIXELS" << endl;
    }
    
    void renderSquare(double side) override {
        cout << "Drawing square with side " << side << " as PIXELS" << endl;
    }
};

// Abstraction: Shape
class Shape {
protected:
    shared_ptr<Renderer> renderer;
    
public:
    Shape(shared_ptr<Renderer> r) : renderer(r) {}
    virtual ~Shape() = default;
    virtual void draw() = 0;
    virtual void resize(double factor) = 0;
};

// Refined Abstraction: Circle
class Circle : public Shape {
private:
    double radius;
    
public:
    Circle(shared_ptr<Renderer> r, double rad) : Shape(r), radius(rad) {}
    
    void draw() override {
        renderer->renderCircle(radius);
    }
    
    void resize(double factor) override {
        radius *= factor;
        cout << "Circle resized to radius " << radius << endl;
    }
};

// Refined Abstraction: Square
class Square : public Shape {
private:
    double side;
    
public:
    Square(shared_ptr<Renderer> r, double s) : Shape(r), side(s) {}
    
    void draw() override {
        renderer->renderSquare(side);
    }
    
    void resize(double factor) override {
        side *= factor;
        cout << "Square resized to side " << side << endl;
    }
};

// ============= Example 3: Message Sending =============

// Implementation
class MessageSender {
public:
    virtual ~MessageSender() = default;
    virtual void sendMessage(const string& message) = 0;
};

// Concrete Implementations
class EmailSender : public MessageSender {
public:
    void sendMessage(const string& message) override {
        cout << "Sending Email: " << message << endl;
    }
};

class SMSSender : public MessageSender {
public:
    void sendMessage(const string& message) override {
        cout << "Sending SMS: " << message << endl;
    }
};

class PushNotificationSender : public MessageSender {
public:
    void sendMessage(const string& message) override {
        cout << "Sending Push Notification: " << message << endl;
    }
};

// Abstraction
class Message {
protected:
    shared_ptr<MessageSender> sender;
    string content;
    
public:
    Message(shared_ptr<MessageSender> s) : sender(s) {}
    virtual ~Message() = default;
    virtual void send() = 0;
};

// Refined Abstractions
class TextMessage : public Message {
public:
    TextMessage(shared_ptr<MessageSender> s, const string& text) : Message(s) {
        content = text;
    }
    
    void send() override {
        cout << "[Text Message] ";
        sender->sendMessage(content);
    }
};

class UrgentMessage : public Message {
public:
    UrgentMessage(shared_ptr<MessageSender> s, const string& text) : Message(s) {
        content = "URGENT: " + text;
    }
    
    void send() override {
        cout << "[Urgent Message] ";
        sender->sendMessage(content);
    }
};

int main() {
    cout << "=== BRIDGE PATTERN DEMO ===" << endl;
    
    // Example 1: Remote Control
    cout << "\n1. REMOTE CONTROL SYSTEM:" << endl;
    cout << "=========================" << endl;
    
    shared_ptr<Device> tv = make_shared<TV>();
    shared_ptr<Device> radio = make_shared<Radio>();
    
    cout << "\n[Basic Remote with TV]" << endl;
    RemoteControl basicRemote(tv);
    basicRemote.togglePower();
    basicRemote.volumeUp();
    basicRemote.channelUp();
    
    cout << "\n[Advanced Remote with TV]" << endl;
    AdvancedRemote advancedRemote(tv);
    advancedRemote.setChannelDirect(99);
    advancedRemote.mute();
    
    cout << "\n[Advanced Remote with Radio]" << endl;
    AdvancedRemote radioRemote(radio);
    radioRemote.togglePower();
    radioRemote.volumeUp();
    radioRemote.setChannelDirect(101);
    
    // Example 2: Drawing Shapes
    cout << "\n\n2. SHAPE RENDERING SYSTEM:" << endl;
    cout << "==========================" << endl;
    
    shared_ptr<Renderer> vectorRenderer = make_shared<VectorRenderer>();
    shared_ptr<Renderer> rasterRenderer = make_shared<RasterRenderer>();
    
    cout << "\n[Vector Rendering]" << endl;
    unique_ptr<Shape> circle1 = make_unique<Circle>(vectorRenderer, 5.0);
    circle1->draw();
    circle1->resize(2.0);
    circle1->draw();
    
    unique_ptr<Shape> square1 = make_unique<Square>(vectorRenderer, 10.0);
    square1->draw();
    
    cout << "\n[Raster Rendering]" << endl;
    unique_ptr<Shape> circle2 = make_unique<Circle>(rasterRenderer, 7.0);
    circle2->draw();
    
    unique_ptr<Shape> square2 = make_unique<Square>(rasterRenderer, 15.0);
    square2->draw();
    
    // Example 3: Message Sending
    cout << "\n\n3. MESSAGE SENDING SYSTEM:" << endl;
    cout << "==========================" << endl;
    
    shared_ptr<MessageSender> emailSender = make_shared<EmailSender>();
    shared_ptr<MessageSender> smsSender = make_shared<SMSSender>();
    shared_ptr<MessageSender> pushSender = make_shared<PushNotificationSender>();
    
    cout << "\n[Sending different messages via different channels]" << endl;
    unique_ptr<Message> textEmail = make_unique<TextMessage>(emailSender, "Hello via Email");
    textEmail->send();
    
    unique_ptr<Message> textSMS = make_unique<TextMessage>(smsSender, "Hello via SMS");
    textSMS->send();
    
    unique_ptr<Message> urgentEmail = make_unique<UrgentMessage>(emailSender, "Server is down!");
    urgentEmail->send();
    
    unique_ptr<Message> urgentPush = make_unique<UrgentMessage>(pushSender, "Meeting in 5 minutes!");
    urgentPush->send();
    
    cout << "\n\n=== KEY TAKEAWAYS ===" << endl;
    cout << "1. Bridge SEPARATES abstraction from implementation" << endl;
    cout << "2. Both can vary independently without affecting each other" << endl;
    cout << "3. Uses composition over inheritance" << endl;
    cout << "4. Solves cartesian product problem (N abstractions × M implementations)" << endl;
    cout << "5. Example: Instead of TVRemote, RadioRemote, AdvancedTVRemote, AdvancedRadioRemote" << endl;
    cout << "   We have: Remote + AdvancedRemote × TV + Radio (2 + 2 vs 4 classes)" << endl;
    cout << "6. Client works with abstraction, unaware of implementation details" << endl;
    
    return 0;
}

