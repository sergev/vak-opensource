//
// Re-implementing Rust’s Box<T> in C++ is a fun challenge that
// lets us peek into Rust’s memory management internals and
// translate them into C++’s world. In Rust, Box<T> is a smart
// pointer that provides heap-allocated ownership with automatic
// cleanup, backed by the global allocator (usually malloc/free
// via Rust’s alloc crate). It’s simple but powerful: it allocates
// memory for a T, moves the value onto the heap, and deallocates
// it when the Box goes out of scope via Drop. In C++, we don’t
// have Rust’s ownership model or Drop trait, so we’ll use RAII
// and a custom class to mimic it.
//
// Let’s design a C++ equivalent, breaking down the essentials
// of Box<T> and handling the nuances of C++’s memory model.
//
// What Box<T> Does in Rust
// ~~~~~~~~~~~~~~~~~~~~~~~~
//  * Allocation: Allocates memory on the heap for a T using the global allocator.
//  * Ownership: Sole owner of the heap memory; no sharing or borrowing by default.
//  * Move Semantics: Transfers ownership when assigned or passed (no copying unless T: Clone).
//  * Deallocation: Automatically frees the memory when dropped,
//                  calling T’s destructor if it has one.
//  * Dereferencing: Provides access to T via * or . (through Deref).
//
// C++ Implementation
// ~~~~~~~~~~~~~~~~~~
// We’ll create a Box class in C++ that:
//  * Uses new and delete for allocation/deallocation (mirroring malloc/free).
//  * Implements RAII for automatic cleanup.
//  * Supports move semantics (C++11 and later) to mimic Rust’s ownership transfer.
//  * Prevents copying to enforce single ownership (like Rust).
//  * Provides dereference operators (* and ->).
//
#include <cassert>
#include <utility> // For std::move

template <typename T>
class Box {
private:
    T *ptr; // Raw pointer to heap-allocated memory

    // Private helper to deallocate
    void deallocate()
    {
        if (ptr) {
            delete ptr; // Calls T's destructor, then frees memory
            ptr = nullptr;
        }
    }

public:
    // Constructor: Allocate and construct T in-place
    template <typename... Args>
    explicit Box(Args &&...args) : ptr(new T(std::forward<Args>(args)...))
    {
    }

    // Destructor: Automatically clean up
    ~Box() { deallocate(); }

    // Disable copying (Rust Box doesn't allow this)
    Box(const Box &) = delete;
    Box &operator=(const Box &) = delete;

    // Move constructor: Transfer ownership
    Box(Box &&other) noexcept : ptr(other.ptr)
    {
        other.ptr = nullptr; // Leave other in a valid, empty state
    }

    // Move assignment: Transfer ownership
    Box &operator=(Box &&other) noexcept
    {
        if (this != &other) {
            deallocate();        // Free current memory
            ptr = other.ptr;     // Take ownership
            other.ptr = nullptr; // Leave other empty
        }
        return *this;
    }

    // Dereference operators
    T &operator*() { return *ptr; }
    const T &operator*() const { return *ptr; }

    T *operator->() { return ptr; }
    const T *operator->() const { return ptr; }

    // Optional: Raw pointer access (like Box::into_raw in Rust)
    T *as_raw() { return ptr; }

    // Optional: Take ownership back (like Box::from_raw, but consumes self)
    T into_inner()
    {
        T value = std::move(*ptr);
        deallocate();
        return value;
    }
};

// Factory function to simplify creation (like Box::new in Rust)
template <typename T, typename... Args>
Box<T> make_box(Args &&...args)
{
    return Box<T>(std::forward<Args>(args)...);
}

#include <iostream>

// Example usage
struct MyStruct {
    int x;
    MyStruct(int val) : x(val) { std::cout << "Constructed: " << x << "\n"; }
    ~MyStruct() { std::cout << "Destructed: " << x << "\n"; }
};

int main()
{
    // Create a Box
    auto box1 = make_box<MyStruct>(42);
    std::cout << "Value: " << box1->x << "\n";

    // Move it
    auto box2 = std::move(box1);
    std::cout << "Moved value: " << box2->x << "\n";
    // box1 is now invalid (ptr is nullptr), but safe to destroy

    // Access via dereference
    (*box2).x = 100;
    std::cout << "Updated value: " << box2->x << "\n";

    // Take ownership back
    MyStruct s = box2.into_inner();
    std::cout << "Extracted value: " << s.x << "\n";

    return 0;
}
