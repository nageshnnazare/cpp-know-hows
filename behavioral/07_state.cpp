/*
 * STATE PATTERN - Behavioral Pattern
 * 
 * PURPOSE:
 * Allows an object to alter its behavior when its internal state changes.
 * The object will appear to change its class.
 * 
 * WHEN TO USE:
 * - Object behavior depends on its state and must change at runtime
 * - Operations have large conditional statements depending on state
 * - Want to avoid duplicate code across similar states
 * 
 * PROS:
 * + Organizes state-specific code
 * + Makes state transitions explicit
 * + Eliminates large conditionals
 * + Single Responsibility: each state in separate class
 * + Open/Closed: add new states without changing existing
 * 
 * CONS:
 * - Overkill if few states or rarely change
 * - Increases number of classes
 * 
 * REAL-WORLD EXAMPLES:
 * - TCP connection states
 * - Vending machines
 * - Document workflow (draft, review, published)
 * - Media players (playing, paused, stopped)
 */

#include <iostream>
#include <string>
#include <memory>

using namespace std;

// ============= Example 1: Vending Machine =============

class VendingMachine;

// State interface
class VendingMachineState {
public:
    virtual ~VendingMachineState() = default;
    virtual void insertCoin(VendingMachine* machine) = 0;
    virtual void selectProduct(VendingMachine* machine) = 0;
    virtual void dispense(VendingMachine* machine) = 0;
    virtual void refund(VendingMachine* machine) = 0;
    virtual string getStateName() const = 0;
};

// Context
class VendingMachine {
private:
    shared_ptr<VendingMachineState> currentState;
    int balance;
    
public:
    VendingMachine();
    
    void setState(shared_ptr<VendingMachineState> state) {
        currentState = state;
        cout << "[Machine State Changed to: " << state->getStateName() << "]" << endl;
    }
    
    void insertCoin() {
        currentState->insertCoin(this);
    }
    
    void selectProduct() {
        currentState->selectProduct(this);
    }
    
    void dispense() {
        currentState->dispense(this);
    }
    
    void refund() {
        currentState->refund(this);
    }
    
    void addBalance(int amount) {
        balance += amount;
        cout << "Balance: $" << balance << endl;
    }
    
    bool hasEnoughBalance() const {
        return balance >= 50;  // Product costs $0.50
    }
    
    void resetBalance() {
        balance = 0;
    }
    
    int getBalance() const { return balance; }
};

// Concrete States
class IdleState : public VendingMachineState {
public:
    void insertCoin(VendingMachine* machine) override;
    void selectProduct(VendingMachine* machine) override;
    void dispense(VendingMachine* machine) override;
    void refund(VendingMachine* machine) override;
    string getStateName() const override { return "Idle"; }
};

class HasCoinState : public VendingMachineState {
public:
    void insertCoin(VendingMachine* machine) override;
    void selectProduct(VendingMachine* machine) override;
    void dispense(VendingMachine* machine) override;
    void refund(VendingMachine* machine) override;
    string getStateName() const override { return "Has Coin"; }
};

class DispensingState : public VendingMachineState {
public:
    void insertCoin(VendingMachine* machine) override;
    void selectProduct(VendingMachine* machine) override;
    void dispense(VendingMachine* machine) override;
    void refund(VendingMachine* machine) override;
    string getStateName() const override { return "Dispensing"; }
};

// State implementations
void IdleState::insertCoin(VendingMachine* machine) {
    cout << "Coin inserted" << endl;
    machine->addBalance(25);  // Quarter = $0.25
    machine->setState(make_shared<HasCoinState>());
}

void IdleState::selectProduct(VendingMachine* machine) {
    cout << "Please insert coin first" << endl;
}

void IdleState::dispense(VendingMachine* machine) {
    cout << "Please insert coin first" << endl;
}

void IdleState::refund(VendingMachine* machine) {
    cout << "No coins to refund" << endl;
}

void HasCoinState::insertCoin(VendingMachine* machine) {
    cout << "Coin inserted" << endl;
    machine->addBalance(25);
}

void HasCoinState::selectProduct(VendingMachine* machine) {
    if (machine->hasEnoughBalance()) {
        cout << "Product selected" << endl;
        machine->setState(make_shared<DispensingState>());
        machine->dispense();
    } else {
        cout << "Insufficient balance. Insert more coins" << endl;
    }
}

void HasCoinState::dispense(VendingMachine* machine) {
    cout << "Select product first" << endl;
}

