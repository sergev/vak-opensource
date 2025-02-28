//
// Let’s define a reusable Result class with a Rust-like interface,
// using std::variant under the hood. This adds convenience methods
// like is_ok(), is_err(), unwrap(), and unwrap_err().
//
#include <variant>
#include <string>
#include <stdexcept>
#include <iostream>

template<typename T, typename E>
class Result {
public:
    Result(T value) : data(value) {}
    Result(E error) : data(error) {}

    bool is_ok() const { return std::holds_alternative<T>(data); }
    bool is_err() const { return std::holds_alternative<E>(data); }

    T unwrap() const {
        if (is_err()) throw std::runtime_error("Called unwrap() on an Err value");
        return std::get<T>(data);
    }

    E unwrap_err() const {
        if (is_ok()) throw std::runtime_error("Called unwrap_err() on an Ok value");
        return std::get<E>(data);
    }

private:
    std::variant<T, E> data;
};

// Specialize for empty value and std::string
struct Void {};
using VoidResult = Result<Void, std::string>;

VoidResult do_something(bool fail = false) {
    if (fail) {
        return std::string("Something went wrong");
    }
    return Void{};
}

int main() {
    // Test success case
    auto success = do_something(false);
    if (success.is_ok()) {
        std::cout << "Success! Unwrapped: Void\n";
        success.unwrap(); // Works fine, returns Void
    }

    // Test failure case
    auto failure = do_something(true);
    if (failure.is_err()) {
        std::cout << "Error: " << failure.unwrap_err() << "\n";
    }

    // Test unwrap() on failure (will throw)
    try {
        failure.unwrap();
    } catch (const std::runtime_error& e) {
        std::cout << "Caught: " << e.what() << "\n";
    }

    // Test unwrap_err() on success (will throw)
    try {
        success.unwrap_err();
    } catch (const std::runtime_error& e) {
        std::cout << "Caught: " << e.what() << "\n";
    }

    return 0;
}
