// Module 06 — copy vs move, Rule of 5, copy-and-swap, noexcept moves.
// Watch the printed ctor/dtor trace to SEE when copies vs moves happen.
#include <algorithm>
#include <iostream>
#include <utility>
#include <vector>

class IntBuffer {
    int*        data_ = nullptr;
    std::size_t size_ = 0;
    static inline int id_counter = 0;
    int id_ = ++id_counter;
public:
    IntBuffer() = default;
    explicit IntBuffer(std::size_t n) : data_(new int[n]{}), size_(n) {
        std::cout << "  ctor #" << id_ << " (n=" << n << ")\n";
    }
    ~IntBuffer() { std::cout << "  dtor #" << id_ << "\n"; delete[] data_; }

    IntBuffer(const IntBuffer& o) : data_(new int[o.size_]), size_(o.size_) {
        std::copy(o.data_, o.data_ + size_, data_);
        std::cout << "  COPY ctor #" << id_ << " <- #" << o.id_ << "\n";
    }
    IntBuffer(IntBuffer&& o) noexcept
        : data_(o.data_), size_(o.size_) {
        o.data_ = nullptr; o.size_ = 0;
        std::cout << "  MOVE ctor #" << id_ << " <- #" << o.id_ << "\n";
    }
    IntBuffer& operator=(IntBuffer o) noexcept { // by value: handles copy & move + self-assign
        swap(o);
        std::cout << "  assign (copy-and-swap) into #" << id_ << "\n";
        return *this;
    }
    void swap(IntBuffer& o) noexcept {
        std::swap(data_, o.data_);
        std::swap(size_, o.size_);
    }
    std::size_t size() const { return size_; }
};

int main() {
    std::cout << "make a:\n";
    IntBuffer a(4);

    std::cout << "copy b = a:\n";
    IntBuffer b = a;            // COPY ctor

    std::cout << "move c = move(a):\n";
    IntBuffer c = std::move(a); // MOVE ctor; a now empty-but-valid

    std::cout << "push into vector (watch reallocation move):\n";
    std::vector<IntBuffer> v;
    v.reserve(2);               // avoid extra reallocs for clarity
    v.push_back(IntBuffer(2));  // move from temporary
    v.push_back(IntBuffer(3));  // move from temporary

    std::cout << "end of main (destructors run in reverse):\n";
}
