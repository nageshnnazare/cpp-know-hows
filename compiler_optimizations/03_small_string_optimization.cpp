/*
 * SMALL STRING OPTIMIZATION (SSO)
 * 
 * DEFINITION:
 * Optimization where small strings are stored directly in the string object
 * instead of allocating memory on the heap.
 * 
 * HOW IT WORKS:
 * - String object has internal buffer (typically 15-23 bytes)
 * - Small strings: stored in buffer (no heap allocation)
 * - Large strings: heap allocation
 * - Transparent to user
 * 
 * BENEFITS:
 * + No heap allocation for small strings
 * + Better cache locality
 * + Faster construction/destruction
 * + Reduced memory fragmentation
 * 
 * TYPICAL SIZES:
 * - GCC/libstdc++: 15 bytes
 * - Clang/libc++: 22 bytes
 * - MSVC: 15 bytes
 */

#include <iostream>
#include <string>
#include <chrono>
#include <vector>
#include <cstring>

using namespace std;
using namespace chrono;

// ============= String Size Analysis =============

void analyzeStringSize() {
    cout << "\n=== String Object Size Analysis ===" << endl;
    cout << "sizeof(std::string): " << sizeof(string) << " bytes" << endl;
    cout << "sizeof(char*): " << sizeof(char*) << " bytes" << endl;
    cout << "sizeof(size_t): " << sizeof(size_t) << " bytes" << endl;
    
    cout << "\nTypical std::string layout:" << endl;
    cout << "• Pointer to data (8 bytes on 64-bit)" << endl;
    cout << "• Size (8 bytes)" << endl;
    cout << "• Capacity or inline buffer (8-16 bytes)" << endl;
    cout << "Total: 24-32 bytes" << endl;
}

// ============= Small vs Large String Behavior =============

void demonstrateSSO() {
    cout << "\n=== SSO Demonstration ===" << endl;
    
    // Small strings (likely SSO)
    string small1 = "Hi";           // 2 bytes
    string small2 = "Hello";        // 5 bytes
    string small3 = "Small String"; // 12 bytes
    
    // Medium string (might use SSO)
    string medium = "This is medium";  // 14 bytes
    
    // Large string (definitely heap-allocated)
    string large = "This is a definitely large string that won't fit in SSO buffer"; // 63 bytes
    
    cout << "\nString lengths:" << endl;
    cout << "small1:  " << small1.length() << " bytes - \"" << small1 << "\"" << endl;
    cout << "small2:  " << small2.length() << " bytes - \"" << small2 << "\"" << endl;
    cout << "small3:  " << small3.length() << " bytes - \"" << small3 << "\"" << endl;
    cout << "medium:  " << medium.length() << " bytes - \"" << medium << "\"" << endl;
    cout << "large:   " << large.length() << " bytes" << endl;
    
    cout << "\nSSO behavior (implementation-dependent):" << endl;
    cout << "• Small strings: Likely stored inline (no allocation)" << endl;
    cout << "• Medium string: May use SSO depending on implementation" << endl;
    cout << "• Large string: Heap-allocated" << endl;
}

// ============= Performance Comparison =============

void performanceTest() {
    cout << "\n=== Performance Comparison ===" << endl;
    
    const int iterations = 1000000;
    
    // Test 1: Small strings (SSO)
    auto start1 = high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        string s = "Small";  // SSO - no allocation
        volatile const char* p = s.c_str();  // Prevent optimization
        (void)p;
    }
    auto end1 = high_resolution_clock::now();
    auto duration1 = duration_cast<microseconds>(end1 - start1).count();
    
    // Test 2: Large strings (heap allocation)
    auto start2 = high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        string s = "This is a large string that requires heap allocation"; // Heap
        volatile const char* p = s.c_str();
        (void)p;
    }
    auto end2 = high_resolution_clock::now();
    auto duration2 = duration_cast<microseconds>(end2 - start2).count();
    
    cout << "\nCreating " << iterations << " strings:" << endl;
    cout << "Small strings (SSO):  " << duration1 << " μs" << endl;
    cout << "Large strings (Heap): " << duration2 << " μs" << endl;
    cout << "Speedup with SSO: " << (double)duration2 / duration1 << "x" << endl;
}

// ============= Copy Performance =============

void copyPerformanceTest() {
    cout << "\n=== Copy Performance ===" << endl;
    
    const int iterations = 1000000;
    
    // Small string copies
    string small_src = "Small";
    auto start1 = high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        string copy = small_src;  // Fast copy (inline buffer)
        volatile const char* p = copy.c_str();
        (void)p;
    }
    auto end1 = high_resolution_clock::now();
    auto duration1 = duration_cast<microseconds>(end1 - start1).count();
    
    // Large string copies
    string large_src = "This is a large string that requires heap allocation and copying";
    auto start2 = high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        string copy = large_src;  // Heap allocation + memcpy
        volatile const char* p = copy.c_str();
        (void)p;
    }
    auto end2 = high_resolution_clock::now();
    auto duration2 = duration_cast<microseconds>(end2 - start2).count();
    
    cout << "\nCopying " << iterations << " strings:" << endl;
    cout << "Small string copies: " << duration1 << " μs" << endl;
    cout << "Large string copies: " << duration2 << " μs" << endl;
    cout << "Speedup: " << (double)duration2 / duration1 << "x" << endl;
}

