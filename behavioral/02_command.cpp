/*
 * COMMAND PATTERN - Behavioral Pattern
 * 
 * PURPOSE:
 * Encapsulates a request as an object, thereby letting you parameterize clients
 * with different requests, queue or log requests, and support undoable operations.
 * 
 * WHEN TO USE:
 * - Want to parameterize objects with operations
 * - Want to queue operations, schedule execution, or execute remotely
 * - Want to support undo/redo functionality
 * - Want to log changes for crash recovery
 * - Want to structure system around high-level operations
 * 
 * PROS:
 * + Decouples invoker from receiver
 * + Can assemble commands into composite commands
 * + Easy to add new commands (Open/Closed)
 * + Supports undo/redo
 * + Supports deferred execution
 * 
 * CONS:
 * - Increases number of classes
 * - Can be overkill for simple operations
 * 
 * REAL-WORLD EXAMPLES:
 * - Text editor (undo/redo)
 * - GUI buttons and menu items
 * - Task scheduling
 * - Database transactions
 */

#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <stack>

using namespace std;

// ============= Example 1: Text Editor with Undo/Redo =============

// Receiver
class TextDocument {
private:
    string text;
    
public:
    void insertText(const string& newText) {
        text += newText;
        cout << "Inserted: \"" << newText << "\"" << endl;
    }
    
    void deleteText(int length) {
        if (length <= text.length()) {
            text = text.substr(0, text.length() - length);
            cout << "Deleted " << length << " characters" << endl;
        }
    }
    
    void replaceText(const string& oldText, const string& newText) {
        size_t pos = text.find(oldText);
        if (pos != string::npos) {
            text.replace(pos, oldText.length(), newText);
            cout << "Replaced \"" << oldText << "\" with \"" << newText << "\"" << endl;
        }
    }
    
    string getText() const { return text; }
    
    void display() const {
        cout << "Current text: \"" << text << "\"" << endl;
    }
};

// Command interface
class Command {
public:
    virtual ~Command() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
};

// Concrete Commands
class InsertCommand : public Command {
private:
    TextDocument& doc;
    string text;
    
public:
    InsertCommand(TextDocument& d, const string& t) : doc(d), text(t) {}
    
    void execute() override {
        doc.insertText(text);
    }
    
    void undo() override {
        doc.deleteText(text.length());
    }
};

class DeleteCommand : public Command {
private:
    TextDocument& doc;
    string deletedText;
    int length;
    
public:
    DeleteCommand(TextDocument& d, int len) : doc(d), length(len) {
        // Save text before deletion for undo
        string currentText = doc.getText();
        if (len <= currentText.length()) {
            deletedText = currentText.substr(currentText.length() - len);
        }
    }
    
    void execute() override {
        doc.deleteText(length);
    }
    
    void undo() override {
        doc.insertText(deletedText);
    }
};

// Invoker
class TextEditor {
private:
    stack<shared_ptr<Command>> undoStack;
    stack<shared_ptr<Command>> redoStack;
    
public:
    void executeCommand(shared_ptr<Command> command) {
        command->execute();
        undoStack.push(command);
        // Clear redo stack when new command is executed
        while (!redoStack.empty()) redoStack.pop();
    }
    
    void undo() {
        if (!undoStack.empty()) {
            shared_ptr<Command> command = undoStack.top();
            undoStack.pop();
            command->undo();
            redoStack.push(command);
            cout << "Undo performed" << endl;
        } else {
            cout << "Nothing to undo" << endl;
        }
    }
    
    void redo() {
        if (!redoStack.empty()) {
            shared_ptr<Command> command = redoStack.top();
            redoStack.pop();
            command->execute();
            undoStack.push(command);
            cout << "Redo performed" << endl;
        } else {
            cout << "Nothing to redo" << endl;
        }
    }
};

// ============= Example 2: Smart Home System =============

// Receivers
class Light {
private:
    string location;
    bool isOn;
    
public:
    Light(const string& loc) : location(loc), isOn(false) {}
    
    void on() {
        isOn = true;
        cout << location << " light is ON" << endl;
    }
    
    void off() {
        isOn = false;
        cout << location << " light is OFF" << endl;
    }
};

class Thermostat {
private:
    int temperature;
    
public:
    Thermostat() : temperature(20) {}
    
    void setTemperature(int temp) {
        temperature = temp;
        cout << "Thermostat set to " << temperature << "°C" << endl;
    }
    
    int getTemperature() const { return temperature; }
};

class GarageDoor {
private:
    bool isOpen;
    
public:
    GarageDoor() : isOpen(false) {}
    
    void open() {
        isOpen = true;
        cout << "Garage door is OPEN" << endl;
    }
    
    void close() {
        isOpen = false;
        cout << "Garage door is CLOSED" << endl;
    }
};

// Commands for Smart Home
class LightOnCommand : public Command {
private:
    Light& light;
    
public:
    LightOnCommand(Light& l) : light(l) {}
    void execute() override { light.on(); }
    void undo() override { light.off(); }
};

class LightOffCommand : public Command {
private:
    Light& light;
    
public:
    LightOffCommand(Light& l) : light(l) {}
    void execute() override { light.off(); }
    void undo() override { light.on(); }
};

class ThermostatCommand : public Command {
private:
    Thermostat& thermostat;
    int newTemp;
    int oldTemp;
    
public:
    ThermostatCommand(Thermostat& t, int temp) 
        : thermostat(t), newTemp(temp) {
        oldTemp = thermostat.getTemperature();
    }
    
