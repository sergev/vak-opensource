//
// Let's create a C++ implementation of Rust's Option type.
// In Rust, Option is an enum that represents either Some(value)
// or None, used for handling cases where a value might be absent.
// Here's a C++ version that mimics this behavior while adapting
// to C++ idioms:
//
#include <utility>
#include <stdexcept>

template <typename T>
class Option {
private:
    bool has_value;
    union {
        T value;
    };

public:
    // Constructors
    Option() : has_value(false) {}

    explicit Option(const T& val) : has_value(true), value(val) {}

    explicit Option(T&& val) : has_value(true), value(std::move(val)) {}

    // Copy constructor
    Option(const Option& other) : has_value(other.has_value) {
        if (has_value) {
            new(&value) T(other.value);
        }
    }

    // Move constructor
    Option(Option&& other) noexcept : has_value(other.has_value) {
        if (has_value) {
            new(&value) T(std::move(other.value));
            other.has_value = false;
        }
    }

    // Destructor
    ~Option() {
        if (has_value) {
            value.~T();
        }
    }

    // Assignment operators
    Option& operator=(const Option& other) {
        if (this != &other) {
            if (has_value) {
                value.~T();
            }
            has_value = other.has_value;
            if (has_value) {
                new(&value) T(other.value);
            }
        }
        return *this;
    }

    Option& operator=(Option&& other) noexcept {
        if (this != &other) {
            if (has_value) {
                value.~T();
            }
            has_value = other.has_value;
            if (has_value) {
                new(&value) T(std::move(other.value));
                other.has_value = false;
            }
        }
        return *this;
    }

    // Static methods to create Some and None
    static Option Some(const T& val) { return Option(val); }
    static Option None() { return Option(); }

    // Check if contains value
    bool is_some() const { return has_value; }
    bool is_none() const { return !has_value; }

    // Access value (unsafe - throws if None)
    T& unwrap() {
        if (!has_value) {
            throw std::runtime_error("Called unwrap on None");
        }
        return value;
    }

    const T& unwrap() const {
        if (!has_value) {
            throw std::runtime_error("Called unwrap on None");
        }
        return value;
    }

    // Access with default value
    T unwrap_or(const T& default_value) const {
        return has_value ? value : default_value;
    }

    // Get value or compute default
    template <typename F>
    T unwrap_or_else(F&& func) const {
        return has_value ? value : func();
    }
};

// Helper function to make creation more convenient
template <typename T>
Option<T> some(const T& value) {
    return Option<T>::Some(value);
}

template <typename T>
Option<T> none() {
    return Option<T>::None();
}

// Example usage
#include <iostream>

int main() {
    Option<int> opt1 = some(42);
    Option<int> opt2 = none<int>();

    std::cout << "opt1 has value: " << opt1.is_some() << std::endl;  // 1
    std::cout << "opt2 has value: " << opt2.is_some() << std::endl;  // 0

    std::cout << "opt1 value: " << opt1.unwrap() << std::endl;       // 42
    std::cout << "opt2 or default: " << opt2.unwrap_or(0) << std::endl;  // 0

    // This would throw: opt2.unwrap();

    return 0;
}
