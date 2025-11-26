/*
 * ABSTRACT FACTORY PATTERN - Creational Pattern
 * 
 * PURPOSE:
 * Provides an interface for creating families of related or dependent objects
 * without specifying their concrete classes.
 * 
 * WHEN TO USE:
 * - System should be independent of how its products are created
 * - System should work with multiple families of related products
 * - Need to provide a library of products revealing only interfaces
 * - Want to enforce that related products are used together
 * 
 * PROS:
 * + Ensures compatibility between products of the same family
 * + Isolates concrete classes
 * + Easy to exchange product families
 * + Promotes consistency among products
 * 
 * CONS:
 * - Adding new products is difficult (must change all factories)
 * - More complex than Factory Method
 * - Lots of classes and interfaces
 * 
 * REAL-WORLD EXAMPLES:
 * - Cross-platform UI libraries (Windows/Mac/Linux widgets)
 * - Database access layers (MySQL/PostgreSQL/Oracle)
 * - Theme systems (Dark/Light themes)
 * - Game asset systems (Medieval/SciFi/Modern themes)
 */

#include <iostream>
#include <string>
#include <memory>

using namespace std;

// Abstract Products - Buttons
class Button {
public:
    virtual ~Button() = default;
    virtual void render() const = 0;
    virtual void onClick() const = 0;
};

// Abstract Products - Checkboxes
class Checkbox {
public:
    virtual ~Checkbox() = default;
    virtual void render() const = 0;
    virtual void check() const = 0;
};

// Abstract Products - TextFields
class TextField {
public:
    virtual ~TextField() = default;
    virtual void render() const = 0;
    virtual void setText(const string& text) const = 0;
};

// ============= WINDOWS FAMILY =============
class WindowsButton : public Button {
public:
    void render() const override {
        cout << "Rendering Windows style button [◻ Button]" << endl;
    }
    
    void onClick() const override {
        cout << "Windows button clicked!" << endl;
    }
};

class WindowsCheckbox : public Checkbox {
public:
    void render() const override {
        cout << "Rendering Windows style checkbox [☐]" << endl;
    }
    
    void check() const override {
        cout << "Windows checkbox checked! [☑]" << endl;
    }
};

class WindowsTextField : public TextField {
public:
    void render() const override {
        cout << "Rendering Windows style text field |_______|" << endl;
    }
    
    void setText(const string& text) const override {
        cout << "Windows text field: |" << text << "|" << endl;
    }
};

// ============= MAC FAMILY =============
class MacButton : public Button {
public:
    void render() const override {
        cout << "Rendering Mac style button (◉ Button)" << endl;
    }
    
    void onClick() const override {
        cout << "Mac button clicked!" << endl;
    }
};

class MacCheckbox : public Checkbox {
public:
    void render() const override {
        cout << "Rendering Mac style checkbox ○" << endl;
    }
    
    void check() const override {
        cout << "Mac checkbox checked! ●" << endl;
    }
};

class MacTextField : public TextField {
public:
    void render() const override {
        cout << "Rendering Mac style text field ⟨_______⟩" << endl;
    }
    
    void setText(const string& text) const override {
        cout << "Mac text field: ⟨" << text << "⟩" << endl;
    }
};

// ============= LINUX FAMILY =============
class LinuxButton : public Button {
public:
    void render() const override {
        cout << "Rendering Linux style button [Button]" << endl;
    }
    
    void onClick() const override {
        cout << "Linux button clicked!" << endl;
    }
};

class LinuxCheckbox : public Checkbox {
public:
    void render() const override {
        cout << "Rendering Linux style checkbox [ ]" << endl;
    }
    
    void check() const override {
        cout << "Linux checkbox checked! [X]" << endl;
    }
};

class LinuxTextField : public TextField {
public:
    void render() const override {
        cout << "Rendering Linux style text field [_______]" << endl;
    }
    
    void setText(const string& text) const override {
        cout << "Linux text field: [" << text << "]" << endl;
    }
};

