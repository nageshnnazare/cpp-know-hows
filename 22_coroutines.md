# Coroutines (C++20)

## Overview

C++20 introduced coroutines as a powerful feature for writing asynchronous and generator-style code. Coroutines are functions that can suspend and resume execution, enabling elegant solutions for async operations, lazy evaluation, and cooperative multitasking.

```
┌────────────────────────────────────────────────────────┐
│                  COROUTINES SYSTEM                     │
├────────────────────────────────────────────────────────┤
│                                                        │
│  KEYWORDS       │  COMPONENTS      │  USE CASES        │
│  ────────       │  ──────────      │  ─────────        │
│  • co_await     │  • Promise type  │  • Generators     │
│  • co_yield     │  • Awaitable     │  • Async tasks    │
│  • co_return    │  • Coroutine     │  • State machines │
│                 │    handle        │  • Lazy eval      │
│                                                        │
│  ADVANTAGES:                                           │
│  • Cleaner async code (no callback hell)               │
│  • Composable operations                               │
│  • Memory efficient (no thread per task)               │
│  • Natural control flow                                │
│                                                        │
└────────────────────────────────────────────────────────┘
```

---

## Coroutine Basics

### What Makes a Coroutine?
```cpp
// A function is a coroutine if it uses any of:
// 1. co_await
// 2. co_yield
// 3. co_return

#include <coroutine>

// Example 1: Using co_return
Task simple_coroutine() {
    co_return;  // This makes it a coroutine
}

// Example 2: Using co_yield
Generator<int> count_to_three() {
    co_yield 1;
    co_yield 2;
    co_yield 3;
}

// Example 3: Using co_await
Task async_operation() {
    auto result = co_await some_async_task();
    co_return result;
}
```

### Coroutine Execution Flow
```
Normal Function:
┌─────────────────────────────────────┐
│ Start → Execute → Return → End      │
└─────────────────────────────────────┘

Coroutine:
┌─────────────────────────────────────────────────────┐
│ Start → Execute → Suspend ─┐                        │
│           ↑                 │                       │
│           └─── Resume ←─────┘                       │
│                 Execute → Suspend ─┐                │
│                   ↑                │                │
│                   └─── Resume ←────┘                │
│                       Execute → Complete → End      │
└─────────────────────────────────────────────────────┘

Key: Can pause and resume multiple times
```

---

## Generator Pattern

### Simple Generator
```cpp
#include <coroutine>
#include <iostream>
#include <exception>

// Generator return type
template<typename T>
struct Generator {
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;
    
    struct promise_type {
        T current_value;
        std::exception_ptr exception_;
        
        Generator get_return_object() {
            return Generator{handle_type::from_promise(*this)};
        }
        
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        
        std::suspend_always yield_value(T value) {
            current_value = std::move(value);
            return {};
        }
        
        void return_void() {}
        
        void unhandled_exception() {
            exception_ = std::current_exception();
        }
    };
    
    handle_type h_;
    
    Generator(handle_type h) : h_(h) {}
    ~Generator() { if (h_) h_.destroy(); }
    
    // Disable copy
    Generator(const Generator&) = delete;
    Generator& operator=(const Generator&) = delete;
    
    // Enable move
    Generator(Generator&& other) noexcept : h_(other.h_) {
        other.h_ = {};
    }
    
    Generator& operator=(Generator&& other) noexcept {
        if (this != &other) {
            if (h_) h_.destroy();
            h_ = other.h_;
            other.h_ = {};
        }
        return *this;
    }
    
    // Iterator interface
    struct iterator {
        handle_type h_;
        
        iterator& operator++() {
            h_.resume();
            return *this;
        }
        
        bool operator==(std::default_sentinel_t) const {
            return !h_ || h_.done();
        }
        
        T const& operator*() const {
            return h_.promise().current_value;
        }
    };
    
    iterator begin() {
        if (h_) h_.resume();
        return iterator{h_};
    }
    
    std::default_sentinel_t end() { return {}; }
};

// Usage
Generator<int> counter(int start, int end) {
    for (int i = start; i < end; ++i) {
        co_yield i;
    }
}

int main() {
    for (int value : counter(1, 5)) {
        std::cout << value << '\n';  // Prints: 1 2 3 4
    }
    
    return 0;
}
```

