/*
 * TEMPLATE METHOD PATTERN - Behavioral Pattern
 * 
 * PURPOSE:
 * Defines the skeleton of an algorithm in a base class, letting subclasses
 * override specific steps without changing the algorithm's structure.
 * 
 * WHEN TO USE:
 * - Want to let clients extend only particular steps of an algorithm
 * - Have several classes with similar algorithms differing in minor details
 * - Want to control extension points in subclasses
 * 
 * PROS:
 * + Reuses code through inheritance
 * + Lets subclasses override specific parts
 * + Controls which steps can be overridden
 * + Eliminates code duplication
 * 
 * CONS:
 * - Clients limited by provided skeleton
 * - Violates Liskov Substitution Principle
 * - More classes to maintain
 * - Template method can be hard to maintain with many steps
 * 
 * REAL-WORLD EXAMPLES:
 * - Game initialization (load assets, create world, start game)
 * - Data mining from different sources
 * - Building process (compile, test, deploy)
 * - Beverage making (boil water, brew, pour, add condiments)
 */

#include <iostream>
#include <string>
#include <memory>

using namespace std;

// ============= Example 1: Beverage Preparation =============

// Abstract class with template method
class Beverage {
public:
    // Template method - defines algorithm skeleton
    void prepareRecipe() {
        boilWater();
        brew();
        pourInCup();
        if (customerWantsCondiments()) {
            addCondiments();
        }
        serve();
    }
    
    virtual ~Beverage() = default;
    
protected:
    // Steps that can be overridden
    virtual void brew() = 0;
    virtual void addCondiments() = 0;
    virtual string getBeverageName() = 0;
    
    // Hook - subclasses can override but don't have to
    virtual bool customerWantsCondiments() {
        return true;
    }
    
private:
    // Common steps (not overridable)
    void boilWater() {
        cout << "Boiling water..." << endl;
    }
    
    void pourInCup() {
        cout << "Pouring into cup..." << endl;
    }
    
    void serve() {
        cout << getBeverageName() << " is ready! Enjoy!" << endl;
    }
};

// Concrete classes
class Coffee : public Beverage {
protected:
    void brew() override {
        cout << "Dripping coffee through filter" << endl;
    }
    
    void addCondiments() override {
        cout << "Adding sugar and milk" << endl;
    }
    
    string getBeverageName() override {
        return "Coffee";
    }
};

class Tea : public Beverage {
protected:
    void brew() override {
        cout << "Steeping the tea" << endl;
    }
    
    void addCondiments() override {
        cout << "Adding lemon" << endl;
    }
    
    string getBeverageName() override {
        return "Tea";
    }
    
    bool customerWantsCondiments() override {
        // Hook implementation
        return false;  // No lemon in this tea
    }
};

class HotChocolate : public Beverage {
protected:
    void brew() override {
        cout << "Mixing chocolate powder with hot water" << endl;
    }
    
    void addCondiments() override {
        cout << "Adding whipped cream and marshmallows" << endl;
    }
    
    string getBeverageName() override {
        return "Hot Chocolate";
    }
};

// ============= Example 2: Data Mining =============

class DataMiner {
public:
    // Template method
    void mine(const string& path) {
        cout << "\n=== Mining data from: " << path << " ===" << endl;
        
        openFile(path);
        extractData();
        parseData();
        analyzeData();
        sendReport();
        closeFile();
    }
    
    virtual ~DataMiner() = default;
    
protected:
    virtual void openFile(const string& path) = 0;
    virtual void extractData() = 0;
    virtual void closeFile() = 0;
    
    // Common methods with default implementation
    virtual void parseData() {
        cout << "Parsing extracted data..." << endl;
    }
    
    virtual void analyzeData() {
        cout << "Analyzing data..." << endl;
    }
    
private:
    void sendReport() {
        cout << "Sending analysis report..." << endl;
    }
};

class PDFMiner : public DataMiner {
protected:
    void openFile(const string& path) override {
        cout << "Opening PDF file: " << path << endl;
    }
    