void HasCoinState::refund(VendingMachine* machine) {
    cout << "Refunding $" << (machine->getBalance() / 100.0) << endl;
    machine->resetBalance();
    machine->setState(make_shared<IdleState>());
}

void DispensingState::insertCoin(VendingMachine* machine) {
    cout << "Please wait, dispensing product" << endl;
}

void DispensingState::selectProduct(VendingMachine* machine) {
    cout << "Already dispensing" << endl;
}

void DispensingState::dispense(VendingMachine* machine) {
    cout << "Dispensing product..." << endl;
    cout << "Product dispensed! Enjoy!" << endl;
    machine->resetBalance();
    machine->setState(make_shared<IdleState>());
}

void DispensingState::refund(VendingMachine* machine) {
    cout << "Cannot refund, product being dispensed" << endl;
}

VendingMachine::VendingMachine() : balance(0) {
    currentState = make_shared<IdleState>();
    cout << "Vending Machine initialized" << endl;
}

// ============= Example 2: Document Workflow =============

class Document;

class DocumentState {
public:
    virtual ~DocumentState() = default;
    virtual void edit(Document* doc) = 0;
    virtual void submit(Document* doc) = 0;
    virtual void approve(Document* doc) = 0;
    virtual void publish(Document* doc) = 0;
    virtual string getStateName() const = 0;
};

class Document {
private:
    string content;
    shared_ptr<DocumentState> state;
    
public:
    Document();
    
    void setState(shared_ptr<DocumentState> newState) {
        state = newState;
        cout << "[Document State: " << state->getStateName() << "]" << endl;
    }
    
    void setContent(const string& text) {
        content = text;
    }
    
    string getContent() const { return content; }
    
    void edit() { state->edit(this); }
    void submit() { state->submit(this); }
    void approve() { state->approve(this); }
    void publish() { state->publish(this); }
};

class DraftState : public DocumentState {
public:
    void edit(Document* doc) override {
        cout << "Editing draft..." << endl;
        doc->setContent("Updated content");
    }
    
    void submit(Document* doc) override;
    void approve(Document* doc) override;
    void publish(Document* doc) override;
    string getStateName() const override { return "Draft"; }
};

class ModerationState : public DocumentState {
public:
    void edit(Document* doc) override;
    void submit(Document* doc) override;
    void approve(Document* doc) override;
    void publish(Document* doc) override;
    string getStateName() const override { return "Moderation"; }
};

class PublishedState : public DocumentState {
public:
    void edit(Document* doc) override;
    void submit(Document* doc) override;
    void approve(Document* doc) override;
    void publish(Document* doc) override;
    string getStateName() const override { return "Published"; }
};

void DraftState::submit(Document* doc) {
    cout << "Submitting document for review..." << endl;
    doc->setState(make_shared<ModerationState>());
}

void DraftState::approve(Document* doc) {
    cout << "Cannot approve draft directly" << endl;
}

void DraftState::publish(Document* doc) {
    cout << "Cannot publish draft directly" << endl;
}

void ModerationState::edit(Document* doc) {
    cout << "Sending back to draft for editing..." << endl;
    doc->setState(make_shared<DraftState>());
}

void ModerationState::submit(Document* doc) {
    cout << "Already in moderation" << endl;
}

void ModerationState::approve(Document* doc) {
    cout << "Document approved!" << endl;
    doc->setState(make_shared<PublishedState>());
}

void ModerationState::publish(Document* doc) {
    cout << "Must be approved first" << endl;
}

void PublishedState::edit(Document* doc) {
    cout << "Cannot edit published document" << endl;
}

void PublishedState::submit(Document* doc) {
    cout << "Document already published" << endl;
}

void PublishedState::approve(Document* doc) {
    cout << "Document already approved" << endl;
}

void PublishedState::publish(Document* doc) {
    cout << "Document is already published" << endl;
}

Document::Document() {
    state = make_shared<DraftState>();
    content = "Initial content";
    cout << "New document created" << endl;
}

// ============= Example 3: Media Player =============

class MediaPlayer;

class PlayerState {
public:
    virtual ~PlayerState() = default;
    virtual void play(MediaPlayer* player) = 0;
    virtual void pause(MediaPlayer* player) = 0;
    virtual void stop(MediaPlayer* player) = 0;
    virtual string getStateName() const = 0;
};

class MediaPlayer {
private:
    shared_ptr<PlayerState> state;
    string currentTrack;
    
public:
    MediaPlayer();
    
    void setState(shared_ptr<PlayerState> newState) {
        state = newState;
        cout << "[Player State: " << state->getStateName() << "]" << endl;
    }
    
