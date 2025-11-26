/*
 * SINGLE RESPONSIBILITY PRINCIPLE (SRP)
 * First of the SOLID Principles
 * 
 * DEFINITION:
 * "A class should have one, and only one, reason to change."
 * - Robert C. Martin (Uncle Bob)
 * 
 * ALTERNATIVE FORMULATION:
 * "A class should have only one responsibility."
 * "Gather together things that change for the same reasons.
 *  Separate things that change for different reasons."
 * 
 * PURPOSE:
 * Each class should focus on a single concern/responsibility.
 * This makes the code more maintainable, testable, and flexible.
 * 
 * WHEN TO APPLY:
 * - Class is doing too many unrelated things
 * - Multiple reasons to modify a class
 * - Class is hard to name (needs "And" or "Or")
 * - Class has many dependencies
 * 
 * BENEFITS:
 * + Easier to understand and maintain
 * + Easier to test (fewer dependencies)
 * + More reusable components
 * + Reduces coupling
 * + Changes affect fewer classes
 * 
 * VIOLATIONS:
 * - God classes (do everything)
 * - Mixed concerns (UI + business logic + data access)
 * - Classes with multiple reasons to change
 */

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

// ============= BAD EXAMPLE - Violates SRP =============

class UserBad {
private:
    string username;
    string email;
    string password;
    
public:
    UserBad(const string& user, const string& mail, const string& pass)
        : username(user), email(mail), password(pass) {}
    
    // Responsibility 1: User data management
    void setUsername(const string& user) { username = user; }
    string getUsername() const { return username; }
    
    // Responsibility 2: Validation (should be separate)
    bool validateEmail() {
        return email.find('@') != string::npos;
    }
    
    // Responsibility 3: Authentication (should be separate)
    bool login(const string& pass) {
        return password == pass;
    }
    
    // Responsibility 4: Database operations (should be separate)
    void saveToDatabase() {
        cout << "Saving user " << username << " to database..." << endl;
        // Database code here
    }
    
    // Responsibility 5: Email operations (should be separate)
    void sendWelcomeEmail() {
        cout << "Sending welcome email to " << email << endl;
        // Email code here
    }
    
    // TOO MANY RESPONSIBILITIES!
    // Changes to email system, database, authentication, or validation
    // would all require changing this class.
};

// ============= GOOD EXAMPLE - Follows SRP =============

// Responsibility 1: User data only
class User {
private:
    string username;
    string email;
    string password;
    
public:
    User(const string& user, const string& mail, const string& pass)
        : username(user), email(mail), password(pass) {}
    
    string getUsername() const { return username; }
    string getEmail() const { return email; }
    string getPassword() const { return password; }
    
    void setUsername(const string& user) { username = user; }
    void setEmail(const string& mail) { email = mail; }
    void setPassword(const string& pass) { password = pass; }
};

// Responsibility 2: Email validation
class EmailValidator {
public:
    bool isValid(const string& email) {
        return email.find('@') != string::npos && 
               email.find('.') != string::npos;
    }
};

// Responsibility 3: User authentication
class UserAuthenticator {
public:
    bool authenticate(const User& user, const string& password) {
        return user.getPassword() == password;
    }
};

// Responsibility 4: Database operations
class UserRepository {
public:
    void save(const User& user) {
        cout << "Saving user '" << user.getUsername() << "' to database..." << endl;
        // Database code here
    }
    
    User load(const string& username) {
        cout << "Loading user '" << username << "' from database..." << endl;
        // Database code here
        return User(username, "user@example.com", "password");
    }
};

// Responsibility 5: Email notifications
class EmailService {
public:
    void sendWelcomeEmail(const User& user) {
        cout << "Sending welcome email to " << user.getEmail() << endl;
        // Email code here
    }
    
    void sendPasswordResetEmail(const User& user) {
        cout << "Sending password reset email to " << user.getEmail() << endl;
    }
};

// ============= ANOTHER EXAMPLE: Report Generation =============

// BAD: Multiple responsibilities
class ReportBad {
public:
    void generateReport() {
        // Fetch data
        cout << "Fetching data..." << endl;
        
        // Calculate statistics
        cout << "Calculating statistics..." << endl;
        
        // Format as HTML
        cout << "Formatting as HTML..." << endl;
        
        // Save to file
        cout << "Saving to file..." << endl;
        
        // Send via email
        cout << "Emailing report..." << endl;
    }
    // Too many reasons to change!
};

// GOOD: Separated responsibilities
class ReportData {
private:
    vector<string> data;
    
public:
    void fetchData() {
        cout << "Fetching data..." << endl;
        data = {"Item1", "Item2", "Item3"};
    }
    
    const vector<string>& getData() const { return data; }
};

class ReportStatistics {
public:
    string calculateStatistics(const vector<string>& data) {
        cout << "Calculating statistics..." << endl;
        return "Total items: " + to_string(data.size());
    }
};

class ReportFormatter {
public:
    string formatAsHTML(const string& content) {
        cout << "Formatting as HTML..." << endl;
        return "<html><body>" + content + "</body></html>";
    }
    
    string formatAsJSON(const string& content) {
        cout << "Formatting as JSON..." << endl;
        return "{\"content\": \"" + content + "\"}";
    }
};

class ReportStorage {
public:
    void saveToFile(const string& content, const string& filename) {
        cout << "Saving report to " << filename << endl;
        ofstream file(filename);
        file << content;
        file.close();
    }
};

class ReportEmailer {
public:
    void sendEmail(const string& recipient, const string& content) {
        cout << "Emailing report to " << recipient << endl;
    }
};

