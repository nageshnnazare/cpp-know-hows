# Design Patterns Index

Complete implementation of all 23 Gang of Four design patterns in C++17.

## 📁 Directory Structure

```
design_patterns_cpp/
├── README.md                    # Overview and introduction
├── INDEX.md                     # This file - complete index
├── PATTERN_SUMMARY.md           # Quick reference guide
├── COMPILE_AND_RUN.md           # How to compile and run
│
├── creational/                  # 5 Creational Patterns
│   ├── 01_singleton.cpp         # One instance only
│   ├── 02_factory_method.cpp    # Create via subclasses
│   ├── 03_abstract_factory.cpp  # Families of objects
│   ├── 04_builder.cpp           # Step-by-step construction
│   └── 05_prototype.cpp         # Clone existing objects
│
├── structural/                  # 7 Structural Patterns
│   ├── 01_adapter.cpp           # Interface compatibility
│   ├── 02_bridge.cpp            # Decouple abstraction/implementation
│   ├── 03_composite.cpp         # Tree structures
│   ├── 04_decorator.cpp         # Add responsibilities
│   ├── 05_facade.cpp            # Simplified interface
│   ├── 06_flyweight.cpp         # Share common state
│   └── 07_proxy.cpp             # Control access
│
└── behavioral/                  # 11 Behavioral Patterns + Bonus
    ├── 01_chain_of_responsibility.cpp  # Pass along chain
    ├── 02_command.cpp                  # Encapsulate requests
    ├── 03_iterator.cpp                 # Sequential access
    ├── 04_mediator.cpp                 # Centralize communication
    ├── 05_memento.cpp                  # Capture state
    ├── 06_observer.cpp                 # Notify dependents
    ├── 07_state.cpp                    # Behavior by state
    ├── 08_strategy.cpp                 # Interchangeable algorithms
    ├── 09_template_method.cpp          # Algorithm skeleton
    ├── 10_visitor.cpp                  # Separate operations
    ├── 11_interpreter.cpp              # Language grammar
    └── 12_iterator_types.cpp           # BONUS: All C++ iterator types
```

## 🎯 Quick Start

```bash
# Clone or navigate to directory
cd /tmp/design_patterns_cpp

# Compile and run any pattern
cd creational
g++ -std=c++17 -o singleton 01_singleton.cpp
./singleton

# Or use the compilation guide
cat COMPILE_AND_RUN.md
```

## 📖 Pattern Details

### 🏗️ Creational Patterns

#### 1. Singleton
**File:** `creational/01_singleton.cpp`  
**Purpose:** Ensure only one instance exists  
**Examples:** Logger, Database Connection, Config Manager  
**Key Concepts:** Meyer's Singleton, Thread-safety, Lazy initialization

#### 2. Factory Method
**File:** `creational/02_factory_method.cpp`  
**Purpose:** Delegate object creation to subclasses  
**Examples:** Transport (Truck/Ship/Airplane), Documents (PDF/Word/Text)  
**Key Concepts:** Creator, Product, Concrete creators

#### 3. Abstract Factory
**File:** `creational/03_abstract_factory.cpp`  
**Purpose:** Create families of related objects  
**Examples:** GUI (Windows/Mac/Linux widgets)  
**Key Concepts:** Product families, Consistency

#### 4. Builder
**File:** `creational/04_builder.cpp`  
**Purpose:** Construct complex objects step by step  
**Examples:** House construction, Pizza ordering  
**Key Concepts:** Director, Fluent interface, Telescoping constructor problem

#### 5. Prototype
**File:** `creational/05_prototype.cpp`  
**Purpose:** Create objects by cloning  
**Examples:** Shape cloning, Document templates  
**Key Concepts:** Clone method, Registry, Deep vs shallow copy

### 🔧 Structural Patterns

#### 1. Adapter
**File:** `structural/01_adapter.cpp`  
**Purpose:** Make incompatible interfaces work together  
**Examples:** Media players, Payment processors, Temperature sensors  
**Key Concepts:** Target, Adaptee, Object vs Class adapter

#### 2. Bridge
**File:** `structural/02_bridge.cpp`  
**Purpose:** Separate abstraction from implementation  
**Examples:** Remote control & devices, Shape & renderer  
**Key Concepts:** Abstraction, Implementor, Cartesian product problem

