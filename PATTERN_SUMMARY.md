# Design Patterns Quick Reference

## 📋 Complete Pattern List

### 🏗️ CREATIONAL PATTERNS (5)

| Pattern | Purpose | When to Use | Key Benefit |
|---------|---------|-------------|-------------|
| **Singleton** | One instance only | Global access to single instance | Controlled access, lazy init |
| **Factory Method** | Create objects via subclasses | Type determined at runtime | Loose coupling, extensible |
| **Abstract Factory** | Create families of related objects | Need compatible product sets | Ensures product compatibility |
| **Builder** | Construct complex objects step-by-step | Many construction parameters | Readable, flexible construction |
| **Prototype** | Clone existing objects | Object creation is expensive | Avoids expensive initialization |

### 🔧 STRUCTURAL PATTERNS (7)

| Pattern | Purpose | When to Use | Key Benefit |
|---------|---------|-------------|-------------|
| **Adapter** | Make incompatible interfaces work | Integrate legacy/3rd-party code | Interface compatibility |
| **Bridge** | Separate abstraction from implementation | Both should vary independently | Independent variation |
| **Composite** | Tree structures (part-whole hierarchies) | Treat individual & groups uniformly | Recursive composition |
| **Decorator** | Add responsibilities dynamically | Extend behavior without subclassing | Flexible alternatives to subclassing |
| **Facade** | Simplified interface to complex system | Hide complexity | Easier to use |
| **Flyweight** | Share common state among many objects | Many similar objects (memory) | Reduced memory usage |
| **Proxy** | Control access to object | Lazy loading, access control, caching | Controls access, adds functionality |

### 🎭 BEHAVIORAL PATTERNS (11)

| Pattern | Purpose | When to Use | Key Benefit |
|---------|---------|-------------|-------------|
| **Chain of Responsibility** | Pass request along chain | Multiple potential handlers | Decouples sender from receiver |
| **Command** | Encapsulate requests as objects | Undo/redo, queue operations | Parameterize, queue, log requests |
| **Iterator** | Access elements sequentially | Traverse collections uniformly | Hide internal structure |
| **Mediator** | Centralize complex communications | Many interconnected objects | Reduces coupling |
| **Memento** | Capture & restore object state | Undo/redo, snapshots | Preserves encapsulation |
| **Observer** | Notify dependents of state changes | One-to-many dependency | Loose coupling, broadcast |
| **State** | Alter behavior when state changes | Behavior depends on state | Eliminates conditionals |
| **Strategy** | Family of interchangeable algorithms | Multiple algorithm variants | Runtime algorithm selection |
| **Template Method** | Define algorithm skeleton | Similar algorithms, minor differences | Code reuse via inheritance |
| **Visitor** | Separate algorithms from objects | Add operations without modifying | Easy to add operations |
| **Interpreter** | Interpret language grammar | Simple language/grammar | Easy grammar representation |

## 🎯 Pattern Selection Guide

### By Problem Type

**Creating Objects:**
- Single instance needed → **Singleton**
- Complex construction → **Builder**
- Create by copying → **Prototype**
- Create without knowing type → **Factory Method**
- Create families of objects → **Abstract Factory**

**Structuring Objects:**
- Incompatible interfaces → **Adapter**
- Simplify complex system → **Facade**
- Add features dynamically → **Decorator**
- Save memory with shared state → **Flyweight**
- Tree structures → **Composite**
- Control access → **Proxy**
- Independent abstraction/implementation → **Bridge**

**Object Behavior:**
- Undo/redo → **Command** or **Memento**
- Notify multiple objects → **Observer**
- Choose algorithm at runtime → **Strategy**
- Behavior changes with state → **State**
- Process chain → **Chain of Responsibility**
- Traverse collection → **Iterator**
- Reduce coupling between objects → **Mediator**
- Define algorithm skeleton → **Template Method**
- Add operations to structure → **Visitor**
- Parse language → **Interpreter**

## 🔗 Pattern Relationships

### Patterns that Work Together

- **Composite + Visitor**: Perform operations on complex trees
- **Strategy + Factory**: Create different strategies
- **Decorator + Factory**: Create decorated objects
- **Observer + Mediator**: MVC architecture
- **Command + Memento**: Implement undo
- **Flyweight + Composite**: Share leaf nodes
- **Abstract Factory + Bridge**: Create platform-specific implementations
- **Prototype + Abstract Factory**: Clone product families