### Infinite Generator
```cpp
Generator<int> fibonacci() {
    int a = 0, b = 1;
    while (true) {
        co_yield a;
        auto next = a + b;
        a = b;
        b = next;
    }
}

void example() {
    auto fib = fibonacci();
    auto it = fib.begin();
    
    // Take first 10 Fibonacci numbers
    for (int i = 0; i < 10; ++i) {
        std::cout << *it << ' ';
        ++it;
    }
    // Output: 0 1 1 2 3 5 8 13 21 34
}
```

---

## Async Task Pattern

### Task Implementation
```cpp
#include <coroutine>
#include <exception>
#include <variant>

template<typename T>
struct Task {
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;
    
    struct promise_type {
        std::variant<std::monostate, T, std::exception_ptr> result_;
        std::coroutine_handle<> continuation_;
        
        Task get_return_object() {
            return Task{handle_type::from_promise(*this)};
        }
        
        std::suspend_always initial_suspend() noexcept { return {}; }
        
        struct final_awaiter {
            bool await_ready() noexcept { return false; }
            
            void await_suspend(handle_type h) noexcept {
                if (auto continuation = h.promise().continuation_) {
                    continuation.resume();
                }
            }
            
            void await_resume() noexcept {}
        };
        
        final_awaiter final_suspend() noexcept { return {}; }
        
        void return_value(T value) {
            result_ = std::move(value);
        }
        
        void unhandled_exception() {
            result_ = std::current_exception();
        }
    };
    
    handle_type h_;
    
    Task(handle_type h) : h_(h) {}
    ~Task() { if (h_) h_.destroy(); }
    
    // Move only
    Task(Task&& other) noexcept : h_(other.h_) {
        other.h_ = {};
    }
    
    Task& operator=(Task&& other) noexcept {
        if (this != &other) {
            if (h_) h_.destroy();
            h_ = other.h_;
            other.h_ = {};
        }
        return *this;
    }
    
    // Awaitable interface
    bool await_ready() { return false; }
    
    void await_suspend(std::coroutine_handle<> continuation) {
        h_.promise().continuation_ = continuation;
        h_.resume();
    }
    
    T await_resume() {
        auto& result = h_.promise().result_;
        if (std::holds_alternative<std::exception_ptr>(result)) {
            std::rethrow_exception(std::get<std::exception_ptr>(result));
        }
        return std::get<T>(result);
    }
};

// Specialization for void
template<>
struct Task<void> {
    // Similar implementation for void
};
```

### Using Tasks
```cpp
Task<int> async_compute() {
    // Simulate async work
    co_await std::suspend_always{};
    co_return 42;
}

Task<int> async_workflow() {
    int result1 = co_await async_compute();
    int result2 = co_await async_compute();
    co_return result1 + result2;
}

void example() {
    auto task = async_workflow();
    // Task is lazy - doesn't start until awaited
    
    // Execute task
    auto handle = std::coroutine_handle<decltype(task)::promise_type>::from_promise(
        task.h_.promise()
    );
    handle.resume();
}
```

---

## Awaitable Objects

