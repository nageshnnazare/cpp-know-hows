/*
 * FACTORY METHOD PATTERN - Creational Pattern
 * 
 * PURPOSE:
 * Defines an interface for creating objects, but lets subclasses decide
 * which class to instantiate. Factory Method lets a class defer instantiation
 * to subclasses.
 * 
 * WHEN TO USE:
 * - Class can't anticipate the type of objects it needs to create
 * - Class wants its subclasses to specify the objects it creates
 * - Classes delegate responsibility to helper subclasses
 * 
 * PROS:
 * + Avoids tight coupling between creator and concrete products
 * + Single Responsibility: product creation code in one place
 * + Open/Closed Principle: can introduce new product types without breaking existing code
 * + More flexible than direct instantiation
 * 
 * CONS:
 * - Code can become more complicated (many subclasses)
 * - Requires subclassing even for simple object creation
 * 
 * REAL-WORLD EXAMPLES:
 * - UI frameworks (creating platform-specific UI elements)
 * - Document frameworks (creating different document types)
 * - Database drivers (creating different connection types)
 * - Logistics apps (creating different transport types)
 */

#include <iostream>
#include <string>
#include <memory>

using namespace std;

// Product interface
class Transport {
public:
    virtual ~Transport() = default;
    virtual void deliver() const = 0;
    virtual string getType() const = 0;
};

// Concrete Products
class Truck : public Transport {
public:
    void deliver() const override {
        cout << "🚚 Delivering by land in a truck" << endl;
    }
    
    string getType() const override {
        return "Truck";
    }
};

class Ship : public Transport {
public:
    void deliver() const override {
        cout << "🚢 Delivering by sea in a ship" << endl;
    }
    
    string getType() const override {
        return "Ship";
    }
};

class Airplane : public Transport {
public:
    void deliver() const override {
        cout << "✈️  Delivering by air in an airplane" << endl;
    }
    
    string getType() const override {
        return "Airplane";
    }
};

// Creator (abstract class with factory method)
class Logistics {
public:
    virtual ~Logistics() = default;
    
    // Factory Method (to be overridden by subclasses)
    virtual unique_ptr<Transport> createTransport() const = 0;
    
    // Business logic that uses the factory method
    void planDelivery() const {
        cout << "Planning delivery..." << endl;
        unique_ptr<Transport> transport = createTransport();
        cout << "Transport type: " << transport->getType() << endl;
        transport->deliver();
        cout << "Delivery completed!" << endl;
    }
};

// Concrete Creators
class RoadLogistics : public Logistics {
public:
    unique_ptr<Transport> createTransport() const override {
        cout << "Creating road transport..." << endl;
        return make_unique<Truck>();
    }
};

class SeaLogistics : public Logistics {
public:
    unique_ptr<Transport> createTransport() const override {
        cout << "Creating sea transport..." << endl;
        return make_unique<Ship>();
    }
};

class AirLogistics : public Logistics {
public:
    unique_ptr<Transport> createTransport() const override {
        cout << "Creating air transport..." << endl;
        return make_unique<Airplane>();
    }
};

// Another example: Document creation system

// Product interface
class Document {
public:
    virtual ~Document() = default;
    virtual void open() const = 0;
    virtual void save() const = 0;
    virtual string getFormat() const = 0;
};

// Concrete Products
class PDFDocument : public Document {
public:
    void open() const override {
        cout << "Opening PDF document..." << endl;
    }
    
    void save() const override {
        cout << "Saving as PDF format" << endl;
    }
    
    string getFormat() const override {
        return "PDF";
    }
};

class WordDocument : public Document {
public:
    void open() const override {
        cout << "Opening Word document..." << endl;
    }
    
    void save() const override {
        cout << "Saving as DOCX format" << endl;
    }
    
    string getFormat() const override {
        return "DOCX";
    }
};

class TextDocument : public Document {
public:
    void open() const override {
        cout << "Opening text document..." << endl;
    }
    
    void save() const override {
        cout << "Saving as TXT format" << endl;
    }
    
    string getFormat() const override {
        return "TXT";
    }
};

// Creator
class Application {
public:
    virtual ~Application() = default;
    virtual unique_ptr<Document> createDocument() const = 0;
    
    void newDocument() const {
        unique_ptr<Document> doc = createDocument();
        cout << "Created new " << doc->getFormat() << " document" << endl;
        doc->open();
        doc->save();
    }
};

// Concrete Creators
class PDFApplication : public Application {
public:
    unique_ptr<Document> createDocument() const override {
        return make_unique<PDFDocument>();
    }
};

class WordApplication : public Application {
public:
    unique_ptr<Document> createDocument() const override {
        return make_unique<WordDocument>();
    }
};

class TextApplication : public Application {
public:
    unique_ptr<Document> createDocument() const override {
        return make_unique<TextDocument>();
    }
};

// Client code
void clientCode(const Logistics& logistics) {
    cout << "Client: Working with logistics (doesn't know the concrete class)" << endl;
    logistics.planDelivery();
}

int main() {
    cout << "=== FACTORY METHOD PATTERN DEMO ===" << endl << endl;
    
    // Example 1: Logistics system
    cout << "1. LOGISTICS SYSTEM:" << endl;
    cout << "-------------------" << endl;
    
    cout << "\n[Scenario 1: Road Delivery]" << endl;
    unique_ptr<Logistics> roadLogistics = make_unique<RoadLogistics>();
    clientCode(*roadLogistics);
    
    cout << "\n[Scenario 2: Sea Delivery]" << endl;
    unique_ptr<Logistics> seaLogistics = make_unique<SeaLogistics>();
    clientCode(*seaLogistics);
    
    cout << "\n[Scenario 3: Air Delivery]" << endl;
    unique_ptr<Logistics> airLogistics = make_unique<AirLogistics>();
    clientCode(*airLogistics);
    
    // Example 2: Document system
    cout << "\n\n2. DOCUMENT CREATION SYSTEM:" << endl;
    cout << "----------------------------" << endl;
    
    cout << "\n[PDF Application]" << endl;
    unique_ptr<Application> pdfApp = make_unique<PDFApplication>();
    pdfApp->newDocument();
    
    cout << "\n[Word Application]" << endl;
    unique_ptr<Application> wordApp = make_unique<WordApplication>();
    wordApp->newDocument();
    
    cout << "\n[Text Editor]" << endl;
    unique_ptr<Application> textApp = make_unique<TextApplication>();
    textApp->newDocument();
    
    cout << "\n\n=== KEY TAKEAWAYS ===" << endl;
    cout << "1. Factory Method delegates object creation to subclasses" << endl;
    cout << "2. Creator class doesn't know which concrete product it will create" << endl;
    cout << "3. Easy to add new product types without modifying existing code" << endl;
    cout << "4. Promotes loose coupling between creator and products" << endl;
    cout << "5. Each concrete creator is responsible for creating one product type" << endl;
    
    return 0;
}