#### 3. Composite
**File:** `structural/03_composite.cpp`  
**Purpose:** Tree structures (part-whole hierarchies)  
**Examples:** File system, Graphics groups, Organization chart  
**Key Concepts:** Component, Leaf, Composite, Recursive composition

#### 4. Decorator
**File:** `structural/04_decorator.cpp`  
**Purpose:** Add responsibilities dynamically  
**Examples:** Coffee shop (adding ingredients), Text formatting, I/O streams  
**Key Concepts:** Wrapper, Layering, Alternative to subclassing

#### 5. Facade
**File:** `structural/05_facade.cpp`  
**Purpose:** Simplified interface to complex system  
**Examples:** Home theater, Computer startup, Online shopping  
**Key Concepts:** Subsystem, Unified interface

#### 6. Flyweight
**File:** `structural/06_flyweight.cpp`  
**Purpose:** Share common data among many objects  
**Examples:** Forest (trees), Text editor (characters), Particles  
**Key Concepts:** Intrinsic vs Extrinsic state, Flyweight factory

#### 7. Proxy
**File:** `structural/07_proxy.cpp`  
**Purpose:** Control access to another object  
**Examples:** Lazy loading images, Access control, Caching, Smart pointers  
**Key Concepts:** Virtual, Protection, Caching, Smart reference proxies

### 🎭 Behavioral Patterns

#### 1. Chain of Responsibility
**File:** `behavioral/01_chain_of_responsibility.cpp`  
**Purpose:** Pass requests along a chain  
**Examples:** Support tickets, Logging, Purchase approvals  
**Key Concepts:** Handler, Successor, Request processing

#### 2. Command
**File:** `behavioral/02_command.cpp`  
**Purpose:** Encapsulate requests as objects  
**Examples:** Text editor undo/redo, Smart home, Macro commands  
**Key Concepts:** Command, Invoker, Receiver, Undo/Redo

#### 3. Iterator
**File:** `behavioral/03_iterator.cpp`  
**Purpose:** Sequential access to collection elements  
**Examples:** Book collection, Tree traversal, Social media feed  
**Key Concepts:** Iterator, Aggregate, Traversal strategies

#### 4. Mediator
**File:** `behavioral/04_mediator.cpp`  
**Purpose:** Centralize complex communications  
**Examples:** Chat room, Air traffic control, GUI dialogs  
**Key Concepts:** Mediator, Colleague, Reduced coupling

#### 5. Memento
**File:** `behavioral/05_memento.cpp`  
**Purpose:** Capture and restore object state  
**Examples:** Text editor, Game saves, Configuration snapshots  
**Key Concepts:** Originator, Memento, Caretaker, Encapsulation

#### 6. Observer
**File:** `behavioral/06_observer.cpp`  
**Purpose:** Notify dependents of state changes  
**Examples:** Weather station, Stock market, Newsletter  
**Key Concepts:** Subject, Observer, Publish-Subscribe, MVC

#### 7. State
**File:** `behavioral/07_state.cpp`  
**Purpose:** Alter behavior when state changes  
**Examples:** Vending machine, Document workflow, Media player  
**Key Concepts:** Context, State, State transitions

#### 8. Strategy
**File:** `behavioral/08_strategy.cpp`  
**Purpose:** Family of interchangeable algorithms  
**Examples:** Sorting algorithms, Payment methods, Navigation routes  
**Key Concepts:** Strategy, Context, Algorithm selection

#### 9. Template Method
**File:** `behavioral/09_template_method.cpp`  
**Purpose:** Define algorithm skeleton  
**Examples:** Beverage preparation, Data mining, Build process  
**Key Concepts:** Abstract class, Template method, Hooks, Hollywood Principle

#### 10. Visitor
**File:** `behavioral/10_visitor.cpp`  
**Purpose:** Separate algorithms from objects  
**Examples:** Shape operations, Computer parts, File system  
**Key Concepts:** Visitor, Element, Double dispatch, Accept method

#### 11. Interpreter
**File:** `behavioral/11_interpreter.cpp`  
**Purpose:** Interpret language grammar  
**Examples:** Boolean expressions, Arithmetic, Roman numerals, Commands  
**Key Concepts:** Abstract Syntax Tree, Terminal/Non-terminal expressions