### Custom Awaitable
```cpp
#include <chrono>
#include <thread>

struct sleep_awaitable {
    std::chrono::milliseconds duration_;
    
    sleep_awaitable(std::chrono::milliseconds duration) 
        : duration_(duration) {}
    
    bool await_ready() const noexcept {
        return duration_.count() <= 0;
    }
    
    void await_suspend(std::coroutine_handle<> handle) const {
        std::thread([handle, duration = duration_]() {
            std::this_thread::sleep_for(duration);
            handle.resume();
        }).detach();
    }
    
    void await_resume() const noexcept {}
};

// Helper function
auto sleep_for(std::chrono::milliseconds duration) {
    return sleep_awaitable{duration};
}

// Usage
Task<void> async_sleep_example() {
    std::cout << "Starting\n";
    co_await sleep_for(std::chrono::seconds(1));
    std::cout << "After 1 second\n";
    co_await sleep_for(std::chrono::seconds(2));
    std::cout << "After 3 seconds total\n";
}
```

### Awaitable Transformation
```cpp
struct transform_awaitable {
    int value_;
    
    bool await_ready() { return false; }
    
    void await_suspend(std::coroutine_handle<>) {}
    
    int await_resume() {
        return value_ * 2;  // Transform the result
    }
};

Task<int> example_transform() {
    int result = co_await transform_awaitable{21};
    co_return result;  // Returns 42
}
```

---

## Promise Type

### Understanding Promise Type
```
Coroutine Components:
┌─────────────────────────────────────────────────────┐
│                                                     │
│  ┌─────────────┐                                    │
│  │  Coroutine  │                                    │
│  │   Frame     │  Stores local variables            │
│  │             │  and suspend state                 │
│  └──────┬──────┘                                    │
│         │                                           │
│         ▼                                           │
│  ┌─────────────┐                                    │
│  │   Promise   │                                    │
│  │    Type     │  Controls coroutine behavior       │
│  │             │  - Initial/final suspend           │
│  │             │  - Return value handling           │
│  └──────┬──────┘  - Exception handling              │
│         │                                           │
│         ▼                                           │
│  ┌─────────────┐                                    │
│  │  Coroutine  │                                    │
│  │   Handle    │  Controls resume/destroy           │
│  │             │                                    │
│  └─────────────┘                                    │
│                                                     │
└─────────────────────────────────────────────────────┘
```

### Promise Type Methods
```cpp
struct my_promise_type {
    // 1. Create return object
    auto get_return_object() {
        return MyCoroutineType{
            std::coroutine_handle<my_promise_type>::from_promise(*this)
        };
    }
    
    // 2. Initial suspend point
    auto initial_suspend() {
        return std::suspend_always{};  // Lazy start
        // or std::suspend_never{};    // Eager start
    }
    
    // 3. Final suspend point
    auto final_suspend() noexcept {
        return std::suspend_always{};  // Keep alive
        // or std::suspend_never{};    // Auto destroy
    }
    
    // 4. Handle return value
    void return_value(T value) {
        // Store value for retrieval
    }
    // OR
    void return_void() {
        // For coroutines with no return
    }
    
    // 5. Handle exceptions
    void unhandled_exception() {
        // Store or rethrow exception
    }
    
    // 6. Handle co_yield
    auto yield_value(T value) {
        // Store value
        return std::suspend_always{};
    }
};
```

---

## Practical Examples

### Async File Reader
```cpp
#include <fstream>
#include <string>
#include <vector>

Generator<std::string> read_lines(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
    
    while (std::getline(file, line)) {
        co_yield line;
    }
}

void example() {
    for (const auto& line : read_lines("data.txt")) {
        std::cout << line << '\n';
    }
}
```

### Range Generator
```cpp
Generator<int> range(int start, int end, int step = 1) {
    for (int i = start; i < end; i += step) {
        co_yield i;
    }
}

void example() {
    // Like Python's range()
    for (int i : range(0, 10, 2)) {
        std::cout << i << ' ';  // 0 2 4 6 8
    }
}
```

### Lazy Filter
```cpp
template<typename Gen, typename Pred>
Generator<typename Gen::value_type> filter(Gen gen, Pred pred) {
    for (auto value : gen) {
        if (pred(value)) {
            co_yield value;
        }
    }
}

void example() {
    auto numbers = range(1, 20);
    auto evens = filter(std::move(numbers), [](int x) { return x % 2 == 0; });
    
    for (int n : evens) {
        std::cout << n << ' ';  // 2 4 6 8 10 12 14 16 18
    }
}
```

