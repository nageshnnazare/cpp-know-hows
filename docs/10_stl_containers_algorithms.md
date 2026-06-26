# Module 10 — The STL: Containers, Iterators & Algorithms

> The Standard Template Library is built on three orthogonal pillars —
> **containers** (store data), **iterators** (the glue), and **algorithms**
> (operate on ranges via iterators). Understanding their *complexity* and
> *invalidation* rules is what separates correct, fast code from subtle bugs.
> `[Primer §9, §10, §11]`

---

## 10.1 The architecture

```
        ALGORITHMS                 ITERATORS                 CONTAINERS
   ┌──────────────────┐        ┌────────────────┐        ┌──────────────────┐
   │ sort, find, copy │        │  abstract the  │        │ vector, list,    │
   │ accumulate, ...  │ ◀────▶ │  position +    │ ◀────▶ │ map, set, ...    │
   │ (know NOTHING    │        │  traversal     │        │ (know NOTHING    │
   │  about container)│        │                │        │ about algorithms)│
   └──────────────────┘        └────────────────┘        └──────────────────┘
              ▲                                                    
              └── FUNCTION OBJECTS / lambdas customize behavior (comparators, predicates)

   Decoupling: M algorithms + N containers = M+N code, not M×N. The iterator is
   the contract between them.
```

---

## 10.2 Sequence containers & their internals

```
   std::vector<T>      contiguous, growable array         ← THE DEFAULT container
   ┌──┬──┬──┬──┬──┬───────────┐
   │0 │1 │2 │3 │4 │ spare cap │   size()=5, capacity()=8
   └──┴──┴──┴──┴──┴───────────┘
   • random access O(1), cache-friendly. push_back AMORTIZED O(1) (doubles capacity).
   • insert/erase in MIDDLE O(n) (shifts elements).

   std::array<T,N>     fixed-size, stack, no allocation (Module 04)

   std::deque<T>       double-ended queue, segmented blocks
   [blk0][blk1][blk2]  push_front/back O(1); random access O(1) but slower than vector

   std::list<T>        doubly-linked list
   ⇄[a]⇄[b]⇄[c]⇄       insert/erase anywhere O(1) (with iterator); NO random access;
                       poor cache locality. splice() is its superpower.

   std::forward_list<T> singly-linked, minimal overhead
```

🧠 **Default to `std::vector`.** Even for middle insertions it usually beats
`std::list` because of cache locality, until N is large. Measure before choosing a
node-based container.

### `size` vs `capacity` & reallocation

```cpp
std::vector<int> v;
v.reserve(1000);   // pre-allocate capacity ⇒ avoid repeated reallocation/copying
v.shrink_to_fit(); // (non-binding) release spare capacity
```

```
   push_back when size == capacity:
   1. allocate a bigger buffer (typically 1.5× or 2×)
   2. MOVE/COPY existing elements over   ← O(n), and INVALIDATES all iterators/refs!
   3. free old buffer
   Amortized O(1) because growth is geometric.
```

---

## 10.3 Associative containers

```
   ORDERED (red-black tree, sorted, O(log n)):
   std::map<K,V>        unique keys → values, sorted by key
   std::set<K>          unique sorted keys
   std::multimap/multiset  allow duplicates

           [m]
          /   \           in-order traversal yields sorted order
        [d]   [t]         lookup/insert/erase: O(log n)
       /  \   /  \        iterators stay valid across insert/erase (except erased)
     [a] [f][p] [z]

   UNORDERED (hash table, average O(1)):
   std::unordered_map<K,V>    hash buckets, NO ordering
   std::unordered_set<K>
   ┌────┬────┬────┬────┐
   │ b0 │ b1 │ b2 │ b3 │   each bucket = linked list of entries with same hash
   └─┬──┴────┴─┬──┴────┘   lookup AVERAGE O(1), WORST O(n) (bad hash/collisions)
     ▼         ▼
   [k1,v1]   [k2,v2]→[k3,v3]
```

```cpp
std::map<std::string,int> m;
m["a"] = 1;                      // ⚠️ operator[] INSERTS a default if key absent!
m.at("a");                       // throws std::out_of_range if absent (no insert)
if (auto it = m.find("a"); it != m.end()) use(it->second);
auto [it, inserted] = m.insert({"b", 2});       // tells you if it was new
m.try_emplace("c", 3);           // C++17: construct value only if key is new
m.insert_or_assign("a", 9);      // C++17: upsert
```

