/*
 * FLYWEIGHT PATTERN - Structural Pattern
 * 
 * PURPOSE:
 * Minimizes memory usage by sharing common data among multiple objects.
 * Uses sharing to support large numbers of fine-grained objects efficiently.
 * 
 * WHEN TO USE:
 * - Application uses large number of objects
 * - Storage costs are high because of sheer quantity
 * - Most object state can be made extrinsic (shared)
 * - Many groups of objects can be replaced by few shared objects
 * - Application doesn't depend on object identity
 * 
 * PROS:
 * + Saves memory when dealing with many similar objects
 * + Improves performance by reducing object creation
 * + Centralized state management
 * 
 * CONS:
 * - Complexity increases
 * - Must carefully distinguish intrinsic vs extrinsic state
 * - May introduce runtime costs for computing extrinsic state
 * 
 * REAL-WORLD EXAMPLES:
 * - Text editors (character objects sharing font/style)
 * - Game development (trees, particles in a forest)
 * - String interning (Java, Python)
 * - Database connection pooling
 */

#include <iostream>
#include <string>
#include <unordered_map>
#include <memory>
#include <vector>

using namespace std;

// ============= Example 1: Forest (Trees) =============

// Flyweight: Intrinsic state (shared)
class TreeType {
private:
    string name;
    string color;
    string texture;
    
public:
    TreeType(const string& n, const string& c, const string& t)
        : name(n), color(c), texture(t) {
        cout << "Creating TreeType: " << name << " (memory allocated)" << endl;
    }
    
    void draw(int x, int y) const {
        cout << "Drawing " << name << " tree (" << color << ", " << texture 
             << ") at (" << x << ", " << y << ")" << endl;
    }
    
    string getName() const { return name; }
};

// Flyweight Factory
class TreeFactory {
private:
    static unordered_map<string, shared_ptr<TreeType>> treeTypes;
    
public:
    static shared_ptr<TreeType> getTreeType(const string& name, 
                                           const string& color, 
                                           const string& texture) {
        string key = name + "_" + color + "_" + texture;
        
        if (treeTypes.find(key) == treeTypes.end()) {
            treeTypes[key] = make_shared<TreeType>(name, color, texture);
        } else {
            cout << "Reusing existing TreeType: " << name << endl;
        }
        
        return treeTypes[key];
    }
    
    static int getTreeTypeCount() {
        return treeTypes.size();
    }
};

// Initialize static member
unordered_map<string, shared_ptr<TreeType>> TreeFactory::treeTypes;

// Context: Extrinsic state (unique per object)
class Tree {
private:
    int x, y;  // Position (extrinsic - unique for each tree)
    shared_ptr<TreeType> type;  // Flyweight (intrinsic - shared)
    
public:
    Tree(int x, int y, shared_ptr<TreeType> type) 
        : x(x), y(y), type(type) {}
    
    void draw() const {
        type->draw(x, y);
    }
};

// Forest that manages many trees
class Forest {
private:
    vector<unique_ptr<Tree>> trees;
    
public:
    void plantTree(int x, int y, const string& name, 
                   const string& color, const string& texture) {
        auto type = TreeFactory::getTreeType(name, color, texture);
        trees.push_back(make_unique<Tree>(x, y, type));
    }
    
    void draw() const {
        cout << "\n=== Drawing Forest ===" << endl;
        for (const auto& tree : trees) {
            tree->draw();
        }
    }
    
    int getTreeCount() const { return trees.size(); }
};

// ============= Example 2: Text Editor (Characters) =============

// Flyweight: Character style (intrinsic)
class CharacterStyle {
private:
    string fontFamily;
    int fontSize;
    string color;
    bool bold;
    bool italic;
    
public:
    CharacterStyle(const string& font, int size, const string& col, 
                   bool b, bool i)
        : fontFamily(font), fontSize(size), color(col), bold(b), italic(i) {
        cout << "Creating CharacterStyle: " << font << ", " << size << "pt" << endl;
    }
    