    void extractData() override {
        cout << "Extracting text from PDF..." << endl;
    }
    
    void closeFile() override {
        cout << "Closing PDF file" << endl;
    }
};

class CSVMiner : public DataMiner {
protected:
    void openFile(const string& path) override {
        cout << "Opening CSV file: " << path << endl;
    }
    
    void extractData() override {
        cout << "Reading CSV rows and columns..." << endl;
    }
    
    void parseData() override {
        cout << "Parsing CSV format (comma-separated values)..." << endl;
    }
    
    void closeFile() override {
        cout << "Closing CSV file" << endl;
    }
};

class JSONMiner : public DataMiner {
protected:
    void openFile(const string& path) override {
        cout << "Opening JSON file: " << path << endl;
    }
    
    void extractData() override {
        cout << "Reading JSON structure..." << endl;
    }
    
    void parseData() override {
        cout << "Parsing JSON objects and arrays..." << endl;
    }
    
    void closeFile() override {
        cout << "Closing JSON file" << endl;
    }
};

// ============= Example 3: Build Process =============

class BuildProcess {
public:
    // Template method
    void build() {
        cout << "\n=== Starting Build Process ===" << endl;
        
        initialize();
        compile();
        
        if (shouldRunTests()) {
            runTests();
        }
        
        package();
        
        if (shouldDeploy()) {
            deploy();
        }
        
        cleanup();
        
        cout << "=== Build Complete ===" << endl;
    }
    
    virtual ~BuildProcess() = default;
    
protected:
    // Abstract steps
    virtual void compile() = 0;
    virtual void package() = 0;
    virtual string getProjectName() = 0;
    
    // Concrete steps with default implementation
    virtual void initialize() {
        cout << "Initializing build for: " << getProjectName() << endl;
    }
    
    virtual void runTests() {
        cout << "Running unit tests..." << endl;
    }
    
    virtual void deploy() {
        cout << "Deploying application..." << endl;
    }
    
    virtual void cleanup() {
        cout << "Cleaning up temporary files..." << endl;
    }
    
    // Hooks
    virtual bool shouldRunTests() {
        return true;
    }
    
    virtual bool shouldDeploy() {
        return false;
    }
};

class JavaBuild : public BuildProcess {
protected:
    void compile() override {
        cout << "Compiling Java source files (.java -> .class)..." << endl;
    }
    
    void package() override {
        cout << "Creating JAR file..." << endl;
    }
    
    string getProjectName() override {
        return "Java Application";
    }
};

class CppBuild : public BuildProcess {
protected:
    void compile() override {
        cout << "Compiling C++ source files (g++ compilation)..." << endl;
    }
    
    void package() override {
        cout << "Creating executable binary..." << endl;
    }
    
    void runTests() override {
        cout << "Running C++ unit tests with Google Test..." << endl;
    }
    
    string getProjectName() override {
        return "C++ Application";
    }
    
    bool shouldDeploy() override {
        return true;
    }
};

class PythonBuild : public BuildProcess {
protected:
    void compile() override {
        cout << "Checking Python syntax..." << endl;
    }
    
    void package() override {
        cout << "Creating Python wheel package..." << endl;
    }
    
    string getProjectName() override {
        return "Python Application";
    }
    
    bool shouldRunTests() override {
        return true;
    }
    
    bool shouldDeploy() override {
        return true;
    }
};

// ============= Example 4: Game AI =============

class GameAI {
public:
    // Template method
    void takeTurn() {
        cout << "\n[" << getAIName() << "'s Turn]" << endl;
        
        collectResources();
        buildStructures();
        buildUnits();
        attackEnemy();
        
        if (shouldRetreat()) {
            retreat();
        }
    }
    
    virtual ~GameAI() = default;
    
protected:
    virtual void collectResources() = 0;
    virtual void buildUnits() = 0;
    virtual string getAIName() = 0;
    
    virtual void buildStructures() {
        cout << "Building defensive structures..." << endl;
    }
    