### Lazy Map
```cpp
template<typename Gen, typename Func>
auto map(Gen gen, Func func) -> Generator<decltype(func(*gen.begin()))> {
    for (auto value : gen) {
        co_yield func(value);
    }
}

void example() {
    auto numbers = range(1, 6);
    auto squares = map(std::move(numbers), [](int x) { return x * x; });
    
    for (int n : squares) {
        std::cout << n << ' ';  // 1 4 9 16 25
    }
}
```

### State Machine
```cpp
enum class State { Init, Processing, Complete };

Generator<State> state_machine() {
    co_yield State::Init;
    
    // Initialization logic
    std::cout << "Initializing...\n";
    co_yield State::Processing;
    
    // Processing logic
    std::cout << "Processing...\n";
    co_yield State::Processing;
    
    // More processing
    std::cout << "Finishing...\n";
    co_yield State::Complete;
}

void example() {
    auto sm = state_machine();
    
    for (auto state : sm) {
        std::cout << "Current state: " << static_cast<int>(state) << '\n';
        // Can add logic based on state
    }
}
```

---

## Async Networking Example

### TCP Echo Server
```cpp
Task<void> handle_client(Socket socket) {
    char buffer[1024];
    
    while (true) {
        // Async read
        int bytes_read = co_await socket.async_read(buffer, sizeof(buffer));
        
        if (bytes_read <= 0) break;
        
        // Async write (echo back)
        co_await socket.async_write(buffer, bytes_read);
    }
    
    co_await socket.close();
}

Task<void> run_server(int port) {
    auto listener = co_await create_listener(port);
    
    while (true) {
        auto client = co_await listener.accept();
        
        // Handle each client concurrently
        handle_client(std::move(client));  // Fire and forget
    }
}
```

---

## Coroutine Patterns

### Lazy Evaluation
```cpp
// Values computed only when requested
Generator<int> expensive_sequence() {
    for (int i = 0; i < 1000000; ++i) {
        // Expensive computation
        int result = compute_expensive(i);
        co_yield result;
    }
}

void example() {
    auto seq = expensive_sequence();
    auto it = seq.begin();
    
    // Only compute first 3 values
    for (int i = 0; i < 3; ++i) {
        std::cout << *it << '\n';
        ++it;
    }
    // Rest never computed!
}
```

### Pipeline Pattern
```cpp
auto pipeline = 
    range(1, 100)
    | filter([](int x) { return x % 2 == 0; })
    | map([](int x) { return x * x; })
    | take(10);

for (int value : pipeline) {
    std::cout << value << ' ';
}
```

### Async Retry Logic
```cpp
Task<Response> retry_async(int max_attempts) {
    for (int attempt = 1; attempt <= max_attempts; ++attempt) {
        try {
            auto response = co_await make_request();
            co_return response;
        }
        catch (const NetworkError& e) {
            if (attempt == max_attempts) throw;
            
            // Exponential backoff
            auto delay = std::chrono::milliseconds(100 * (1 << attempt));
            co_await sleep_for(delay);
        }
    }
}
```

---

## Performance Considerations

### Memory Overhead
```
Traditional Callback:
┌────────────────────────┐
│ Heap allocations for   │
│ callback objects       │  ~50-100 bytes per callback
└────────────────────────┘

Thread per Task:
┌────────────────────────┐
│ Thread stack           │  ~1-8 MB per thread
│ OS scheduler overhead  │
└────────────────────────┘

Coroutine:
┌────────────────────────┐
│ Coroutine frame        │  ~100-500 bytes
│ Promise object         │  Minimal overhead
└────────────────────────┘

Coroutines: Much more memory efficient than threads
```