    void display(char character, int x, int y) const {
        cout << "Char '" << character << "' at (" << x << "," << y << ") - "
             << fontFamily << ", " << fontSize << "pt, " << color;
        if (bold) cout << ", Bold";
        if (italic) cout << ", Italic";
        cout << endl;
    }
};

// Flyweight Factory
class StyleFactory {
private:
    static unordered_map<string, shared_ptr<CharacterStyle>> styles;
    
public:
    static shared_ptr<CharacterStyle> getStyle(const string& font, int size, 
                                              const string& color,
                                              bool bold, bool italic) {
        string key = font + "_" + to_string(size) + "_" + color + "_" +
                    to_string(bold) + "_" + to_string(italic);
        
        if (styles.find(key) == styles.end()) {
            styles[key] = make_shared<CharacterStyle>(font, size, color, bold, italic);
        }
        
        return styles[key];
    }
    
    static int getStyleCount() { return styles.size(); }
};

unordered_map<string, shared_ptr<CharacterStyle>> StyleFactory::styles;

// Context: Individual character
class Character {
private:
    char character;
    int x, y;  // Position (extrinsic)
    shared_ptr<CharacterStyle> style;  // Flyweight (intrinsic)
    
public:
    Character(char c, int x, int y, shared_ptr<CharacterStyle> s)
        : character(c), x(x), y(y), style(s) {}
    
    void display() const {
        style->display(character, x, y);
    }
};

// Document
class Document {
private:
    vector<unique_ptr<Character>> characters;
    
public:
    void addCharacter(char c, int x, int y, const string& font, 
                     int size, const string& color, bool bold, bool italic) {
        auto style = StyleFactory::getStyle(font, size, color, bold, italic);
        characters.push_back(make_unique<Character>(c, x, y, style));
    }
    
    void display() const {
        cout << "\n=== Document Content ===" << endl;
        for (const auto& ch : characters) {
            ch->display();
        }
    }
    
    int getCharacterCount() const { return characters.size(); }
};

// ============= Example 3: Game Particles =============

// Flyweight: Particle properties
class ParticleType {
private:
    string texture;
    string color;
    int size;
    
public:
    ParticleType(const string& tex, const string& col, int sz)
        : texture(tex), color(col), size(sz) {
        cout << "Creating ParticleType: " << texture << " particle" << endl;
    }
    
    void render(int x, int y, int velocityX, int velocityY) const {
        cout << texture << " particle (" << color << ", size:" << size 
             << ") at (" << x << "," << y << ") moving (" 
             << velocityX << "," << velocityY << ")" << endl;
    }
};

// Factory
class ParticleFactory {
private:
    static unordered_map<string, shared_ptr<ParticleType>> particles;
    
public:
    static shared_ptr<ParticleType> getParticle(const string& texture, 
                                                const string& color, int size) {
        string key = texture + "_" + color + "_" + to_string(size);
        
        if (particles.find(key) == particles.end()) {
            particles[key] = make_shared<ParticleType>(texture, color, size);
        }
        
        return particles[key];
    }
};

unordered_map<string, shared_ptr<ParticleType>> ParticleFactory::particles;

// Context
class Particle {
private:
    int x, y;
    int velocityX, velocityY;
    shared_ptr<ParticleType> type;
    
public:
    Particle(int x, int y, int vx, int vy, shared_ptr<ParticleType> t)
        : x(x), y(y), velocityX(vx), velocityY(vy), type(t) {}
    
    void render() const {
        type->render(x, y, velocityX, velocityY);
    }
};

// Particle System
class ParticleSystem {
private:
    vector<unique_ptr<Particle>> particles;
    
public:
    void addParticle(int x, int y, int vx, int vy, 
                     const string& texture, const string& color, int size) {
        auto type = ParticleFactory::getParticle(texture, color, size);
        particles.push_back(make_unique<Particle>(x, y, vx, vy, type));
    }
    
    void render() const {
        cout << "\n=== Rendering Particles ===" << endl;
        for (const auto& particle : particles) {
            particle->render();
        }
    }
    
