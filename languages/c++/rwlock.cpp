//
// Here is a C++ implementation of RwLock<T> similar to Rust's RwLock,
// which provides a reader-writer lock allowing multiple readers
// or one writer. We'll use C++'s std::shared_mutex (available
// since C++17) as the underlying synchronization primitive.
//
#include <shared_mutex>
#include <memory>

template<typename T>
class RwLock {
private:
    T data;
    mutable std::shared_mutex mtx;  // mutable to allow locking in const methods

public:
    // Constructor
    explicit RwLock(T value) : data(std::move(value)) {}

    // Read guard class (multiple readers allowed)
    class ReadGuard {
    private:
        const T& data_ref;
        std::shared_lock<std::shared_mutex> lock;

    public:
        ReadGuard(const T& d, std::shared_mutex& m)
            : data_ref(d), lock(m) {}

        const T& operator*() const { return data_ref; }
        const T* operator->() const { return &data_ref; }
    };

    // Write guard class (exclusive writer)
    class WriteGuard {
    private:
        T& data_ref;
        std::unique_lock<std::shared_mutex> lock;

    public:
        WriteGuard(T& d, std::shared_mutex& m)
            : data_ref(d), lock(m) {}

        T& operator*() { return data_ref; }
        T* operator->() { return &data_ref; }
    };

    // Lock methods
    ReadGuard read_lock() const {
        return ReadGuard(data, mtx);
    }

    WriteGuard write_lock() {
        return WriteGuard(data, mtx);
    }

    // Prevent copying
    RwLock(const RwLock&) = delete;
    RwLock& operator=(const RwLock&) = delete;

    // Allow move construction
    RwLock(RwLock&&) = default;
    RwLock& operator=(RwLock&&) = default;
};

// Example usage
#include <iostream>
#include <thread>
#include <vector>

void example() {
    RwLock<int> counter(0);

    auto reader = [&counter](int id) {
        for (int i = 0; i < 5; ++i) {
            auto guard = counter.read_lock();
            std::cout << "Reader " << id << " read: " << *guard << "\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    };

    auto writer = [&counter](int id) {
        for (int i = 0; i < 5; ++i) {
            auto guard = counter.write_lock();
            *guard += 1;
            std::cout << "Writer " << id << " wrote: " << *guard << "\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
    };

    std::vector<std::thread> threads;
    // Spawn multiple readers
    for (int i = 1; i <= 3; ++i) {
        threads.emplace_back(reader, i);
    }
    // Spawn one writer
    threads.emplace_back(writer, 1);

    for (auto& t : threads) {
        t.join();
    }

    auto final = counter.read_lock();
    std::cout << "Final value: " << *final << "\n";
}

int main() {
    example();
    return 0;
}

// Expected output:
// Reader 1 read: 0
// Reader 3 read: 0
// Reader 2 read: 0
// Writer 1 wrote: 1
// Writer 1 wrote: 2
// Writer 1 wrote: 3
// Writer 1 wrote: 4
// Writer 1 wrote: 5
// Reader 3 read: 5
// Reader 1 read: 5
// Reader 2 read: 5
// Reader 3 read: 5
// Reader 1 read: 5
// Reader 2 read: 5
// Reader 2 read: 5
// Reader 1 read: 5
// Reader 3 read: 5
// Reader 1 read: 5
// Reader 3 read: 5
// Reader 2 read: 5
// Final value: 5