### Allocation Elision
```cpp
// Compiler may optimize away heap allocation
Task<int> small_coroutine() {
    co_return 42;
}

// HALO (Heap Allocation eLision Optimization)
// If coroutine lifetime is obvious, compiler can
// allocate on stack instead of heap
```

---

## Best Practices

### 1. Use Coroutines for I/O-Bound Operations
```cpp
// GOOD: I/O-bound async operations
Task<Data> fetch_data_async(const std::string& url) {
    auto response = co_await http_get(url);
    co_return parse(response);
}

// BAD: CPU-bound operations (use threads instead)
// Coroutines don't provide parallelism, just concurrency
```

### 2. Avoid Capturing References
```cpp
// BAD: Captured reference may dangle
Task<void> dangerous(int& value) {
    co_await something();
    value = 42;  // 'value' may be destroyed!
}

// GOOD: Pass by value or use shared_ptr
Task<void> safe(std::shared_ptr<int> value) {
    co_await something();
    *value = 42;  // Safe
}
```

### 3. Be Careful with Lifetimes
```cpp
// BAD: Coroutine outlives local variable
void bad_example() {
    std::string data = "important data";
    auto task = process_data(data);  // Coroutine captures reference
    // task may resume after this function returns!
}

// GOOD: Ensure proper lifetime
Task<void> good_example() {
    std::string data = "important data";
    co_await process_data(data);  // Wait for completion
}
```

### 4. Use Structured Concurrency
```cpp
// Launch multiple coroutines and wait for all
Task<void> process_batch(const std::vector<Item>& items) {
    std::vector<Task<Result>> tasks;
    
    for (const auto& item : items) {
        tasks.push_back(process_item(item));
    }
    
    // Wait for all to complete
    for (auto& task : tasks) {
        co_await task;
    }
}
```

---

## Common Pitfalls

### 1. Forgetting co_await
```cpp
// BAD: Returns Task, doesn't execute
Task<int> compute();

void bad() {
    auto task = compute();  // Created but not executed
    // Forgot co_await!
}

// GOOD: Actually execute
Task<void> good() {
    int result = co_await compute();  // Executes
}
```

### 2. Dangling References
```cpp
// BAD
Generator<int> bad_generator() {
    std::vector<int> vec = {1, 2, 3};
    for (int value : vec) {
        co_yield value;  // vec destroyed after first suspend!
    }
}

// GOOD: Ensure data lifetime
Generator<int> good_generator() {
    std::vector<int> vec = {1, 2, 3};
    for (int i = 0; i < vec.size(); ++i) {
        co_yield vec[i];  // Copy value before yielding
    }
}
```

### 3. Exception Safety
```cpp
// Ensure exceptions in coroutines are handled
Task<void> example() {
    try {
        co_await may_throw();
    }
    catch (const std::exception& e) {
        // Handle error
        std::cerr << "Error: " << e.what() << '\n';
    }
}
```

---

## Compiler Support

### Current Status (2024)
```
┌────────────────────────────────────────────────────────┐
│              Coroutine Support Status                  │
├────────────────────────────────────────────────────────┤
│ Compiler      │ Version  │ Support                     │
├───────────────┼──────────┼─────────────────────────────┤
│ GCC           │ 10+      │ Full support                │
│ Clang         │ 14+      │ Full support                │
│ MSVC          │ 19.28+   │ Full support                │
│                                                        │
│ Note: Requires -std=c++20 or later                     │
│                                                        │
│ Libraries:                                             │
│ • cppcoro: Popular coroutine utilities library         │
│ • folly: Facebook's coroutine support                  │
│ • boost.asio: Coroutine integration                    │
└────────────────────────────────────────────────────────┘
```

---

## Libraries and Frameworks

### cppcoro
```cpp
#include <cppcoro/task.hpp>
#include <cppcoro/sync_wait.hpp>

cppcoro::task<int> example() {
    co_return 42;
}

int main() {
    int result = cppcoro::sync_wait(example());
    std::cout << result << '\n';
}
```

