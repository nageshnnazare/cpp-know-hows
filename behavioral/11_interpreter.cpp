/*
 * INTERPRETER PATTERN - Behavioral Pattern
 * 
 * PURPOSE:
 * Defines a representation for a language's grammar along with an interpreter
 * that uses the representation to interpret sentences in the language.
 * 
 * WHEN TO USE:
 * - Have a simple language to interpret
 * - Can represent statements as abstract syntax trees
 * - Efficiency is not critical concern
 * - Grammar is simple and stable
 * 
 * PROS:
 * + Easy to change and extend grammar
 * + Easy to implement grammar
 * + Can add new expressions easily
 * + Each rule is a separate class
 * 
 * CONS:
 * - Complex grammars are hard to maintain
 * - Not efficient for complex grammars
 * - Can result in many small classes
 * 
 * REAL-WORLD EXAMPLES:
 * - SQL parsing
 * - Regular expressions
 * - Mathematical expression evaluators
 * - Configuration file parsers
 * - Scripting languages
 */

#include <iostream>
#include <string>
#include <memory>
#include <unordered_map>
#include <sstream>
#include <cctype>
#include <vector>

using namespace std;

// ============= Example 1: Boolean Expression Interpreter =============

// Context
class Context {
private:
    unordered_map<string, bool> variables;
    
public:
    void setVariable(const string& name, bool value) {
        variables[name] = value;
    }
    
    bool getVariable(const string& name) const {
        auto it = variables.find(name);
        if (it != variables.end()) {
            return it->second;
        }
        return false;
    }
};

// Abstract Expression
class BooleanExpression {
public:
    virtual ~BooleanExpression() = default;
    virtual bool interpret(const Context& context) = 0;
};

// Terminal Expression - Variable
class VariableExpression : public BooleanExpression {
private:
    string name;
    
public:
    VariableExpression(const string& n) : name(n) {}
    
    bool interpret(const Context& context) override {
        bool value = context.getVariable(name);
        cout << "  Variable '" << name << "' = " << (value ? "true" : "false") << endl;
        return value;
    }
};

// Terminal Expression - Constant
class ConstantExpression : public BooleanExpression {
private:
    bool value;
    
public:
    ConstantExpression(bool v) : value(v) {}
    
    bool interpret(const Context& context) override {
        cout << "  Constant = " << (value ? "true" : "false") << endl;
        return value;
    }
};

// Non-terminal Expression - AND
class AndExpression : public BooleanExpression {
private:
    shared_ptr<BooleanExpression> left;
    shared_ptr<BooleanExpression> right;
    
public:
    AndExpression(shared_ptr<BooleanExpression> l, shared_ptr<BooleanExpression> r)
        : left(l), right(r) {}
    
    bool interpret(const Context& context) override {
        cout << "  Evaluating AND:" << endl;
        bool leftValue = left->interpret(context);
        bool rightValue = right->interpret(context);
        bool result = leftValue && rightValue;
        cout << "  AND result: " << (result ? "true" : "false") << endl;
        return result;
    }
};

// Non-terminal Expression - OR
class OrExpression : public BooleanExpression {
private:
    shared_ptr<BooleanExpression> left;
    shared_ptr<BooleanExpression> right;
    
public:
    OrExpression(shared_ptr<BooleanExpression> l, shared_ptr<BooleanExpression> r)
        : left(l), right(r) {}
    
    bool interpret(const Context& context) override {
        cout << "  Evaluating OR:" << endl;
        bool leftValue = left->interpret(context);
        bool rightValue = right->interpret(context);
        bool result = leftValue || rightValue;
        cout << "  OR result: " << (result ? "true" : "false") << endl;
        return result;
    }
};

// Non-terminal Expression - NOT
class NotExpression : public BooleanExpression {
private:
    shared_ptr<BooleanExpression> expression;
    
public:
    NotExpression(shared_ptr<BooleanExpression> expr) : expression(expr) {}
    
    bool interpret(const Context& context) override {
        cout << "  Evaluating NOT:" << endl;
        bool value = expression->interpret(context);
        bool result = !value;
        cout << "  NOT result: " << (result ? "true" : "false") << endl;
        return result;
    }
};

// ============= Example 2: Arithmetic Expression Interpreter =============

class ArithmeticContext {
private:
    unordered_map<string, int> variables;
    
public:
    void setVariable(const string& name, int value) {
        variables[name] = value;
    }
    
    int getVariable(const string& name) const {
        auto it = variables.find(name);
        if (it != variables.end()) {
            return it->second;
        }
        return 0;
    }
};

class ArithmeticExpression {
public:
    virtual ~ArithmeticExpression() = default;
    virtual int interpret(const ArithmeticContext& context) = 0;
};

