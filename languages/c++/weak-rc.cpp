//
// Let’s dive into implementing a Weak pointer analogous to Rust’s
// Weak for Rc in C++. In Rust, Rc (Reference Counting) provides
// shared ownership of data, and Weak is a non-owning reference
// that prevents reference cycles and allows access to the data
// only if it still exists (i.e., if the strong reference count
// hasn’t dropped to zero). We’ll replicate this behavior in C++
// using a combination of smart pointers and manual reference counting.
//
// In C++, the closest equivalent to Rc is std::shared_ptr, which
// manages a strong reference count. For Weak, we’ll mimic Rust’s
// Weak by using std::weak_ptr, which is designed to work with
// std::shared_ptr and provides a non-owning reference. However,
// to make this a clear learning exercise and align it closely
// with Rust’s semantics, I’ll first show a simplified manual
// implementation before tying it to the standard library.
//
// Rust’s Rc and Weak rely on two counters:
//  * Strong count: Number of owning Rc references.
//  * Weak count: Number of non-owning Weak references.
//
// The data is deallocated when the strong count reaches zero, and
// the control block (holding the counters) is deallocated when
// both strong and weak counts reach zero. Let’s implement this in C++.
//
#include <iostream>
#include <memory>
#include <optional>

// Forward declaration of Weak
template <typename T>
class Weak;

class RcControlBlock {
public:
    size_t strong_count;
    size_t weak_count;

    RcControlBlock() : strong_count(1), weak_count(0) {}
};

template <typename T>
class Rc {
private:
    T* data;
    RcControlBlock* control;

    // Private constructor for internal use
    Rc(T* data, RcControlBlock* control) : data(data), control(control) {}

    // Friend declaration to allow Weak<T> to access private constructor
    friend class Weak<T>;

public:
    // Constructor: Creates a new Rc with data
    static Rc<T> new_rc(T value) {
        T* data_ptr = new T(std::move(value));
        RcControlBlock* ctrl = new RcControlBlock();
        return Rc(data_ptr, ctrl);
    }

    // Copy constructor: Increments strong count
    Rc(const Rc<T>& other) : data(other.data), control(other.control) {
        control->strong_count++;
    }

    // Destructor: Decrements strong count and cleans up if needed
    ~Rc() {
        control->strong_count--;
        if (control->strong_count == 0) {
            delete data;
            if (control->weak_count == 0) {
                delete control;
            }
        }
    }

    // Access the data
    T& operator*() const { return *data; }
    T* operator->() const { return data; }

    // Getter methods for demonstration
    size_t get_strong_count() const { return control->strong_count; }
    size_t get_weak_count() const { return control->weak_count; }

    // Declare create_weak
    Weak<T> create_weak() const;
};

template <typename T>
class Weak {
private:
    T* data;
    RcControlBlock* control;

    friend class Rc<T>; // Allow Rc to construct Weak

    Weak(T* data, RcControlBlock* control) : data(data), control(control) {
        control->weak_count++;
    }

public:
    // Default constructor for invalid Weak
    Weak() : data(nullptr), control(nullptr) {}

    // Copy constructor
    Weak(const Weak<T>& other) : data(other.data), control(other.control) {
        if (control) {
            control->weak_count++;
        }
    }

    // Destructor: Decrements weak count and cleans up if needed
    ~Weak() {
        if (control) {
            control->weak_count--;
            if (control->strong_count == 0 && control->weak_count == 0) {
                delete control;
            }
        }
    }

    // Attempt to upgrade to an Rc if the data still exists
    std::optional<Rc<T>> upgrade() const {
        if (control && control->strong_count > 0) {
            control->strong_count++;
            return Rc<T>(data, control); // Now legal due to friend declaration
        }
        return std::nullopt;
    }
};

// Define create_weak after both classes are fully defined
template <typename T>
Weak<T> Rc<T>::create_weak() const {
    return Weak<T>(data, control);
}

// Example usage
int main() {
    // Create an Rc
    auto rc = Rc<int>::new_rc(42);
    std::cout << "Strong count: " << rc.get_strong_count() << ", Weak count: " << rc.get_weak_count() << "\n";

    // Create a Weak pointer
    auto weak = rc.create_weak();
    std::cout << "Strong count: " << rc.get_strong_count() << ", Weak count: " << rc.get_weak_count() << "\n";

    // Upgrade the Weak to an Rc
    if (auto upgraded = weak.upgrade()) {
        std::cout << "Upgraded value: " << **upgraded << "\n";
        std::cout << "Strong count: " << rc.get_strong_count() << ", Weak count: " << rc.get_weak_count() << "\n";
    }

    // Drop the original Rc temporarily
    {
        auto rc_copy = rc; // Increase strong count
        std::cout << "Strong count after copy: " << rc.get_strong_count() << "\n";
    } // rc_copy goes out of scope, strong count decreases

    // After rc_copy is dropped, try upgrading weak
    if (auto upgraded = weak.upgrade()) {
        std::cout << "Still alive: " << **upgraded << "\n";
    } else {
        std::cout << "Data has been deallocated\n";
    }

    return 0;
}
