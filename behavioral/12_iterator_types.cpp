/*
 * C++ ITERATOR TYPES - Comprehensive Guide
 * 
 * PURPOSE:
 * Demonstrates all 5 iterator categories in C++ Standard Library
 * (6 categories if including C++20's Contiguous Iterator)
 * 
 * ITERATOR CATEGORIES (in order of capability):
 * 1. Input Iterator       - Read once, forward only (e.g., istream_iterator)
 * 2. Output Iterator      - Write once, forward only (e.g., ostream_iterator)
 * 3. Forward Iterator     - Read/write multiple times, forward only
 * 4. Bidirectional Iterator - Forward + backward (e.g., list, set, map)
 * 5. Random Access Iterator - Jump to any position (e.g., vector, deque, array)
 * 6. Contiguous Iterator  - Random access + contiguous memory (C++20)
 * 
 * HIERARCHY:
 * Input/Output Iterator (base capabilities)
 *        ↓
 * Forward Iterator (can read/write multiple times)
 *        ↓
 * Bidirectional Iterator (can move backward)
 *        ↓
 * Random Access Iterator (can jump)
 *        ↓
 * Contiguous Iterator (guaranteed contiguous memory - C++20)
 * 
 * OPERATIONS BY CATEGORY:
 * 
 * Input Iterator:
 *   - *it (read), ++it, it++, ==, !=
 * 
 * Output Iterator:
 *   - *it (write), ++it, it++
 * 
 * Forward Iterator:
 *   - All Input Iterator operations
 *   - Can make multiple passes
 *   - Default constructible
 * 
 * Bidirectional Iterator:
 *   - All Forward Iterator operations
 *   - --it, it--
 * 
 * Random Access Iterator:
 *   - All Bidirectional Iterator operations
 *   - it + n, it - n, it += n, it -= n
 *   - it[n], it1 - it2
 *   - <, >, <=, >=
 * 
 * Contiguous Iterator (C++20):
 *   - All Random Access Iterator operations
 *   - Elements stored in contiguous memory
 *   - &*(it + n) == &*it + n
 */

#include <iostream>
#include <vector>
#include <list>
#include <set>
#include <deque>
#include <forward_list>
#include <iterator>
#include <algorithm>
#include <sstream>
#include <fstream>

using namespace std;

// ============= 1. INPUT ITERATOR =============
// Can read, move forward (single pass)

void demonstrateInputIterator() {
    cout << "\n=== 1. INPUT ITERATOR ===" << endl;
    cout << "Read-only, forward-only, single-pass" << endl;
    cout << "Examples: istream_iterator, find() return value" << endl;
    
    // Example 1: istream_iterator (reading from input stream)
    cout << "\n[Reading from string stream]" << endl;
    istringstream iss("10 20 30 40 50");
    istream_iterator<int> inputIt(iss);
    istream_iterator<int> endIt;
    
    cout << "Values: ";
    while (inputIt != endIt) {
        cout << *inputIt << " ";  // Read
        ++inputIt;                // Move forward
    }
    cout << endl;
    
    // Example 2: Using algorithms with input iterators
    cout << "\n[Using with algorithms]" << endl;
    istringstream iss2("5 15 25 35 45");
    istream_iterator<int> start(iss2);
    istream_iterator<int> end;
    
    int sum = 0;
    for_each(start, end, [&sum](int n) {
        sum += n;
        cout << n << " ";
    });
    cout << "\nSum: " << sum << endl;
    
    // Characteristics
    cout << "\n[Input Iterator Characteristics]" << endl;
    cout << "✓ Can read: *it" << endl;
    cout << "✓ Can increment: ++it, it++" << endl;
    cout << "✓ Can compare: it1 == it2, it1 != it2" << endl;
    cout << "✗ Cannot write" << endl;
    cout << "✗ Cannot go backward" << endl;
    cout << "✗ Cannot make multiple passes" << endl;
}

// ============= 2. OUTPUT ITERATOR =============
// Can write, move forward (single pass)

