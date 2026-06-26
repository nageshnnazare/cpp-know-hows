// Module 15 — data race vs mutex vs atomic, futures, condition_variable.
#include <atomic>
#include <condition_variable>
#include <future>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

int main() {
    constexpr int kThreads = 4;
    constexpr int kIters = 100000;

    // 1) mutex-protected counter
    {
        long counter = 0;
        std::mutex m;
        std::vector<std::thread> ts;
        for (int t = 0; t < kThreads; ++t)
            ts.emplace_back([&] {
                for (int i = 0; i < kIters; ++i) {
                    std::lock_guard<std::mutex> lk(m);
                    ++counter;
                }
            });
        for (auto& t : ts) t.join();
        std::cout << "mutex counter   = " << counter
                  << " (expected " << (long)kThreads * kIters << ")\n";
    }

    // 2) atomic counter (lock-free, no mutex)
    {
        std::atomic<long> counter{0};
        std::vector<std::thread> ts;
        for (int t = 0; t < kThreads; ++t)
            ts.emplace_back([&] {
                for (int i = 0; i < kIters; ++i) counter.fetch_add(1);
            });
        for (auto& t : ts) t.join();
        std::cout << "atomic counter  = " << counter.load() << '\n';
    }

    // 3) future / async returns a result computed on another thread
    {
        std::future<long> f = std::async(std::launch::async, [] {
            long s = 0;
            for (int i = 1; i <= 1000; ++i) s += i;
            return s;
        });
        std::cout << "async sum 1..1000 = " << f.get() << '\n';
    }

    // 4) producer/consumer with a condition variable
    {
        std::mutex m;
        std::condition_variable cv;
        std::queue<int> q;
        bool done = false;
        long total = 0;

        std::thread consumer([&] {
            for (;;) {
                std::unique_lock<std::mutex> lk(m);
                cv.wait(lk, [&] { return !q.empty() || done; });  // predicate!
                while (!q.empty()) { total += q.front(); q.pop(); }
                if (done) break;
            }
        });

        for (int i = 1; i <= 10; ++i) {
            { std::lock_guard<std::mutex> lk(m); q.push(i); }
            cv.notify_one();
        }
        { std::lock_guard<std::mutex> lk(m); done = true; }
        cv.notify_one();
        consumer.join();
        std::cout << "consumer total  = " << total << " (expected 55)\n";
    }
}