    int getParticleCount() const { return particles.size(); }
};

int main() {
    cout << "=== FLYWEIGHT PATTERN DEMO ===" << endl;
    
    // Example 1: Forest
    cout << "\n1. FOREST SIMULATION:" << endl;
    cout << "=====================" << endl;
    
    Forest forest;
    
    // Plant many oak trees
    cout << "\n[Planting Oak Trees]" << endl;
    forest.plantTree(10, 20, "Oak", "Green", "Rough");
    forest.plantTree(50, 60, "Oak", "Green", "Rough");
    forest.plantTree(100, 120, "Oak", "Green", "Rough");
    forest.plantTree(150, 180, "Oak", "Green", "Rough");
    
    // Plant pine trees
    cout << "\n[Planting Pine Trees]" << endl;
    forest.plantTree(30, 40, "Pine", "Dark Green", "Smooth");
    forest.plantTree(80, 90, "Pine", "Dark Green", "Smooth");
    forest.plantTree(130, 140, "Pine", "Dark Green", "Smooth");
    
    // Plant birch trees
    cout << "\n[Planting Birch Trees]" << endl;
    forest.plantTree(200, 210, "Birch", "White", "Peeling");
    forest.plantTree(220, 230, "Birch", "White", "Peeling");
    
    forest.draw();
    
    cout << "\n[Memory Statistics]" << endl;
    cout << "Total trees: " << forest.getTreeCount() << endl;
    cout << "TreeType objects created: " << TreeFactory::getTreeTypeCount() << endl;
    cout << "Memory saved: " << (forest.getTreeCount() - TreeFactory::getTreeTypeCount()) 
         << " object reuses" << endl;
    
    // Example 2: Text Editor
    cout << "\n\n2. TEXT EDITOR:" << endl;
    cout << "===============" << endl;
    
    Document doc;
    
    // Add text with different styles
    cout << "\n[Adding styled text]" << endl;
    string text = "Hello";
    int x = 0;
    
    // "Hello" in Arial
    for (char c : text) {
        doc.addCharacter(c, x++, 0, "Arial", 12, "Black", false, false);
    }
    
    // "World" in bold
    text = "World";
    for (char c : text) {
        doc.addCharacter(c, x++, 0, "Arial", 12, "Black", true, false);
    }
    
    doc.display();
    
    cout << "\n[Memory Statistics]" << endl;
    cout << "Total characters: " << doc.getCharacterCount() << endl;
    cout << "Style objects created: " << StyleFactory::getStyleCount() << endl;
    
    // Example 3: Particle System
    cout << "\n\n3. PARTICLE SYSTEM:" << endl;
    cout << "===================" << endl;
    
    ParticleSystem particleSystem;
    
    cout << "\n[Creating explosion effect]" << endl;
    // Create many fire particles
    for (int i = 0; i < 5; i++) {
        particleSystem.addParticle(100 + i*10, 100 + i*5, i, i, 
                                   "Fire", "Red", 5);
    }
    
    // Create smoke particles
    for (int i = 0; i < 3; i++) {
        particleSystem.addParticle(200 + i*15, 200 + i*10, -i, i, 
                                   "Smoke", "Gray", 8);
    }
    
    particleSystem.render();
    
    cout << "\n[Memory Statistics]" << endl;
    cout << "Total particles: " << particleSystem.getParticleCount() << endl;
    
    cout << "\n\n=== KEY TAKEAWAYS ===" << endl;
    cout << "1. Flyweight SHARES common state among many objects" << endl;
    cout << "2. Separates intrinsic (shared) from extrinsic (unique) state" << endl;
    cout << "3. Intrinsic state stored in flyweight (immutable, shared)" << endl;
    cout << "4. Extrinsic state passed to flyweight methods (context-dependent)" << endl;
    cout << "5. Factory manages flyweight pool and ensures sharing" << endl;
    cout << "6. Dramatically reduces memory when many similar objects exist" << endl;
    cout << "7. Trade-off: saves memory but may increase complexity" << endl;
    
    return 0;
}

