// generator.hpp — a minimal C++20 coroutine "generator".
//
// C++20 gives us the coroutine *keywords* (co_yield, co_await, co_return) and
// the <coroutine> plumbing, but it does NOT ship a ready-made generator type
// (that arrives as std::generator in C++23). So we write our own tiny one here.
//
// A Generator<T> is the object that a coroutine returns. It owns a handle to
// the suspended coroutine and knows how to resume it to pull out the next value.

#pragma once

#include <coroutine>
#include <exception>
#include <iterator>
#include <utility>

template <typename T>
class Generator {
public:
    // The compiler looks for a nested type named `promise_type`. It is the
    // "control panel" of the coroutine: the compiler calls its methods at
    // well-defined points to decide what happens when the coroutine starts,
    // yields a value, finishes, or throws.
    struct promise_type {
        T current_value;  // the value handed over by the most recent co_yield

        // Called first: builds the object that the coroutine returns to its
        // caller (our Generator), wrapping the handle for this coroutine.
        Generator get_return_object() {
            return Generator{
                std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        // suspend_always here means "pause immediately, before running any of
        // the coroutine body". This makes the generator *lazy*: nothing runs
        // until the caller asks for the first value.
        std::suspend_always initial_suspend() noexcept { return {}; }

        // Pause (instead of destroying) when the coroutine reaches its end, so
        // the caller can still observe that it is done() before we clean up.
        std::suspend_always final_suspend() noexcept { return {}; }

        // Called by `co_yield value;`. We stash the value and suspend, handing
        // control back to the caller.
        std::suspend_always yield_value(T value) noexcept {
            current_value = std::move(value);
            return {};
        }

        // Our coroutine just runs off the end (no `co_return expr;`).
        void return_void() noexcept {}

        // If the coroutine body throws, we surface it to the caller.
        void unhandled_exception() { std::terminate(); }
    };

    using handle_type = std::coroutine_handle<promise_type>;

    explicit Generator(handle_type h) : handle_(h) {}

    // A coroutine handle is a unique resource, so the Generator is move-only.
    Generator(const Generator&) = delete;
    Generator& operator=(const Generator&) = delete;

    Generator(Generator&& other) noexcept
        : handle_(std::exchange(other.handle_, {})) {}

    Generator& operator=(Generator&& other) noexcept {
        if (this != &other) {
            if (handle_) handle_.destroy();
            handle_ = std::exchange(other.handle_, {});
        }
        return *this;
    }

    // RAII: destroying the Generator destroys the underlying coroutine frame.
    ~Generator() {
        if (handle_) handle_.destroy();
    }

    // --- Range-for support -------------------------------------------------
    //
    // These let you write:  for (auto v : fib(1000)) { ... }
    // The iterator resumes the coroutine on each ++ and reads the value the
    // coroutine yielded.

    class iterator {
    public:
        explicit iterator(handle_type h) : handle_(h) {}

        // Advance: resume the coroutine until its next co_yield (or its end).
        iterator& operator++() {
            handle_.resume();
            if (handle_.done()) handle_ = nullptr;  // becomes the end sentinel
            return *this;
        }

        const T& operator*() const { return handle_.promise().current_value; }

        // We only ever compare against end(); equal when both are exhausted.
        bool operator!=(std::default_sentinel_t) const {
            return handle_ && !handle_.done();
        }

    private:
        handle_type handle_;
    };

    iterator begin() {
        if (handle_) {
            handle_.resume();  // run up to the first co_yield
            if (handle_.done()) return iterator{nullptr};
        }
        return iterator{handle_};
    }

    std::default_sentinel_t end() { return {}; }

private:
    handle_type handle_;
};