class NumberExpression : public ArithmeticExpression {
private:
    int number;
    
public:
    NumberExpression(int n) : number(n) {}
    
    int interpret(const ArithmeticContext& context) override {
        return number;
    }
};

class VariableArithmeticExpression : public ArithmeticExpression {
private:
    string name;
    
public:
    VariableArithmeticExpression(const string& n) : name(n) {}
    
    int interpret(const ArithmeticContext& context) override {
        return context.getVariable(name);
    }
};

class AddExpression : public ArithmeticExpression {
private:
    shared_ptr<ArithmeticExpression> left;
    shared_ptr<ArithmeticExpression> right;
    
public:
    AddExpression(shared_ptr<ArithmeticExpression> l, shared_ptr<ArithmeticExpression> r)
        : left(l), right(r) {}
    
    int interpret(const ArithmeticContext& context) override {
        return left->interpret(context) + right->interpret(context);
    }
};

class SubtractExpression : public ArithmeticExpression {
private:
    shared_ptr<ArithmeticExpression> left;
    shared_ptr<ArithmeticExpression> right;
    
public:
    SubtractExpression(shared_ptr<ArithmeticExpression> l, shared_ptr<ArithmeticExpression> r)
        : left(l), right(r) {}
    
    int interpret(const ArithmeticContext& context) override {
        return left->interpret(context) - right->interpret(context);
    }
};

class MultiplyExpression : public ArithmeticExpression {
private:
    shared_ptr<ArithmeticExpression> left;
    shared_ptr<ArithmeticExpression> right;
    
public:
    MultiplyExpression(shared_ptr<ArithmeticExpression> l, shared_ptr<ArithmeticExpression> r)
        : left(l), right(r) {}
    
    int interpret(const ArithmeticContext& context) override {
        return left->interpret(context) * right->interpret(context);
    }
};

// ============= Example 3: Roman Numeral Interpreter =============

class RomanExpression {
public:
    virtual ~RomanExpression() = default;
    virtual int interpret() = 0;
};

class RomanNumber : public RomanExpression {
private:
    int value;
    
public:
    RomanNumber(char roman) {
        switch(roman) {
            case 'I': value = 1; break;
            case 'V': value = 5; break;
            case 'X': value = 10; break;
            case 'L': value = 50; break;
            case 'C': value = 100; break;
            case 'D': value = 500; break;
            case 'M': value = 1000; break;
            default: value = 0;
        }
    }
    
    int interpret() override {
        return value;
    }
};

class RomanNumeralInterpreter {
public:
    static int interpret(const string& roman) {
        int result = 0;
        int prevValue = 0;
        
        cout << "Interpreting Roman numeral: " << roman << endl;
        
        for (int i = roman.length() - 1; i >= 0; i--) {
            RomanNumber num(roman[i]);
            int value = num.interpret();
            
            if (value < prevValue) {
                cout << "  " << roman[i] << " (subtractive): -" << value << endl;
                result -= value;
            } else {
                cout << "  " << roman[i] << " (additive): +" << value << endl;
                result += value;
            }
            
            prevValue = value;
        }
        
        return result;
    }
};

// ============= Example 4: Simple Command Interpreter =============

class Command {
public:
    virtual ~Command() = default;
    virtual void execute() = 0;
};

class PrintCommand : public Command {
private:
    string message;
    
public:
    PrintCommand(const string& msg) : message(msg) {}
    
    void execute() override {
        cout << message << endl;
    }
};

class SetCommand : public Command {
private:
    string variable;
    string value;
    static unordered_map<string, string> variables;
    
public:
    SetCommand(const string& var, const string& val) 
        : variable(var), value(val) {}
    
    void execute() override {
        variables[variable] = value;
        cout << "Set " << variable << " = " << value << endl;
    }
    
    static string getVariable(const string& var) {
        return variables[var];
    }
};

unordered_map<string, string> SetCommand::variables;

class GetCommand : public Command {
private:
    string variable;
    
public:
    GetCommand(const string& var) : variable(var) {}
    
    void execute() override {
        string value = SetCommand::getVariable(variable);
        cout << variable << " = " << value << endl;
    }
};

class CommandInterpreter {
public:
    static shared_ptr<Command> parse(const string& input) {
        istringstream iss(input);
        string command;
        iss >> command;
        
        if (command == "PRINT") {
            string message;
            getline(iss, message);
            // Trim leading whitespace
            message = message.substr(message.find_first_not_of(" \t"));
            return make_shared<PrintCommand>(message);
        }
        else if (command == "SET") {
            string var, value;
            iss >> var >> value;
            return make_shared<SetCommand>(var, value);
        }
        else if (command == "GET") {
            string var;
            iss >> var;
            return make_shared<GetCommand>(var);
        }
        
        return nullptr;
    }
};

