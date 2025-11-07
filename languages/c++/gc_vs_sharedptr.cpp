//
// Here is a concise, reproducible microbenchmark in C++17 that directly
// compares std::shared_ptr vs Boehm–Demers–Weiser GC (libgc).
// It measures simple allocation/deallocation throughput for short-lived objects -
// the kind of test where both systems’ overhead is visible and comparable.
//
// Goal: allocate a million small objects (each with a few ints), link them in
// a vector, and then release them. We measure wall-clock time for two versions:
//  * Using std::shared_ptr<Foo>
//  * Using GC_MALLOC and plain pointers (Boehm GC manages memory)
//
// We run each test multiple times for stability and print results in milliseconds.
//
// Build:
//  g++ -std=c++17 -O2 `pkgconf --cflags bdw-gc` gc_vs_sharedptr.cpp -o gc_vs_sharedptr `pkgconf --libs bdw-gc`
//
#include <iostream>
#include <memory>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <gc/gc.h>

struct Foo {
    int x, y, z;
    Foo(int a, int b, int c) : x(a), y(b), z(c) {}
};

static constexpr size_t N = 1'000'000;

template <typename F>
double timeit(F&& func) {
    auto t0 = std::chrono::high_resolution_clock::now();
    func();
    auto t1 = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(t1 - t0).count();
}

void test_shared_ptr() {
    std::vector<std::shared_ptr<Foo>> v;
    v.reserve(N);
    for (size_t i = 0; i < N; ++i)
        v.emplace_back(std::make_shared<Foo>(i, i + 1, i + 2));
    // drop all references (objects destroyed deterministically)
    v.clear();
}

void test_boehm_gc() {
    std::vector<Foo*> v;
    v.reserve(N);
    for (size_t i = 0; i < N; ++i) {
        Foo* p = static_cast<Foo*>(GC_MALLOC(sizeof(Foo)));
        new (p) Foo(i, i + 1, i + 2); // placement new
        v.push_back(p);
    }
    v.clear(); // GC will reclaim later
    GC_gcollect(); // force collection
}

int main() {
    GC_INIT();

    std::cout << "Benchmark: " << N << " objects\n";

    // warm-up run
    test_shared_ptr();
    test_boehm_gc();

    constexpr int REPEATS = 5;
    double total_sp = 0, total_gc = 0;

    for (int i = 0; i < REPEATS; ++i) {
        total_sp += timeit(test_shared_ptr);
        total_gc += timeit(test_boehm_gc);
    }

    std::cout << "shared_ptr avg time: " << (total_sp / REPEATS) << " ms\n";
    std::cout << "Boehm GC avg time:  " << (total_gc / REPEATS) << " ms\n";
    return 0;
}
