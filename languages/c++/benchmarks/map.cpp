#include <map>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <sys/resource.h>

//
// Parameters of Lorenz attractor.
//
const long K = 100'000'000;
const long RH = 100;
const long A = 10;
const long B = 28;
const long CN = 8;
const long CD = 3;

//
// Compute N samples of data and store into container.
//
void fill_with_data(const int N, std::map<long, long> &bag)
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
void extract_data(std::map<long, long> &bag)
{
    long long sum = 0;
    while (!bag.empty()) {
        auto x = bag.extract(bag.begin());
        sum += x.key();
    }
    std::cout << "Sum " << ((double) sum/K) << '\n';
}

int main()
{
    // Beginning timestamp.
    auto const t0 = std::chrono::steady_clock::now();

    std::map<long, long> bag;
    fill_with_data(2'000'000, bag);
    extract_data(bag);

    // Get duration in microseconds.
    auto const t1 = std::chrono::steady_clock::now();
    auto const sec = 0.000'001 * std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
    std::cout << "Elapsed time: " << std::fixed << std::setprecision(3) << sec << " seconds" << std::endl;

    // Get memory usage.
    struct rusage ru;
    getrusage(RUSAGE_SELF, &ru);
    auto const mbytes = ru.ru_maxrss / 1024.0 / 1024.0;
    std::cout << "Resident memory: " << std::setprecision(3) << mbytes << " Mbytes" << std::endl;
}
