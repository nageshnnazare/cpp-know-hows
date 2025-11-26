/*
 * DECORATOR PATTERN - Structural Pattern
 * 
 * PURPOSE:
 * Attaches additional responsibilities to an object dynamically.
 * Provides a flexible alternative to subclassing for extending functionality.
 * 
 * WHEN TO USE:
 * - Want to add responsibilities to objects dynamically and transparently
 * - Need to add responsibilities that can be withdrawn
 * - Extension by subclassing is impractical (too many combinations)
 * - Want to avoid class explosion
 * 
 * PROS:
 * + More flexible than static inheritance
 * + Avoids feature-laden classes high up in hierarchy
 * + Can add/remove responsibilities at runtime
 * + Can combine multiple decorators
 * + Single Responsibility Principle: each decorator has one job
 * + Open/Closed Principle: extend behavior without modifying existing code
 * 
 * CONS:
 * - Lots of small classes can be hard to understand
 * - Order of decorators matters
 * - Hard to remove a specific decorator from the stack
 * 
 * REAL-WORLD EXAMPLES:
 * - GUI components (scrollbars, borders)
 * - I/O streams (BufferedReader, GZIPInputStream in Java)
 * - Coffee shop (coffee + milk + sugar + whipped cream)
 * - Text formatting (bold, italic, underline)
 */

#include <iostream>
#include <string>
#include <memory>

using namespace std;

// ============= Example 1: Coffee Shop =============

// Component interface
class Coffee {
public:
    virtual ~Coffee() = default;
    virtual string getDescription() const = 0;
    virtual double getCost() const = 0;
};

// Concrete Component
class SimpleCoffee : public Coffee {
public:
    string getDescription() const override {
        return "Simple Coffee";
    }
    
    double getCost() const override {
        return 2.0;
    }
};

class Espresso : public Coffee {
public:
    string getDescription() const override {
        return "Espresso";
    }
    
    double getCost() const override {
        return 3.0;
    }
};

// Base Decorator
class CoffeeDecorator : public Coffee {
protected:
    unique_ptr<Coffee> coffee;
    
public:
    CoffeeDecorator(unique_ptr<Coffee> c) : coffee(move(c)) {}
};

// Concrete Decorators
class Milk : public CoffeeDecorator {
public:
    Milk(unique_ptr<Coffee> c) : CoffeeDecorator(move(c)) {}
    
    string getDescription() const override {
        return coffee->getDescription() + " + Milk";
    }
    
    double getCost() const override {
        return coffee->getCost() + 0.5;
    }
};

class Sugar : public CoffeeDecorator {
public:
    Sugar(unique_ptr<Coffee> c) : CoffeeDecorator(move(c)) {}
    
    string getDescription() const override {
        return coffee->getDescription() + " + Sugar";
    }
    
    double getCost() const override {
        return coffee->getCost() + 0.2;
    }
};

class WhippedCream : public CoffeeDecorator {
public:
    WhippedCream(unique_ptr<Coffee> c) : CoffeeDecorator(move(c)) {}
    
    string getDescription() const override {
        return coffee->getDescription() + " + Whipped Cream";
    }
    
    double getCost() const override {
        return coffee->getCost() + 0.7;
    }
};

class Caramel : public CoffeeDecorator {
public:
    Caramel(unique_ptr<Coffee> c) : CoffeeDecorator(move(c)) {}
    
    string getDescription() const override {
        return coffee->getDescription() + " + Caramel";
    }
    
    double getCost() const override {
        return coffee->getCost() + 0.6;
    }
};

// ============= Example 2: Text Formatting =============

// Component
class Text {
public:
    virtual ~Text() = default;
    virtual string render() const = 0;
};

// Concrete Component
class PlainText : public Text {
private:
    string content;
    
public:
    PlainText(const string& text) : content(text) {}
    
    string render() const override {
        return content;
    }
};

