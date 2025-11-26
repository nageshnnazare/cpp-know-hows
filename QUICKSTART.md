# Quick Start Guide

## ✅ All Patterns Tested and Working!

All 23 Gang of Four design patterns have been compiled and tested successfully.

## 🚀 Getting Started in 5 Minutes

### 1. Navigate to the Directory
```bash
cd /tmp/design_patterns_cpp
```

### 2. Run the Automated Tests
```bash
make test
```

This will:
- Compile all 23 patterns
- Run each one to verify it works
- Show pass/fail status for each

### 3. Try Individual Patterns

**Option A: Run a pre-compiled pattern**
```bash
make demo-01_singleton
```

**Option B: Compile and run manually**
```bash
cd creational
g++ -std=c++17 -o singleton 01_singleton.cpp
./singleton
```

### 4. Explore the Examples

Each file contains:
- 📝 Detailed explanations in comments
- 🎯 Multiple real-world examples (usually 3-4 per pattern)
- ✅ When to use / when NOT to use
- 📊 Pros and cons
- 💡 Key takeaways

## 📋 Quick Reference

### Test All Patterns
```bash
make test          # Silent test
make test-verbose  # See full output
```

### Test by Category
```bash
make test-creational   # Test 5 creational patterns
make test-structural   # Test 7 structural patterns
make test-behavioral   # Test 11 behavioral patterns
```

### Compile Specific Pattern
```bash
make creational/01_singleton
make structural/04_decorator
make behavioral/06_observer
```

### Run Specific Pattern
```bash
make demo-01_singleton              # Compile and run
make run-01_singleton               # Run if already compiled
```

### Other Useful Commands
```bash
make clean         # Remove all compiled binaries
make stats         # Show code statistics
make help          # Show all available commands
```

## 📚 Recommended Learning Path

### Day 1: Creational Patterns (Easy)
Start with these - they're the easiest to understand:
```bash
make demo-01_singleton        # One instance only
make demo-02_factory_method   # Create objects via subclasses
make demo-04_builder          # Step-by-step construction
```

### Day 2: Structural Patterns (Medium)
```bash
make demo-01_adapter          # Interface compatibility
make demo-04_decorator        # Add responsibilities
make demo-03_composite        # Tree structures
```

### Day 3: Behavioral Patterns (Advanced)
```bash
make demo-08_strategy         # Interchangeable algorithms
make demo-06_observer         # Notify dependents
make demo-02_command          # Undo/redo functionality
```

### Continue at Your Pace
Work through remaining patterns in any order. Each is self-contained.

## 🎯 Pattern Selector

**I need to...**

| Need | Pattern | File |
|------|---------|------|
| One instance only | Singleton | `creational/01_singleton.cpp` |
| Create without knowing type | Factory Method | `creational/02_factory_method.cpp` |
| Create related objects | Abstract Factory | `creational/03_abstract_factory.cpp` |
| Build complex objects | Builder | `creational/04_builder.cpp` |
| Clone objects | Prototype | `creational/05_prototype.cpp` |
| Fix incompatible interfaces | Adapter | `structural/01_adapter.cpp` |
| Decouple abstraction/impl | Bridge | `structural/02_bridge.cpp` |
| Tree structures | Composite | `structural/03_composite.cpp` |
| Add features dynamically | Decorator | `structural/04_decorator.cpp` |
| Simplify complex system | Facade | `structural/05_facade.cpp` |
| Save memory | Flyweight | `structural/06_flyweight.cpp` |
| Control access | Proxy | `structural/07_proxy.cpp` |
| Process chain | Chain of Responsibility | `behavioral/01_chain_of_responsibility.cpp` |
| Undo/redo | Command | `behavioral/02_command.cpp` |
| Traverse collections | Iterator | `behavioral/03_iterator.cpp` |
| Reduce coupling | Mediator | `behavioral/04_mediator.cpp` |
| Save/restore state | Memento | `behavioral/05_memento.cpp` |
| Notify multiple objects | Observer | `behavioral/06_observer.cpp` |
| Behavior by state | State | `behavioral/07_state.cpp` |
| Choose algorithm | Strategy | `behavioral/08_strategy.cpp` |
| Algorithm skeleton | Template Method | `behavioral/09_template_method.cpp` |
| Separate operations | Visitor | `behavioral/10_visitor.cpp` |
| Parse language | Interpreter | `behavioral/11_interpreter.cpp` |

## 💡 Pro Tips

1. **Read the file headers first** - They explain the pattern before diving into code
2. **Run the examples** - Seeing output helps understanding
3. **Modify the code** - Change parameters, add features
4. **Compare similar patterns** - e.g., Adapter vs Bridge, State vs Strategy
5. **Combine patterns** - Real apps use multiple patterns together
6. **Don't memorize** - Understand the problems they solve

## 🐛 Troubleshooting

### Compilation Error
```bash
# Make sure you have C++17 support
g++ --version  # Should be 7.0 or higher

# If issues persist, try C++14
g++ -std=c++14 -o pattern pattern.cpp
```

### Pattern Crashes
All patterns have been tested and work correctly. If one crashes:
```bash
# Recompile with debug symbols
g++ -std=c++17 -g -o pattern pattern.cpp
gdb ./pattern
```

### Make Command Not Found
```bash
# Install make
sudo apt install build-essential  # Ubuntu/Debian
sudo yum install make gcc-c++      # CentOS/RHEL
brew install make                  # macOS
```

## 📖 Next Steps

1. ✅ Run `make test` to verify everything works
2. 📚 Read `PATTERN_SUMMARY.md` for quick reference
3. 🎓 Follow the learning path above
4. 💻 Implement patterns in your own projects
5. 📝 Review `INDEX.md` for complete documentation

## 🎉 Success Metrics

After working through these examples, you should be able to:
- ✅ Recognize when to use each pattern
- ✅ Implement patterns in your own code
- ✅ Explain trade-offs between similar patterns
- ✅ Combine multiple patterns effectively
- ✅ Critique pattern usage in code reviews

## 🌟 Test Results

```
✅ Creational Patterns:  5/5  (100%)
✅ Structural Patterns:  7/7  (100%)
✅ Behavioral Patterns: 11/11 (100%)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
✅ Total: 23/23 patterns working perfectly!
```

---

**You're all set! Happy learning! 🚀**

Start with: `make demo-01_singleton`

