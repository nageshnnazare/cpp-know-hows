# Modules (C++20)

## Overview

C++20 introduced modules as a modern alternative to header files. Modules provide faster compilation, better encapsulation, and eliminate many preprocessor-related issues.

```
┌────────────────────────────────────────────────────────┐
│                   MODULES SYSTEM                       │
├────────────────────────────────────────────────────────┤
│                                                        │
│  DEFINITION     │  USAGE           │  PARTITIONS       │
│  ──────────     │  ─────           │  ──────────       │
│  • export       │  • import        │  • Submodules     │
│  • module       │  • No #include   │  • Private impl   │
│  • Interface    │  • No macros     │  • Organization   │
│                                                        │
│  BENEFITS:                                             │
│  • Faster compilation (no redundant parsing)           │
│  • Better encapsulation (export control)               │
│  • No header guards needed                             │
│  • No macro leakage                                    │
│  • Order-independent imports                           │
│                                                        │
└────────────────────────────────────────────────────────┘
```

---

## Why Modules?

### Problems with Headers
```cpp
// Traditional header problems:

// 1. Redundant parsing
#include <vector>    // Parsed in every .cpp file
#include <string>    // Thousands of lines parsed repeatedly
#include <iostream>

// 2. Macro leakage
#include <windows.h>  // Defines 'min', 'max' macros
int min = 5;          // ERROR: min is a macro!

// 3. Header order matters
#include "b.h"
#include "a.h"  // Might fail if a.h needed before b.h

// 4. Multiple inclusion guards everywhere
#ifndef HEADER_H
#define HEADER_H
// ...
#endif

// 5. Can't hide implementation details
// Everything in header is visible
```

### Modules Advantages
```
┌───────────────────────────────────────────────────────┐
│         Headers vs Modules Comparison                 │
├───────────────────────────────────────────────────────┤
│ Feature        │ Headers      │ Modules               │
├────────────────┼──────────────┼───────────────────────┤
│ Compilation    │ Slow         │ Fast                  │
│ Encapsulation  │ Weak         │ Strong                │
│ Macros         │ Leak         │ Don't leak            │
│ Order          │ Matters      │ Doesn't matter        │
│ Guards         │ Required     │ Not needed            │
│ Isolation      │ No           │ Yes                   │
│                                                       │
│ Typical compilation speedup: 2-10x                    │
└───────────────────────────────────────────────────────┘
```

---

## Basic Module Syntax

### Creating a Module
```cpp
// math.cppm (module interface file)
export module math;

export int add(int a, int b) {
    return a + b;
}

export int multiply(int a, int b) {
    return a * b;
}

// Not exported, internal only
int helper() {
    return 42;
}
```

### Using a Module
```cpp
// main.cpp
import math;  // Import module

int main() {
    int sum = add(5, 3);           // OK: exported
    int product = multiply(4, 2);  // OK: exported
    
    // int x = helper();           // ERROR: not exported
    
    return 0;
}
```

### Compilation
```bash
# Compile module interface (creates BMI - Binary Module Interface)
g++ -std=c++20 -fmodules-ts -c math.cppm -o math.o

# Compile main (uses BMI)
g++ -std=c++20 -fmodules-ts main.cpp math.o -o program

# Run
./program
```

---

## Module Structure

### Module Declaration
```cpp
// shapes.cppm
export module shapes;  // Module declaration

// Everything below is part of module 'shapes'
// but only 'export' declarations are visible to importers
```

### Export Declarations
```cpp
export module geometry;

// Export single function
export double area_circle(double radius) {
    return 3.14159 * radius * radius;
}

// Export multiple declarations
export {
    double area_square(double side);
    double area_rectangle(double width, double height);
}

// Export class
export class Rectangle {
public:
    Rectangle(double w, double h);
    double area() const;
private:
    double width_, height_;
};

// Export namespace
export namespace shapes {
    class Triangle { /* ... */ };
    class Circle { /* ... */ };
}

// Export type alias
export using Point = std::pair<double, double>;

// Export concept
export template<typename T>
concept Numeric = std::is_arithmetic_v<T>;
```