// Base Decorator
class TextDecorator : public Text {
protected:
    unique_ptr<Text> text;
    
public:
    TextDecorator(unique_ptr<Text> t) : text(move(t)) {}
};

// Concrete Decorators
class BoldDecorator : public TextDecorator {
public:
    BoldDecorator(unique_ptr<Text> t) : TextDecorator(move(t)) {}
    
    string render() const override {
        return "<b>" + text->render() + "</b>";
    }
};

class ItalicDecorator : public TextDecorator {
public:
    ItalicDecorator(unique_ptr<Text> t) : TextDecorator(move(t)) {}
    
    string render() const override {
        return "<i>" + text->render() + "</i>";
    }
};

class UnderlineDecorator : public TextDecorator {
public:
    UnderlineDecorator(unique_ptr<Text> t) : TextDecorator(move(t)) {}
    
    string render() const override {
        return "<u>" + text->render() + "</u>";
    }
};

// ============= Example 3: Data Source (I/O) =============

// Component
class DataSource {
public:
    virtual ~DataSource() = default;
    virtual void writeData(const string& data) = 0;
    virtual string readData() = 0;
};

// Concrete Component
class FileDataSource : public DataSource {
private:
    string filename;
    string content;
    
public:
    FileDataSource(const string& name) : filename(name) {}
    
    void writeData(const string& data) override {
        content = data;
        cout << "Writing to file '" << filename << "': " << data << endl;
    }
    
    string readData() override {
        cout << "Reading from file '" << filename << "'" << endl;
        return content;
    }
};

// Base Decorator
class DataSourceDecorator : public DataSource {
protected:
    unique_ptr<DataSource> wrappee;
    
public:
    DataSourceDecorator(unique_ptr<DataSource> source) : wrappee(move(source)) {}
};

// Concrete Decorators
class EncryptionDecorator : public DataSourceDecorator {
public:
    EncryptionDecorator(unique_ptr<DataSource> source) 
        : DataSourceDecorator(move(source)) {}
    
    void writeData(const string& data) override {
        string encrypted = encrypt(data);
        cout << "Encrypting data..." << endl;
        wrappee->writeData(encrypted);
    }
    
    string readData() override {
        string data = wrappee->readData();
        cout << "Decrypting data..." << endl;
        return decrypt(data);
    }
    
private:
    string encrypt(const string& data) {
        // Simple Caesar cipher for demonstration
        string result = data;
        for (char& c : result) {
            if (isalpha(c)) {
                c = ((c - 'a' + 3) % 26) + 'a';
            }
        }
        return result;
    }
    
    string decrypt(const string& data) {
        string result = data;
        for (char& c : result) {
            if (isalpha(c)) {
                c = ((c - 'a' - 3 + 26) % 26) + 'a';
            }
        }
        return result;
    }
};

class CompressionDecorator : public DataSourceDecorator {
public:
    CompressionDecorator(unique_ptr<DataSource> source) 
        : DataSourceDecorator(move(source)) {}
    
    void writeData(const string& data) override {
        string compressed = compress(data);
        cout << "Compressing data (from " << data.length() 
             << " to " << compressed.length() << " chars)..." << endl;
        wrappee->writeData(compressed);
    }
    
    string readData() override {
        string data = wrappee->readData();
        cout << "Decompressing data..." << endl;
        return decompress(data);
    }
    
private:
    string compress(const string& data) {
        // Simple mock compression (remove spaces)
        string result;
        for (char c : data) {
            if (c != ' ') result += c;
        }
        return result;
    }
    
    string decompress(const string& data) {
        // Mock decompression (add spaces back)
        return data;
    }
};

void displayCoffee(const Coffee& coffee) {
    cout << coffee.getDescription() << " - $" << coffee.getCost() << endl;
}