int main() {
    cout << "=== INTERPRETER PATTERN DEMO ===" << endl;
    
    // Example 1: Boolean Expression Interpreter
    cout << "\n1. BOOLEAN EXPRESSION INTERPRETER:" << endl;
    cout << "===================================" << endl;
    
    Context boolContext;
    boolContext.setVariable("x", true);
    boolContext.setVariable("y", false);
    boolContext.setVariable("z", true);
    
    // Expression: (x AND y) OR z
    cout << "\nExpression: (x AND y) OR z" << endl;
    auto expr1 = make_shared<OrExpression>(
        make_shared<AndExpression>(
            make_shared<VariableExpression>("x"),
            make_shared<VariableExpression>("y")
        ),
        make_shared<VariableExpression>("z")
    );
    
    bool result1 = expr1->interpret(boolContext);
    cout << "Final Result: " << (result1 ? "true" : "false") << endl;
    
    // Expression: NOT(x AND y)
    cout << "\n\nExpression: NOT(x AND y)" << endl;
    auto expr2 = make_shared<NotExpression>(
        make_shared<AndExpression>(
            make_shared<VariableExpression>("x"),
            make_shared<VariableExpression>("y")
        )
    );
    
    bool result2 = expr2->interpret(boolContext);
    cout << "Final Result: " << (result2 ? "true" : "false") << endl;
    
    // Example 2: Arithmetic Expression Interpreter
    cout << "\n\n2. ARITHMETIC EXPRESSION INTERPRETER:" << endl;
    cout << "=====================================" << endl;
    
    ArithmeticContext arithContext;
    arithContext.setVariable("a", 10);
    arithContext.setVariable("b", 5);
    arithContext.setVariable("c", 3);
    
    // Expression: (a + b) * c
    cout << "\nExpression: (a + b) * c" << endl;
    cout << "Where a=10, b=5, c=3" << endl;
    auto arithExpr = make_shared<MultiplyExpression>(
        make_shared<AddExpression>(
            make_shared<VariableArithmeticExpression>("a"),
            make_shared<VariableArithmeticExpression>("b")
        ),
        make_shared<VariableArithmeticExpression>("c")
    );
    
    int arithResult = arithExpr->interpret(arithContext);
    cout << "Result: " << arithResult << endl;
    
    // Example 3: Roman Numeral Interpreter
    cout << "\n\n3. ROMAN NUMERAL INTERPRETER:" << endl;
    cout << "==============================" << endl;
    
    string roman1 = "XIV";  // 14
    int decimal1 = RomanNumeralInterpreter::interpret(roman1);
    cout << "Decimal: " << decimal1 << "\n" << endl;
    
    string roman2 = "MCMXCIV";  // 1994
    int decimal2 = RomanNumeralInterpreter::interpret(roman2);
    cout << "Decimal: " << decimal2 << "\n" << endl;
    
    string roman3 = "MMXXIII";  // 2023
    int decimal3 = RomanNumeralInterpreter::interpret(roman3);
    cout << "Decimal: " << decimal3 << endl;
    
    // Example 4: Command Interpreter
    cout << "\n\n4. SIMPLE COMMAND INTERPRETER:" << endl;
    cout << "===============================" << endl;
    
    vector<string> commands = {
        "PRINT Hello, World!",
        "SET name Alice",
        "SET age 25",
        "GET name",
        "GET age",
        "PRINT Command executed successfully"
    };
    
    cout << "\nExecuting commands:" << endl;
    for (const auto& cmdStr : commands) {
        cout << "\n> " << cmdStr << endl;
        auto cmd = CommandInterpreter::parse(cmdStr);
        if (cmd) {
            cmd->execute();
        } else {
            cout << "Unknown command" << endl;
        }
    }
    
    cout << "\n\n=== KEY TAKEAWAYS ===" << endl;
    cout << "1. Interpreter defines GRAMMAR and interprets sentences" << endl;
    cout << "2. Each grammar rule becomes a class" << endl;
    cout << "3. Uses Composite pattern for abstract syntax tree" << endl;
    cout << "4. Terminal expressions are leaf nodes (variables, constants)" << endl;
    cout << "5. Non-terminal expressions are composite nodes (AND, OR, etc.)" << endl;
    cout << "6. Good for simple grammars, not suitable for complex ones" << endl;
    cout << "7. Easy to extend grammar by adding new expression classes" << endl;
    cout << "8. Consider parser generators for complex languages" << endl;
    
    return 0;
}