### Implementation
```cpp
// geometry.cppm
export module geometry;

export class Rectangle {
public:
    Rectangle(double w, double h);
    double area() const;
private:
    double width_, height_;
};

// Definitions in same file (module interface)
Rectangle::Rectangle(double w, double h) 
    : width_(w), height_(h) {}

double Rectangle::area() const {
    return width_ * height_;
}

// Or separate implementation file
// geometry_impl.cpp
module geometry;  // Implementation unit

// More implementations...
```

---

## Module Partitions

### Interface Partitions
```cpp
// math:basic.cppm
export module math:basic;  // Partition declaration

export int add(int a, int b) {
    return a + b;
}

export int subtract(int a, int b) {
    return a - b;
}
```

```cpp
// math:advanced.cppm
export module math:advanced;

export double power(double base, double exp) {
    return std::pow(base, exp);
}

export double sqrt(double x) {
    return std::sqrt(x);
}
```

```cpp
// math.cppm (primary module interface)
export module math;

// Re-export partitions
export import :basic;
export import :advanced;

// Or selectively export
// import :basic;
// import :advanced;
// export using math::basic::add;
```

### Implementation Partitions
```cpp
// math:internal.cppm (internal partition, not exported)
module math:internal;

// Helper functions, not visible outside module
int helper_function() {
    return 42;
}
```

```cpp
// math.cppm
export module math;

import :internal;  // Use internal partition

export int public_function() {
    return helper_function();  // Use internal helper
}
```

---

## Importing Standard Library

### Header Units
```cpp
// Import standard library as header units
import <vector>;
import <string>;
import <iostream>;

int main() {
    std::vector<std::string> names = {"Alice", "Bob"};
    
    for (const auto& name : names) {
        std::cout << name << '\n';
    }
    
    return 0;
}
```

### Standard Library Modules (Future)
```cpp
// C++23 and beyond (proposal)
import std;  // Import entire standard library as module

int main() {
    std::vector<int> vec = {1, 2, 3};
    std::cout << "Hello, modules!\n";
    
    return 0;
}
```

---

## Mixing Modules and Headers

### Global Module Fragment
```cpp
// mymodule.cppm
module;  // Start global module fragment

// Include headers here (before module declaration)
#include <vector>
#include <string>

export module mymodule;  // End global fragment, start module

// Use imported headers
export class MyClass {
    std::vector<std::string> data_;  // OK: vector included above
};
```

### Interoperability
```cpp
// Can import modules and include headers together

// main.cpp
#include <iostream>  // Traditional header
import mymodule;     // Modern module

int main() {
    // Use both
    std::cout << "Using both!\n";
    MyClass obj;
    
    return 0;
}
```

---

## Module Examples

### Simple Math Module
```cpp
// math.cppm
export module math;

import <cmath>;

export namespace math {
    double add(double a, double b) {
        return a + b;
    }
    
    double multiply(double a, double b) {
        return a * b;
    }
    
    double power(double base, double exp) {
        return std::pow(base, exp);
    }
}
```

### String Utilities Module
```cpp
// string_utils.cppm
export module string_utils;

import <string>;
import <algorithm>;
import <cctype>;

export namespace string_utils {
    std::string to_upper(std::string str) {
        std::transform(str.begin(), str.end(), str.begin(),
                      [](unsigned char c) { return std::toupper(c); });
        return str;
    }
    
    std::string to_lower(std::string str) {
        std::transform(str.begin(), str.end(), str.begin(),
                      [](unsigned char c) { return std::tolower(c); });
        return str;
    }
    
    bool starts_with(const std::string& str, const std::string& prefix) {
        return str.substr(0, prefix.size()) == prefix;
    }
}
```

### Container Module with Template
```cpp
// container.cppm
export module container;

import <vector>;
import <stdexcept>;

export template<typename T>
class SafeVector {
    std::vector<T> data_;
    
public:
    void push_back(const T& value) {
        data_.push_back(value);
    }
    
    T& at(size_t index) {
        if (index >= data_.size()) {
            throw std::out_of_range("Index out of range");
        }
        return data_[index];
    }
    
    size_t size() const {
        return data_.size();
    }
};
```