🧠 `operator[]` on a map **creates** the element if missing (and requires V to be
default-constructible). Use `.at()`, `.find()`, or `.contains()` (C++20) for pure
lookup. For `unordered_*`, provide a good `std::hash` specialization for custom keys.

### Container adaptors

```
   std::stack<T>     LIFO   (wraps deque by default)
   std::queue<T>     FIFO
   std::priority_queue<T>  max-heap by default (O(log n) push/pop, O(1) top)
```

---

## 10.4 Iterators: categories & the hierarchy

```
   INPUT        ──▶  read once, single-pass (istream_iterator)
   OUTPUT       ──▶  write once, single-pass (back_inserter, ostream_iterator)
   FORWARD      ──▶  read/write, multi-pass        (forward_list)
   BIDIRECTIONAL──▶  + can go backwards (--)        (list, map, set)
   RANDOM ACCESS──▶  + it+n, it[n], it1-it2, < >    (vector, deque, array)
   CONTIGUOUS   ──▶  (C++17) + elements are contiguous in memory (vector, array, string)

   capability increases ──────────────────────────────────────────────▶
   (each category supports everything the ones to its left support)
```

```cpp
std::vector<int> v{1,2,3};
auto b = v.begin();   // iterator to first
auto e = v.end();     // ONE PAST the last (a sentinel; do not dereference)
*b;                   // 1
++b; b += 2;          // random access ops (vector only)
for (auto it = v.begin(); it != v.end(); ++it) use(*it);
```

```
   [1][2][3]
    ▲        ▲
  begin()   end()  ← half-open range [begin, end); end is unreachable/sentinel
```

🧠 The half-open range `[begin, end)` is pervasive: `end - begin == size`, an empty
range is `begin == end`, and you never dereference `end()`. Reverse iterators
(`rbegin`/`rend`), `const_iterator` (`cbegin`/`cend`), and inserters
(`std::back_inserter(v)`) all plug into the same algorithm interface.

---

## 10.5 Iterator & reference invalidation 🧠 (the bug source)

```
   CONTAINER        operation                    invalidates
   ────────────────────────────────────────────────────────────────────────
   vector           push_back / insert (realloc) ALL iterators, pointers, refs
   vector           erase                        iterators/refs AT and AFTER point
   deque            insert/erase in middle       ALL
   deque            push/pop front/back          iterators (not refs to other elems)
   list/forward_list insert                      NONE
   list             erase                        only the erased iterator
   map/set (tree)   insert                       NONE
   map/set          erase                        only the erased iterator
   unordered_*      insert (rehash)              ALL iterators (refs/ptrs survive)
   unordered_*      erase                        only erased
```

⚠️ The classic crash:

```cpp
for (auto it = v.begin(); it != v.end(); ++it)
    if (pred(*it)) v.erase(it);          // ⚠️ erase invalidates 'it' → UB
// FIX: erase returns the next valid iterator
for (auto it = v.begin(); it != v.end(); )
    it = pred(*it) ? v.erase(it) : it + 1;
// or, cleanest (C++20):
std::erase_if(v, pred);
```

⚠️ Holding a pointer/reference/iterator into a `vector` across a `push_back` that
reallocates is use-after-free.

---

## 10.6 Algorithms — operate on ranges

```cpp
#include <algorithm>
#include <numeric>
std::vector<int> v{5,3,1,4,2};

std::sort(v.begin(), v.end());                       // 1 2 3 4 5  (O(n log n))
std::sort(v.begin(), v.end(), std::greater<>{});     // 5 4 3 2 1  (custom comparator)
auto it = std::find(v.begin(), v.end(), 3);
bool any = std::any_of(v.begin(), v.end(), [](int x){ return x>4; });
int sum  = std::accumulate(v.begin(), v.end(), 0);   // 15
std::transform(v.begin(), v.end(), v.begin(), [](int x){ return x*x; });
auto last = std::unique(v.begin(), v.end());         // dedupe ADJACENT (sort first!)
v.erase(last, v.end());                              // erase-remove idiom
std::for_each(v.begin(), v.end(), [](int x){ std::cout << x; });
```

