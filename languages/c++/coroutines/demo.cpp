//
// Demo of coroutines, based on article: https://www.scs.stanford.edu/~dm/blog/c++-coroutines.html
// Use g++ 10.3 to compile.
//
#include <iostream>
#include <coroutine>

struct ReturnObject {
    struct promise_type {
        ReturnObject get_return_object()
        {
            return {};
        }

        std::suspend_never initial_suspend()
        {
            return {};
        }

        std::suspend_never final_suspend() noexcept
        {
            return {};
        }

        void unhandled_exception() {}

        void return_void() noexcept {}
    };
};

struct Awaiter {
    std::coroutine_handle<> *handle_ptr;

    constexpr bool await_ready() const noexcept
    {
        return false;
    }

    void await_suspend(std::coroutine_handle<> h)
    {
        *handle_ptr = h;
        //std::cout << "handle: " << h.address() << std::endl;
    }

    constexpr void await_resume() const noexcept
    {
        // Empty.
    }
};

ReturnObject counter(std::coroutine_handle<> *continuation_out)
{
    Awaiter a{ continuation_out };
    for (unsigned i = 0;; ++i) {
        co_await a;
        std::cout << "counter: " << i << std::endl;
    }
}

int main()
{
    std::coroutine_handle<> counter_continuation;
    counter(&counter_continuation);
    for (int i = 0; i < 3; ++i) {
        std::cout << "In main function\n";
        counter_continuation();
    }
    counter_continuation.destroy();
}