void demonstrateOutputIterator() {
    cout << "\n\n=== 2. OUTPUT ITERATOR ===" << endl;
    cout << "Write-only, forward-only, single-pass" << endl;
    cout << "Examples: ostream_iterator, insert_iterator, back_inserter" << endl;
    
    // Example 1: ostream_iterator (writing to output stream)
    cout << "\n[Writing to output stream]" << endl;
    vector<int> numbers = {1, 2, 3, 4, 5};
    cout << "Numbers: ";
    copy(numbers.begin(), numbers.end(), 
         ostream_iterator<int>(cout, " "));
    cout << endl;
    
    // Example 2: back_inserter (inserting at end)
    cout << "\n[Using back_inserter]" << endl;
    vector<int> source = {10, 20, 30};
    vector<int> destination;
    copy(source.begin(), source.end(), back_inserter(destination));
    cout << "Copied: ";
    for (int n : destination) cout << n << " ";
    cout << endl;
    
    // Example 3: inserter (inserting at position)
    cout << "\n[Using inserter]" << endl;
    vector<int> vec = {1, 2, 5, 6};
    vector<int> toInsert = {3, 4};
    copy(toInsert.begin(), toInsert.end(), 
         inserter(vec, vec.begin() + 2));
    cout << "After insert: ";
    for (int n : vec) cout << n << " ";
    cout << endl;
    
    // Characteristics
    cout << "\n[Output Iterator Characteristics]" << endl;
    cout << "✓ Can write: *it = value" << endl;
    cout << "✓ Can increment: ++it, it++" << endl;
    cout << "✗ Cannot read" << endl;
    cout << "✗ Cannot compare" << endl;
    cout << "✗ Cannot go backward" << endl;
}

// ============= 3. FORWARD ITERATOR =============
// Can read/write, move forward (multi-pass)

void demonstrateForwardIterator() {
    cout << "\n\n=== 3. FORWARD ITERATOR ===" << endl;
    cout << "Read/write, forward-only, multi-pass" << endl;
    cout << "Examples: forward_list iterators, unordered containers" << endl;
    
    // Example 1: forward_list (singly-linked list)
    cout << "\n[Using forward_list]" << endl;
    forward_list<int> flist = {10, 20, 30, 40, 50};
    
    cout << "Original: ";
    for (auto it = flist.begin(); it != flist.end(); ++it) {
        cout << *it << " ";
    }
    cout << endl;
    
    // Modify elements (multi-pass capability)
    cout << "Doubling all values..." << endl;
    for (auto it = flist.begin(); it != flist.end(); ++it) {
        *it *= 2;  // Can write
    }
    
    cout << "Modified: ";
    for (auto it = flist.begin(); it != flist.end(); ++it) {
        cout << *it << " ";  // Can read again (multi-pass)
    }
    cout << endl;
    
    // Example 2: Multiple passes
    cout << "\n[Multiple passes demonstration]" << endl;
    forward_list<string> words = {"Hello", "World", "C++", "Iterator"};
    
    // First pass: count
    int count = 0;
    for (auto it = words.begin(); it != words.end(); ++it) {
        count++;
    }
    cout << "Count: " << count << endl;
    
    // Second pass: print
    cout << "Words: ";
    for (auto it = words.begin(); it != words.end(); ++it) {
        cout << *it << " ";
    }
    cout << endl;
    
    // Characteristics
    cout << "\n[Forward Iterator Characteristics]" << endl;
    cout << "✓ Can read: *it" << endl;
    cout << "✓ Can write: *it = value" << endl;
    cout << "✓ Can increment: ++it, it++" << endl;
    cout << "✓ Can compare: it1 == it2, it1 != it2" << endl;
    cout << "✓ Multi-pass (can iterate multiple times)" << endl;
    cout << "✗ Cannot go backward" << endl;
    cout << "✗ Cannot jump to arbitrary position" << endl;
}

// ============= 4. BIDIRECTIONAL ITERATOR =============
// Forward + backward movement

void demonstrateBidirectionalIterator() {
    cout << "\n\n=== 4. BIDIRECTIONAL ITERATOR ===" << endl;
    cout << "Read/write, forward and backward, multi-pass" << endl;
    cout << "Examples: list, set, map iterators" << endl;
    
    // Example 1: list (doubly-linked list)
    cout << "\n[Using std::list]" << endl;
    list<int> lst = {10, 20, 30, 40, 50};
    
    cout << "Forward: ";
    for (auto it = lst.begin(); it != lst.end(); ++it) {
        cout << *it << " ";
    }
    cout << endl;
    
    cout << "Backward: ";
    for (auto it = --lst.end(); it != --lst.begin(); --it) {
        cout << *it << " ";
    }
    cout << endl;
    
    // Example 2: set (sorted, unique elements)
    cout << "\n[Using std::set]" << endl;
    set<string> names = {"Alice", "Charlie", "Bob", "Diana"};
    
    cout << "Ascending: ";
    for (auto it = names.begin(); it != names.end(); ++it) {
        cout << *it << " ";
    }
    cout << endl;
    
    cout << "Descending: ";
    for (auto it = names.rbegin(); it != names.rend(); ++it) {
        cout << *it << " ";
    }
    cout << endl;
    
    // Example 3: Reverse algorithm
    cout << "\n[Using reverse algorithm]" << endl;
    list<int> nums = {1, 2, 3, 4, 5};
    cout << "Before: ";
    for (int n : nums) cout << n << " ";
    cout << endl;
    
    reverse(nums.begin(), nums.end());  // Requires bidirectional iterator
    cout << "After:  ";
    for (int n : nums) cout << n << " ";
    cout << endl;
    
    // Characteristics
    cout << "\n[Bidirectional Iterator Characteristics]" << endl;
    cout << "✓ All Forward Iterator capabilities" << endl;
    cout << "✓ Can decrement: --it, it--" << endl;
    cout << "✓ Can move backward" << endl;
    cout << "✗ Cannot jump to arbitrary position" << endl;
}