// Orchestrator (follows SRP too - only coordinates)
class ReportGenerator {
public:
    void generateAndSendReport() {
        ReportData reportData;
        ReportStatistics stats;
        ReportFormatter formatter;
        ReportStorage storage;
        ReportEmailer emailer;
        
        // Coordinate the process
        reportData.fetchData();
        string statistics = stats.calculateStatistics(reportData.getData());
        string formatted = formatter.formatAsHTML(statistics);
        storage.saveToFile(formatted, "report.html");
        emailer.sendEmail("boss@company.com", formatted);
    }
};

// ============= EXAMPLE: Book Management =============

// BAD: Book class doing everything
class BookBad {
private:
    string title;
    string author;
    double price;
    
public:
    BookBad(const string& t, const string& a, double p)
        : title(t), author(a), price(p) {}
    
    // Data access - OK
    string getTitle() const { return title; }
    
    // Pricing logic - Should be separate
    double calculateDiscount() {
        return price * 0.1;
    }
    
    // Printing logic - Should be separate
    void printBook() {
        cout << "Title: " << title << ", Author: " << author << endl;
    }
    
    // Database logic - Should be separate
    void saveToDatabase() {
        cout << "Saving book to database..." << endl;
    }
};

// GOOD: Separated concerns
class Book {
private:
    string title;
    string author;
    double price;
    
public:
    Book(const string& t, const string& a, double p)
        : title(t), author(a), price(p) {}
    
    string getTitle() const { return title; }
    string getAuthor() const { return author; }
    double getPrice() const { return price; }
};

class BookPricingService {
public:
    double calculateDiscount(const Book& book, double discountRate) {
        return book.getPrice() * discountRate;
    }
    
    double calculateFinalPrice(const Book& book, double discountRate) {
        return book.getPrice() - calculateDiscount(book, discountRate);
    }
};

class BookPrinter {
public:
    void printToConsole(const Book& book) {
        cout << "Title: " << book.getTitle() 
             << ", Author: " << book.getAuthor() 
             << ", Price: $" << book.getPrice() << endl;
    }
    
    void printToHTML(const Book& book) {
        cout << "<div><h2>" << book.getTitle() << "</h2>"
             << "<p>By " << book.getAuthor() << "</p>"
             << "<p>$" << book.getPrice() << "</p></div>" << endl;
    }
};

class BookRepository {
public:
    void save(const Book& book) {
        cout << "Saving '" << book.getTitle() << "' to database..." << endl;
    }
    
    Book load(const string& title) {
        cout << "Loading '" << title << "' from database..." << endl;
        return Book(title, "Unknown", 0.0);
    }
};

int main() {
    cout << "=== SINGLE RESPONSIBILITY PRINCIPLE (SRP) ===" << endl;
    
    // Example 1: User Management
    cout << "\n1. USER MANAGEMENT SYSTEM:" << endl;
    cout << "===========================" << endl;
    
    cout << "\n[BAD - Violates SRP]" << endl;
    UserBad badUser("john_doe", "john@example.com", "password123");
    cout << "User has too many responsibilities:" << endl;
    cout << "- Data management" << endl;
    cout << "- Validation" << endl;
    cout << "- Authentication" << endl;
    cout << "- Database operations" << endl;
    cout << "- Email operations" << endl;
    badUser.saveToDatabase();
    badUser.sendWelcomeEmail();
    
    cout << "\n[GOOD - Follows SRP]" << endl;
    User user("john_doe", "john@example.com", "password123");
    
    EmailValidator validator;
    if (validator.isValid(user.getEmail())) {
        cout << "Email is valid" << endl;
    }
    
    UserAuthenticator authenticator;
    if (authenticator.authenticate(user, "password123")) {
        cout << "User authenticated" << endl;
    }
    
    UserRepository repository;
    repository.save(user);
    
    EmailService emailService;
    emailService.sendWelcomeEmail(user);
    
    cout << "\nBENEFIT: Each class has ONE reason to change" << endl;
    
    // Example 2: Report Generation
    cout << "\n\n2. REPORT GENERATION:" << endl;
    cout << "=====================" << endl;
    
    cout << "\n[BAD - Single class does everything]" << endl;
    ReportBad badReport;
    badReport.generateReport();
    
    cout << "\n[GOOD - Separated responsibilities]" << endl;
    ReportGenerator reportGenerator;
    reportGenerator.generateAndSendReport();
    
    // Example 3: Book Management
    cout << "\n\n3. BOOK MANAGEMENT:" << endl;
    cout << "===================" << endl;
    
    cout << "\n[Creating book with separated concerns]" << endl;
    Book book("Design Patterns", "Gang of Four", 59.99);
    
    BookPrinter printer;
    printer.printToConsole(book);
    
    BookPricingService pricingService;
    double discount = pricingService.calculateDiscount(book, 0.15);
    cout << "Discount (15%): $" << discount << endl;
    cout << "Final price: $" << pricingService.calculateFinalPrice(book, 0.15) << endl;
    
    BookRepository bookRepo;
    bookRepo.save(book);
    
    cout << "\n\n=== KEY TAKEAWAYS ===" << endl;
    cout << "1. Each class should have ONE responsibility" << endl;
    cout << "2. A class should have ONE reason to change" << endl;
    cout << "3. Separate concerns into different classes" << endl;
    cout << "4. Makes code more maintainable and testable" << endl;
    cout << "5. Reduces coupling between components" << endl;
    cout << "\nRed Flags (SRP Violations):" << endl;
    cout << "• Class name needs 'And' or 'Or' (UserAndEmailer)" << endl;
    cout << "• Class has many public methods doing different things" << endl;
    cout << "• Changes in one area require changing the class" << endl;
    cout << "• Class has too many dependencies" << endl;
    cout << "• Hard to name the class precisely" << endl;
    
    return 0;
}

