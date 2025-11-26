# C++ Iterator Types - Complete Guide

## 📍 Location
`behavioral/12_iterator_types.cpp`

## 🎯 Purpose

This comprehensive guide complements the Iterator design pattern by covering all C++ Standard Library iterator categories. While `03_iterator.cpp` demonstrates the Iterator *pattern*, this file explains the different *types* of iterators available in C++.

## 📚 What's Covered

### 1. **Input Iterator**
- Read-only, single-pass
- Forward movement only
- **Example:** `istream_iterator`
- **Use Case:** Reading from input streams

### 2. **Output Iterator**
- Write-only, single-pass
- Forward movement only
- **Example:** `ostream_iterator`, `back_inserter`
- **Use Case:** Writing to output streams, inserting into containers

### 3. **Forward Iterator**
- Read/write, multi-pass
- Forward movement only
- **Example:** `forward_list::iterator`
- **Use Case:** Single-direction traversal with multiple passes

### 4. **Bidirectional Iterator**
- Read/write, multi-pass
- Forward and backward movement
- **Example:** `list::iterator`, `set::iterator`, `map::iterator`
- **Use Case:** Two-way traversal

### 5. **Random Access Iterator**
- Full arithmetic operations
- Jump to any position in O(1)
- **Example:** `vector::iterator`, `deque::iterator`
- **Use Case:** Fast random access, sorting

### 6. **Contiguous Iterator** (C++20)
- Random access + contiguous memory
- Guaranteed sequential storage
- **Example:** `vector::iterator`, `array::iterator`, pointers
- **Use Case:** Interfacing with C APIs, cache-friendly access

## 🔗 Iterator Hierarchy

```
Input Iterator ────┐
                   ├──→ Forward Iterator
Output Iterator ───┘         ↓
                    Bidirectional Iterator
                              ↓
                    Random Access Iterator
                              ↓
                    Contiguous Iterator (C++20)
```

Each level inherits capabilities from the previous level.

## 📊 Container Iterator Types

| Container | Iterator Type |
|-----------|--------------|
| `vector` | Random Access (Contiguous) |
| `array` | Random Access (Contiguous) |
| `string` | Random Access (Contiguous) |
| `deque` | Random Access |
| `list` | Bidirectional |
| `set`, `map` | Bidirectional |
| `multiset`, `multimap` | Bidirectional |
| `forward_list` | Forward |
| `unordered_set`, `unordered_map` | Forward |

## 🚀 Quick Start

### Compile and Run
```bash
cd /tmp/design_patterns_cpp/behavioral
g++ -std=c++17 -o iterator_types 12_iterator_types.cpp
./iterator_types
```

### Using Make
```bash
make demo-12_iterator_types
```

## 💡 Key Concepts Demonstrated

### 1. Iterator Categories
Each section demonstrates a different iterator category with:
- **Capabilities:** What operations are supported
- **Limitations:** What operations are NOT supported
- **Examples:** Real code showing usage
- **Use Cases:** When to use this iterator type

### 2. Container Selection
Learn which container to choose based on iterator requirements:
- Need sorting? → Use `vector` (random access required)
- Need fast insert/delete in middle? → Use `list` (bidirectional sufficient)
- Need ordered unique elements? → Use `set` (bidirectional)

### 3. Algorithm Requirements
Understand what iterators algorithms need:
- `sort()` → Requires Random Access Iterator
- `reverse()` → Requires Bidirectional Iterator
- `find()` → Works with Input Iterator
- `copy()` → Requires Input + Output Iterator

### 4. Performance Implications
- **Random Access:** O(1) for `advance()` and `distance()`
- **Bidirectional:** O(n) for `advance()` and `distance()`
- **Contiguous:** Cache-friendly, optimal performance

## 🎓 Learning Outcomes

After studying this file, you will understand:

1. ✅ **Six iterator categories** and their capabilities
2. ✅ **Container-iterator relationships** (which container provides which iterator)
3. ✅ **Algorithm requirements** (which algorithms need which iterators)
4. ✅ **Performance characteristics** of different iterator types
5. ✅ **When to use each type** in your own code
6. ✅ **Iterator traits** and compile-time information
7. ✅ **Best practices** for working with iterators

## 📖 Code Examples Included

### Input Iterator Examples
- Reading from `istream_iterator`
- Using algorithms with input iterators
- Single-pass iteration limitations

### Output Iterator Examples
- Writing with `ostream_iterator`
- Using `back_inserter` and `inserter`
- Insert vs append operations

