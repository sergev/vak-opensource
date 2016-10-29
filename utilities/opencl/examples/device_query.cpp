#include <iostream>
#include <stdlib.h>

#define __NO_STD_VECTOR // Use cl::vector instead of STL version
#define __CL_ENABLE_EXCEPTIONS

#ifdef __APPLE__
//#include <OpenCL/opencl.h>
#include <CL/cl.hpp> /* read cpp_wrapper_fix.txt */
#else
#include <CL/cl.hpp>
#endif


int main(int, char**) {
    cl::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);

    int platform_id = 0;
    int device_id = 0;

    std::cout << "Number of Platforms: " << platforms.size() << std::endl;

    for(cl::vector<cl::Platform>::iterator it = platforms.begin(); it != platforms.end(); ++it){
        cl::Platform platform(*it);

        std::cout << "Platform ID: " << platform_id++ << std::endl;
        std::cout << "Platform Name: " << platform.getInfo<CL_PLATFORM_NAME>() << std::endl;
        std::cout << "Platform Vendor: " << platform.getInfo<CL_PLATFORM_VENDOR>() << std::endl;

        cl::vector<cl::Device> devices;
        platform.getDevices(CL_DEVICE_TYPE_GPU | CL_DEVICE_TYPE_CPU, &devices);

        for(cl::vector<cl::Device>::iterator it2 = devices.begin(); it2 != devices.end(); ++it2){
            cl::Device device(*it2);

            std::cout << "\tDevice " << device_id++ << ": " << std::endl;
            std::cout << "\t\tDevice Name: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
            std::cout << "\t\tDevice Type: " << device.getInfo<CL_DEVICE_TYPE>();
            std::cout << " (GPU: " << CL_DEVICE_TYPE_GPU << ", CPU: " << CL_DEVICE_TYPE_CPU << ")" << std::endl;
            std::cout << "\t\tDevice Vendor: " << device.getInfo<CL_DEVICE_VENDOR>() << std::endl;
            std::cout << "\t\tDevice Max Compute Units: " << device.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>() << std::endl;
            std::cout << "\t\tDevice Global Memory: " << device.getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>() << std::endl;
            std::cout << "\t\tDevice Max Clock Frequency: " << device.getInfo<CL_DEVICE_MAX_CLOCK_FREQUENCY>() << std::endl;
            std::cout << "\t\tDevice Max Allocateable Memory: " << device.getInfo<CL_DEVICE_MAX_MEM_ALLOC_SIZE>() << std::endl;
            std::cout << "\t\tDevice Local Memory: " << device.getInfo<CL_DEVICE_LOCAL_MEM_SIZE>() << std::endl;
            std::cout << "\t\tDevice Available: " << device.getInfo< CL_DEVICE_AVAILABLE>() << std::endl;
        }
        std::cout<< std::endl;
    }
}
