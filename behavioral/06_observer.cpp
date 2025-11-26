/*
 * OBSERVER PATTERN - Behavioral Pattern
 * 
 * PURPOSE:
 * Defines a one-to-many dependency between objects so that when one object
 * changes state, all its dependents are notified and updated automatically.
 * 
 * WHEN TO USE:
 * - Change to one object requires changing others (unknown how many)
 * - Object should notify others without making assumptions about who they are
 * - Want loose coupling between subject and observers
 * 
 * PROS:
 * + Open/Closed Principle: can add new subscribers without modifying publisher
 * + Establishes relationships at runtime
 * + Loose coupling between subject and observers
 * + Supports broadcast communication
 * 
 * CONS:
 * - Observers notified in random order
 * - Can cause memory leaks if not unsubscribed properly
 * - Can be hard to debug
 * 
 * REAL-WORLD EXAMPLES:
 * - Event handling systems (GUI, event listeners)
 * - Model-View-Controller (MVC) pattern
 * - Stock market price updates
 * - Social media notifications
 */

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <memory>

using namespace std;

// ============= Example 1: Weather Station =============

// Observer interface
class Observer {
public:
    virtual ~Observer() = default;
    virtual void update(float temperature, float humidity, float pressure) = 0;
};

// Subject interface
class Subject {
public:
    virtual ~Subject() = default;
    virtual void attach(shared_ptr<Observer> observer) = 0;
    virtual void detach(shared_ptr<Observer> observer) = 0;
    virtual void notify() = 0;
};

// Concrete Subject
class WeatherStation : public Subject {
private:
    vector<shared_ptr<Observer>> observers;
    float temperature;
    float humidity;
    float pressure;
    
public:
    void attach(shared_ptr<Observer> observer) override {
        observers.push_back(observer);
        cout << "Observer attached to weather station" << endl;
    }
    
    void detach(shared_ptr<Observer> observer) override {
        auto it = find(observers.begin(), observers.end(), observer);
        if (it != observers.end()) {
            observers.erase(it);
            cout << "Observer detached from weather station" << endl;
        }
    }
    
    void notify() override {
        cout << "\n[Weather Station] Notifying all observers..." << endl;
        for (auto& observer : observers) {
            observer->update(temperature, humidity, pressure);
        }
    }
    
    void setMeasurements(float temp, float hum, float press) {
        cout << "\n[Weather Station] New measurements received" << endl;
        temperature = temp;
        humidity = hum;
        pressure = press;
        notify();
    }
};

// Concrete Observers
class CurrentConditionsDisplay : public Observer {
public:
    void update(float temperature, float humidity, float pressure) override {
        cout << "[Current Conditions Display]" << endl;
        cout << "  Temperature: " << temperature << "°C" << endl;
        cout << "  Humidity: " << humidity << "%" << endl;
    }
};

class StatisticsDisplay : public Observer {
private:
    float maxTemp = 0;
    float minTemp = 100;
    float tempSum = 0;
    int numReadings = 0;
    
public:
    void update(float temperature, float humidity, float pressure) override {
        tempSum += temperature;
        numReadings++;
        
        if (temperature > maxTemp) maxTemp = temperature;
        if (temperature < minTemp) minTemp = temperature;
        
        cout << "[Statistics Display]" << endl;
        cout << "  Avg Temperature: " << (tempSum / numReadings) << "°C" << endl;
        cout << "  Max Temperature: " << maxTemp << "°C" << endl;
        cout << "  Min Temperature: " << minTemp << "°C" << endl;
    }
};

class ForecastDisplay : public Observer {
private:
    float lastPressure = 1013;
    
public:
    void update(float temperature, float humidity, float pressure) override {
        cout << "[Forecast Display]" << endl;
        if (pressure > lastPressure) {
            cout << "  Improving weather on the way!" << endl;
        } else if (pressure < lastPressure) {
            cout << "  Watch out for cooler, rainy weather" << endl;
        } else {
            cout << "  More of the same" << endl;
        }
        lastPressure = pressure;
    }
};

// ============= Example 2: Stock Market =============

class StockObserver {
public:
    virtual ~StockObserver() = default;
    virtual void update(const string& symbol, double price) = 0;
};

class Stock {
private:
    string symbol;
    double price;
    vector<shared_ptr<StockObserver>> observers;
    
public:
    Stock(const string& sym, double initialPrice) 
        : symbol(sym), price(initialPrice) {}
    
    void subscribe(shared_ptr<StockObserver> observer) {
        observers.push_back(observer);
    }
    
    void unsubscribe(shared_ptr<StockObserver> observer) {
        auto it = find(observers.begin(), observers.end(), observer);
        if (it != observers.end()) {
            observers.erase(it);
        }
    }
    
    void setPrice(double newPrice) {
        if (newPrice != price) {
            price = newPrice;
            cout << "\n[" << symbol << "] Price changed to $" << price << endl;
            notifyObservers();
        }
    }
    
    double getPrice() const { return price; }
    string getSymbol() const { return symbol; }
    
private:
    void notifyObservers() {
        for (auto& observer : observers) {
            observer->update(symbol, price);
        }
    }
};

class Investor : public StockObserver {
private:
    string name;
    double buyThreshold;
    double sellThreshold;
    
public:
    Investor(const string& n, double buy, double sell)
        : name(n), buyThreshold(buy), sellThreshold(sell) {}
    