### Forward Iterator Examples
- Using `forward_list`
- Multi-pass capability demonstration
- Modifying elements while iterating

### Bidirectional Iterator Examples
- Forward and backward traversal with `list`
- Using `set` with bidirectional iteration
- `reverse()` algorithm demonstration

### Random Access Iterator Examples
- Jump operations with `vector`
- Iterator arithmetic
- Sorting and binary search
- Subscript operator usage

### Contiguous Iterator Examples
- Memory layout verification
- Pointer arithmetic guarantees
- Interface with C APIs
- Cache-friendly access patterns

## 🔍 Comparison with Iterator Pattern

| Aspect | Iterator Pattern (03_iterator.cpp) | Iterator Types (12_iterator_types.cpp) |
|--------|-----------------------------------|----------------------------------------|
| **Focus** | Design pattern for custom collections | C++ Standard Library iterator categories |
| **Scope** | How to implement iterator pattern | Understanding existing iterator types |
| **Examples** | Custom BookCollection, Tree, Feed | STL containers (vector, list, set, etc.) |
| **Purpose** | Learn pattern implementation | Learn STL iterator hierarchy |

**Both are complementary!**
- Study `03_iterator.cpp` to learn how to implement the pattern
- Study `12_iterator_types.cpp` to learn C++ STL iterator types

## 🛠️ Practical Applications

### 1. Choosing Algorithms
```cpp
// sort() needs random access
vector<int> v = {3, 1, 4, 1, 5};
sort(v.begin(), v.end());  // ✓ Works

list<int> l = {3, 1, 4, 1, 5};
// sort(l.begin(), l.end());  // ✗ Compile error
l.sort();  // ✓ Use list's member function
```

### 2. Container Selection
```cpp
// Need fast sorting? Use vector
vector<int> data;  // Random access iterator

// Need fast insert/delete? Use list
list<int> data;    // Bidirectional iterator

// Need forward-only? Use forward_list
forward_list<int> data;  // Forward iterator
```

### 3. Generic Algorithms
```cpp
// advance() adapts to iterator type
auto it = vec.begin();
advance(it, 5);  // O(1) for random access

auto lit = list.begin();
advance(lit, 5);  // O(n) for bidirectional
```

## 📈 Performance Characteristics

| Operation | Random Access | Bidirectional | Forward |
|-----------|--------------|---------------|---------|
| `++it` | O(1) | O(1) | O(1) |
| `--it` | O(1) | O(1) | ❌ |
| `it + n` | O(1) | ❌ | ❌ |
| `advance(it, n)` | O(1) | O(n) | O(n) |
| `distance(it1, it2)` | O(1) | O(n) | O(n) |

## 🎯 Best Practices Covered

1. **Use `auto` for iterator types**
2. **Prefer range-based for loops** when possible
3. **Use algorithms** instead of manual loops
4. **Don't invalidate iterators** (understand container-specific rules)
5. **Choose right container** for your iterator needs
6. **Understand algorithm requirements** before using them

## 🔗 Related Files

- `03_iterator.cpp` - Iterator design pattern implementation
- `structural/03_composite.cpp` - Tree traversal with iterators
- `behavioral/10_visitor.cpp` - Traversing object structures

## 📝 Test Status

✅ **Compiles:** Successfully with C++17  
✅ **Runs:** Completes without errors  
✅ **Tested:** Part of automated test suite

## 🚀 Quick Demo

```bash
# Run the comprehensive demonstration
make demo-12_iterator_types

# Or compile and run manually
cd behavioral
g++ -std=c++17 -o iterator_types 12_iterator_types.cpp
./iterator_types
```

## 📚 Further Reading

### Books
- "Effective STL" by Scott Meyers - Chapter on iterators
- "C++ Standard Library" by Nicolai Josuttis
- "The C++ Programming Language" by Bjarne Stroustrup

### Online Resources
- cppreference.com - Iterator library documentation
- cplusplus.com - Iterator categories tutorial

## ✨ Summary

This file is a **comprehensive reference** for understanding C++ iterator types. It bridges the gap between the Iterator design pattern and the Standard Library's implementation. Whether you're choosing a container, understanding algorithm requirements, or optimizing performance, this guide provides the knowledge you need.

**Total Examples:** 30+ practical code demonstrations  
**Total Concepts:** 6 iterator categories + traits + best practices  
**Lines of Code:** ~600 lines with extensive comments

---

**Part of:** Design Patterns in C++ - Complete Collection  
**Category:** Behavioral Patterns (Bonus Material)  
**Status:** Production Ready ✅