---

## Private Module Fragment

### Hiding Implementation
```cpp
// widget.cppm
export module widget;

import <string>;

export class Widget {
public:
    Widget(std::string name);
    void display();
    
private:
    class Impl;  // Forward declaration
    Impl* pimpl_;
};

// Private module fragment (implementation hidden)
module :private;

class Widget::Impl {
public:
    std::string name_;
    int internal_data_;
    
    void internal_helper() {
        // Complex implementation
    }
};

Widget::Widget(std::string name) 
    : pimpl_(new Impl{std::move(name), 0}) {}

void Widget::display() {
    pimpl_->internal_helper();
}
```

---

## Module Visibility

### What Gets Exported?
```cpp
export module visibility;

// Exported: Visible to importers
export int public_var = 10;
export void public_func() {}
export class PublicClass {};

// Not exported: Module-internal
int private_var = 20;
void private_func() {}
class PrivateClass {};

// Exported function using private types
export void process() {
    PrivateClass obj;  // OK: Can use private types internally
    private_func();    // OK: Can call private functions
}

// ERROR: Can't export function with private type in signature
// export void bad(PrivateClass obj);  // ERROR!
```

### Transitive Visibility
```cpp
// a.cppm
export module a;

export struct A {
    int value;
};

// b.cppm
export module b;
import a;

export struct B {
    A a_member;  // A is visible in B's interface
};

// main.cpp
import b;

int main() {
    B obj;
    obj.a_member.value = 42;  // OK: A is transitively visible
    
    return 0;
}
```

---

## Compilation Model

### Build Process
```
Traditional Headers:
┌──────────────────────────────────────────────────┐
│ main.cpp    lib.cpp      util.cpp                │
│    ↓           ↓             ↓                   │
│ Preprocess  Preprocess   Preprocess              │
│ (parse all  (parse all   (parse all              │
│  headers    headers      headers                 │
│  again)     again)       again)   ← Redundant!   │
│    ↓           ↓             ↓                   │
│ Compile     Compile      Compile                 │
│    ↓           ↓             ↓                   │
│    └───────────┴─────────────┘                   │
│                ↓                                 │
│              Link                                │
└──────────────────────────────────────────────────┘

Modules:
┌──────────────────────────────────────────────────┐
│ mymodule.cppm                                    │
│    ↓                                             │
│ Compile once → BMI (Binary Module Interface)     │
│                 ↓                                │
│    ┌────────────┼────────────┐                   │
│    ↓            ↓             ↓                  │
│ main.cpp    lib.cpp      util.cpp                │
│    ↓            ↓             ↓                  │
│ Compile     Compile       Compile                │
│ (import     (import       (import                │
│  BMI)       BMI)          BMI)    ← Fast!        │
│    ↓            ↓             ↓                  │
│    └────────────┴─────────────┘                  │
│                 ↓                                │
│               Link                               │
└──────────────────────────────────────────────────┘
```

---

## Best Practices

### 1. Module Names
```cpp
// GOOD: Descriptive, hierarchical
export module company.project.component;
export module myapp.utils.string;

// OK: Simple
export module math;

// BAD: Too generic
export module utils;
```

### 2. Organize with Partitions
```cpp
// Large module split into partitions
export module mylib;

export import :core;
export import :algorithms;
export import :io;

// Each partition in separate file
```

### 3. Export Judiciously
```cpp
// Only export what's needed
export module mymodule;

export class PublicAPI {
    // Public interface
};

// Internal helpers (not exported)
namespace internal {
    void helper() {}
}
```

### 4. Use Global Module Fragment for Legacy Headers
```cpp
module;

// Legacy headers that need macros/includes
#include <legacy_api.h>

export module wrapper;

// Wrap legacy code in modern module
```

---

## Migration from Headers