// Abstract Factory
class GUIFactory {
public:
    virtual ~GUIFactory() = default;
    virtual unique_ptr<Button> createButton() const = 0;
    virtual unique_ptr<Checkbox> createCheckbox() const = 0;
    virtual unique_ptr<TextField> createTextField() const = 0;
};

// Concrete Factories
class WindowsFactory : public GUIFactory {
public:
    unique_ptr<Button> createButton() const override {
        return make_unique<WindowsButton>();
    }
    
    unique_ptr<Checkbox> createCheckbox() const override {
        return make_unique<WindowsCheckbox>();
    }
    
    unique_ptr<TextField> createTextField() const override {
        return make_unique<WindowsTextField>();
    }
};

class MacFactory : public GUIFactory {
public:
    unique_ptr<Button> createButton() const override {
        return make_unique<MacButton>();
    }
    
    unique_ptr<Checkbox> createCheckbox() const override {
        return make_unique<MacCheckbox>();
    }
    
    unique_ptr<TextField> createTextField() const override {
        return make_unique<MacTextField>();
    }
};

class LinuxFactory : public GUIFactory {
public:
    unique_ptr<Button> createButton() const override {
        return make_unique<LinuxButton>();
    }
    
    unique_ptr<Checkbox> createCheckbox() const override {
        return make_unique<LinuxCheckbox>();
    }
    
    unique_ptr<TextField> createTextField() const override {
        return make_unique<LinuxTextField>();
    }
};

// Client code that uses the factory
class Application {
private:
    unique_ptr<Button> button;
    unique_ptr<Checkbox> checkbox;
    unique_ptr<TextField> textField;
    
public:
    Application(const GUIFactory& factory) {
        button = factory.createButton();
        checkbox = factory.createCheckbox();
        textField = factory.createTextField();
    }
    
    void render() {
        cout << "\nRendering UI components:" << endl;
        button->render();
        checkbox->render();
        textField->render();
    }
    
    void interact() {
        cout << "\nInteracting with UI components:" << endl;
        button->onClick();
        checkbox->check();
        textField->setText("Hello World");
    }
};

// Factory for selecting the appropriate GUI factory based on OS
unique_ptr<GUIFactory> getFactory(const string& os) {
    if (os == "Windows") {
        return make_unique<WindowsFactory>();
    } else if (os == "Mac") {
        return make_unique<MacFactory>();
    } else if (os == "Linux") {
        return make_unique<LinuxFactory>();
    }
    return make_unique<WindowsFactory>(); // Default
}

int main() {
    cout << "=== ABSTRACT FACTORY PATTERN DEMO ===" << endl;
    cout << "Creating cross-platform GUI applications" << endl;
    
    // Simulate different operating systems
    string operatingSystems[] = {"Windows", "Mac", "Linux"};
    
    for (const string& os : operatingSystems) {
        cout << "\n" << string(50, '=') << endl;
        cout << "Running on: " << os << endl;
        cout << string(50, '=') << endl;
        
        // Get appropriate factory for the OS
        unique_ptr<GUIFactory> factory = getFactory(os);
        
        // Create application with the factory
        Application app(*factory);
        
        // Render and interact with UI
        app.render();
        app.interact();
    }
    
    cout << "\n\n=== KEY TAKEAWAYS ===" << endl;
    cout << "1. Abstract Factory creates FAMILIES of related objects" << endl;
    cout << "2. All products in a family are compatible with each other" << endl;
    cout << "3. Client code works with factories through abstract interfaces" << endl;
    cout << "4. Easy to switch between different product families" << endl;
    cout << "5. Ensures consistency (e.g., all UI elements match the OS style)" << endl;
    cout << "\nDifference from Factory Method:" << endl;
    cout << "- Factory Method: Creates ONE product" << endl;
    cout << "- Abstract Factory: Creates FAMILIES of related products" << endl;
    
    return 0;
}

