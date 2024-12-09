#include <map>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <sys/resource.h>

//
// Container type to test.
//
using Container = std::map<int32_t, int32_t>;

//
// How many records to create.
//
const long N = 2'000'000;

//
// Parameters of Lorenz attractor.
//
const long long K = 100'000'000;
const long RH = 100;
const long A = 10;
const long B = 28;
const long CN = 8;
const long CD = 3;

//
// Compute N samples of data and store into container.
//
void fill_with_data(Container &bag)
{
    // Compute sequence of Lorenz attractor.
    long long x = K / 10;
    long long y = 0;
    long long z = 0;
    for (int i = 0; i < N; i++) {
        auto x_next = x + A * (y - x) / RH;
        auto y_next = y + (x * (B*K - z) - y*K) / RH / K;
        auto z_next = z + (x * y - CN * z * K / CD) / RH / K;
        x = x_next;
        y = y_next;
        z = z_next;
        //std::cout << i << ' ' << ((double) x/K) << ' ' << ((double)y/K) << ' ' << ((double)z/K)) << '\n';

        // Put data into container.
        bag[x] = y;
    }
    std::cout << "Last " << ((double) x/K) << ' ' << ((double)y/K) << ' ' << ((double)z/K) << '\n';
}

//
// Extract data from container, one by one.
//
void extract_data(Container &bag)
{
    long long sum = 0;
    while (!bag.empty()) {
        auto x = bag.extract(bag.begin());
        sum += x.key();
    }
    std::cout << "Sum " << ((double) sum/K) << '\n';
}

//
// Get resident memory size in bytes.
//
double get_memory_usage()
{
    struct rusage ru;
    getrusage(RUSAGE_SELF, &ru);
#ifdef __APPLE__
    return ru.ru_maxrss;
#else
    return ru.ru_maxrss * 1024.0;
#endif
}

int main()
{
    // Beginning timestamp.
    auto const t0 = std::chrono::steady_clock::now();

    Container bag;
    fill_with_data(bag);
    extract_data(bag);

    // Get duration in microseconds.
    auto const t1 = std::chrono::steady_clock::now();
    auto const usec = 1e-3 * std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
    auto const sec = 1e-6 * usec;
    auto const bytes = get_memory_usage();

    std::cout << "Elapsed time: " << std::fixed << std::setprecision(3) << sec << " seconds" << std::endl;
    std::cout << "Resident memory: " << (bytes * 1e-6) << " Mbytes" << std::endl;

    std::cout << "Time per record: " << (usec / N) << " microseconds" << std::endl;
    std::cout << "Memory per record: " << (bytes / N) << " bytes" << std::endl;
}