### Gradual Migration
```cpp
// Step 1: Keep headers, create modules alongside
// header.h
class Widget {
    // ...
};

// module.cppm
export module mymodule;
#include "header.h"  // Wrap existing header
export using ::Widget;

// Step 2: Importers use modules
import mymodule;

// Step 3: Eventually move implementation to module
```

### Conversion Example
```cpp
// Before (header):
// math.h
#ifndef MATH_H
#define MATH_H

namespace math {
    int add(int a, int b);
    int multiply(int a, int b);
}

#endif

// math.cpp
#include "math.h"

int math::add(int a, int b) { return a + b; }
int math::multiply(int a, int b) { return a * b; }

// After (module):
// math.cppm
export module math;

export namespace math {
    int add(int a, int b) { return a + b; }
    int multiply(int a, int b) { return a * b; }
}
```

---

## Compiler Support

### As of 2024
```
┌────────────────────────────────────────────────────────┐
│              Compiler Support Status                   │
├────────────────────────────────────────────────────────┤
│ Compiler      │ Version  │ Support                     │
├───────────────┼──────────┼─────────────────────────────┤
│ GCC           │ 11+      │ Good, improving             │
│ Clang         │ 16+      │ Good                        │
│ MSVC          │ 19.28+   │ Good                        │
│                                                        │
│ Build Systems:                                         │
│ • CMake 3.28+: Good module support                     │
│ • Ninja: Module support                                │
│ • Others: Varying support                              │
│                                                        │
│ Note: Module support is still evolving                 │
└────────────────────────────────────────────────────────┘
```

### CMake Example
```cmake
# CMakeLists.txt
cmake_minimum_required(VERSION 3.28)
project(ModuleExample CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Module target
add_library(mymodule)
target_sources(mymodule
    PUBLIC
        FILE_SET CXX_MODULES FILES
            mymodule.cppm
)

# Executable using module
add_executable(main main.cpp)
target_link_libraries(main PRIVATE mymodule)
```

---

## Common Pitfalls

### 1. Circular Dependencies
```cpp
// BAD: Circular module imports
// a.cppm
export module a;
import b;  // ERROR if b imports a

// Use forward declarations or redesign
```

### 2. Macro Issues
```cpp
// BAD: Macros don't cross module boundaries
export module mymodule;

#define MY_MACRO 42  // Not visible to importers!

// GOOD: Use constexpr
export constexpr int MY_CONSTANT = 42;
```

### 3. Forgetting export
```cpp
export module mymodule;

// BAD: Not exported
class MyClass {};

// GOOD: Exported
export class MyClass {};
```

---

## Future Directions

### Standard Library Modules (C++23)
```cpp
// Proposed for C++23
import std;           // All of std library
import std.io;        // I/O components
import std.containers;// Container components
```

### Module Macros (Proposed)
```cpp
// Better macro handling in modules
export module mymodule;

export {
    #define PUBLIC_MACRO 1
}
```

---

## Resources

- **C++20 Standard**: ISO/IEC 14882:2020
- **Compiler Documentation**: Check GCC, Clang, MSVC docs
- **Build System Support**: CMake, Ninja, others

---

## Complete Practical Example: Math Library Module

Here's a comprehensive example showing module definition, partitions, and usage:

```cpp
// ==================== math.cppm ====================
// Primary module interface

export module math;

export import :core;       // Re-export core partition
export import :advanced;   // Re-export advanced partition

export namespace math {
    constexpr double PI = 3.14159265358979323846;
    constexpr double E  = 2.71828182845904523536;
}

// ==================== math_core.cppm ====================
// Core partition

export module math:core;

export namespace math {
    // Basic operations
    inline int add(int a, int b) {
        return a + b;
    }
    
    inline int multiply(int a, int b) {
        return a * b;
    }
    
    inline double divide(double a, double b) {
        if (b == 0.0) {
            throw std::invalid_argument("Division by zero");
        }
        return a / b;
    }
}

// ==================== math_advanced.cppm ====================
// Advanced partition

export module math:advanced;

import <cmath>;

export namespace math {
    inline double power(double base, double exp) {
        return std::pow(base, exp);
    }
    
    inline double sqrt(double x) {
        return std::sqrt(x);
    }
    
    inline double sin(double x) {
        return std::sin(x);
    }
    
    inline double cos(double x) {
        return std::cos(x);
    }
}

// ==================== main.cpp ====================
// Using the module

import math;
import <iostream>;

int main() {
    std::cout << "=== Math Module Demo ===\n\n";
    
    // Use core functions
    std::cout << "Basic operations:\n";
    std::cout << "5 + 3 = " << math::add(5, 3) << "\n";
    std::cout << "5 * 3 = " << math::multiply(5, 3) << "\n";
    std::cout << "10 / 2 = " << math::divide(10.0, 2.0) << "\n\n";
    
    // Use advanced functions  
    std::cout << "Advanced operations:\n";
    std::cout << "2^8 = " << math::power(2, 8) << "\n";
    std::cout << "sqrt(16) = " << math::sqrt(16) << "\n";
    std::cout << "sin(PI/2) = " << math::sin(math::PI / 2) << "\n";
    std::cout << "cos(0) = " << math::cos(0) << "\n\n";
    
    // Use constants
    std::cout << "Constants:\n";
    std::cout << "PI = " << math::PI << "\n";
    std::cout << "E = " << math::E << "\n";
    
    return 0;
}
```

### Compilation (GCC example):
```bash
# Compile module partitions first
g++ -std=c++20 -fmodules-ts -c math_core.cppm
g++ -std=c++20 -fmodules-ts -c math_advanced.cppm

# Compile primary module interface
g++ -std=c++20 -fmodules-ts -c math.cppm

# Compile and link main
g++ -std=c++20 -fmodules-ts main.cpp math.o math_core.o math_advanced.o -o program

# Run
./program
```

### CMakeLists.txt (C++20 modules support):
```cmake
cmake_minimum_required(VERSION 3.28)
project(MathModule CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Module library
add_library(math_module)
target_sources(math_module
    PUBLIC
        FILE_SET CXX_MODULES FILES
            math_core.cppm
            math_advanced.cppm
            math.cppm
)

# Executable using module
add_executable(main main.cpp)
target_link_libraries(main PRIVATE math_module)
```

### Key Concepts Demonstrated:
1. **Module declaration**: `export module math;`
2. **Module partitions**: `:core`, `:advanced`
3. **Re-exporting**: `export import :core;`
4. **Importing modules**: `import math;`
5. **Importing headers as modules**: `import <iostream>;`
6. **Export namespace**: Organizing exported symbols
7. **Inline functions**: Definitions in module interface
8. **Constants**: Exported compile-time constants
9. **No header guards needed**: Built into modules
10. **Compilation order**: Partitions → primary → users

### Benefits Shown:
- ✅ **Faster compilation**: No redundant parsing
- ✅ **Better encapsulation**: Only exported symbols visible
- ✅ **No macro leakage**: Clean interfaces
- ✅ **Order independence**: Import order doesn't matter
- ✅ **Scalability**: Easy to split into partitions

### Module vs Header Comparison:

**Traditional Headers (math.h)**:
```cpp
#ifndef MATH_H
#define MATH_H

namespace math {
    int add(int a, int b);  // Declaration
    int multiply(int a, int b);
}

#endif

// math.cpp - separate implementation
namespace math {
    int add(int a, int b) { return a + b; }
    int multiply(int a, int b) { return a * b; }
}
```

**Modern Modules (math.cppm)**:
```cpp
export module math;

export namespace math {
    inline int add(int a, int b) { return a + b; }
    inline int multiply(int a, int b) { return a * b; }
}
// No header guards, no separate .cpp needed for inline functions
```

---

## Next Steps
- **Next**: [Coroutines (C++20) →](22_coroutines.md)
- **Previous**: [← Regular Expressions](20_regex.md)
- **Back to**: [Table of Contents](README.md)

---
*Part 21 of 22 - Modules (C++20)*

**Note**: Module support varies by compiler and build system. Check your toolchain's documentation for the latest features and syntax.