// ============= 5. RANDOM ACCESS ITERATOR =============
// Jump to any position in constant time

void demonstrateRandomAccessIterator() {
    cout << "\n\n=== 5. RANDOM ACCESS ITERATOR ===" << endl;
    cout << "Full capabilities: jump, arithmetic, comparison" << endl;
    cout << "Examples: vector, deque, array iterators, pointers" << endl;
    
    // Example 1: vector
    cout << "\n[Using std::vector]" << endl;
    vector<int> vec = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    
    auto it = vec.begin();
    cout << "it[0] = " << it[0] << endl;           // Subscript
    cout << "it[5] = " << it[5] << endl;
    cout << "*(it + 3) = " << *(it + 3) << endl;   // Addition
    
    // Iterator arithmetic
    cout << "\n[Iterator Arithmetic]" << endl;
    auto start = vec.begin();
    auto end = vec.end();
    cout << "Distance: " << (end - start) << endl;
    
    auto mid = start + (end - start) / 2;
    cout << "Middle element: " << *mid << endl;
    
    // Example 2: Sorting (requires random access)
    cout << "\n[Sorting with random access iterator]" << endl;
    vector<int> unsorted = {64, 34, 25, 12, 22, 11, 90};
    cout << "Before: ";
    for (int n : unsorted) cout << n << " ";
    cout << endl;
    
    sort(unsorted.begin(), unsorted.end());  // Requires random access
    cout << "After:  ";
    for (int n : unsorted) cout << n << " ";
    cout << endl;
    
    // Example 3: Binary search
    cout << "\n[Binary search (requires random access)]" << endl;
    vector<int> sorted = {10, 20, 30, 40, 50, 60, 70, 80, 90};
    bool found = binary_search(sorted.begin(), sorted.end(), 50);
    cout << "Found 50: " << (found ? "Yes" : "No") << endl;
    
    // Example 4: Comparison operators
    cout << "\n[Iterator Comparison]" << endl;
    auto first = vec.begin();
    auto fifth = vec.begin() + 4;
    cout << "first < fifth: " << (first < fifth) << endl;
    cout << "fifth > first: " << (fifth > first) << endl;
    
    // Characteristics
    cout << "\n[Random Access Iterator Characteristics]" << endl;
    cout << "✓ All Bidirectional Iterator capabilities" << endl;
    cout << "✓ Arithmetic: it + n, it - n, it += n, it -= n" << endl;
    cout << "✓ Subscript: it[n]" << endl;
    cout << "✓ Distance: it1 - it2" << endl;
    cout << "✓ Comparison: <, >, <=, >=" << endl;
    cout << "✓ O(1) access to any element" << endl;
}

// ============= 6. CONTIGUOUS ITERATOR (C++20) =============
// Random access + guaranteed contiguous memory