#### 12. Iterator Types (BONUS)
**File:** `behavioral/12_iterator_types.cpp`  
**Purpose:** Comprehensive guide to all C++ iterator categories  
**Examples:** Input, Output, Forward, Bidirectional, Random Access, Contiguous iterators  
**Key Concepts:** Iterator hierarchy, Container requirements, Algorithm compatibility  
**Note:** This complements the Iterator pattern with detailed STL iterator types

## 🎓 Learning Recommendations

### Suggested Order

**Week 1 - Foundations (5 patterns):**
1. Singleton
2. Factory Method
3. Adapter
4. Strategy
5. Observer

**Week 2 - Common Patterns (6 patterns):**
6. Builder
7. Decorator
8. Composite
9. Command
10. State
11. Template Method

**Week 3 - Advanced (6 patterns):**
12. Abstract Factory
13. Facade
14. Proxy
15. Chain of Responsibility
16. Iterator
17. Memento

**Week 4 - Specialized (6 patterns):**
18. Prototype
19. Bridge
20. Flyweight
21. Mediator
22. Visitor
23. Interpreter

### Study Method

For each pattern:
1. **Read** the file header comments
2. **Understand** the problem it solves
3. **Study** the code examples
4. **Compile** and run it
5. **Modify** it to test understanding
6. **Implement** it in a personal project
7. **Review** after a few days

## 🔍 Finding the Right Pattern

### By Use Case

**Need to...**
- Create one instance? → **Singleton**
- Create complex object? → **Builder**
- Create without knowing type? → **Factory Method**
- Create compatible families? → **Abstract Factory**
- Clone objects? → **Prototype**
- Fix incompatible interfaces? → **Adapter**
- Simplify complex system? → **Facade**
- Add features without changing class? → **Decorator**
- Build tree structures? → **Composite**
- Save memory with many objects? → **Flyweight**
- Control object access? → **Proxy**
- Decouple abstraction/implementation? → **Bridge**
- Pass requests through chain? → **Chain of Responsibility**
- Implement undo/redo? → **Command**
- Traverse collections? → **Iterator**
- Reduce object coupling? → **Mediator**
- Save/restore state? → **Memento**
- Notify multiple objects? → **Observer**
- Change behavior by state? → **State**
- Switch algorithms at runtime? → **Strategy**
- Define algorithm template? → **Template Method**
- Add operations to structure? → **Visitor**
- Parse simple language? → **Interpreter**

## 📊 Pattern Characteristics

### Complexity Level
- **Simple:** Singleton, Adapter, Strategy, Facade
- **Medium:** Factory Method, Decorator, Observer, Command, State
- **Complex:** Abstract Factory, Builder, Composite, Bridge, Mediator, Visitor
- **Advanced:** Prototype, Flyweight, Chain, Iterator, Memento, Template Method, Interpreter

### Usage Frequency
- **Very Common:** Singleton, Factory Method, Observer, Strategy
- **Common:** Adapter, Decorator, Facade, Command, State
- **Moderate:** Builder, Composite, Proxy, Iterator
- **Specialized:** Abstract Factory, Prototype, Bridge, Flyweight, Chain, Mediator, Memento, Template Method, Visitor, Interpreter

## 🛠️ Code Quality

Each example includes:
- ✅ Detailed comments explaining purpose and usage
- ✅ Multiple real-world examples per pattern
- ✅ When to use / when not to use guidance
- ✅ Pros and cons clearly listed
- ✅ Compile-ready code (C++17)
- ✅ Clean, readable formatting
- ✅ Modern C++ practices (smart pointers, etc.)
- ✅ Complete working demonstrations

## 📞 Support

For questions or issues:
1. Read the pattern file comments
2. Check COMPILE_AND_RUN.md
3. Review PATTERN_SUMMARY.md
4. Consult Gang of Four book
5. Visit refactoring.guru

## 📄 License

Educational purposes. Learn, modify, and share freely.

---

**Total Patterns:** 23  
**Total Examples:** 69 (3 per pattern average)  
**Lines of Code:** ~8,000  
**Documentation:** Comprehensive comments in every file

**Happy Learning! 🚀**

