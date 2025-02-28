//
// Let’s re-implement Rust’s Rc<T> in C++—a fascinating exercise
// since Rc<T> introduces reference-counted shared ownership,
// contrasting with the unique ownership of Box<T>. In Rust, Rc<T>
// (short for "Reference Counted") allows multiple owners of a
// heap-allocated T, tracking the number of references via a
// counter. When the last Rc is dropped, the T is deallocated.
// This is akin to C++’s std::shared_ptr<T>, but we’ll craft a
// custom version to mirror Rust’s Rc<T> behavior, focusing on its
// single-threaded nature (unlike Arc<T> for multi-threading).
//
// What Rc<T> Does in Rust
// ~~~~~~~~~~~~~~~~~~~~~~~
//  * Allocation: Places a T on the heap with a reference count.
//  * Shared Ownership: Multiple Rc<T> instances can point to the same T, incrementing the count.
//  * Cloning: Rc::clone creates a new reference, bumping the count.
//  * Deallocation: When the last Rc is dropped (count hits 0), T is destroyed and memory freed.
//  * No Mutability: Rc<T> alone doesn’t allow mutation (use RefCell<T> or Cell<T> for that).
//  * Non-Atomic: Single-threaded, unlike Arc<T>.
//
// C++ Implementation
// ~~~~~~~~~~~~~~~~~~
// We’ll create an Rc<T> class in C++ that:
//
//  * Uses a control block to store the reference count and T together.
//  * Supports copying (to increment the count) and RAII cleanup.
//  * Mimics Rust’s immutability by providing const access (mutation would need a separate mechanism).
//  * Avoids atomic operations, matching Rc’s single-threaded design.
//
#include <cassert>
#include <utility>
#include <iostream>

// Control block to hold the reference count and the value
template <typename T>
struct RcControlBlock {
    size_t count;
    T value;

    template <typename... Args>
    RcControlBlock(Args&&... args) : count(1), value(std::forward<Args>(args)...) {}
};

template <typename T>
class Rc {
private:
    RcControlBlock<T>* block;

    // Private constructor from raw block (for internal use)
    explicit Rc(RcControlBlock<T>* b) : block(b) {}

    void increment() {
        if (block) {
            ++block->count;
        }
    }

    void decrement() {
        if (block && --block->count == 0) {
            delete block;
            block = nullptr;
        }
    }

public:
    // Constructor: Allocate and construct T
    template <typename... Args>
    explicit Rc(Args&&... args) : block(new RcControlBlock<T>(std::forward<Args>(args)...)) {}

    // Destructor: Decrease count, cleanup if last reference
    ~Rc() {
        decrement();
    }

    // Copy constructor: Share ownership
    Rc(const Rc& other) : block(other.block) {
        increment();
    }

    // Copy assignment: Share ownership
    Rc& operator=(const Rc& other) {
        if (this != &other) {
            decrement();           // Drop old reference
            block = other.block;   // Take new reference
            increment();
        }
        return *this;
    }

    // Move constructor: Transfer pointer, no count change
    Rc(Rc&& other) noexcept : block(other.block) {
        other.block = nullptr; // Leave moved-from empty
    }

    // Move assignment: Transfer pointer, no count change
    Rc& operator=(Rc&& other) noexcept {
        if (this != &other) {
            decrement();           // Drop old reference
            block = other.block;   // Take new reference
            other.block = nullptr; // Leave moved-from empty
        }
        return *this;
    }

    // Accessors (const only, mimicking Rust's immutability)
    const T& operator*() const {
        assert(block != nullptr);
        return block->value;
    }

    const T* operator->() const {
        assert(block != nullptr);
        return &block->value;
    }

    // Clone method (like Rc::clone)
    Rc clone() const {
        return Rc(*this); // Uses copy constructor
    }

    // Get reference count (for debugging)
    size_t use_count() const {
        return block ? block->count : 0;
    }
};

// Factory function (like Rc::new)
template <typename T, typename... Args>
Rc<T> make_rc(Args&&... args) {
    return Rc<T>(std::forward<Args>(args)...);
}

// Example usage
struct MyStruct {
    int x;
    MyStruct(int val) : x(val) { std::cout << "Constructed: " << x << "\n"; }
    ~MyStruct() { std::cout << "Destructed: " << x << "\n"; }
};

int main() {
    // Create an Rc
    auto rc1 = make_rc<MyStruct>(42);
    std::cout << "Value: " << rc1->x << "\n";
    std::cout << "Count: " << rc1.use_count() << "\n"; // 1

    // Clone it
    auto rc2 = rc1.clone();
    std::cout << "Cloned value: " << rc2->x << "\n";
        std::cout << "Count: " << rc1.use_count() << "\n"; // 2

    // Move it
    auto rc3 = std::move(rc2);
    std::cout << "Moved value: " << rc3->x << "\n";
    std::cout << "Count: " << rc1.use_count() << "\n"; // 2 (rc2 is null)

    // Scope-based cleanup
    {
        auto rc4 = rc1.clone();
        std::cout << "Count in scope: " << rc1.use_count() << "\n"; // 3
    } // rc4 drops here
    std::cout << "Count after scope: " << rc1.use_count() << "\n"; // 2

    return 0; // rc1, rc3 drop, count hits 0, MyStruct destructs
}
