//
// Let’s dive into how we might implement Rust’s Cell<T> in C++.
// Rust’s Cell<T> is a fascinating type - it provides interior mutability,
// allowing you to mutate its contents even when you only have
// a shared reference, all while being safe in a single-threaded
// context. It’s lightweight, with no runtime overhead beyond
// the mutation itself, and it doesn’t involve locks since it’s
// not thread-safe (that’s what RefCell or atomics are for in Rust).
// In C++, we don’t have the exact same ownership and borrowing rules,
// so we’ll need to adapt the concept while preserving its spirit.
//
// The core idea of Cell<T> is simple: it wraps a value of type T
// and provides methods to get and set that value, bypassing C++’s
// usual const-correctness restrictions in a controlled way.
// Rust uses it to allow mutation through shared references, so
// in C++, we’ll aim for something that lets us modify a value even
// when we might otherwise be constrained, like through a const
// reference, while keeping the implementation straightforward.
//
// By making the value member mutable, we allow the Cell to modify
// its contents even when the Cell object itself is const,
// mimicking how Rust permits mutation through a shared reference.
//
#include <utility>
#include <iostream>

template <typename T>
class Cell {
private:
    mutable T value; // Mutable to allow mutation even when Cell is const

public:
    // Constructor
    explicit Cell(T initial) : value(std::move(initial)) {}

    // Get a copy of the value
    T get() const {
        return value;
    }

    // Set a new value
    void set(T new_value) const {
        value = std::move(new_value);
    }

    // Replace the value and return the old one
    T replace(T new_value) const {
        T old_value = std::move(value);
        value = std::move(new_value);
        return old_value;
    }
};

// Example usage
int main() {
    // Non-const Cell
    Cell<int> cell(42);
    std::cout << "Initial value: " << cell.get() << "\n"; // Prints 42

    cell.set(100);
    std::cout << "After set: " << cell.get() << "\n";    // Prints 100

    int old = cell.replace(200);
    std::cout << "Old value: " << old << "\n";           // Prints 100
    std::cout << "New value: " << cell.get() << "\n";    // Prints 200

    // Const Cell
    const Cell<int> const_cell(300);
    std::cout << "Const cell initial value: " << const_cell.get() << "\n"; // Prints 300

    const_cell.set(400); // Works despite constness!
    std::cout << "Const cell after set: " << const_cell.get() << "\n";     // Prints 400

    int old_const = const_cell.replace(500);
    std::cout << "Const cell old value: " << old_const << "\n";           // Prints 400
    std::cout << "Const cell new value: " << const_cell.get() << "\n";    // Prints 500

    return 0;
}