    void update(const string& symbol, double price) override {
        cout << "[Investor " << name << "] ";
        if (price <= buyThreshold) {
            cout << "BUY signal for " << symbol << " at $" << price << endl;
        } else if (price >= sellThreshold) {
            cout << "SELL signal for " << symbol << " at $" << price << endl;
        } else {
            cout << "HOLD " << symbol << " at $" << price << endl;
        }
    }
};

class StockAnalyzer : public StockObserver {
public:
    void update(const string& symbol, double price) override {
        cout << "[Stock Analyzer] Analyzing " << symbol 
             << " - Current price: $" << price << endl;
        // Perform analysis
        if (price > 150) {
            cout << "  Analysis: Overvalued - consider selling" << endl;
        } else if (price < 90) {
            cout << "  Analysis: Undervalued - consider buying" << endl;
        } else {
            cout << "  Analysis: Fairly priced" << endl;
        }
    }
};

// ============= Example 3: Newsletter System =============

class Subscriber {
public:
    virtual ~Subscriber() = default;
    virtual void notify(const string& article) = 0;
    virtual string getEmail() const = 0;
};

class Newsletter {
private:
    vector<shared_ptr<Subscriber>> subscribers;
    string latestArticle;
    
public:
    void subscribe(shared_ptr<Subscriber> subscriber) {
        subscribers.push_back(subscriber);
        cout << subscriber->getEmail() << " subscribed to newsletter" << endl;
    }
    
    void unsubscribe(shared_ptr<Subscriber> subscriber) {
        auto it = find(subscribers.begin(), subscribers.end(), subscriber);
        if (it != subscribers.end()) {
            subscribers.erase(it);
            cout << subscriber->getEmail() << " unsubscribed from newsletter" << endl;
        }
    }
    
    void publishArticle(const string& article) {
        cout << "\n[Newsletter] Publishing new article..." << endl;
        latestArticle = article;
        notifySubscribers();
    }
    
private:
    void notifySubscribers() {
        for (auto& subscriber : subscribers) {
            subscriber->notify(latestArticle);
        }
    }
};

class EmailSubscriber : public Subscriber {
private:
    string email;
    
public:
    EmailSubscriber(const string& e) : email(e) {}
    
    void notify(const string& article) override {
        cout << "[Email to " << email << "] New article: " << article << endl;
    }
    
    string getEmail() const override { return email; }
};

class SMSSubscriber : public Subscriber {
private:
    string phoneNumber;
    
public:
    SMSSubscriber(const string& phone) : phoneNumber(phone) {}
    
    void notify(const string& article) override {
        cout << "[SMS to " << phoneNumber << "] New article published" << endl;
    }
    
    string getEmail() const override { return phoneNumber; }
};

int main() {
    cout << "=== OBSERVER PATTERN DEMO ===" << endl;
    
    // Example 1: Weather Station
    cout << "\n1. WEATHER STATION:" << endl;
    cout << "===================" << endl;
    
    WeatherStation station;
    
    auto currentDisplay = make_shared<CurrentConditionsDisplay>();
    auto statsDisplay = make_shared<StatisticsDisplay>();
    auto forecastDisplay = make_shared<ForecastDisplay>();
    
    station.attach(currentDisplay);
    station.attach(statsDisplay);
    station.attach(forecastDisplay);
    
    station.setMeasurements(25.0, 65.0, 1013.0);
    station.setMeasurements(27.0, 70.0, 1012.0);
    station.setMeasurements(23.0, 75.0, 1011.0);
    
    // Example 2: Stock Market
    cout << "\n\n2. STOCK MARKET:" << endl;
    cout << "================" << endl;
    
    Stock appleStock("AAPL", 120.0);
    
    auto investor1 = make_shared<Investor>("Alice", 100.0, 150.0);
    auto investor2 = make_shared<Investor>("Bob", 90.0, 140.0);
    auto analyzer = make_shared<StockAnalyzer>();
    
    appleStock.subscribe(investor1);
    appleStock.subscribe(investor2);
    appleStock.subscribe(analyzer);
    
    appleStock.setPrice(110.0);
    appleStock.setPrice(95.0);
    appleStock.setPrice(155.0);
    
    // Example 3: Newsletter
    cout << "\n\n3. NEWSLETTER SYSTEM:" << endl;
    cout << "=====================" << endl;
    
    Newsletter techNewsletter;
    
    auto emailSub1 = make_shared<EmailSubscriber>("alice@example.com");
    auto emailSub2 = make_shared<EmailSubscriber>("bob@example.com");
    auto smsSub = make_shared<SMSSubscriber>("+1-555-1234");
    
    techNewsletter.subscribe(emailSub1);
    techNewsletter.subscribe(emailSub2);
    techNewsletter.subscribe(smsSub);
    
    techNewsletter.publishArticle("C++ Design Patterns Explained");
    techNewsletter.publishArticle("Observer Pattern in Real World");
    
    cout << "\n\n=== KEY TAKEAWAYS ===" << endl;
    cout << "1. Observer defines ONE-TO-MANY dependency" << endl;
    cout << "2. Subject notifies all observers when state changes" << endl;
    cout << "3. Observers can subscribe/unsubscribe at runtime" << endl;
    cout << "4. Loose coupling - subject doesn't know observer details" << endl;
    cout << "5. Observers can be of different types" << endl;
    cout << "6. Foundation of event-driven programming and MVC" << endl;
    cout << "7. Also known as Publish-Subscribe pattern" << endl;
    
    return 0;
}

