# Module 15 — Concurrency & the Memory Model

> C++11 gave C++ a **formal memory model** and a portable threading library. This
> module covers threads, synchronization (mutexes, condition variables), the
> futures/promises model, atomics, and the **happens-before** relationship that
> makes lock-free code possible (and dangerous).

---

## 15.1 Threads of execution

```cpp
#include <thread>
void work(int id) { /* ... */ }

std::thread t(work, 1);     // starts immediately, runs work(1) on a new thread
t.join();                   // wait for it to finish (BLOCKS caller)
// OR t.detach();           // let it run independently (you lose the handle)
```

```
   main thread ──────────────●(create t)──────────────●(t.join blocks)────▶
                              │                         ▲
   thread t                   └──▶ work(1) running ─────┘ (joins back)
```

⚠️ A `std::thread` that is neither `join`ed nor `detach`ed before destruction calls
`std::terminate`. **C++20 `std::jthread`** fixes this — it auto-joins in its
destructor and supports cooperative cancellation:

```cpp
#include <stop_token>
std::jthread jt([](std::stop_token st){
    while (!st.stop_requested()) { doWork(); }
});                 // destructor requests stop AND joins — no terminate, no leak
```

🧠 Prefer `std::jthread` over `std::thread`. Better still, prefer higher-level
tools (`std::async`, thread pools, parallel algorithms) over raw threads.

---

## 15.2 Data races — the central hazard ⚠️

```
   DATA RACE (Undefined Behavior): two threads access the SAME memory location,
   at least one WRITES, and there is NO synchronization (happens-before) ordering
   between them.  ⇒ The whole program has UB — not just a wrong value.
```

```cpp
int counter = 0;                     // shared
void inc() { for (int i=0;i<100000;++i) ++counter; }  // ⚠️ ++counter is read-modify-write
std::thread a(inc), b(inc);
a.join(); b.join();
// counter is NOT 200000 — it's a data race ⇒ UB. Lost updates / torn values.
```

```
   ++counter is THREE steps, interleavable between threads:
   T1: load counter(5) ─┐                 both read 5,
   T2:        load counter(5) ─┐          both write 6
   T1: add 1 → 6  store 6 ─────┘          → ONE increment LOST
   T2: add 1 → 6  store 6 ──────┘
```

Detect with `-fsanitize=thread` (ThreadSanitizer).

---

## 15.3 Mutexes & RAII locks

```cpp
#include <mutex>
std::mutex m;
int counter = 0;

void inc() {
    for (int i=0;i<100000;++i) {
        std::lock_guard<std::mutex> lk(m);   // RAII: locks now, unlocks at scope end
        ++counter;                           // critical section — exclusive access
    }
}
```

```
   std::lock_guard<M>        simplest RAII lock; lock in ctor, unlock in dtor. No manual unlock.
   std::unique_lock<M>       movable, deferrable, supports condition_variable, manual lock/unlock
   std::scoped_lock<Ms...>   (C++17) locks MULTIPLE mutexes deadlock-free (all-or-nothing)
   std::shared_mutex         reader/writer: many shared (read) OR one exclusive (write)
       std::shared_lock<M>   (read lock)   std::unique_lock<M> (write lock)
```

⚠️ **Deadlock** — two threads each hold a lock the other wants:

```
   T1: lock A ... wait for B
   T2: lock B ... wait for A      → both stuck forever
   FIX: always acquire locks in the SAME global order, OR use std::scoped_lock(A,B)
        which locks both atomically without ordering issues.
```

🧠 Always use RAII locks, never bare `m.lock()/m.unlock()` (an exception or early
return would leak the lock → deadlock). Keep critical sections **short**. Prefer
`shared_mutex` only when reads vastly outnumber writes (it has overhead).

---

## 15.4 Condition variables — waiting for a state change

```cpp
std::mutex m;
std::condition_variable cv;
std::queue<Task> q;
bool done = false;

void producer(Task t) {
    { std::lock_guard lk(m); q.push(std::move(t)); }
    cv.notify_one();                          // wake one waiter
}
void consumer() {
    std::unique_lock lk(m);
    cv.wait(lk, []{ return !q.empty() || done; });  // ⟵ predicate guards against
                                                      //    SPURIOUS WAKEUPS
    if (!q.empty()) process(q.front()), q.pop();
}
```

```
   cv.wait(lk, pred):
   1. checks pred() — if true, returns immediately
   2. else ATOMICALLY unlocks lk and sleeps
   3. on notify (or spurious wakeup) re-locks lk, re-checks pred, loops if false
```

⚠️ **Always pass a predicate** to `wait` — condition variables can wake spuriously
(without any `notify`). The predicate loop is mandatory for correctness. Notify
*after* releasing or while holding the lock (both are valid; releasing first can
reduce contention).

---

## 15.5 Futures, promises & `std::async`

For "run this and give me the result later", the future/promise model beats raw
threads — and it **propagates exceptions** (Module 13) automatically.

```cpp
#include <future>
std::future<int> fut = std::async(std::launch::async, []{
    return expensiveComputation();           // runs on another thread
});
// ... do other work ...
int result = fut.get();                       // blocks until ready; rethrows if it threw
```

```
   promise (producer side)  ───set_value/​set_exception──▶  shared state
                                                                │
   future  (consumer side)  ◀──── get() / wait()  ──────────────┘
```

```cpp
std::promise<int> p;
std::future<int> f = p.get_future();
std::thread t([&]{ p.set_value(compute()); });   // fulfil the promise elsewhere
int r = f.get();
t.join();

std::packaged_task<int()> task(compute);          // wraps a callable + future
auto f2 = task.get_future();
```

