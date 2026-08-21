// task_main.cpp — drives the co_await demo.

#include <iostream>

#include "compute.hpp"

int main()
{
    std::cout << "Calling compute() (nothing runs yet — Tasks are lazy)\n";
    Task<int> job = compute();

    std::cout << "Running the task...\n";
    int answer = job.get();   // resumes compute(), which co_awaits its way to the result

    std::cout << "compute() = " << answer << "\n";
    return 0;
}
