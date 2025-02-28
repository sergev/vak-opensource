#include <utility>

template <typename T>
class Cell {
private:
    T value;

public:
    // Constructor
    explicit Cell(T initial) : value(std::move(initial)) {}

    // Get a copy of the value
    T get() const {
        return value;
    }

    // Set a new value
    void set(T new_value) {
        value = std::move(new_value);
    }

    // Replace the value and return the old one
    T replace(T new_value) {
        T old_value = std::move(value);
        value = std::move(new_value);
        return old_value;
    }
};

// Example usage
#include <iostream>

int main() {
    Cell<int> cell(42);

    std::cout << "Initial value: " << cell.get() << "\n"; // Prints 42

    cell.set(100);
    std::cout << "After set: " << cell.get() << "\n";    // Prints 100

    int old = cell.replace(200);
    std::cout << "Old value: " << old << "\n";           // Prints 100
    std::cout << "New value: " << cell.get() << "\n";    // Prints 200

    // Using with const
    const Cell<int> const_cell(300);
    std::cout << "Const cell value: " << const_cell.get() << "\n"; // Prints 300
    // const_cell.set(400); // This won’t compile, as expected

    return 0;
}
