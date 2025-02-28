//
// Let’s tackle implementing Rust’s RefCell<T> in C++. This is
// an exciting challenge because RefCell<T> takes interior mutability
// a step further than Cell<T>. While Cell<T> allows mutation
// through a shared reference with a simple copy or swap,
// RefCell<T> provides borrowed access—either immutable (shared)
// or mutable (exclusive)—with runtime checks to enforce Rust’s
// borrowing rules: you can have many immutable borrows or one
// mutable borrow, but never both at once. In Rust, this is all
// about safety without locks, and we’ll aim to replicate that
// dynamic enforcement in C++.
//
// In C++, we don’t have a borrow checker, so we’ll use a runtime
// mechanism to track borrows, similar to how RefCell uses a counter
// in Rust. The implementation will involve:
//
//  * A container for the value.
//  * Counters for active immutable and mutable borrows.
//  * Methods to borrow the value immutably (borrow) or mutably (borrow_mut),
//    returning smart pointers that enforce the borrowing rules.
//  * Runtime checks to panic (or throw, in C++ terms) if the rules are violated.
//
#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>

template <typename T>
class RefCell {
private:
    T value;
    mutable size_t borrow_count;  // Tracks immutable borrows
    mutable bool borrowed_mut;    // Tracks if there's a mutable borrow

public:
    explicit RefCell(T initial) : value(std::move(initial)), borrow_count(0), borrowed_mut(false) {}

    class Ref {
    private:
        const RefCell<T>* cell;
        friend class RefCell<T>;

        Ref(const RefCell<T>* c) : cell(c) {
            if (cell->borrowed_mut) {
                throw std::runtime_error("Already mutably borrowed");
            }
            cell->borrow_count++;
        }

    public:
        ~Ref() {
            if (cell) {
                cell->borrow_count--;
            }
        }

        Ref(const Ref&) = delete;
        Ref& operator=(const Ref&) = delete;
        Ref(Ref&& other) noexcept : cell(other.cell) { other.cell = nullptr; }

        const T& get() const { return cell->value; }
    };

    class RefMut {
    private:
        RefCell<T>* cell;
        friend class RefCell<T>;

        RefMut(RefCell<T>* c) : cell(c) {
            if (cell->borrow_count > 0 || cell->borrowed_mut) {
                throw std::runtime_error("Already borrowed");
            }
            cell->borrowed_mut = true;
        }

    public:
        ~RefMut() {
            if (cell) {
                cell->borrowed_mut = false;
            }
        }

        RefMut(const RefMut&) = delete;
        RefMut& operator=(const RefMut&) = delete;
        RefMut(RefMut&& other) noexcept : cell(other.cell) { other.cell = nullptr; }

        T& get() { return cell->value; }
    };

    Ref borrow() const { return Ref(this); }
    RefMut borrow_mut() { return RefMut(this); }
};

//
// Running the example gives:
//
// Immutable borrow: 42
// Second immutable borrow: 42
// After mutable borrow: 100
// Final value with multiple borrows: 100
// Error: Already borrowed
//
int main() {
    RefCell<int> cell(42);

    // Immutable borrows
    {
        auto ref1 = cell.borrow();
        std::cout << "Immutable borrow: " << ref1.get() << "\n";

        auto ref2 = cell.borrow();
        std::cout << "Second immutable borrow: " << ref2.get() << "\n";
    }

    // Mutable borrow
    {
        auto mut_ref = cell.borrow_mut();
        mut_ref.get() = 100;
    }
    std::cout << "After mutable borrow: " << cell.borrow().get() << "\n";

    // Multiple immutable borrows
    auto ref3 = cell.borrow();
    auto ref4 = cell.borrow();
    std::cout << "Final value with multiple borrows: " << ref3.get() << "\n";

    // Demonstrate failure
    try {
        auto mut_ref = cell.borrow_mut(); // Should fail
    } catch (const std::runtime_error& e) {
        std::cout << "Error: " << e.what() << "\n";
    }

    return 0;
}