🧠 `std::async` with `std::launch::async` guarantees a new thread; the default
policy *may* run lazily on `get()` (deferred) — be explicit. The returned future's
destructor for an `async` task **blocks** until completion (a famous gotcha).

---

## 15.6 Atomics — lock-free synchronization

```cpp
#include <atomic>
std::atomic<int> counter{0};
counter.fetch_add(1);        // atomic read-modify-write — NO data race
++counter;                   // same, atomically
int v = counter.load();
counter.store(5);
int old = counter.exchange(9);
counter.compare_exchange_strong(expected, desired);  // CAS — the lock-free primitive

std::atomic<bool> ready{false};
std::atomic_flag flag = ATOMIC_FLAG_INIT;            // guaranteed lock-free
```

```
   atomic<T> guarantees:
   • indivisible read-modify-write (no torn/lost updates)
   • a MEMORY ORDER controlling visibility/reordering of OTHER memory around it
   is_lock_free()/is_always_lock_free → whether it uses a hidden lock (large T do)
```

🧠 `std::atomic<int> x; ++x;` is correct and race-free, but `atomic` is **not** a
magic "make it thread-safe" wrapper for big objects — for non-trivially-copyable
or large `T`, it may use an internal lock. Use atomics for counters, flags, and
carefully-designed lock-free structures, not as a substitute for proper locking.

---

## 15.7 The memory model: happens-before & memory orders 🧠

The crux: when is a write by thread A **guaranteed visible** to thread B? Answer:
when there is a **happens-before** relationship, established by synchronization.

```
   memory_order_seq_cst   (default) total global order; easiest to reason about, costliest
   memory_order_acquire   a load: no later reads/writes move BEFORE it
   memory_order_release   a store: no earlier reads/writes move AFTER it
   memory_order_acq_rel   both (for read-modify-write)
   memory_order_relaxed   atomicity only — NO ordering guarantees vs other vars
```

The canonical **release/acquire handoff** (publishing data without a mutex):

```cpp
std::atomic<bool> ready{false};
int data = 0;                       // plain, non-atomic

// Thread A (producer)
data = 42;                          // (1) ordinary write
ready.store(true, std::memory_order_release);   // (2) RELEASE: (1) cannot move after (2)

// Thread B (consumer)
while (!ready.load(std::memory_order_acquire)) {}  // (3) ACQUIRE
assert(data == 42);                 // ✅ guaranteed: (1) happens-before (4) reads
use(data);                          // (4)
```

```
   release store ──synchronizes-with──▶ acquire load (that reads the stored value)
        ▲                                       │
        │ everything BEFORE the release   ──▶   is visible AFTER the acquire
   data=42 (1)                                  read data (4) sees 42
```

⚠️ Without the acquire/release (or a mutex), thread B might see `ready==true` but
`data==0` due to CPU/compiler reordering — even though the code "looks" ordered.
`memory_order_relaxed` gives atomicity but **no** such ordering — only use it for
independent counters/statistics where ordering doesn't matter.

🧠 **Default to `seq_cst` (the default) and mutexes.** Reach for acquire/release
and relaxed only with a concrete performance need and a proof of correctness;
lock-free programming is a specialist skill where intuition routinely fails.

---

## 15.8 `thread_local`, `std::call_once`, and one-time init

```cpp
thread_local int tls = 0;                 // one independent instance PER thread

std::once_flag once;
void initOnce() { std::call_once(once, []{ expensiveInit(); }); }  // runs exactly once

// function-local static = thread-safe one-time init (C++11 "magic statics")
Config& config() { static Config c; return c; }  // initialization is race-free
```

🧠 Function-local `static` initialization is guaranteed thread-safe since C++11
(the compiler inserts a guard). This is the cleanest singleton and the fix for the
static-init-order fiasco (Module 02).

---

## 15.9 C++20 concurrency additions

```cpp
std::jthread          // auto-joining, cancellable thread (15.1)
std::stop_token / std::stop_source   // cooperative cancellation
std::latch            // single-use countdown rendezvous: count_down(); wait();
std::barrier          // reusable phase barrier for N threads
std::counting_semaphore<N> / std::binary_semaphore   // classic semaphores
std::atomic<T>::wait/notify_one/notify_all           // efficient atomic waiting
std::atomic_ref<T>    // atomic operations on a non-atomic object
```

```cpp
std::latch start{1};
// workers: start.wait();   main: setup(); start.count_down();  // release all at once
```

---

## 15.10 Higher-level guidance & parallel algorithms

```cpp
#include <execution>
std::sort(std::execution::par, v.begin(), v.end());          // parallel sort
std::for_each(std::execution::par_unseq, v.begin(), v.end(), f);
std::reduce(std::execution::par, v.begin(), v.end());        // parallel accumulate
```

```
   PREFER, in order:
   1. immutable / message-passing designs (no shared mutable state → no races)
   2. parallel STL algorithms (std::execution::par)
   3. std::async / futures / a thread pool
   4. std::jthread + RAII locks (lock_guard/scoped_lock) + condition_variable
   5. atomics (seq_cst) for simple counters/flags
   6. lock-free with relaxed/acquire/release  ← experts only, with benchmarks + TSan
```

---

## 15.11 Checklist

- [ ] You can define a data race and know it's UB; you run TSan.
- [ ] You use RAII locks, keep critical sections short, and avoid deadlock by lock ordering / `scoped_lock`.
- [ ] You always use a predicate with `condition_variable::wait`.
- [ ] You use `async`/futures for result-bearing tasks and know they propagate exceptions.
- [ ] You understand acquire/release/seq_cst/relaxed and default to seq_cst + mutexes.
- [ ] You prefer `jthread`, function-local statics, and parallel algorithms.

➡ Next: [Module 16 — Undefined Behavior & Expert Gotchas](16_undefined_behavior_gotchas.md)