    void execute() override { 
        thermostat.setTemperature(newTemp); 
    }
    
    void undo() override { 
        thermostat.setTemperature(oldTemp); 
    }
};

class GarageDoorOpenCommand : public Command {
private:
    GarageDoor& door;
    
public:
    GarageDoorOpenCommand(GarageDoor& d) : door(d) {}
    void execute() override { door.open(); }
    void undo() override { door.close(); }
};

// Macro Command (composite)
class MacroCommand : public Command {
private:
    vector<shared_ptr<Command>> commands;
    
public:
    void addCommand(shared_ptr<Command> cmd) {
        commands.push_back(cmd);
    }
    
    void execute() override {
        cout << "\n[Executing Macro Command]" << endl;
        for (auto& cmd : commands) {
            cmd->execute();
        }
    }
    
    void undo() override {
        cout << "\n[Undoing Macro Command]" << endl;
        for (auto it = commands.rbegin(); it != commands.rend(); ++it) {
            (*it)->undo();
        }
    }
};

// Remote Control (Invoker)
class RemoteControl {
private:
    vector<shared_ptr<Command>> commands;
    
public:
    void setCommand(int slot, shared_ptr<Command> cmd) {
        if (slot >= commands.size()) {
            commands.resize(slot + 1);
        }
        commands[slot] = cmd;
    }
    
    void pressButton(int slot) {
        if (slot < commands.size() && commands[slot]) {
            commands[slot]->execute();
        }
    }
};

int main() {
    cout << "=== COMMAND PATTERN DEMO ===" << endl;
    
    // Example 1: Text Editor with Undo/Redo
    cout << "\n1. TEXT EDITOR WITH UNDO/REDO:" << endl;
    cout << "===============================" << endl;
    
    TextDocument doc;
    TextEditor editor;
    
    cout << "\n[Initial state]" << endl;
    doc.display();
    
    cout << "\n[Insert 'Hello']" << endl;
    editor.executeCommand(make_shared<InsertCommand>(doc, "Hello"));
    doc.display();
    
    cout << "\n[Insert ' World']" << endl;
    editor.executeCommand(make_shared<InsertCommand>(doc, " World"));
    doc.display();
    
    cout << "\n[Insert '!']" << endl;
    editor.executeCommand(make_shared<InsertCommand>(doc, "!"));
    doc.display();
    
    cout << "\n[Undo last operation]" << endl;
    editor.undo();
    doc.display();
    
    cout << "\n[Undo again]" << endl;
    editor.undo();
    doc.display();
    
    cout << "\n[Redo]" << endl;
    editor.redo();
    doc.display();
    
    cout << "\n[Insert ' from C++']" << endl;
    editor.executeCommand(make_shared<InsertCommand>(doc, " from C++"));
    doc.display();
    
    // Example 2: Smart Home System
    cout << "\n\n2. SMART HOME SYSTEM:" << endl;
    cout << "=====================" << endl;
    
    // Create devices (receivers)
    Light livingRoomLight("Living Room");
    Light bedroomLight("Bedroom");
    Thermostat thermostat;
    GarageDoor garageDoor;
    
    // Create remote control (invoker)
    RemoteControl remote;
    
    // Assign commands to buttons
    remote.setCommand(0, make_shared<LightOnCommand>(livingRoomLight));
    remote.setCommand(1, make_shared<LightOffCommand>(livingRoomLight));
    remote.setCommand(2, make_shared<LightOnCommand>(bedroomLight));
    remote.setCommand(3, make_shared<ThermostatCommand>(thermostat, 22));
    remote.setCommand(4, make_shared<GarageDoorOpenCommand>(garageDoor));
    
    // Use remote control
    cout << "\n[Pressing buttons]" << endl;
    remote.pressButton(0);  // Living room light on
    remote.pressButton(2);  // Bedroom light on
    remote.pressButton(3);  // Set thermostat
    remote.pressButton(4);  // Open garage
    
    // Macro command - "Leaving Home"
    cout << "\n\n[Creating 'Leaving Home' Macro]" << endl;
    auto leavingHomeMacro = make_shared<MacroCommand>();
    leavingHomeMacro->addCommand(make_shared<LightOffCommand>(livingRoomLight));
    leavingHomeMacro->addCommand(make_shared<LightOffCommand>(bedroomLight));
    leavingHomeMacro->addCommand(make_shared<ThermostatCommand>(thermostat, 18));
    leavingHomeMacro->addCommand(make_shared<GarageDoorOpenCommand>(garageDoor));
    
    cout << "\n[Executing 'Leaving Home']" << endl;
    leavingHomeMacro->execute();
    
    cout << "\n[Undoing 'Leaving Home']" << endl;
    leavingHomeMacro->undo();
    
    cout << "\n\n=== KEY TAKEAWAYS ===" << endl;
    cout << "1. Command ENCAPSULATES requests as objects" << endl;
    cout << "2. Decouples invoker (caller) from receiver (executor)" << endl;
    cout << "3. Supports undo/redo by storing state" << endl;
    cout << "4. Can queue, log, or schedule commands" << endl;
    cout << "5. Macro commands combine multiple commands" << endl;
    cout << "6. Essential for implementing undo/redo functionality" << endl;
    cout << "7. Common in GUI frameworks and transactional systems" << endl;
    
    return 0;
}