// ============= Vector of Strings =============

void vectorStringTest() {
    cout << "\n=== Vector of Strings ===" << endl;
    
    vector<string> small_strings;
    vector<string> large_strings;
    
    const int count = 1000;
    
    // Small strings
    auto start1 = high_resolution_clock::now();
    for (int i = 0; i < count; ++i) {
        small_strings.push_back("Small" + to_string(i));
    }
    auto end1 = high_resolution_clock::now();
    auto duration1 = duration_cast<microseconds>(end1 - start1).count();
    
    // Large strings
    auto start2 = high_resolution_clock::now();
    for (int i = 0; i < count; ++i) {
        large_strings.push_back("This is a large string number " + to_string(i) + " with extra content");
    }
    auto end2 = high_resolution_clock::now();
    auto duration2 = duration_cast<microseconds>(end2 - start2).count();
    
    cout << "\nCreating vector with " << count << " strings:" << endl;
    cout << "Small strings: " << duration1 << " μs" << endl;
    cout << "Large strings: " << duration2 << " μs" << endl;
}

// ============= Practical Guidelines =============

void practicalGuidelines() {
    cout << "\n=== Practical Guidelines ===" << endl;
    
    cout << "\nWhen SSO Helps:" << endl;
    cout << "✓ Short identifiers, names, labels" << endl;
    cout << "✓ Configuration keys" << endl;
    cout << "✓ Enum-like string constants" << endl;
    cout << "✓ File extensions" << endl;
    cout << "✓ Status messages" << endl;
    
    cout << "\nExamples:" << endl;
    string id = "user123";           // Likely SSO
    string status = "OK";             // Likely SSO
    string ext = ".cpp";              // Likely SSO
    string key = "config.port";       // Likely SSO
    
    cout << "id: \"" << id << "\" (" << id.length() << " bytes)" << endl;
    cout << "status: \"" << status << "\" (" << status.length() << " bytes)" << endl;
    cout << "ext: \"" << ext << "\" (" << ext.length() << " bytes)" << endl;
    cout << "key: \"" << key << "\" (" << key.length() << " bytes)" << endl;
    
    cout << "\nAll likely use SSO - zero heap allocations!" << endl;
}

// ============= Memory Locality Benefits =============

void memoryLocalityDemo() {
    cout << "\n=== Memory Locality Benefits ===" << endl;
    
    struct Record {
        string name;
        int value;
        string type;
    };
    
    vector<Record> records;
    
    // With SSO, small strings are stored inline
    // Better cache locality when iterating
    records.push_back({"Item1", 100, "A"});
    records.push_back({"Item2", 200, "B"});
    records.push_back({"Item3", 300, "C"});
    
    cout << "\nWith SSO:" << endl;
    cout << "• Strings stored inline in Record objects" << endl;
    cout << "• All data in contiguous memory" << endl;
    cout << "• Better CPU cache utilization" << endl;
    cout << "• Fewer cache misses when iterating" << endl;
    
    cout << "\nWithout SSO:" << endl;
    cout << "• Strings point to heap allocations" << endl;
    cout << "• Data scattered in memory" << endl;
    cout << "• More cache misses" << endl;
    cout << "• Slower iteration" << endl;
}

int main() {
    cout << "=== SMALL STRING OPTIMIZATION (SSO) ===" << endl;
    
    analyzeStringSize();
    demonstrateSSO();
    performanceTest();
    copyPerformanceTest();
    vectorStringTest();
    practicalGuidelines();
    memoryLocalityDemo();
    
    cout << "\n\n=== KEY TAKEAWAYS ===" << endl;
    cout << "1. SSO stores small strings inline (no heap allocation)" << endl;
    cout << "2. Typical threshold: 15-23 bytes" << endl;
    cout << "3. 2-10x faster for small strings" << endl;
    cout << "4. Better cache locality" << endl;
    cout << "5. Transparent to user" << endl;
    
    cout << "\nSSO Implementation:" << endl;
    cout << "• String object has internal buffer" << endl;
    cout << "• Small: use buffer" << endl;
    cout << "• Large: heap allocation" << endl;
    cout << "• Automatic switching" << endl;
    
    cout << "\nBenefits:" << endl;
    cout << "✓ No allocation for small strings" << endl;
    cout << "✓ Faster construction/destruction" << endl;
    cout << "✓ Better cache performance" << endl;
    cout << "✓ Reduced memory fragmentation" << endl;
    cout << "✓ Lower overhead" << endl;
    
    cout << "\nOptimization Tips:" << endl;
    cout << "• Keep strings short when possible" << endl;
    cout << "• Use string_view for read-only" << endl;
    cout << "• Reserve() for large strings" << endl;
    cout << "• Consider fixed-size buffers for known sizes" << endl;
    
    cout << "\nWhen SSO Matters Most:" << endl;
    cout << "• High-frequency string operations" << endl;
    cout << "• Many short-lived strings" << endl;
    cout << "• Cache-sensitive code" << endl;
    cout << "• Low-latency applications" << endl;
    cout << "• Embedded systems (limited heap)" << endl;
    
    return 0;
}