### Similar Patterns Compared

**Adapter vs Bridge:**
- Adapter: Fix incompatible interfaces (after design)
- Bridge: Design abstraction/implementation to vary independently (before design)

**Decorator vs Proxy:**
- Decorator: Add responsibilities
- Proxy: Control access (same interface)

**State vs Strategy:**
- State: Object changes behavior based on internal state
- Strategy: Client chooses algorithm

**Factory Method vs Abstract Factory:**
- Factory Method: Creates ONE product
- Abstract Factory: Creates FAMILIES of products

**Composite vs Decorator:**
- Composite: Structural pattern for tree structures
- Decorator: Adds responsibilities to individual objects

## 💡 Design Principles

The patterns embody these principles:

1. **Program to interface, not implementation**
2. **Favor composition over inheritance**
3. **Encapsulate what varies**
4. **Strive for loosely coupled designs**
5. **Classes should be open for extension, closed for modification** (Open/Closed)
6. **Depend on abstractions, not concrete classes** (Dependency Inversion)
7. **Single Responsibility Principle**
8. **Liskov Substitution Principle**

## 📚 Study Tips

### For Retention

1. **Understand the problem first** - Know WHY before HOW
2. **Learn by category** - Group related patterns
3. **Compare similar patterns** - Note differences
4. **Practice with examples** - Code them yourself
5. **Use in projects** - Apply to real problems
6. **Teach others** - Best way to solidify understanding
7. **Review regularly** - Spaced repetition helps

### Red Flags (Pattern Overuse)

❌ Don't use patterns just because you know them  
❌ Don't over-engineer simple problems  
❌ Don't force patterns where they don't fit  
❌ Don't use patterns you don't understand  

✅ Use patterns when they solve real problems  
✅ Start simple, refactor to patterns when needed  
✅ Understand trade-offs of each pattern  
✅ Keep code readable and maintainable  

## 🎓 Learning Path

### Beginner (Start Here)
1. Singleton - Easiest to understand
2. Factory Method - Basic creation pattern
3. Adapter - Simple structural pattern
4. Strategy - Fundamental behavioral pattern
5. Observer - Common event pattern

### Intermediate
6. Builder - More complex creation
7. Decorator - Flexible extension
8. Composite - Tree structures
9. Command - Undo/redo functionality
10. State - State management
11. Template Method - Algorithm framework

### Advanced
12. Abstract Factory - Families of objects
13. Prototype - Object cloning
14. Bridge - Complex decoupling
15. Facade - System simplification
16. Flyweight - Memory optimization
17. Proxy - Access control
18. Chain of Responsibility - Processing chain
19. Iterator - Collection traversal
20. Mediator - Complex interactions
21. Memento - State preservation
22. Visitor - Operation separation
23. Interpreter - Grammar interpretation

## 📊 Pattern Usage Frequency

**Very Common (Learn First):**
- Singleton, Factory Method, Observer, Strategy, Decorator

**Common:**
- Adapter, Facade, Command, State, Template Method, Composite

**Moderate:**
- Builder, Proxy, Iterator, Chain of Responsibility, Memento

**Less Common:**
- Abstract Factory, Prototype, Bridge, Flyweight, Mediator

**Specialized:**
- Visitor, Interpreter

## 🌟 Real-World Examples

- **GUI Frameworks**: Observer, Command, Composite, Decorator, Strategy
- **Game Development**: State, Strategy, Command, Prototype, Flyweight
- **Database Libraries**: Singleton, Factory, Proxy, Iterator
- **Web Frameworks**: MVC (Observer + Strategy), Factory, Singleton
- **Compilers**: Visitor, Interpreter, Composite
- **Operating Systems**: Proxy, Flyweight, Facade

## 📖 Additional Resources

### Books
- Design Patterns (Gang of Four) - The original
- Head First Design Patterns - Beginner friendly
- Clean Code (Robert Martin) - Principles

### Websites
- refactoring.guru - Interactive tutorials
- sourcemaking.com - Examples and explanations
- github.com - Real code examples

### Practice
- Implement each pattern from scratch
- Refactor existing code using patterns
- Build a project using multiple patterns
- Code reviews focusing on patterns
- Contribute to open source

---

**Remember**: Patterns are tools, not rules. Use them wisely to solve real problems, not to show off knowledge. Good code is simple, readable, and maintainable. Patterns should make your code better, not more complex.