```
   THE ERASE-REMOVE IDIOM (pre-C++20)
   std::remove shuffles "kept" elements to the front and returns the new logical end;
   it does NOT change the container size. You must erase the tail:
       v.erase(std::remove(v.begin(), v.end(), value), v.end());
   ┌──┬──┬──┬──┬──┐  remove 2  ┌──┬──┬──┬──┬──┐
   │1 │2 │3 │2 │4 │   ──────▶  │1 │3 │4 │? │? │   returns iterator here ─┘ then erase
   └──┴──┴──┴──┴──┘            └──┴──┴──┴──┴──┘
   C++20: just std::erase(v, value);  /  std::erase_if(v, pred);
```

🧠 **Prefer algorithms over hand-written loops** ("no raw loops"): they express
intent, are correct (off-by-one free), and are often optimized (vectorized). Many
have **execution policies** (C++17) for parallelism:
`std::sort(std::execution::par, b, e);`.

Algorithm families: searching (`find`, `binary_search`, `lower_bound`),
modifying (`copy`, `transform`, `fill`, `replace`), partitioning (`partition`,
`stable_partition`), sorting (`sort`, `stable_sort`, `nth_element`,
`partial_sort`), set ops (`set_union`, `set_intersection`), heap ops, numeric
(`accumulate`, `reduce`, `inner_product`, `iota`, `partial_sum`).

---

## 10.7 Function objects, predicates & projections

```cpp
std::sort(v.begin(), v.end(),
          [](const auto& a, const auto& b){ return a.score > b.score; });  // comparator

std::count_if(v.begin(), v.end(), [](int x){ return x % 2 == 0; });        // predicate

// standard functors
std::less<>{}, std::greater<>{}, std::plus<>{}   // transparent (heterogeneous) since C++14
```

---

## 10.8 Allocators (awareness level)

Every container has an `Allocator` template parameter controlling how it gets raw
memory. `std::pmr` (C++17, polymorphic memory resources) lets you swap allocation
strategy (e.g. a stack/monotonic arena) at runtime without changing the type:

```cpp
#include <memory_resource>
std::array<std::byte, 4096> buf;
std::pmr::monotonic_buffer_resource pool{buf.data(), buf.size()};
std::pmr::vector<int> v{&pool};   // allocates from the stack buffer — no heap calls
```

🧠 Custom allocators matter for latency-sensitive/embedded code (avoid `malloc`,
control fragmentation). For most code the default allocator is fine.

---

## 10.9 Choosing a container — decision flow

```
   Need key→value lookup?
     yes → need ordering / range queries?  yes → std::map / std::set
                                            no  → std::unordered_map / _set (faster avg)
     no  → mostly append + index + iterate? → std::vector (DEFAULT)
           heavy insert/erase at BOTH ends?  → std::deque
           heavy splice / stable iterators / insert-erase in middle? → std::list
           fixed compile-time size?          → std::array
           LIFO/FIFO/priority semantics?     → stack / queue / priority_queue
```

---

## 10.10 `std::span` (C++20) — a non-owning view over contiguous data

```cpp
void process(std::span<int> data) {       // accepts vector, array, C array — no copy
    for (int& x : data) x *= 2;
    data.size(); data[0]; data.subspan(1, 3);
}
std::vector<int> v{1,2,3}; process(v);
int a[4]{}; process(a);
```

🧠 `span` is to arrays what `string_view` is to strings: a `{pointer, length}` pair
that unifies APIs over any contiguous storage — but it does **not** own and can
dangle if the underlying data dies.

---

## 10.11 Checklist

- [ ] You default to `vector`, and pick others only with a complexity reason.
- [ ] You know each container's lookup/insert/erase complexity by heart.
- [ ] You know the invalidation rules and avoid the erase-in-loop trap.
- [ ] You prefer algorithms (and `erase_if`, ranges) over raw loops.
- [ ] You use `map::at/find/contains` instead of accidental `operator[]` inserts.
- [ ] You reach for `span`/`string_view` for non-owning views.

➡ Next: [Module 11 — Smart Pointers & RAII](11_smart_pointers_raii.md)
