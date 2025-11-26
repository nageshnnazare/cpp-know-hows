/*
 * MEDIATOR PATTERN - Behavioral Pattern
 * 
 * PURPOSE:
 * Defines an object that encapsulates how a set of objects interact.
 * Promotes loose coupling by keeping objects from referring to each other explicitly.
 * 
 * WHEN TO USE:
 * - Set of objects communicate in well-defined but complex ways
 * - Reusing object is difficult because it refers to many other objects
 * - Behavior distributed between classes should be customizable without subclassing
 * - Too many interconnections between objects
 * 
 * PROS:
 * + Reduces coupling between components
 * + Centralizes control logic
 * + Simplifies object protocols
 * + Easier to understand object interactions
 * + Single Responsibility Principle
 * + Open/Closed Principle
 * 
 * CONS:
 * - Mediator can become too complex (god object)
 * - Can be harder to maintain
 * 
 * REAL-WORLD EXAMPLES:
 * - Air traffic control (coordinates planes)
 * - Chat rooms (routes messages)
 * - GUI dialog boxes (coordinates widgets)
 * - Auction systems
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>

using namespace std;

// ============= Example 1: Chat Room =============

class User;

// Mediator interface
class ChatRoomMediator {
public:
    virtual ~ChatRoomMediator() = default;
    virtual void sendMessage(const string& message, User* sender) = 0;
    virtual void addUser(User* user) = 0;
};

// Colleague
class User {
protected:
    string name;
    ChatRoomMediator* mediator;
    
public:
    User(const string& n, ChatRoomMediator* med) : name(n), mediator(med) {}
    virtual ~User() = default;
    
    string getName() const { return name; }
    
    virtual void send(const string& message) {
        cout << name << " sends: " << message << endl;
        mediator->sendMessage(message, this);
    }
    
    virtual void receive(const string& message, const string& from) {
        cout << name << " receives from " << from << ": " << message << endl;
    }
};

// Concrete Mediator
class ChatRoom : public ChatRoomMediator {
private:
    vector<User*> users;
    
public:
    void addUser(User* user) override {
        users.push_back(user);
        cout << "[ChatRoom] " << user->getName() << " joined the chat" << endl;
    }
    
    void sendMessage(const string& message, User* sender) override {
        for (User* user : users) {
            // Don't send to self
            if (user != sender) {
                user->receive(message, sender->getName());
            }
        }
    }
};

// ============= Example 2: Air Traffic Control =============

class Aircraft;

// Mediator
class AirTrafficControl {
public:
    virtual ~AirTrafficControl() = default;
    virtual void registerAircraft(Aircraft* aircraft) = 0;
    virtual void requestLanding(Aircraft* aircraft) = 0;
    virtual void requestTakeoff(Aircraft* aircraft) = 0;
};

// Colleague
class Aircraft {
protected:
    string callSign;
    AirTrafficControl* atc;
    
public:
    Aircraft(const string& sign, AirTrafficControl* control) 
        : callSign(sign), atc(control) {
        atc->registerAircraft(this);
    }
    
    virtual ~Aircraft() = default;
    
    string getCallSign() const { return callSign; }
    
    void requestLanding() {
        cout << callSign << " requesting landing permission" << endl;
        atc->requestLanding(this);
    }
    
    void requestTakeoff() {
        cout << callSign << " requesting takeoff permission" << endl;
        atc->requestTakeoff(this);
    }
    
    virtual void receivePermission(const string& instruction) {
        cout << callSign << " received: " << instruction << endl;
    }
};

// Concrete Mediator
class ControlTower : public AirTrafficControl {
private:
    vector<Aircraft*> aircrafts;
    bool runwayAvailable;
    
public:
    ControlTower() : runwayAvailable(true) {}
    
    void registerAircraft(Aircraft* aircraft) override {
        aircrafts.push_back(aircraft);
        cout << "[Tower] " << aircraft->getCallSign() << " registered" << endl;
    }
    
    void requestLanding(Aircraft* aircraft) override {
        if (runwayAvailable) {
            runwayAvailable = false;
            aircraft->receivePermission("Cleared to land on Runway 27");
            // Notify others
            for (Aircraft* other : aircrafts) {
                if (other != aircraft) {
                    other->receivePermission("Hold position, runway occupied");
                }
            }
        } else {
            aircraft->receivePermission("Negative, runway occupied. Hold at 5000ft");
        }
    }
    
    void requestTakeoff(Aircraft* aircraft) override {
        if (runwayAvailable) {
            runwayAvailable = false;
            aircraft->receivePermission("Cleared for takeoff, Runway 27");
        } else {
            aircraft->receivePermission("Negative, runway occupied. Hold short");
        }
    }
    
    void releaseRunway() {
        runwayAvailable = true;
        cout << "[Tower] Runway is now available" << endl;
    }
};

// ============= Example 3: GUI Dialog Box =============

class Widget;

// Mediator
class DialogMediator {
public:
    virtual ~DialogMediator() = default;
    virtual void notify(Widget* sender, const string& event) = 0;
};

// Colleague
class Widget {
protected:
    string name;
    DialogMediator* mediator;
    
public:
    Widget(const string& n, DialogMediator* med) : name(n), mediator(med) {}
    virtual ~Widget() = default;
    
    string getName() const { return name; }
    
    virtual void click() = 0;
    virtual void enable() = 0;
    virtual void disable() = 0;
};

// Concrete Colleagues
class Button : public Widget {
private:
    bool enabled;
    
public:
    Button(const string& n, DialogMediator* med) 
        : Widget(n, med), enabled(true) {}
    
    void click() override {
        if (enabled) {
            cout << "Button '" << name << "' clicked" << endl;
            mediator->notify(this, "click");
        }
    }
    
    void enable() override {
        enabled = true;
        cout << "Button '" << name << "' enabled" << endl;
    }
    
    void disable() override {
        enabled = false;
        cout << "Button '" << name << "' disabled" << endl;
    }
};

class Checkbox : public Widget {
private:
    bool checked;
    
public:
    Checkbox(const string& n, DialogMediator* med) 
        : Widget(n, med), checked(false) {}
    
    void click() override {
        checked = !checked;
        cout << "Checkbox '" << name << "' is now " 
             << (checked ? "checked" : "unchecked") << endl;
        mediator->notify(this, checked ? "checked" : "unchecked");
    }
    
    void enable() override {
        cout << "Checkbox '" << name << "' enabled" << endl;
    }
    
    void disable() override {
        cout << "Checkbox '" << name << "' disabled" << endl;
    }
    
    bool isChecked() const { return checked; }
};

class TextBox : public Widget {
private:
    string text;
    bool enabled;
    
public:
    TextBox(const string& n, DialogMediator* med) 
        : Widget(n, med), enabled(true) {}
    
    void click() override {
        if (enabled) {
            cout << "TextBox '" << name << "' focused" << endl;
        }
    }
    
    void enable() override {
        enabled = true;
        cout << "TextBox '" << name << "' enabled" << endl;
    }
    
    void disable() override {
        enabled = false;
        text = "";
        cout << "TextBox '" << name << "' disabled and cleared" << endl;
    }
    
    void setText(const string& t) { text = t; }
    string getText() const { return text; }
};

// Concrete Mediator
class LoginDialog : public DialogMediator {
private:
    Checkbox* rememberMe;
    Button* loginButton;
    Button* cancelButton;
    TextBox* usernameBox;
    TextBox* passwordBox;
    
public:
    LoginDialog() {
        rememberMe = new Checkbox("Remember Me", this);
        loginButton = new Button("Login", this);
        cancelButton = new Button("Cancel", this);
        usernameBox = new TextBox("Username", this);
        passwordBox = new TextBox("Password", this);
    }
    
    ~LoginDialog() {
        delete rememberMe;
        delete loginButton;
        delete cancelButton;
        delete usernameBox;
        delete passwordBox;
    }
    
    void notify(Widget* sender, const string& event) override {
        cout << "[Dialog] Handling event: " << event 
             << " from " << sender->getName() << endl;
        
        // Coordinate behavior between widgets
        if (sender == rememberMe && event == "checked") {
            cout << "[Dialog] Remember Me enabled - future logins will be saved" << endl;
        }
        else if (sender == rememberMe && event == "unchecked") {
            cout << "[Dialog] Remember Me disabled" << endl;
        }
        else if (sender == loginButton && event == "click") {
            cout << "[Dialog] Processing login..." << endl;
            if (rememberMe->isChecked()) {
                cout << "[Dialog] Saving credentials for future use" << endl;
            }
        }
        else if (sender == cancelButton && event == "click") {
            cout << "[Dialog] Login cancelled - clearing fields" << endl;
            usernameBox->disable();
            passwordBox->disable();
        }
    }
    
    void show() {
        cout << "\n[Dialog] Showing login dialog" << endl;
        cout << "Components: " << endl;
        cout << "  - Username field" << endl;
        cout << "  - Password field" << endl;
        cout << "  - Remember Me checkbox" << endl;
        cout << "  - Login button" << endl;
        cout << "  - Cancel button" << endl;
    }
    
    Button* getLoginButton() { return loginButton; }
    Button* getCancelButton() { return cancelButton; }
    Checkbox* getRememberMeCheckbox() { return rememberMe; }
};

int main() {
    cout << "=== MEDIATOR PATTERN DEMO ===" << endl;
    
    // Example 1: Chat Room
    cout << "\n1. CHAT ROOM:" << endl;
    cout << "=============" << endl;
    
    ChatRoom chatRoom;
    
    User alice("Alice", &chatRoom);
    User bob("Bob", &chatRoom);
    User charlie("Charlie", &chatRoom);
    
    chatRoom.addUser(&alice);
    chatRoom.addUser(&bob);
    chatRoom.addUser(&charlie);
    
    cout << "\n[Chat conversation]" << endl;
    alice.send("Hi everyone!");
    bob.send("Hey Alice!");
    charlie.send("Hello all!");
    
    // Example 2: Air Traffic Control
    cout << "\n\n2. AIR TRAFFIC CONTROL:" << endl;
    cout << "=======================" << endl;
    
    ControlTower tower;
    
    Aircraft flight1("UA123", &tower);
    Aircraft flight2("AA456", &tower);
    Aircraft flight3("DL789", &tower);
    
    cout << "\n[Landing requests]" << endl;
    flight1.requestLanding();
    cout << endl;
    flight2.requestLanding();
    
    cout << "\n[Runway released]" << endl;
    tower.releaseRunway();
    
    cout << "\n[Takeoff request]" << endl;
    flight3.requestTakeoff();
    
    // Example 3: GUI Dialog
    cout << "\n\n3. GUI DIALOG BOX:" << endl;
    cout << "==================" << endl;
    
    LoginDialog dialog;
    dialog.show();
    
    cout << "\n[User interactions]" << endl;
    dialog.getRememberMeCheckbox()->click();
    dialog.getLoginButton()->click();
    
    cout << "\n[Another session]" << endl;
    dialog.getRememberMeCheckbox()->click();  // Uncheck
    dialog.getCancelButton()->click();
    
    cout << "\n\n=== KEY TAKEAWAYS ===" << endl;
    cout << "1. Mediator CENTRALIZES communication between objects" << endl;
    cout << "2. Objects don't communicate directly with each other" << endl;
    cout << "3. Reduces coupling between communicating objects" << endl;
    cout << "4. Makes it easier to understand and modify interactions" << endl;
    cout << "5. Can become complex if mediator has too many responsibilities" << endl;
    cout << "6. Common in GUI frameworks and coordination systems" << endl;
    
    return 0;
}

