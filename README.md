# C++ Design Patterns - Complete Guide

This repository contains detailed examples of all 23 Gang of Four design patterns in C++.

## 📚 Pattern Categories

### 🏗️ Creational Patterns (5)
Deal with object creation mechanisms, trying to create objects in a manner suitable to the situation.

1. **Singleton** - Ensures a class has only one instance with global access
2. **Factory Method** - Creates objects without specifying exact classes
3. **Abstract Factory** - Creates families of related objects
4. **Builder** - Constructs complex objects step by step
5. **Prototype** - Creates new objects by copying existing ones

### 🔧 Structural Patterns (7)
Deal with object composition, creating relationships between objects.

1. **Adapter** - Makes incompatible interfaces work together
2. **Bridge** - Separates abstraction from implementation
3. **Composite** - Composes objects into tree structures
4. **Decorator** - Adds new functionality to objects dynamically
5. **Facade** - Provides simplified interface to complex subsystem
6. **Flyweight** - Shares common state between multiple objects
7. **Proxy** - Provides placeholder/surrogate for another object

### 🎭 Behavioral Patterns (11 + Bonus)
Deal with communication between objects, how objects interact and distribute responsibility.

1. **Chain of Responsibility** - Passes requests along chain of handlers
2. **Command** - Encapsulates requests as objects
3. **Iterator** - Accesses elements of collection sequentially
4. **Mediator** - Reduces coupling between communicating objects
5. **Memento** - Captures and restores object's state
6. **Observer** - Notifies multiple objects about state changes
7. **State** - Alters behavior when internal state changes
8. **Strategy** - Selects algorithm at runtime
9. **Template Method** - Defines algorithm skeleton, letting subclasses override steps
10. **Visitor** - Separates algorithms from objects they operate on
11. **Interpreter** - Implements specialized language grammar

**Bonus:**
- **Iterator Types** - Comprehensive guide to all C++ iterator categories (Input, Output, Forward, Bidirectional, Random Access, Contiguous)

### 🚀 Advanced Patterns & Principles (NEW!)
Modern C++ patterns and design principles that complement the GoF patterns.

**SOLID Principles (5):**
- Single Responsibility, Open/Closed, Liskov Substitution, Interface Segregation, Dependency Inversion

**Modern C++ Patterns (8):**
- CRTP, RAII, PIMPL, Type Erasure, SFINAE, Null Object, Object Pool, Policy-Based Design

**Total: 13 advanced patterns** - See `advanced_patterns/` directory for details.

### ⚡ Compiler Optimizations (NEW!)
Understanding how compilers optimize your code for performance.

**Optimizations Covered:**
- RVO/NRVO (Return Value Optimization)
- Copy Elision (Zero-cost abstractions)
- Small String Optimization (SSO)
- Empty Base Optimization (EBO)

See `compiler_optimizations/` directory for examples and performance analysis.

## 🚀 How to Use

Each pattern has its own `.cpp` file with:
- Detailed comments explaining the pattern
- Real-world use cases
- When to use it
- Pros and cons
- Complete working example

To compile and run any example:
```bash
g++ -std=c++17 -o pattern_name pattern_name.cpp
./pattern_name
```

## 💡 Learning Tips

1. **Start with Creational** - Easiest to understand
2. **Move to Structural** - Learn about object relationships
3. **Finish with Behavioral** - Most complex but powerful
4. **Practice by doing** - Try modifying examples
5. **Combine patterns** - Real applications use multiple patterns together

