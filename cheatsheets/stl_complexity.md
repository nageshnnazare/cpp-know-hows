# Cheatsheet — STL Complexity & Invalidation

## Container operation complexity
| Container | Access | Insert | Erase | Find | Notes |
|-----------|--------|--------|-------|------|-------|
| `vector` | O(1) | O(1)* back / O(n) mid | O(n) mid / O(1) back | O(n) | *amortized; contiguous, cache-friendly |
| `array` | O(1) | — | — | O(n) | fixed size, no alloc |
| `deque` | O(1) | O(1) ends / O(n) mid | O(1) ends | O(n) | segmented |
| `list` | O(n) | O(1) | O(1) | O(n) | doubly linked, splice O(1) |
| `forward_list` | O(n) | O(1) | O(1) | O(n) | singly linked |
| `map`/`set` | O(log n) | O(log n) | O(log n) | O(log n) | sorted, red-black tree |
| `multimap`/`multiset` | O(log n) | O(log n) | O(log n) | O(log n) | duplicates allowed |
| `unordered_map`/`set` | — | O(1) avg, O(n) worst | O(1) avg | O(1) avg | hash table |
| `priority_queue` | O(1) top | O(log n) push | O(log n) pop | — | binary heap |
| `stack`/`queue` | O(1) | O(1) | O(1) | — | adaptors |

## Iterator / reference invalidation
| Operation | Invalidates |
|-----------|-------------|
| `vector` push_back/insert (realloc) | **all** iterators, pointers, references |
| `vector` insert/erase (no realloc) | those **at and after** the point |
| `deque` insert/erase middle | **all** iterators (refs too) |
| `deque` push/pop front/back | iterators yes; refs to other elements no |
| `list`/`forward_list` insert | **none** |
| `list` erase | only the **erased** iterator |
| `map`/`set` insert | **none** |
| `map`/`set` erase | only the **erased** iterator |
| `unordered_*` insert (rehash) | **all iterators**; refs/pointers survive |
| `unordered_*` erase | only erased |

## Key algorithm complexities
```
   sort           O(n log n)        stable_sort  O(n log n) (extra mem)
   nth_element    O(n) average      partial_sort O(n log k)
   find/count     O(n)              binary_search/lower_bound O(log n) (sorted)
   accumulate     O(n)              unique       O(n) (adjacent only — sort first)
   make_heap      O(n)              push/pop_heap O(log n)
```

## Modern erase
```cpp
std::erase(v, value);        // C++20 — replaces erase-remove idiom
std::erase_if(v, pred);      // C++20
v.erase(std::remove(v.begin(),v.end(),value), v.end());  // pre-C++20
```