    virtual void attackEnemy() {
        cout << "Sending units to attack..." << endl;
    }
    
    virtual void retreat() {
        cout << "Retreating to safe position..." << endl;
    }
    
    virtual bool shouldRetreat() {
        return false;
    }
};

class AggressiveAI : public GameAI {
protected:
    void collectResources() override {
        cout << "Quickly gathering minimum resources..." << endl;
    }
    
    void buildUnits() override {
        cout << "Building offensive units (tanks, soldiers)..." << endl;
    }
    
    string getAIName() override {
        return "Aggressive AI";
    }
};

class DefensiveAI : public GameAI {
protected:
    void collectResources() override {
        cout << "Thoroughly gathering all available resources..." << endl;
    }
    
    void buildStructures() override {
        cout << "Building walls, towers, and fortifications..." << endl;
    }
    
    void buildUnits() override {
        cout << "Building defensive units (archers, guards)..." << endl;
    }
    
    void attackEnemy() override {
        cout << "Defending position, no attack..." << endl;
    }
    
    string getAIName() override {
        return "Defensive AI";
    }
    
    bool shouldRetreat() override {
        return true;
    }
};

int main() {
    cout << "=== TEMPLATE METHOD PATTERN DEMO ===" << endl;
    
    // Example 1: Beverage Preparation
    cout << "\n1. BEVERAGE PREPARATION:" << endl;
    cout << "========================" << endl;
    
    cout << "\n[Making Coffee]" << endl;
    unique_ptr<Beverage> coffee = make_unique<Coffee>();
    coffee->prepareRecipe();
    
    cout << "\n[Making Tea]" << endl;
    unique_ptr<Beverage> tea = make_unique<Tea>();
    tea->prepareRecipe();
    
    cout << "\n[Making Hot Chocolate]" << endl;
    unique_ptr<Beverage> hotChocolate = make_unique<HotChocolate>();
    hotChocolate->prepareRecipe();
    
    // Example 2: Data Mining
    cout << "\n\n2. DATA MINING:" << endl;
    cout << "===============" << endl;
    
    unique_ptr<DataMiner> pdfMiner = make_unique<PDFMiner>();
    pdfMiner->mine("document.pdf");
    
    unique_ptr<DataMiner> csvMiner = make_unique<CSVMiner>();
    csvMiner->mine("data.csv");
    
    unique_ptr<DataMiner> jsonMiner = make_unique<JSONMiner>();
    jsonMiner->mine("config.json");
    
    // Example 3: Build Process
    cout << "\n\n3. BUILD PROCESS:" << endl;
    cout << "=================" << endl;
    
    unique_ptr<BuildProcess> javaBuild = make_unique<JavaBuild>();
    javaBuild->build();
    
    unique_ptr<BuildProcess> cppBuild = make_unique<CppBuild>();
    cppBuild->build();
    
    unique_ptr<BuildProcess> pythonBuild = make_unique<PythonBuild>();
    pythonBuild->build();
    
    // Example 4: Game AI
    cout << "\n\n4. GAME AI:" << endl;
    cout << "===========" << endl;
    
    unique_ptr<GameAI> aggressive = make_unique<AggressiveAI>();
    aggressive->takeTurn();
    
    unique_ptr<GameAI> defensive = make_unique<DefensiveAI>();
    defensive->takeTurn();
    
    cout << "\n\n=== KEY TAKEAWAYS ===" << endl;
    cout << "1. Template Method defines SKELETON of algorithm" << endl;
    cout << "2. Subclasses override specific steps without changing structure" << endl;
    cout << "3. Promotes code reuse through inheritance" << endl;
    cout << "4. Base class controls algorithm flow" << endl;
    cout << "5. Abstract methods MUST be implemented by subclasses" << endl;
    cout << "6. Hook methods CAN be overridden (optional)" << endl;
    cout << "7. Final methods CANNOT be overridden" << endl;
    cout << "8. Hollywood Principle: 'Don't call us, we'll call you'" << endl;
    
    return 0;
}

