// task.hpp — a minimal C++20 awaitable "Task<T>" for the co_await demo.
//
// Where Generator<T> (see generator.hpp) streams many values OUT of one
// coroutine via co_yield, a Task<T> represents a single coroutine that
// eventually produces one result via co_return. The interesting part is that
// one Task can `co_await` another Task: it pauses, lets the other run, and
// picks up the returned value when the other finishes.
//
// Two small helper types make co_await work:
//   * an "awaiter"       — returned by `co_await task`; it decides whether to
//                          pause, what to do while paused, and what value the
//                          co_await expression evaluates to.
//   * a "final_awaiter"  — runs when a task finishes, so it can wake up
//                          whoever was waiting on it (its "continuation").

#pragma once

#include <coroutine>
#include <exception>
#include <utility>

template <typename T>
class Task {
public:
    struct promise_type {
        T result;                              // filled in by co_return
        std::coroutine_handle<> continuation{};  // who to resume when we finish

        Task get_return_object() {
            return Task{
                std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        // Lazy: don't run the body until someone awaits (or get()s) us.
        std::suspend_always initial_suspend() noexcept { return {}; }

        // When the task finishes, hand control to our awaiter (the coroutine
        // that was waiting for our result). If nobody is waiting (we are the
        // top-level task), fall back to noop_coroutine, which simply returns
        // control to whoever called resume().
        struct final_awaiter {
            bool await_ready() noexcept { return false; }

            std::coroutine_handle<> await_suspend(
                std::coroutine_handle<promise_type> me) noexcept {
                auto cont = me.promise().continuation;
                return cont ? cont : std::noop_coroutine();
            }

            void await_resume() noexcept {}
        };

        final_awaiter final_suspend() noexcept { return {}; }

        void return_value(T value) { result = std::move(value); }
        void unhandled_exception() { std::terminate(); }
    };

    using handle_type = std::coroutine_handle<promise_type>;

    explicit Task(handle_type h) : handle_(h) {}

    // Move-only: a coroutine handle is a unique resource.
    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;

    Task(Task&& other) noexcept : handle_(std::exchange(other.handle_, {})) {}

    Task& operator=(Task&& other) noexcept {
        if (this != &other) {
            if (handle_) handle_.destroy();
            handle_ = std::exchange(other.handle_, {});
        }
        return *this;
    }

    ~Task() {
        if (handle_) handle_.destroy();
    }

    // This is what makes a Task awaitable. `co_await someTask` uses it.
    struct awaiter {
        handle_type coro;  // the task being awaited

        // Never "already ready" — we always want to run the awaited task.
        bool await_ready() noexcept { return false; }

        // `awaiting` is the coroutine doing the co_await. We remember it as the
        // awaited task's continuation, then return the awaited task's handle:
        // returning a handle here is "symmetric transfer" — it immediately
        // resumes that coroutine (starts the awaited task running).
        std::coroutine_handle<> await_suspend(
            std::coroutine_handle<> awaiting) noexcept {
            coro.promise().continuation = awaiting;
            return coro;
        }

        // Once the awaited task has finished, this produces the value that the
        // `co_await` expression evaluates to.
        T await_resume() { return std::move(coro.promise().result); }
    };

    awaiter operator co_await() && { return awaiter{handle_}; }

    // Top-level entry point for non-coroutine code (e.g. main). Runs the whole
    // chain to completion synchronously and returns the final result.
    T get() {
        handle_.resume();
        return std::move(handle_.promise().result);
    }

private:
    handle_type handle_;
};
