/*
 * CHAIN OF RESPONSIBILITY PATTERN - Behavioral Pattern
 * 
 * PURPOSE:
 * Passes requests along a chain of handlers. Each handler decides either
 * to process the request or to pass it to the next handler in the chain.
 * 
 * WHEN TO USE:
 * - More than one object can handle a request, and handler isn't known a priori
 * - Want to issue request to one of several objects without specifying receiver
 * - Set of objects that can handle request should be specified dynamically
 * 
 * PROS:
 * + Decouples sender and receiver
 * + Adds/removes responsibilities dynamically
 * + Flexible assignment of responsibilities
 * + Single Responsibility Principle
 * + Open/Closed Principle
 * 
 * CONS:
 * - Request might not be handled (end of chain)
 * - Can be hard to debug/observe
 * - Performance concerns with long chains
 * 
 * REAL-WORLD EXAMPLES:
 * - Event handling in GUI frameworks
 * - Logging frameworks (log levels)
 * - Support ticket systems
 * - Authentication/authorization chains
 */

#include <iostream>
#include <string>
#include <memory>

using namespace std;

// ============= Example 1: Support Ticket System =============

enum class Priority { LOW, MEDIUM, HIGH, CRITICAL };

class SupportTicket {
public:
    Priority priority;
    string description;
    
    SupportTicket(Priority p, const string& desc) 
        : priority(p), description(desc) {}
};

// Handler interface
class SupportHandler {
protected:
    shared_ptr<SupportHandler> nextHandler;
    
public:
    virtual ~SupportHandler() = default;
    
    void setNext(shared_ptr<SupportHandler> handler) {
        nextHandler = handler;
    }
    
    virtual void handleTicket(const SupportTicket& ticket) {
        if (nextHandler) {
            nextHandler->handleTicket(ticket);
        } else {
            cout << "No handler available for this ticket!" << endl;
        }
    }
};

// Concrete Handlers
class Level1Support : public SupportHandler {
public:
    void handleTicket(const SupportTicket& ticket) override {
        if (ticket.priority == Priority::LOW) {
            cout << "[Level 1 Support] Handling: " << ticket.description << endl;
        } else {
            cout << "[Level 1 Support] Escalating ticket..." << endl;
            SupportHandler::handleTicket(ticket);
        }
    }
};

class Level2Support : public SupportHandler {
public:
    void handleTicket(const SupportTicket& ticket) override {
        if (ticket.priority == Priority::MEDIUM) {
            cout << "[Level 2 Support] Handling: " << ticket.description << endl;
        } else {
            cout << "[Level 2 Support] Escalating ticket..." << endl;
            SupportHandler::handleTicket(ticket);
        }
    }
};

class Level3Support : public SupportHandler {
public:
    void handleTicket(const SupportTicket& ticket) override {
        if (ticket.priority == Priority::HIGH) {
            cout << "[Level 3 Support] Handling: " << ticket.description << endl;
        } else {
            cout << "[Level 3 Support] Escalating to management..." << endl;
            SupportHandler::handleTicket(ticket);
        }
    }
};

class ManagementSupport : public SupportHandler {
public:
    void handleTicket(const SupportTicket& ticket) override {
        cout << "[Management] Handling CRITICAL issue: " << ticket.description << endl;
    }
};

// ============= Example 2: Logging System =============

enum class LogLevel { DEBUG, INFO, WARNING, ERROR };

class LogMessage {
public:
    LogLevel level;
    string message;
    
    LogMessage(LogLevel l, const string& msg) : level(l), message(msg) {}
};

// Handler
class Logger {
protected:
    LogLevel level;
    shared_ptr<Logger> next;
    
public:
    Logger(LogLevel l) : level(l), next(nullptr) {}
    
    void setNext(shared_ptr<Logger> logger) {
        next = logger;
    }
    
    void log(const LogMessage& msg) {
        if (msg.level >= level) {
            writeMessage(msg);
        }
        if (next) {
            next->log(msg);
        }
    }
    
    virtual void writeMessage(const LogMessage& msg) = 0;
    virtual ~Logger() = default;
};

// Concrete Handlers
class ConsoleLogger : public Logger {
public:
    ConsoleLogger(LogLevel level) : Logger(level) {}
    
    void writeMessage(const LogMessage& msg) override {
        cout << "[CONSOLE] " << getMessage(msg) << endl;
    }
    
private:
    string getMessage(const LogMessage& msg) {
        string levelStr;
        switch(msg.level) {
            case LogLevel::DEBUG: levelStr = "DEBUG"; break;
            case LogLevel::INFO: levelStr = "INFO"; break;
            case LogLevel::WARNING: levelStr = "WARNING"; break;
            case LogLevel::ERROR: levelStr = "ERROR"; break;
        }
        return levelStr + ": " + msg.message;
    }
};

class FileLogger : public Logger {
public:
    FileLogger(LogLevel level) : Logger(level) {}
    
    void writeMessage(const LogMessage& msg) override {
        cout << "[FILE] Writing to log.txt: " << msg.message << endl;
    }
};

class EmailLogger : public Logger {
public:
    EmailLogger(LogLevel level) : Logger(level) {}
    
    void writeMessage(const LogMessage& msg) override {
        cout << "[EMAIL] Sending alert: " << msg.message << endl;
    }
};

// ============= Example 3: Approval Chain =============

class PurchaseRequest {
public:
    double amount;
    string purpose;
    
    PurchaseRequest(double amt, const string& purp) 
        : amount(amt), purpose(purp) {}
};

