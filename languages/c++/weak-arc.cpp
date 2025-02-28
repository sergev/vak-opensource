//
// Let’s implement a version of Rust’s Weak for Arc (Atomic
// Reference Counting) in C++. In Rust, Arc is similar to Rc but
// is thread-safe, using atomic operations for reference counting.
// This allows Arc to be shared across threads, and its companion
// Weak provides a non-owning reference that can be upgraded to an
// Arc if the data still exists. The key difference from Rc is
// that Arc uses atomic counters (std::sync::atomic::AtomicUsize
// in Rust) instead of plain integers, ensuring safe concurrent
// access.
//
// In C++, we’ll use std::atomic<size_t> for the counters to mimic
// this thread-safety. The structure will be similar to our Rc and
// Weak implementation, but we’ll adjust the reference counting to
// be atomic.
//
#include <iostream>
#include <memory>
#include <optional>
#include <atomic>
#include <thread>

// Forward declaration of Weak
template <typename T>
class Weak;

class ArcControlBlock {
public:
    std::atomic<size_t> strong_count;
    std::atomic<size_t> weak_count;

    ArcControlBlock() : strong_count(1), weak_count(0) {}
};

template <typename T>
class Arc {
private:
    T* data;
    ArcControlBlock* control;

    // Private constructor for internal use
    Arc(T* data, ArcControlBlock* control) : data(data), control(control) {}

    // Friend declaration to allow Weak<T> to access private constructor
    friend class Weak<T>;

public:
    // Constructor: Creates a new Arc with data
    static Arc<T> new_arc(T value) {
        T* data_ptr = new T(std::move(value));
        ArcControlBlock* ctrl = new ArcControlBlock();
        return Arc(data_ptr, ctrl);
    }

    // Copy constructor: Atomically increments strong count
    Arc(const Arc<T>& other) : data(other.data), control(other.control) {
        control->strong_count.fetch_add(1, std::memory_order_relaxed);
    }

    // Destructor: Atomically decrements strong count and cleans up if needed
    ~Arc() {
        if (control->strong_count.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            delete data;
            if (control->weak_count.load(std::memory_order_acquire) == 0) {
                delete control;
            }
        }
    }

    // Access the data (not thread-safe by itself; user must synchronize if needed)
    T& operator*() const { return *data; }
    T* operator->() const { return data; }

    // Getter methods for demonstration
    size_t get_strong_count() const { return control->strong_count.load(std::memory_order_relaxed); }
    size_t get_weak_count() const { return control->weak_count.load(std::memory_order_relaxed); }

    // Declare create_weak
    Weak<T> create_weak() const;
};

template <typename T>
class Weak {
private:
    T* data;
    ArcControlBlock* control;

    friend class Arc<T>; // Allow Arc to construct Weak

    Weak(T* data, ArcControlBlock* control) : data(data), control(control) {
        control->weak_count.fetch_add(1, std::memory_order_relaxed);
    }

public:
    // Default constructor for invalid Weak
    Weak() : data(nullptr), control(nullptr) {}

    // Copy constructor: Atomically increments weak count
    Weak(const Weak<T>& other) : data(other.data), control(other.control) {
        if (control) {
            control->weak_count.fetch_add(1, std::memory_order_relaxed);
        }
    }

    // Destructor: Atomically decrements weak count and cleans up if needed
    ~Weak() {
        if (control) {
            if (control->weak_count.fetch_sub(1, std::memory_order_acq_rel) == 1 &&
                control->strong_count.load(std::memory_order_acquire) == 0) {
                delete control;
            }
        }
    }

    // Attempt to upgrade to an Arc if the data still exists
    std::optional<Arc<T>> upgrade() const {
        if (control) {
            size_t old_count = control->strong_count.load(std::memory_order_acquire);
            while (old_count > 0) {
                if (control->strong_count.compare_exchange_weak(old_count, old_count + 1,
                                                                std::memory_order_acq_rel,
                                                                std::memory_order_relaxed)) {
                    return Arc<T>(data, control);
                }
            }
        }
        return std::nullopt;
    }
};

// Define create_weak after both classes are fully defined
template <typename T>
Weak<T> Arc<T>::create_weak() const {
    return Weak<T>(data, control);
}

// Example usage with threading
void thread_func(Arc<int> arc) {
    std::cout << "Thread sees value: " << *arc << "\n";
    std::cout << "Thread strong count: " << arc.get_strong_count() << ", weak count: " << arc.get_weak_count() << "\n";
}

int main() {
    // Create an Arc
    auto arc = Arc<int>::new_arc(42);
    std::cout << "Strong count: " << arc.get_strong_count() << ", Weak count: " << arc.get_weak_count() << "\n";

    // Create a Weak pointer
    auto weak = arc.create_weak();
    std::cout << "Strong count: " << arc.get_strong_count() << ", Weak count: " << arc.get_weak_count() << "\n";

    // Upgrade the Weak to an Arc
    if (auto upgraded = weak.upgrade()) {
        std::cout << "Upgraded value: " << **upgraded << "\n";
        std::cout << "Strong count: " << arc.get_strong_count() << ", Weak count: " << arc.get_weak_count() << "\n";
    }

    // Demonstrate thread-safety
    std::thread t(thread_func, arc);
    t.join();

    // Drop the original Arc and test weak
    {
        auto arc_copy = arc;
        std::cout << "Strong count after copy: " << arc.get_strong_count() << "\n";
    } // arc_copy goes out of scope

    // Test weak after dropping one Arc
    if (auto upgraded = weak.upgrade()) {
        std::cout << "Still alive: " << **upgraded << "\n";
    } else {
        std::cout << "Data has been deallocated\n";
    }

    return 0;
}
