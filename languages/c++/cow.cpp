//
// Here is a simplified version of Rust's std::borrow::Cow (Copy-on-Write)
// in C++. Rust's Cow is an enum that can hold either a borrowed
// reference (&'a T) or an owned value (T), typically used to avoid
// unnecessary cloning. In C++, we can approximate this using
// a combination of templates, variants, and smart pointers.
//
#include <variant>
#include <memory>
#include <string>
#include <stdexcept>

// A simplified Cow implementation in C++
template<typename T>
class Cow {
private:
    // Using variant to hold either a borrowed pointer or an owned value
    std::variant<const T*, std::unique_ptr<T>> data;

public:
    // Constructor for borrowed value
    explicit Cow(const T* borrowed) : data(borrowed) {
        if (!borrowed) {
            throw std::invalid_argument("Borrowed pointer cannot be null");
        }
    }

    // Constructor for owned value
    explicit Cow(T&& owned) : data(std::make_unique<T>(std::move(owned))) {}

    // Copy constructor
    Cow(const Cow& other) {
        if (std::holds_alternative<const T*>(other.data)) {
            data = std::get<const T*>(other.data);
        } else {
            const auto& owned = std::get<std::unique_ptr<T>>(other.data);
            data = std::make_unique<T>(*owned);
        }
    }

    // Move constructor
    Cow(Cow&& other) noexcept = default;

    // Assignment operators
    Cow& operator=(const Cow& other) {
        if (this != &other) {
            Cow temp(other);
            swap(*this, temp);
        }
        return *this;
    }

    Cow& operator=(Cow&& other) noexcept = default;

    // Access the underlying value (non-mutating)
    const T& operator*() const {
        if (std::holds_alternative<const T*>(data)) {
            return *std::get<const T*>(data);
        }
        return *std::get<std::unique_ptr<T>>(data);
    }

    const T* operator->() const {
        return &(**this);
    }

    // Check if the Cow holds a borrowed value
    bool is_borrowed() const {
        return std::holds_alternative<const T*>(data);
    }

    // Check if the Cow holds an owned value
    bool is_owned() const {
        return std::holds_alternative<std::unique_ptr<T>>(data);
    }

    // Convert to owned value if borrowed (clone on write)
    void to_owned() {
        if (is_borrowed()) {
            data = std::make_unique<T>(*std::get<const T*>(data));
        }
    }

    // Swap implementation
    friend void swap(Cow& lhs, Cow& rhs) noexcept {
        using std::swap;
        swap(lhs.data, rhs.data);
    }
};

// Example usage
#include <iostream>

int main() {
    // Create a string we'll borrow from
    std::string original = "Hello";

    // Borrowed Cow
    Cow<std::string> borrowed(&original);
    std::cout << "Borrowed: " << *borrowed << ", Is borrowed: " << borrowed.is_borrowed() << "\n";

    // Owned Cow
    Cow<std::string> owned(std::string("World"));
    std::cout << "Owned: " << *owned << ", Is owned: " << owned.is_owned() << "\n";

    // Copy a borrowed Cow
    Cow<std::string> borrowed_copy = borrowed;
    std::cout << "Borrowed copy: " << *borrowed_copy << "\n";

    // Convert borrowed to owned
    borrowed_copy.to_owned();
    std::cout << "After to_owned: " << *borrowed_copy << ", Is owned: " << borrowed_copy.is_owned() << "\n";

    return 0;
}

//
// Expected output:
//
// Borrowed: Hello, Is borrowed: 1
// Owned: World, Is owned: 1
// Borrowed copy: Hello
// After to_owned: Hello, Is owned: 1
//