// Handler
class Approver {
protected:
    shared_ptr<Approver> successor;
    double approvalLimit;
    string title;
    
public:
    Approver(const string& t, double limit) 
        : title(t), approvalLimit(limit), successor(nullptr) {}
    
    void setSuccessor(shared_ptr<Approver> next) {
        successor = next;
    }
    
    virtual void processRequest(const PurchaseRequest& request) {
        if (request.amount <= approvalLimit) {
            cout << title << " approves $" << request.amount 
                 << " for: " << request.purpose << endl;
        } else if (successor) {
            cout << title << " cannot approve. Forwarding..." << endl;
            successor->processRequest(request);
        } else {
            cout << "Request for $" << request.amount 
                 << " exceeds all approval limits!" << endl;
        }
    }
    
    virtual ~Approver() = default;
};

// Concrete Handlers
class TeamLead : public Approver {
public:
    TeamLead() : Approver("Team Lead", 1000.0) {}
};

class Manager : public Approver {
public:
    Manager() : Approver("Manager", 5000.0) {}
};

class Director : public Approver {
public:
    Director() : Approver("Director", 20000.0) {}
};

class CEO : public Approver {
public:
    CEO() : Approver("CEO", 100000.0) {}
};

int main() {
    cout << "=== CHAIN OF RESPONSIBILITY PATTERN DEMO ===" << endl;
    
    // Example 1: Support Ticket System
    cout << "\n1. SUPPORT TICKET SYSTEM:" << endl;
    cout << "=========================" << endl;
    
    // Build the chain
    auto level1 = make_shared<Level1Support>();
    auto level2 = make_shared<Level2Support>();
    auto level3 = make_shared<Level3Support>();
    auto management = make_shared<ManagementSupport>();
    
    level1->setNext(level2);
    level2->setNext(level3);
    level3->setNext(management);
    
    // Process tickets
    cout << "\n[Ticket 1]" << endl;
    SupportTicket ticket1(Priority::LOW, "Password reset request");
    level1->handleTicket(ticket1);
    
    cout << "\n[Ticket 2]" << endl;
    SupportTicket ticket2(Priority::MEDIUM, "Software installation issue");
    level1->handleTicket(ticket2);
    
    cout << "\n[Ticket 3]" << endl;
    SupportTicket ticket3(Priority::HIGH, "Database connection failure");
    level1->handleTicket(ticket3);
    
    cout << "\n[Ticket 4]" << endl;
    SupportTicket ticket4(Priority::CRITICAL, "Security breach detected");
    level1->handleTicket(ticket4);
    
    // Example 2: Logging System
    cout << "\n\n2. LOGGING SYSTEM:" << endl;
    cout << "==================" << endl;
    
    // Build chain: Console logs all, File logs warnings+errors, Email only errors
    auto consoleLogger = make_shared<ConsoleLogger>(LogLevel::DEBUG);
    auto fileLogger = make_shared<FileLogger>(LogLevel::WARNING);
    auto emailLogger = make_shared<EmailLogger>(LogLevel::ERROR);
    
    consoleLogger->setNext(fileLogger);
    fileLogger->setNext(emailLogger);
    
    // Log messages
    cout << "\n[Debug Message]" << endl;
    consoleLogger->log(LogMessage(LogLevel::DEBUG, "Variable x = 42"));
    
    cout << "\n[Info Message]" << endl;
    consoleLogger->log(LogMessage(LogLevel::INFO, "User logged in"));
    
    cout << "\n[Warning Message]" << endl;
    consoleLogger->log(LogMessage(LogLevel::WARNING, "Disk space low"));
    
    cout << "\n[Error Message]" << endl;
    consoleLogger->log(LogMessage(LogLevel::ERROR, "Database connection failed"));
    
    // Example 3: Purchase Approval Chain
    cout << "\n\n3. PURCHASE APPROVAL CHAIN:" << endl;
    cout << "============================" << endl;
    
    // Build chain
    auto teamLead = make_shared<TeamLead>();
    auto manager = make_shared<Manager>();
    auto director = make_shared<Director>();
    auto ceo = make_shared<CEO>();
    
    teamLead->setSuccessor(manager);
    manager->setSuccessor(director);
    director->setSuccessor(ceo);
    
    // Process requests
    cout << "\n[Request 1]" << endl;
    PurchaseRequest req1(500, "Office supplies");
    teamLead->processRequest(req1);
    
    cout << "\n[Request 2]" << endl;
    PurchaseRequest req2(3000, "New laptops");
    teamLead->processRequest(req2);
    
    cout << "\n[Request 3]" << endl;
    PurchaseRequest req3(15000, "Server upgrade");
    teamLead->processRequest(req3);
    
    cout << "\n[Request 4]" << endl;
    PurchaseRequest req4(50000, "Company acquisition");
    teamLead->processRequest(req4);
    
    cout << "\n[Request 5]" << endl;
    PurchaseRequest req5(150000, "Building expansion");
    teamLead->processRequest(req5);
    
    cout << "\n\n=== KEY TAKEAWAYS ===" << endl;
    cout << "1. Chain of Responsibility PASSES requests along a chain" << endl;
    cout << "2. Each handler decides to process or pass to next handler" << endl;
    cout << "3. Decouples sender from receiver" << endl;
    cout << "4. Can add/remove handlers dynamically" << endl;
    cout << "5. Request may not be handled if no suitable handler exists" << endl;
    cout << "6. Common in event handling, logging, and approval workflows" << endl;
    
    return 0;
}

