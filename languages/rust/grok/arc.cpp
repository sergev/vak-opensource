//
// Let’s dive into how we might implement something like Rust’s
// Arc<T> in C++. For those unfamiliar, Arc<T> in Rust is
// a thread-safe, reference-counted smart pointer that allows
// multiple owners of some data, incrementing a counter when
// cloned and decrementing it when dropped, freeing the data when
// the count hits zero. It’s built for concurrent use, so it uses
// atomic operations to manage the count safely across threads.
// We’ll aim to replicate that behavior in C++.
//
// In C++, we don’t have a direct equivalent in the standard
// library, but std::shared_ptr comes close. However,
// std::shared_ptr isn’t guaranteed to use atomic reference
// counting for the control block in all implementations
// (pre-C++20, at least), and its atomic operations were only
// standardized with std::atomic<std::shared_ptr> in C++20.
// To truly mirror Arc<T>, we’ll roll our own simplified version
// using atomic counters, focusing on thread-safety and the core
// mechanics.
//
#include <atomic>
#include <memory>
#include <utility>
#include <type_traits>

// Define Inner as a standalone template struct
template <typename T>
struct ArcInner {
    T data;
    std::atomic<int> ref_count;

    template <typename... Args>
    ArcInner(Args&&... args) : data(std::forward<Args>(args)...), ref_count(1) {}
};

template <typename T>
class Arc {
private:
    ArcInner<T>* ptr;

    void increment_count() {
        if (ptr) {
            ptr->ref_count.fetch_add(1, std::memory_order_relaxed);
        }
    }

    void decrement_count() {
        if (ptr) {
            if (ptr->ref_count.fetch_sub(1, std::memory_order_acq_rel) == 1) {
                delete ptr;
                ptr = nullptr;
            }
        }
    }

public:
    // Constructor with constraint: Args must be convertible to T
    template <typename... Args, typename = std::enable_if_t<std::is_constructible_v<T, Args...>>>
    explicit Arc(Args&&... args) : ptr(new ArcInner<T>(std::forward<Args>(args)...)) {}

    // Copy constructor
    Arc(const Arc& other) : ptr(other.ptr) {
        increment_count();
    }

    // Move constructor
    Arc(Arc&& other) noexcept : ptr(other.ptr) {
        other.ptr = nullptr;
    }

    // Assignment operators
    Arc& operator=(const Arc& other) {
        if (this != &other) {
            decrement_count();
            ptr = other.ptr;
            increment_count();
        }
        return *this;
    }

    Arc& operator=(Arc&& other) noexcept {
        if (this != &other) {
            decrement_count();
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }

    // Destructor
    ~Arc() {
        decrement_count();
    }

    // Dereference operators
    T& operator*() const { return ptr->data; }
    T* operator->() const { return &ptr->data; }

    // Get the reference count
    int use_count() const { return ptr ? ptr->ref_count.load(std::memory_order_relaxed) : 0; }
};

// Helper function to create an Arc
template <typename T, typename... Args>
Arc<T> make_arc(Args&&... args) {
    return Arc<T>(std::forward<Args>(args)...);
}

#include <iostream>
#include <thread>

// Test function for threading
void thread_func(Arc<int> arc) {
    std::cout << "Thread value: " << *arc << ", count: " << arc.use_count() << "\n";
}

int main() {
    auto arc = make_arc<int>(42); // Create Arc with int value 42
    std::cout << "Main value: " << *arc << ", count: " << arc.use_count() << "\n";

    std::thread t(thread_func, arc);
    t.join();

    std::cout << "After thread, count: " << arc.use_count() << "\n";
    return 0;
}