### Boost.Asio Coroutines
```cpp
#include <boost/asio.hpp>
#include <boost/asio/co_spawn.hpp>

boost::asio::awaitable<void> echo(tcp::socket socket) {
    try {
        char data[1024];
        for (;;) {
            std::size_t n = co_await socket.async_read_some(
                boost::asio::buffer(data),
                boost::asio::use_awaitable
            );
            
            co_await async_write(socket,
                boost::asio::buffer(data, n),
                boost::asio::use_awaitable
            );
        }
    }
    catch (std::exception& e) {
        // Handle error
    }
}
```

---

## Future Directions

### C++23 and Beyond
- std::generator (standardized generator type)
- Improved coroutine support in standard library
- Better debugging support
- Performance improvements

---

## Resources

- **C++20 Standard**: ISO/IEC 14882:2020
- **cppcoro**: https://github.com/lewissbaker/cppcoro
- **CppCon Talks**: Numerous talks on coroutines
- **Compiler Documentation**: GCC, Clang, MSVC coroutine docs

---

## Complete Practical Example: Async Data Processing Pipeline

Here's a comprehensive example integrating generators, async tasks, and lazy evaluation:

```cpp
#include <coroutine>
#include <iostream>
#include <vector>
#include <string>
#include <optional>
#include <chrono>
#include <thread>
#include <map>

// Generator implementation (from earlier in chapter)
template<typename T>
struct Generator {
    struct promise_type {
        T current_value;
        
        Generator get_return_object() {
            return Generator{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        std::suspend_always yield_value(T value) {
            current_value = std::move(value);
            return {};
        }
        void return_void() {}
        void unhandled_exception() { std::terminate(); }
    };
    
    using handle_type = std::coroutine_handle<promise_type>;
    handle_type h_;
    
    Generator(handle_type h) : h_(h) {}
    ~Generator() { if (h_) h_.destroy(); }
    
    Generator(Generator&& other) noexcept : h_(other.h_) { other.h_ = {}; }
    Generator& operator=(Generator&& other) noexcept {
        if (this != &other) {
            if (h_) h_.destroy();
            h_ = other.h_;
            other.h_ = {};
        }
        return *this;
    }
    
    struct iterator {
        handle_type h_;
        
        iterator& operator++() { h_.resume(); return *this; }
        bool operator==(std::default_sentinel_t) const {
            return !h_ || h_.done();
        }
        T const& operator*() const { return h_.promise().current_value; }
    };
    
    iterator begin() {
        if (h_) h_.resume();
        return iterator{h_};
    }
    std::default_sentinel_t end() { return {}; }
};

// Data record structure
struct DataRecord {
    int id;
    std::string content;
    double value;
    
    DataRecord(int i, std::string c, double v)
        : id(i), content(std::move(c)), value(v) {}
};

// 1. Data source generator (lazy evaluation)
Generator<DataRecord> read_data_source(const std::string& source) {
    std::cout << "[Generator] Starting to read from: " << source << "\n";
    
    // Simulate reading data lazily
    for (int i = 1; i <= 10; ++i) {
        // Simulate I/O delay
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        
        std::cout << "[Generator] Yielding record " << i << "\n";
        co_yield DataRecord{i, "Data_" + std::to_string(i), i * 10.5};
    }
    
    std::cout << "[Generator] Finished reading from: " << source << "\n";
}

// 2. Filter generator (lazy filtering)
template<typename Gen, typename Pred>
Generator<DataRecord> filter(Gen& gen, Pred predicate) {
    std::cout << "[Filter] Starting filter\n";
    
    for (auto& record : gen) {
        if (predicate(record)) {
            std::cout << "[Filter] Passing record " << record.id << "\n";
            co_yield record;
        } else {
            std::cout << "[Filter] Filtering out record " << record.id << "\n";
        }
    }
    
    std::cout << "[Filter] Finished filtering\n";
}

// 3. Transform generator (lazy transformation)
template<typename Gen, typename Func>
auto transform(Gen& gen, Func func) -> Generator<decltype(func(std::declval<DataRecord>()))> {
    std::cout << "[Transform] Starting transform\n";
    
    for (auto& record : gen) {
        std::cout << "[Transform] Transforming record " << record.id << "\n";
        co_yield func(record);
    }
    
    std::cout << "[Transform] Finished transforming\n";
}

// 4. Batching generator (accumulate N items)
Generator<std::vector<DataRecord>> batch(Generator<DataRecord>& gen, size_t batch_size) {
    std::cout << "[Batch] Starting batching (size=" << batch_size << ")\n";
    
    std::vector<DataRecord> current_batch;
    current_batch.reserve(batch_size);
    
    for (auto& record : gen) {
        current_batch.push_back(record);
        
        if (current_batch.size() >= batch_size) {
            std::cout << "[Batch] Yielding batch of " << current_batch.size() << "\n";
            co_yield std::move(current_batch);
            current_batch.clear();
            current_batch.reserve(batch_size);
        }
    }
    
    // Yield remaining items
    if (!current_batch.empty()) {
        std::cout << "[Batch] Yielding final batch of " << current_batch.size() << "\n";
        co_yield std::move(current_batch);
    }
    
    std::cout << "[Batch] Finished batching\n";
}

// 5. Take generator (limit number of items)
Generator<DataRecord> take(Generator<DataRecord>& gen, size_t count) {
    std::cout << "[Take] Taking first " << count << " items\n";
    
    size_t taken = 0;
    for (auto& record : gen) {
        if (taken >= count) break;
        
        std::cout << "[Take] Passing record " << record.id << "\n";
        co_yield record;
        taken++;
    }
    
    std::cout << "[Take] Finished taking\n";
}

// 6. Async processing simulation
struct AsyncResult {
    bool success;
    std::string message;
};

Generator<AsyncResult> async_process_batch(const std::vector<DataRecord>& batch) {
    std::cout << "[Async] Processing batch of " << batch.size() << " records\n";
    
    for (const auto& record : batch) {
        // Simulate async processing
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        
        bool success = (record.id % 3) != 0;  // Simulate failures
        
        AsyncResult result{
            success,
            success ? "Processed ID " + std::to_string(record.id)
                    : "Failed to process ID " + std::to_string(record.id)
        };
        
        co_yield result;
    }
}

// 7. Statistics accumulator using generator
struct Statistics {
    int total_records = 0;
    int successful = 0;
    int failed = 0;
    double sum_values = 0.0;
    
    void print() const {
        std::cout << "\n=== Statistics ===\n";
        std::cout << "Total records: " << total_records << "\n";
        std::cout << "Successful: " << successful << "\n";
        std::cout << "Failed: " << failed << "\n";
        if (total_records > 0) {
            std::cout << "Success rate: " 
                      << (100.0 * successful / total_records) << "%\n";
            std::cout << "Average value: " << (sum_values / total_records) << "\n";
        }
    }
};

// 8. Infinite sequence generator
Generator<int> infinite_sequence(int start = 0, int step = 1) {
    int current = start;
    while (true) {
        co_yield current;
        current += step;
    }
}

// 9. Fibonacci generator
Generator<uint64_t> fibonacci() {
    uint64_t a = 0, b = 1;
    co_yield a;
    co_yield b;
    
    while (true) {
        uint64_t next = a + b;
        co_yield next;
        a = b;
        b = next;
    }
}

// Main processing pipeline
void run_data_pipeline() {
    std::cout << "=== Data Processing Pipeline ===\n\n";
    
    Statistics stats;
    
    // Create pipeline: Read -> Filter -> Transform -> Batch -> Process
    auto data_source = read_data_source("database");
    
    // Filter: Only records with even IDs
    auto filtered = filter(data_source, [](const DataRecord& r) {
        return r.id % 2 == 0;
    });
    
    // Transform: Double the values
    auto transformed = transform(filtered, [](DataRecord r) {
        r.value *= 2.0;
        return r;
    });
    
    // Batch into groups of 2
    auto batched = batch(transformed, 2);
    
    // Process each batch
    std::cout << "\n--- Processing Batches ---\n";
    for (auto& current_batch : batched) {
        stats.total_records += current_batch.size();
        
        // Calculate sum before async processing
        for (const auto& record : current_batch) {
            stats.sum_values += record.value;
        }
        
        // Async process batch
        auto results = async_process_batch(current_batch);
        
        for (const auto& result : results) {
            if (result.success) {
                stats.successful++;
                std::cout << "  ✓ " << result.message << "\n";
            } else {
                stats.failed++;
                std::cout << "  ✗ " << result.message << "\n";
            }
        }
    }
    
    stats.print();
}

// Demonstrate lazy evaluation
void demonstrate_lazy_evaluation() {
    std::cout << "\n\n=== Lazy Evaluation Demo ===\n\n";
    
    // Create infinite sequence
    auto numbers = infinite_sequence(0, 1);
    
    // Take only first 5
    auto limited = take(numbers, 5);
    
    std::cout << "First 5 numbers: ";
    for (int n : limited) {
        std::cout << n << " ";
    }
    std::cout << "\n";
    
    // Fibonacci sequence
    std::cout << "\nFirst 10 Fibonacci numbers: ";
    auto fib = fibonacci();
    int count = 0;
    for (auto n : fib) {
        std::cout << n << " ";
        if (++count >= 10) break;
    }
    std::cout << "\n";
}

// Demonstrate composable generators
void demonstrate_composition() {
    std::cout << "\n\n=== Generator Composition ===\n\n";
    
    // Create a simple data source
    auto source = read_data_source("source");
    
    // Compose multiple operations
    auto filtered = filter(source, [](const DataRecord& r) {
        return r.value > 50.0;
    });
    
    auto limited = take(filtered, 3);
    
    std::cout << "Filtered and limited results:\n";
    for (const auto& record : limited) {
        std::cout << "  ID: " << record.id 
                  << ", Value: " << record.value << "\n";
    }
}

int main() {
    // Run full data pipeline
    run_data_pipeline();
    
    // Demonstrate lazy evaluation
    demonstrate_lazy_evaluation();
    
    // Demonstrate composition
    demonstrate_composition();
    
    std::cout << "\n=== Pipeline Complete ===\n";
    
    return 0;
}
```