void demonstrateContiguousIterator() {
    cout << "\n\n=== 6. CONTIGUOUS ITERATOR (C++20) ===" << endl;
    cout << "Random access + elements in contiguous memory" << endl;
    cout << "Examples: vector, array, string iterators, pointers" << endl;
    
    // Example 1: vector (contiguous)
    cout << "\n[Vector - Contiguous Memory]" << endl;
    vector<int> vec = {1, 2, 3, 4, 5};
    
    auto it = vec.begin();
    cout << "Address of it[0]: " << &*it << endl;
    cout << "Address of it[1]: " << &*(it + 1) << endl;
    cout << "Address of it[2]: " << &*(it + 2) << endl;
    
    // Verify contiguous
    cout << "\nVerifying contiguous property:" << endl;
    cout << "&*(it + 1) - &*it = " << (&*(it + 1) - &*it) << " (should be 1)" << endl;
    cout << "&*(it + 2) - &*it = " << (&*(it + 2) - &*it) << " (should be 2)" << endl;
    
    // Example 2: Array
    cout << "\n[Array - Contiguous Memory]" << endl;
    int arr[] = {10, 20, 30, 40, 50};
    int* ptr = arr;  // Pointers are contiguous iterators
    
    cout << "Using pointer arithmetic:" << endl;
    for (int i = 0; i < 5; i++) {
        cout << "ptr[" << i << "] = " << ptr[i] 
             << " at address " << (ptr + i) << endl;
    }
    
    // Example 3: Why contiguous matters
    cout << "\n[Why Contiguous Matters]" << endl;
    vector<int> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    // Can use C APIs that expect contiguous memory
    cout << "Can pass to C functions: data.data()" << endl;
    cout << "Cache-friendly: elements in sequential memory" << endl;
    cout << "Pointer arithmetic guaranteed to work" << endl;
    
    // Characteristics
    cout << "\n[Contiguous Iterator Characteristics]" << endl;
    cout << "✓ All Random Access Iterator capabilities" << endl;
    cout << "✓ Elements stored in contiguous memory" << endl;
    cout << "✓ &*(it + n) == &*it + n (guaranteed)" << endl;
    cout << "✓ Can convert to pointers safely" << endl;
    cout << "✓ Cache-friendly access patterns" << endl;
}

// ============= COMPARISON OF CONTAINERS AND THEIR ITERATORS =============

void compareContainerIterators() {
    cout << "\n\n=== CONTAINER ITERATOR COMPARISON ===" << endl;
    
    cout << "\n┌────────────────────┬──────────────────────────┐" << endl;
    cout << "│ Container          │ Iterator Type            │" << endl;
    cout << "├────────────────────┼──────────────────────────┤" << endl;
    cout << "│ vector             │ Random Access (Contig.)  │" << endl;
    cout << "│ deque              │ Random Access            │" << endl;
    cout << "│ array              │ Random Access (Contig.)  │" << endl;
    cout << "│ string             │ Random Access (Contig.)  │" << endl;
    cout << "│ list               │ Bidirectional            │" << endl;
    cout << "│ set, map           │ Bidirectional            │" << endl;
    cout << "│ multiset, multimap │ Bidirectional            │" << endl;
    cout << "│ forward_list       │ Forward                  │" << endl;
    cout << "│ unordered_set/map  │ Forward                  │" << endl;
    cout << "│ istream_iterator   │ Input                    │" << endl;
    cout << "│ ostream_iterator   │ Output                   │" << endl;
    cout << "└────────────────────┴──────────────────────────┘" << endl;
}

// ============= PRACTICAL EXAMPLES =============

void practicalExamples() {
    cout << "\n\n=== PRACTICAL EXAMPLES ===" << endl;
    
    // Example 1: Choosing right container based on iterator needs
    cout << "\n[Example 1: Choosing Container]" << endl;
    cout << "Need sorting? → use vector (random access for sort())" << endl;
    cout << "Need fast insert/delete middle? → use list (bidirectional)" << endl;
    cout << "Need ordered unique? → use set (bidirectional)" << endl;
    
    // Example 2: Algorithm requirements
    cout << "\n[Example 2: Algorithm Requirements]" << endl;
    
    vector<int> v = {5, 2, 8, 1, 9};
    list<int> l = {5, 2, 8, 1, 9};
    
    // sort() requires random access
    cout << "sort() on vector: ";
    sort(v.begin(), v.end());
    for (int n : v) cout << n << " ";
    cout << endl;
    
    // list has its own sort (doesn't need random access)
    cout << "list.sort(): ";
    l.sort();
    for (int n : l) cout << n << " ";
    cout << endl;
    
    // Example 3: advance() works with any iterator
    cout << "\n[Example 3: advance() - Generic Iterator Movement]" << endl;
    
    vector<int> vec = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto vecIt = vec.begin();
    advance(vecIt, 5);  // O(1) for random access
    cout << "Vector after advance(5): " << *vecIt << endl;
    
    list<int> lst = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto lstIt = lst.begin();
    advance(lstIt, 5);  // O(n) for bidirectional
    cout << "List after advance(5): " << *lstIt << endl;
    
    // Example 4: distance() works with any iterator
    cout << "\n[Example 4: distance() - Generic Distance Calculation]" << endl;
    cout << "Vector distance: " << distance(vec.begin(), vec.end()) << endl;
    cout << "List distance: " << distance(lst.begin(), lst.end()) << endl;
}

// ============= ITERATOR TRAITS =============

