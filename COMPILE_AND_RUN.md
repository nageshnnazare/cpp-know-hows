# How to Compile and Run the Examples

## Compilation

Each pattern example is a standalone C++ file that can be compiled individually.

### Basic Compilation

```bash
g++ -std=c++17 -o pattern_name pattern_name.cpp
./pattern_name
```

### With Optimization

```bash
g++ -std=c++17 -O2 -o pattern_name pattern_name.cpp
./pattern_name
```

### With Debug Symbols

```bash
g++ -std=c++17 -g -o pattern_name pattern_name.cpp
gdb ./pattern_name
```

## Examples by Category

### Creational Patterns

```bash
cd creational

# Singleton
g++ -std=c++17 -o singleton 01_singleton.cpp && ./singleton

# Factory Method
g++ -std=c++17 -o factory_method 02_factory_method.cpp && ./factory_method

# Abstract Factory
g++ -std=c++17 -o abstract_factory 03_abstract_factory.cpp && ./abstract_factory

# Builder
g++ -std=c++17 -o builder 04_builder.cpp && ./builder

# Prototype
g++ -std=c++17 -o prototype 05_prototype.cpp && ./prototype
```

### Structural Patterns

```bash
cd structural

# Adapter
g++ -std=c++17 -o adapter 01_adapter.cpp && ./adapter

# Bridge
g++ -std=c++17 -o bridge 02_bridge.cpp && ./bridge

# Composite
g++ -std=c++17 -o composite 03_composite.cpp && ./composite

# Decorator
g++ -std=c++17 -o decorator 04_decorator.cpp && ./decorator

# Facade
g++ -std=c++17 -o facade 05_facade.cpp && ./facade

# Flyweight
g++ -std=c++17 -o flyweight 06_flyweight.cpp && ./flyweight

# Proxy
g++ -std=c++17 -o proxy 07_proxy.cpp && ./proxy
```

### Behavioral Patterns

```bash
cd behavioral

# Chain of Responsibility
g++ -std=c++17 -o chain 01_chain_of_responsibility.cpp && ./chain

# Command
g++ -std=c++17 -o command 02_command.cpp && ./command

# Iterator
g++ -std=c++17 -o iterator 03_iterator.cpp && ./iterator

# Mediator
g++ -std=c++17 -o mediator 04_mediator.cpp && ./mediator

# Memento
g++ -std=c++17 -o memento 05_memento.cpp && ./memento

# Observer
g++ -std=c++17 -o observer 06_observer.cpp && ./observer

# State
g++ -std=c++17 -o state 07_state.cpp && ./state

# Strategy
g++ -std=c++17 -o strategy 08_strategy.cpp && ./strategy

# Template Method
g++ -std=c++17 -o template_method 09_template_method.cpp && ./template_method

# Visitor
g++ -std=c++17 -o visitor 10_visitor.cpp && ./visitor

# Interpreter
g++ -std=c++17 -o interpreter 11_interpreter.cpp && ./interpreter
```

## Compile All at Once

### Bash Script

Create a file named `compile_all.sh`:

```bash
#!/bin/bash

echo "Compiling all patterns..."

for dir in creational structural behavioral; do
    echo ""
    echo "=== Compiling $dir patterns ==="
    cd $dir
    for file in *.cpp; do
        output="${file%.cpp}"
        echo "Compiling $file..."
        g++ -std=c++17 -O2 -o "$output" "$file"
        if [ $? -eq 0 ]; then
            echo "✓ $output compiled successfully"
        else
            echo "✗ $output compilation failed"
        fi
    done
    cd ..
done

echo ""
echo "Compilation complete!"
```

Then run:
```bash
chmod +x compile_all.sh
./compile_all.sh
```

## Requirements

- **Compiler**: GCC 7.0+ or Clang 5.0+ (with C++17 support)
- **Standard**: C++17 or later
- **OS**: Linux, macOS, Windows (with MinGW/MSYS2)

## Troubleshooting

### Error: "unrecognized command line option '-std=c++17'"

Your compiler is too old. Try:
- Installing a newer GCC: `sudo apt install g++-9` (or higher)
- Using C++14: `g++ -std=c++14 -o pattern_name pattern_name.cpp`

### Error: "undefined reference to 'std::__cxx11::basic_string'"

Add this flag: `-D_GLIBCXX_USE_CXX11_ABI=1`

### Windows Compilation

Using MinGW:
```cmd
g++ -std=c++17 -o pattern_name.exe pattern_name.cpp
pattern_name.exe
```

Using MSVC:
```cmd
cl /EHsc /std:c++17 pattern_name.cpp
pattern_name.exe
```

## Learning Path

Recommended order to study the patterns:

1. **Start with Creational**: Easiest to understand
   - Singleton → Factory Method → Builder → Prototype → Abstract Factory

2. **Move to Structural**: Learn about object composition
   - Adapter → Decorator → Composite → Facade → Proxy → Bridge → Flyweight

3. **Master Behavioral**: Most complex but powerful
   - Strategy → Observer → Command → State → Template Method → 
     Iterator → Chain of Responsibility → Mediator → Memento → Visitor → Interpreter

## Tips

1. **Read the comments**: Each file has detailed explanations
2. **Run the examples**: See the output to understand behavior
3. **Modify the code**: Try changing parameters and adding features
4. **Combine patterns**: Real applications use multiple patterns together
5. **Practice**: Implement patterns in your own projects

## Further Resources

- "Design Patterns: Elements of Reusable Object-Oriented Software" by Gang of Four
- "Head First Design Patterns" by Freeman & Robson
- refactoring.guru/design-patterns