### Concepts Demonstrated:
- **Generator pattern**: Lazy data generation
- **co_yield**: Suspending and returning values
- **Lazy evaluation**: Computing only when needed
- **Pipeline composition**: Chaining generators
- **Filter/Map**: Functional programming patterns
- **Batching**: Accumulating items
- **Infinite sequences**: Never-ending generators
- **State preservation**: Coroutine state across suspensions
- **Resource efficiency**: Processing one item at a time
- **Async simulation**: Asynchronous-style processing
- **Fibonacci**: Classic recursive generator
- **Take**: Limiting infinite sequences

This example shows how coroutines enable elegant data processing pipelines!

---

## Next Steps
- **Previous**: [← Modules](21_modules.md)
- **Back to**: [Table of Contents](README.md)

---
*Part 22 of 22 - Coroutines (C++20)*

## Summary

Coroutines are a powerful C++20 feature for:
- ✓ Writing async code that looks synchronous
- ✓ Creating lazy generators
- ✓ Building state machines
- ✓ Implementing cooperative multitasking
- ✓ Avoiding callback hell

Start with simple generators, then move to async tasks!

---

## 🎉 Congratulations!

You've completed the comprehensive C++ STL tutorial covering all 22 chapters with practical, working examples that integrate the concepts from each chapter!

Keep coding and exploring the power of modern C++! 🚀

---