void demonstrateIteratorTraits() {
    cout << "\n\n=== ITERATOR TRAITS ===" << endl;
    cout << "Compile-time information about iterators" << endl;
    
    cout << "\n[Iterator Categories]" << endl;
    
    using VectorIt = vector<int>::iterator;
    using ListIt = list<int>::iterator;
    using ForwardListIt = forward_list<int>::iterator;
    
    // Check iterator categories (simplified display)
    cout << "vector::iterator → Random Access Iterator" << endl;
    cout << "list::iterator → Bidirectional Iterator" << endl;
    cout << "forward_list::iterator → Forward Iterator" << endl;
    
    cout << "\n[Why Iterator Traits Matter]" << endl;
    cout << "• Algorithms can optimize based on iterator type" << endl;
    cout << "• advance() uses O(1) for random access, O(n) otherwise" << endl;
    cout << "• distance() uses O(1) for random access, O(n) otherwise" << endl;
    cout << "• sort() requires random access iterator" << endl;
}

// ============= BEST PRACTICES =============

void bestPractices() {
    cout << "\n\n=== BEST PRACTICES ===" << endl;
    
    cout << "\n1. Use Auto for Iterator Types:" << endl;
    cout << "   ✓ auto it = vec.begin();" << endl;
    cout << "   ✗ vector<int>::iterator it = vec.begin();" << endl;
    
    cout << "\n2. Prefer Range-Based For Loops:" << endl;
    cout << "   ✓ for (const auto& item : container)" << endl;
    cout << "   ✗ for (auto it = c.begin(); it != c.end(); ++it)" << endl;
    
    cout << "\n3. Use Algorithms Instead of Manual Loops:" << endl;
    cout << "   ✓ auto it = find(v.begin(), v.end(), value);" << endl;
    cout << "   ✗ for loop with manual search" << endl;
    
    cout << "\n4. Don't Invalidate Iterators:" << endl;
    cout << "   • vector: insert/erase invalidates" << endl;
    cout << "   • list: only erased element invalidated" << endl;
    cout << "   • set/map: only erased element invalidated" << endl;
    
    cout << "\n5. Choose Right Container for Iterator Needs:" << endl;
    cout << "   • Need random access? → vector, deque" << endl;
    cout << "   • Need bidirectional? → list, set, map" << endl;
    cout << "   • Need forward only? → forward_list" << endl;
}

int main() {
    cout << "══════════════════════════════════════════════" << endl;
    cout << "   C++ ITERATOR TYPES - COMPREHENSIVE GUIDE" << endl;
    cout << "══════════════════════════════════════════════" << endl;
    
    demonstrateInputIterator();
    demonstrateOutputIterator();
    demonstrateForwardIterator();
    demonstrateBidirectionalIterator();
    demonstrateRandomAccessIterator();
    demonstrateContiguousIterator();
    
    compareContainerIterators();
    practicalExamples();
    demonstrateIteratorTraits();
    bestPractices();
    
    cout << "\n\n══════════════════════════════════════════════" << endl;
    cout << "   KEY TAKEAWAYS" << endl;
    cout << "══════════════════════════════════════════════" << endl;
    cout << "\n1. Iterator Hierarchy:" << endl;
    cout << "   Input/Output → Forward → Bidirectional → Random Access → Contiguous" << endl;
    
    cout << "\n2. Each level adds capabilities:" << endl;
    cout << "   • Input: Read once" << endl;
    cout << "   • Output: Write once" << endl;
    cout << "   • Forward: Read/write multiple times" << endl;
    cout << "   • Bidirectional: Can go backward" << endl;
    cout << "   • Random Access: Can jump" << endl;
    cout << "   • Contiguous: Sequential memory" << endl;
    
    cout << "\n3. Container → Iterator Type:" << endl;
    cout << "   • vector, array → Random Access (Contiguous)" << endl;
    cout << "   • deque → Random Access" << endl;
    cout << "   • list, set, map → Bidirectional" << endl;
    cout << "   • forward_list → Forward" << endl;
    
    cout << "\n4. Algorithm Requirements:" << endl;
    cout << "   • sort() → Random Access" << endl;
    cout << "   • reverse() → Bidirectional" << endl;
    cout << "   • find() → Input" << endl;
    cout << "   • copy() → Input + Output" << endl;
    
    cout << "\n5. Performance Implications:" << endl;
    cout << "   • Random access: O(1) for advance/distance" << endl;
    cout << "   • Bidirectional: O(n) for advance/distance" << endl;
    cout << "   • Contiguous: Cache-friendly access" << endl;
    
    return 0;
}