int main() {
    cout << "=== DECORATOR PATTERN DEMO ===" << endl;
    
    // Example 1: Coffee Shop
    cout << "\n1. COFFEE SHOP:" << endl;
    cout << "===============" << endl;
    
    // Simple coffee
    unique_ptr<Coffee> coffee1 = make_unique<SimpleCoffee>();
    displayCoffee(*coffee1);
    
    // Coffee with milk
    unique_ptr<Coffee> coffee2 = make_unique<Milk>(make_unique<SimpleCoffee>());
    displayCoffee(*coffee2);
    
    // Coffee with milk and sugar
    unique_ptr<Coffee> coffee3 = make_unique<Sugar>(
        make_unique<Milk>(make_unique<SimpleCoffee>())
    );
    displayCoffee(*coffee3);
    
    // Espresso with everything
    unique_ptr<Coffee> coffee4 = make_unique<Caramel>(
        make_unique<WhippedCream>(
            make_unique<Sugar>(
                make_unique<Milk>(make_unique<Espresso>())
            )
        )
    );
    displayCoffee(*coffee4);
    
    // Example 2: Text Formatting
    cout << "\n2. TEXT FORMATTING:" << endl;
    cout << "===================" << endl;
    
    // Plain text
    unique_ptr<Text> text1 = make_unique<PlainText>("Hello World");
    cout << "Plain: " << text1->render() << endl;
    
    // Bold text
    unique_ptr<Text> text2 = make_unique<BoldDecorator>(
        make_unique<PlainText>("Hello World")
    );
    cout << "Bold: " << text2->render() << endl;
    
    // Bold and italic
    unique_ptr<Text> text3 = make_unique<ItalicDecorator>(
        make_unique<BoldDecorator>(
            make_unique<PlainText>("Hello World")
        )
    );
    cout << "Bold + Italic: " << text3->render() << endl;
    
    // Bold, italic, and underline
    unique_ptr<Text> text4 = make_unique<UnderlineDecorator>(
        make_unique<ItalicDecorator>(
            make_unique<BoldDecorator>(
                make_unique<PlainText>("Hello World")
            )
        )
    );
    cout << "Bold + Italic + Underline: " << text4->render() << endl;
    
    // Example 3: Data Source with Encryption and Compression
    cout << "\n3. DATA SOURCE (I/O with Encryption & Compression):" << endl;
    cout << "===================================================" << endl;
    
    // Plain file
    cout << "\n[Plain File]" << endl;
    unique_ptr<DataSource> source1 = make_unique<FileDataSource>("data.txt");
    source1->writeData("Hello World");
    cout << "Read: " << source1->readData() << endl;
    
    // File with encryption
    cout << "\n[File with Encryption]" << endl;
    unique_ptr<DataSource> source2 = make_unique<EncryptionDecorator>(
        make_unique<FileDataSource>("encrypted.txt")
    );
    source2->writeData("hello world");
    cout << "Read: " << source2->readData() << endl;
    
    // File with compression
    cout << "\n[File with Compression]" << endl;
    unique_ptr<DataSource> source3 = make_unique<CompressionDecorator>(
        make_unique<FileDataSource>("compressed.txt")
    );
    source3->writeData("hello world with spaces");
    cout << "Read: " << source3->readData() << endl;
    
    // File with both encryption and compression
    cout << "\n[File with Encryption + Compression]" << endl;
    unique_ptr<DataSource> source4 = make_unique<EncryptionDecorator>(
        make_unique<CompressionDecorator>(
            make_unique<FileDataSource>("secure.txt")
        )
    );
    source4->writeData("hello world with spaces");
    cout << "Read: " << source4->readData() << endl;
    
    cout << "\n\n=== KEY TAKEAWAYS ===" << endl;
    cout << "1. Decorator ADDS functionality to objects dynamically" << endl;
    cout << "2. Alternative to subclassing for extending behavior" << endl;
    cout << "3. Can wrap objects in multiple decorators (like layers)" << endl;
    cout << "4. Each decorator adds one specific feature" << endl;
    cout << "5. Order of decorators matters!" << endl;
    cout << "6. Decorators and component share the same interface" << endl;
    cout << "7. Prevents class explosion from all possible combinations" << endl;
    
    return 0;
}

