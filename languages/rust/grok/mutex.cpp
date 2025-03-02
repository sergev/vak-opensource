//
// Here is an implementation of a Mutex<T> type in C++ that mimics
// Rust's Mutex. Rust's Mutex provides mutual exclusion with a
// lock guard pattern, so we'll create a similar construct in C++
// using the standard library's threading primitives.
//
#include <mutex>
#include <memory>

template<typename T>
class Mutex {
private:
    T data;
    std::mutex mtx;

public:
    // Constructor
    explicit Mutex(T value) : data(std::move(value)) {}

    // Lock guard class (similar to Rust's MutexGuard)
    class LockGuard {
    private:
        T& data_ref;
        std::unique_lock<std::mutex> lock;

    public:
        LockGuard(T& d, std::mutex& m)
            : data_ref(d), lock(m) {}

        // Access the underlying data
        T& operator*() { return data_ref; }
        T* operator->() { return &data_ref; }
    };

    // Lock method that returns the guard
    LockGuard lock() {
        return LockGuard(data, mtx);
    }

    // Prevent copying
    Mutex(const Mutex&) = delete;
    Mutex& operator=(const Mutex&) = delete;

    // Allow move construction
    Mutex(Mutex&&) = default;
    Mutex& operator=(Mutex&&) = default;
};

// Example usage
#include <iostream>
#include <thread>

void example() {
    Mutex<int> counter(0);

    auto worker = [&counter](int id) {
        for (int i = 0; i < 100; ++i) {
            auto guard = counter.lock();
            *guard += 1;
            std::cout << "Thread " << id << " incremented to " << *guard << "\n";
        }
    };

    std::thread t1(worker, 1);
    std::thread t2(worker, 2);

    t1.join();
    t2.join();

    auto final = counter.lock();
    std::cout << "Final value: " << *final << "\n";
}

int main() {
    example();
    return 0;
}
