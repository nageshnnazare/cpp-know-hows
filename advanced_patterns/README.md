# Advanced C++ Patterns & Principles

This directory contains advanced patterns and principles that complement the Gang of Four design patterns. These patterns are essential for modern C++ development.

## 📚 Contents

### SOLID Principles (5)
Object-Oriented Design Principles that form the foundation of maintainable software:

1. **01_solid_srp.cpp** - Single Responsibility Principle
   - One class, one responsibility
   - Reduces coupling, improves maintainability

2. **02_solid_ocp.cpp** - Open/Closed Principle
   - Open for extension, closed for modification
   - Use abstraction and polymorphism

3. **03_solid_lsp.cpp** - Liskov Substitution Principle
   - Subtypes must be substitutable for base types
   - Proper inheritance hierarchies

4. **04_solid_isp.cpp** - Interface Segregation Principle
   - Many specific interfaces > one general interface
   - Clients shouldn't depend on unused methods

5. **05_solid_dip.cpp** - Dependency Inversion Principle
   - Depend on abstractions, not concretions
   - High-level modules independent of low-level

### Modern C++ Patterns (8)
Advanced techniques for high-performance, maintainable C++ code:

6. **06_crtp.cpp** - Curiously Recurring Template Pattern
   - Compile-time polymorphism
   - Zero overhead abstraction
   - Used in high-performance code

7. **07_raii.cpp** - Resource Acquisition Is Initialization
   - Automatic resource management
   - Exception-safe code
   - Foundation of modern C++

8. **08_pimpl.cpp** - Pointer to Implementation
   - Compilation firewall
   - Binary compatibility (ABI stable)
   - Hide implementation details

9. **09_type_erasure.cpp** - Type Erasure
   - Runtime polymorphism without inheritance
   - Value semantics with polymorphism
   - Used in std::function, std::any

10. **10_sfinae.cpp** - Substitution Failure Is Not An Error
    - Template metaprogramming
    - Compile-time type introspection
    - Enable/disable templates conditionally

11. **11_null_object.cpp** - Null Object Pattern
    - Eliminate null checks
    - Provide default "do-nothing" behavior
    - Cleaner, safer code

12. **12_object_pool.cpp** - Object Pool Pattern
    - Reuse expensive objects
    - Performance optimization
    - Essential for games and servers

13. **13_policy_based_design.cpp** - Policy-Based Design
    - Compile-time behavior configuration
    - Zero runtime overhead
    - Highly flexible and reusable

## 🎯 Learning Path

### Beginners
Start with SOLID principles in order:
1. Single Responsibility (SRP)
2. Open/Closed (OCP)
3. Liskov Substitution (LSP)
4. Interface Segregation (ISP)
5. Dependency Inversion (DIP)

Then learn RAII (fundamental to C++).

### Intermediate
After SOLID, study these patterns:
- RAII (essential for C++)
- Null Object (simplifies code)
- Object Pool (performance)
- PIMPL (library design)

### Advanced
Master these for expert-level C++:
- CRTP (advanced templates)
- Type Erasure (advanced polymorphism)
- SFINAE (metaprogramming)
- Policy-Based Design (generic programming)

## 🚀 Quick Start

### Compile and Run
```bash
# Compile a specific pattern
g++ -std=c++17 -pthread -o srp 01_solid_srp.cpp
./srp

# Compile all patterns
make advanced

# Test all patterns
make test-advanced
```

### Example Usage
```cpp
// SOLID: Single Responsibility
class User {
    string name;
    // Only user data - one responsibility
};

class UserRepository {
    // Only database operations
};

// RAII: Automatic resource management
{
    File file("data.txt");  // Opens in constructor
    file.write("Hello");
} // Automatically closed in destructor

// Null Object: No null checks
logger->log("message");  // Works even if logger is NullLogger

// CRTP: Compile-time polymorphism
template <typename Derived>
class Base {
    void interface() {
        static_cast<Derived*>(this)->implementation();
    }
};
```

## 📊 Pattern Comparison

### When to Use Each Pattern

**SOLID Principles** - Always!
- Foundation of good OOP design
- Apply to all code

**RAII** - Always in C++!
- Core C++ idiom
- Use for all resource management

**CRTP** - Performance-critical code
- Game engines
- High-performance computing
- Embedded systems (no virtual functions)

**Type Erasure** - Generic containers
- std::function-like interfaces
- Value semantics with polymorphism
- Can't modify existing types

**SFINAE** - Template constraints
- Type trait detection
- Before C++20 Concepts
- Library development

**Null Object** - Many null checks
- Optional functionality
- Default behavior makes sense

**Object Pool** - Expensive objects
- Game development (particles, bullets)
- Server applications (connections)
- Performance-critical code

**PIMPL** - Library interfaces
- ABI stability required
- Hide implementation
- Large, complex classes

**Policy-Based Design** - Flexible libraries
- STL-style generic programming
- Compile-time configuration
- Zero overhead required

## 🎓 Key Concepts

### SOLID Benefits
- ✅ Maintainable code
- ✅ Flexible design
- ✅ Testable components
- ✅ Reduced coupling
- ✅ Better reusability

### Performance Patterns
- **CRTP**: Zero overhead polymorphism
- **RAII**: No runtime cost
- **Object Pool**: Reduced allocation overhead
- **Policy-Based**: Compile-time configuration

### Modern C++ Idioms
- **RAII**: Resource management
- **CRTP**: Static polymorphism
- **Type Erasure**: Duck typing
- **SFINAE**: Template constraints

## 📖 Additional Resources

### Books
- "Clean Code" - Robert C. Martin (SOLID)
- "Design Patterns" - Gang of Four
- "Modern C++ Design" - Andrei Alexandrescu (Policies, CRTP)
- "Effective C++" - Scott Meyers (RAII, idioms)
- "C++ Templates: The Complete Guide" - Vandevoorde & Josuttis

### Online
- C++ Core Guidelines
- cppreference.com
- isocpp.org

## 🔧 Compilation

All files use C++17 standard:
```bash
g++ -std=c++17 -pthread -o program file.cpp
```

Some patterns (Object Pool) require `-pthread` for threading support.

## ✅ Status

All 13 patterns:
- ✅ Fully implemented
- ✅ Compile successfully
- ✅ Tested and working
- ✅ Comprehensively documented
- ✅ Real-world examples included

## 🌟 Highlights

### Most Important
1. **SOLID Principles** - Foundation of good design
2. **RAII** - Essential C++ idiom
3. **CRTP** - High-performance alternative to virtual functions

### Most Practical
1. **Null Object** - Eliminates null checks
2. **Object Pool** - Performance in games/servers
3. **PIMPL** - Library interface design

### Most Advanced
1. **Type Erasure** - Advanced polymorphism technique
2. **SFINAE** - Template metaprogramming
3. **Policy-Based Design** - Generic programming mastery

---

**Total Patterns:** 13  
**Lines of Code:** ~4,000+  
**Compilation Status:** ✅ All working  
**Documentation:** ✅ Complete  

Enjoy mastering advanced C++ patterns! 🚀
