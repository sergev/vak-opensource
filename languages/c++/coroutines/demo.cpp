//
// Demo of coroutines, based on article: https://www.scs.stanford.edu/~dm/blog/c++-coroutines.html
// Use g++ 10.3 to compile.
//
#include <iostream>
#include <coroutine>

//
// Return type for coroutines.
// With this return type, on the first call of the coroutine function,
// a handle is returned (with void value), right before the body of the function starts.
//
class co_void_t {
public:
    // This structure defines the behavior of the coroutine:
    // (1) suspend initially right before executing the target routine;
    // (2) suspend when done, let caller destroy the handle.
    struct promise_type {
        co_void_t get_return_object() { return { *this }; }
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void unhandled_exception() {}
        void return_void() noexcept {}
    };

    // Constructor: extract the coroutine handle from promise.
    co_void_t(promise_type &promise)
        : handle(std::coroutine_handle<promise_type>::from_promise(promise))
    {
    }

    // Return the handle, cast to void value.
    operator std::coroutine_handle<>() const { return handle; }

private:
    // Store the coroutine handle here.
    std::coroutine_handle<promise_type> handle;
};

//
// Info for co_await, to switch from coroutine back to sim.run().
//
struct co_await_t {
    constexpr bool await_ready() const noexcept { return false; }
    void await_suspend(std::coroutine_handle<> handle) {}
    constexpr void await_resume() const noexcept {}
};

//
// Coroutine: print next number on each invocation.
//
co_void_t counter()
{
    for (int i = 1; ; ++i) {
        std::cout << "counter: " << i << std::endl;
        co_await co_await_t{};
    }
}

//
// Start coroutine and continue it for a few times.
//
int main()
{
    std::coroutine_handle<> continuation = counter();
    for (int i = 0; i < 5; ++i) {
        std::cout << "In main function\n";
        continuation.resume();
    }
    continuation.destroy();
}