    void setTrack(const string& track) { currentTrack = track; }
    string getTrack() const { return currentTrack; }
    
    void play() { state->play(this); }
    void pause() { state->pause(this); }
    void stop() { state->stop(this); }
};

class StoppedState : public PlayerState {
public:
    void play(MediaPlayer* player) override;
    void pause(MediaPlayer* player) override;
    void stop(MediaPlayer* player) override;
    string getStateName() const override { return "Stopped"; }
};

class PlayingState : public PlayerState {
public:
    void play(MediaPlayer* player) override;
    void pause(MediaPlayer* player) override;
    void stop(MediaPlayer* player) override;
    string getStateName() const override { return "Playing"; }
};

class PausedState : public PlayerState {
public:
    void play(MediaPlayer* player) override;
    void pause(MediaPlayer* player) override;
    void stop(MediaPlayer* player) override;
    string getStateName() const override { return "Paused"; }
};

void StoppedState::play(MediaPlayer* player) {
    cout << "▶ Playing: " << player->getTrack() << endl;
    player->setState(make_shared<PlayingState>());
}

void StoppedState::pause(MediaPlayer* player) {
    cout << "Cannot pause - player is stopped" << endl;
}

void StoppedState::stop(MediaPlayer* player) {
    cout << "Already stopped" << endl;
}

void PlayingState::play(MediaPlayer* player) {
    cout << "Already playing" << endl;
}

void PlayingState::pause(MediaPlayer* player) {
    cout << "⏸ Paused" << endl;
    player->setState(make_shared<PausedState>());
}

void PlayingState::stop(MediaPlayer* player) {
    cout << "⏹ Stopped" << endl;
    player->setState(make_shared<StoppedState>());
}

void PausedState::play(MediaPlayer* player) {
    cout << "▶ Resuming playback" << endl;
    player->setState(make_shared<PlayingState>());
}

void PausedState::pause(MediaPlayer* player) {
    cout << "Already paused" << endl;
}

void PausedState::stop(MediaPlayer* player) {
    cout << "⏹ Stopped" << endl;
    player->setState(make_shared<StoppedState>());
}

MediaPlayer::MediaPlayer() {
    state = make_shared<StoppedState>();
    currentTrack = "Song.mp3";
    cout << "Media Player initialized" << endl;
}

int main() {
    cout << "=== STATE PATTERN DEMO ===" << endl;
    
    // Example 1: Vending Machine
    cout << "\n1. VENDING MACHINE:" << endl;
    cout << "===================" << endl;
    
    VendingMachine machine;
    
    cout << "\n[Trying to select without coins]" << endl;
    machine.selectProduct();
    
    cout << "\n[Inserting coins]" << endl;
    machine.insertCoin();
    machine.insertCoin();
    
    cout << "\n[Selecting product]" << endl;
    machine.selectProduct();
    
    // Example 2: Document Workflow
    cout << "\n\n2. DOCUMENT WORKFLOW:" << endl;
    cout << "=====================" << endl;
    
    Document doc;
    
    cout << "\n[Editing draft]" << endl;
    doc.edit();
    
    cout << "\n[Submitting for review]" << endl;
    doc.submit();
    
    cout << "\n[Trying to edit]" << endl;
    doc.edit();  // Sends back to draft
    
    cout << "\n[Resubmitting]" << endl;
    doc.submit();
    
    cout << "\n[Approving]" << endl;
    doc.approve();
    
    cout << "\n[Trying to edit published]" << endl;
    doc.edit();
    
    // Example 3: Media Player
    cout << "\n\n3. MEDIA PLAYER:" << endl;
    cout << "================" << endl;
    
    MediaPlayer player;
    
    cout << "\n[Playing]" << endl;
    player.play();
    
    cout << "\n[Pausing]" << endl;
    player.pause();
    
    cout << "\n[Resuming]" << endl;
    player.play();
    
    cout << "\n[Stopping]" << endl;
    player.stop();
    
    cout << "\n[Trying to pause when stopped]" << endl;
    player.pause();
    
    cout << "\n\n=== KEY TAKEAWAYS ===" << endl;
    cout << "1. State pattern changes behavior based on internal state" << endl;
    cout << "2. Each state is a separate class" << endl;
    cout << "3. Eliminates large conditional statements" << endl;
    cout << "4. Makes state transitions explicit" << endl;
    cout << "5. Easy to add new states without modifying existing ones" << endl;
    cout << "6. Context delegates behavior to current state object" << endl;
    
    return 0;
}

