/*
 * MEMENTO PATTERN - Behavioral Pattern
 * 
 * PURPOSE:
 * Captures and externalizes an object's internal state so it can be restored later,
 * without violating encapsulation.
 * 
 * WHEN TO USE:
 * - Need to save/restore object state (undo/redo, snapshots)
 * - Direct access to state would expose implementation details
 * - Want to provide rollback capability
 * 
 * PROS:
 * + Preserves encapsulation boundaries
 * + Simplifies originator (state management extracted)
 * + Can implement undo/redo functionality
 * 
 * CONS:
 * - Can be expensive if originator's state is large
 * - Caretaker must track memento lifecycle
 * - May require lots of memory for multiple snapshots
 * 
 * REAL-WORLD EXAMPLES:
 * - Text editor undo/redo
 * - Game save states
 * - Database transactions (rollback)
 * - Version control systems
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <ctime>

using namespace std;

// ============= Example 1: Text Editor =============

// Memento - stores state
class EditorMemento {
private:
    string content;
    int cursorPosition;
    string timestamp;
    
    // Only Originator can access private members
    friend class TextEditor;
    
public:
    EditorMemento(const string& cont, int pos) 
        : content(cont), cursorPosition(pos) {
        time_t now = time(0);
        timestamp = ctime(&now);
    }
    
    string getTimestamp() const { return timestamp; }
};

// Originator
class TextEditor {
private:
    string content;
    int cursorPosition;
    
public:
    TextEditor() : content(""), cursorPosition(0) {}
    
    void type(const string& text) {
        content += text;
        cursorPosition = content.length();
        cout << "Typed: \"" << text << "\"" << endl;
    }
    
    void deleteLast(int count) {
        if (count <= content.length()) {
            content = content.substr(0, content.length() - count);
            cursorPosition = content.length();
            cout << "Deleted last " << count << " characters" << endl;
        }
    }
    
    void display() const {
        cout << "Content: \"" << content << "\" (cursor at " 
             << cursorPosition << ")" << endl;
    }
    
    // Create memento
    shared_ptr<EditorMemento> save() {
        cout << "Saving state..." << endl;
        return make_shared<EditorMemento>(content, cursorPosition);
    }
    
    // Restore from memento
    void restore(shared_ptr<EditorMemento> memento) {
        content = memento->content;
        cursorPosition = memento->cursorPosition;
        cout << "Restored state from " << memento->getTimestamp();
    }
};

// Caretaker
class History {
private:
    vector<shared_ptr<EditorMemento>> mementos;
    
public:
    void addMemento(shared_ptr<EditorMemento> memento) {
        mementos.push_back(memento);
    }
    
    shared_ptr<EditorMemento> getMemento(int index) {
        if (index >= 0 && index < mementos.size()) {
            return mementos[index];
        }
        return nullptr;
    }
    
    int getHistorySize() const {
        return mementos.size();
    }
};

// ============= Example 2: Game Save System =============

class GameMemento {
private:
    int level;
    int health;
    int score;
    string playerName;
    
    friend class Game;
    
public:
    GameMemento(int lvl, int hp, int sc, const string& name)
        : level(lvl), health(hp), score(sc), playerName(name) {}
};

class Game {
private:
    string playerName;
    int level;
    int health;
    int score;
    
public:
    Game(const string& name) : playerName(name), level(1), health(100), score(0) {}
    
    void play() {
        level++;
        score += 100;
        health -= 20;
        cout << "Playing... Level " << level << " completed!" << endl;
    }
    
    void takeDamage(int damage) {
        health -= damage;
        cout << "Took " << damage << " damage!" << endl;
    }
    
    void displayStatus() const {
        cout << "\n=== Game Status ===" << endl;
        cout << "Player: " << playerName << endl;
        cout << "Level: " << level << endl;
        cout << "Health: " << health << endl;
        cout << "Score: " << score << endl;
    }
    
    shared_ptr<GameMemento> saveGame() {
        cout << "Saving game..." << endl;
        return make_shared<GameMemento>(level, health, score, playerName);
    }
    
    void loadGame(shared_ptr<GameMemento> save) {
        level = save->level;
        health = save->health;
        score = save->score;
        playerName = save->playerName;
        cout << "Game loaded!" << endl;
    }
};

// Caretaker
class SaveManager {
private:
    vector<shared_ptr<GameMemento>> saves;
    
public:
    void save(shared_ptr<GameMemento> memento) {
        saves.push_back(memento);
        cout << "Save slot " << saves.size() << " created" << endl;
    }
    
    shared_ptr<GameMemento> load(int slot) {
        if (slot > 0 && slot <= saves.size()) {
            cout << "Loading save slot " << slot << endl;
            return saves[slot - 1];
        }
        cout << "Invalid save slot!" << endl;
        return nullptr;
    }
    
    void listSaves() const {
        cout << "\nAvailable saves: " << saves.size() << endl;
        for (size_t i = 0; i < saves.size(); i++) {
            cout << "  Slot " << (i + 1) << endl;
        }
    }
};

// ============= Example 3: Configuration Manager =============

class ConfigMemento {
private:
    string theme;
    int fontSize;
    bool autoSave;
    string language;
    
    friend class Configuration;
    
public:
    ConfigMemento(const string& t, int fs, bool as, const string& lang)
        : theme(t), fontSize(fs), autoSave(as), language(lang) {}
};

class Configuration {
private:
    string theme;
    int fontSize;
    bool autoSave;
    string language;
    
public:
    Configuration() : theme("Light"), fontSize(12), autoSave(false), language("English") {}
    
    void setTheme(const string& t) { 
        theme = t; 
        cout << "Theme changed to: " << theme << endl;
    }
    
    void setFontSize(int size) { 
        fontSize = size; 
        cout << "Font size changed to: " << fontSize << endl;
    }
    
    void setAutoSave(bool enable) { 
        autoSave = enable; 
        cout << "Auto-save " << (enable ? "enabled" : "disabled") << endl;
    }
    
    void setLanguage(const string& lang) { 
        language = lang; 
        cout << "Language changed to: " << language << endl;
    }
    
    void display() const {
        cout << "\n=== Configuration ===" << endl;
        cout << "Theme: " << theme << endl;
        cout << "Font Size: " << fontSize << endl;
        cout << "Auto-save: " << (autoSave ? "On" : "Off") << endl;
        cout << "Language: " << language << endl;
    }
    
    shared_ptr<ConfigMemento> createSnapshot() {
        return make_shared<ConfigMemento>(theme, fontSize, autoSave, language);
    }
    
    void restoreSnapshot(shared_ptr<ConfigMemento> snapshot) {
        theme = snapshot->theme;
        fontSize = snapshot->fontSize;
        autoSave = snapshot->autoSave;
        language = snapshot->language;
        cout << "Configuration restored from snapshot" << endl;
    }
};

class ConfigHistory {
private:
    vector<shared_ptr<ConfigMemento>> snapshots;
    int currentIndex;
    
public:
    ConfigHistory() : currentIndex(-1) {}
    
    void saveSnapshot(shared_ptr<ConfigMemento> snapshot) {
        // Remove any snapshots after current index
        if (currentIndex < snapshots.size() - 1) {
            snapshots.erase(snapshots.begin() + currentIndex + 1, snapshots.end());
        }
        
        snapshots.push_back(snapshot);
        currentIndex++;
        cout << "Snapshot " << (currentIndex + 1) << " saved" << endl;
    }
    
    shared_ptr<ConfigMemento> undo() {
        if (currentIndex > 0) {
            currentIndex--;
            return snapshots[currentIndex];
        }
        cout << "Nothing to undo" << endl;
        return nullptr;
    }
    
    shared_ptr<ConfigMemento> redo() {
        if (currentIndex < snapshots.size() - 1) {
            currentIndex++;
            return snapshots[currentIndex];
        }
        cout << "Nothing to redo" << endl;
        return nullptr;
    }
};

int main() {
    cout << "=== MEMENTO PATTERN DEMO ===" << endl;
    
    // Example 1: Text Editor with Undo
    cout << "\n1. TEXT EDITOR WITH HISTORY:" << endl;
    cout << "=============================" << endl;
    
    TextEditor editor;
    History history;
    
    editor.display();
    
    cout << "\n[Edit 1]" << endl;
    editor.type("Hello");
    editor.display();
    history.addMemento(editor.save());
    
    cout << "\n[Edit 2]" << endl;
    editor.type(" World");
    editor.display();
    history.addMemento(editor.save());
    
    cout << "\n[Edit 3]" << endl;
    editor.type("!!!");
    editor.display();
    history.addMemento(editor.save());
    
    cout << "\n[Restore to previous state]" << endl;
    editor.restore(history.getMemento(1));
    editor.display();
    
    // Example 2: Game Save System
    cout << "\n\n2. GAME SAVE SYSTEM:" << endl;
    cout << "====================" << endl;
    
    Game game("Player1");
    SaveManager saveManager;
    
    game.displayStatus();
    
    cout << "\n[Playing...]" << endl;
    game.play();
    game.play();
    game.displayStatus();
    
    cout << "\n[Creating save point]" << endl;
    saveManager.save(game.saveGame());
    
    cout << "\n[Continue playing...]" << endl;
    game.play();
    game.takeDamage(50);
    game.displayStatus();
    
    cout << "\n[Creating another save point]" << endl;
    saveManager.save(game.saveGame());
    
    cout << "\n[Continue playing - things go bad!]" << endl;
    game.takeDamage(80);
    game.displayStatus();
    
    cout << "\n[Loading previous save]" << endl;
    saveManager.listSaves();
    game.loadGame(saveManager.load(1));
    game.displayStatus();
    
    // Example 3: Configuration with Undo/Redo
    cout << "\n\n3. CONFIGURATION MANAGER:" << endl;
    cout << "=========================" << endl;
    
    Configuration config;
    ConfigHistory configHistory;
    
    config.display();
    configHistory.saveSnapshot(config.createSnapshot());
    
    cout << "\n[Change 1]" << endl;
    config.setTheme("Dark");
    config.display();
    configHistory.saveSnapshot(config.createSnapshot());
    
    cout << "\n[Change 2]" << endl;
    config.setFontSize(14);
    config.setAutoSave(true);
    config.display();
    configHistory.saveSnapshot(config.createSnapshot());
    
    cout << "\n[Change 3]" << endl;
    config.setLanguage("Spanish");
    config.display();
    configHistory.saveSnapshot(config.createSnapshot());
    
    cout << "\n[Undo]" << endl;
    auto snapshot = configHistory.undo();
    if (snapshot) {
        config.restoreSnapshot(snapshot);
        config.display();
    }
    
    cout << "\n[Undo again]" << endl;
    snapshot = configHistory.undo();
    if (snapshot) {
        config.restoreSnapshot(snapshot);
        config.display();
    }
    
    cout << "\n[Redo]" << endl;
    snapshot = configHistory.redo();
    if (snapshot) {
        config.restoreSnapshot(snapshot);
        config.display();
    }
    
    cout << "\n\n=== KEY TAKEAWAYS ===" << endl;
    cout << "1. Memento CAPTURES and RESTORES object state" << endl;
    cout << "2. Preserves encapsulation (state details hidden)" << endl;
    cout << "3. Three roles: Originator, Memento, Caretaker" << endl;
    cout << "4. Originator creates and restores from mementos" << endl;
    cout << "5. Caretaker stores mementos but doesn't modify them" << endl;
    cout << "6. Essential for undo/redo, save/load, snapshots" << endl;
    cout << "7. Can be memory-intensive with large states" << endl;
    
    return 0;
}

