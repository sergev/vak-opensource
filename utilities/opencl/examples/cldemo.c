/*
 *  Simple OpenCL demo program
 *
 *  Copyright (C) 2009  Clifford Wolf <clifford@clifford.at>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  gcc -o cldemo -std=gnu99 -Wall -I/usr/include/nvidia-current cldemo.c -lOpenCL
 *
 */

#include <CL/cl.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NUM_DATA 100

#define CL_CHECK(_expr)                                                         \
   do {                                                                         \
     cl_int _err = _expr;                                                       \
     if (_err == CL_SUCCESS)                                                    \
       break;                                                                   \
     fprintf(stderr, "OpenCL Error: '%s' returned %d!\n", #_expr, (int)_err);   \
     abort();                                                                   \
   } while (0)

#define CL_CHECK_ERR(_expr)                                                     \
   ({                                                                           \
     cl_int _err = CL_INVALID_VALUE;                                            \
     typeof(_expr) _ret = _expr;                                                \
     if (_err != CL_SUCCESS) {                                                  \
       fprintf(stderr, "OpenCL Error: '%s' returned %d!\n", #_expr, (int)_err); \
       abort();                                                                 \
     }                                                                          \
     _ret;                                                                      \
   })

void pfn_notify(const char *errinfo, const void *private_info, size_t cb, void *user_data)
{
	fprintf(stderr, "OpenCL Error (via pfn_notify): %s\n", errinfo);
}

int main(int argc, char **argv)
{
	cl_platform_id platforms[100];
	cl_uint platforms_n = 0;
	CL_CHECK(clGetPlatformIDs(100, platforms, &platforms_n));

	printf("=== %d OpenCL platform(s) found: ===\n", platforms_n);
	for (int i=0; i<platforms_n; i++)
	{
		char buffer[10240];
		printf("  -- %d --\n", i);
		CL_CHECK(clGetPlatformInfo(platforms[i], CL_PLATFORM_PROFILE, 10240, buffer, NULL));
		printf("  PROFILE = %s\n", buffer);
		CL_CHECK(clGetPlatformInfo(platforms[i], CL_PLATFORM_VERSION, 10240, buffer, NULL));
		printf("  VERSION = %s\n", buffer);
		CL_CHECK(clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, 10240, buffer, NULL));
		printf("  NAME = %s\n", buffer);
		CL_CHECK(clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, 10240, buffer, NULL));
		printf("  VENDOR = %s\n", buffer);
		CL_CHECK(clGetPlatformInfo(platforms[i], CL_PLATFORM_EXTENSIONS, 10240, buffer, NULL));
		printf("  EXTENSIONS = %s\n", buffer);
	}

	if (platforms_n == 0)
		return 1;

	cl_device_id devices[100];
	cl_uint devices_n = 0;
	// CL_CHECK(clGetDeviceIDs(NULL, CL_DEVICE_TYPE_ALL, 100, devices, &devices_n));
	CL_CHECK(clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, 100, devices, &devices_n));

	printf("=== %d OpenCL device(s) found on platform:\n", platforms_n);
	for (int i=0; i<devices_n; i++)
	{
		char buffer[10240];
		cl_uint buf_uint;
		cl_ulong buf_ulong;
		printf("  -- %d --\n", i);
		CL_CHECK(clGetDeviceInfo(devices[i], CL_DEVICE_NAME, sizeof(buffer), buffer, NULL));
		printf("  DEVICE_NAME = %s\n", buffer);
		CL_CHECK(clGetDeviceInfo(devices[i], CL_DEVICE_VENDOR, sizeof(buffer), buffer, NULL));
		printf("  DEVICE_VENDOR = %s\n", buffer);
		CL_CHECK(clGetDeviceInfo(devices[i], CL_DEVICE_VERSION, sizeof(buffer), buffer, NULL));
		printf("  DEVICE_VERSION = %s\n", buffer);
		CL_CHECK(clGetDeviceInfo(devices[i], CL_DRIVER_VERSION, sizeof(buffer), buffer, NULL));
		printf("  DRIVER_VERSION = %s\n", buffer);
		CL_CHECK(clGetDeviceInfo(devices[i], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(buf_uint), &buf_uint, NULL));
		printf("  DEVICE_MAX_COMPUTE_UNITS = %u\n", (unsigned int)buf_uint);
		CL_CHECK(clGetDeviceInfo(devices[i], CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(buf_uint), &buf_uint, NULL));
		printf("  DEVICE_MAX_CLOCK_FREQUENCY = %u\n", (unsigned int)buf_uint);
		CL_CHECK(clGetDeviceInfo(devices[i], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(buf_ulong), &buf_ulong, NULL));
		printf("  DEVICE_GLOBAL_MEM_SIZE = %llu\n", (unsigned long long)buf_ulong);
	}

	if (devices_n == 0)
		return 1;
printf ("=1=\n");
	cl_context context;
	context = CL_CHECK_ERR(clCreateContext(NULL, 1, devices, &pfn_notify, NULL, &_err));
printf ("=2=\n");

	const char *program_source[] = {
		"__kernel void simple_demo(__global int *src, __global int *dst, int factor)\n",
		"{\n",
		"	int i = get_global_id(0);\n",
		"	dst[i] = src[i] * factor;\n",
		"}\n"
	};

	cl_program program;
	program = CL_CHECK_ERR(clCreateProgramWithSource(context, sizeof(program_source)/sizeof(*program_source), program_source, NULL, &_err));
printf ("=3=\n");
	if (clBuildProgram(program, 1, devices, "", NULL, NULL) != CL_SUCCESS) {
		char buffer[10240];
printf ("=3a=\n");
		clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, NULL);
		fprintf(stderr, "CL Compilation failed:\n%s", buffer);
		abort();
	}
printf ("=4=\n");
	CL_CHECK(clUnloadCompiler());
printf ("=5=\n");

	cl_mem input_buffer;
	input_buffer = CL_CHECK_ERR(clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(int)*NUM_DATA, NULL, &_err));
printf ("=6=\n");

	cl_mem output_buffer;
	output_buffer = CL_CHECK_ERR(clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(int)*NUM_DATA, NULL, &_err));
printf ("=7=\n");

	int factor = 2;

	cl_kernel kernel;
	kernel = CL_CHECK_ERR(clCreateKernel(program, "simple_demo", &_err));
printf ("=8=\n");
	CL_CHECK(clSetKernelArg(kernel, 0, sizeof(input_buffer), &input_buffer));
printf ("=9=\n");
	CL_CHECK(clSetKernelArg(kernel, 1, sizeof(output_buffer), &output_buffer));
printf ("=10=\n");
	CL_CHECK(clSetKernelArg(kernel, 2, sizeof(factor), &factor));
printf ("=11=\n");

	cl_command_queue queue;
	queue = CL_CHECK_ERR(clCreateCommandQueue(context, devices[0], 0, &_err));
printf ("=12=\n");

	for (int i=0; i<NUM_DATA; i++) {
		CL_CHECK(clEnqueueWriteBuffer(queue, input_buffer, CL_TRUE, i*sizeof(int), sizeof(int), &i, 0, NULL, NULL));
printf ("=13=\n");
	}

	cl_event kernel_completion;
	size_t global_work_size[1] = { NUM_DATA };
	CL_CHECK(clEnqueueNDRangeKernel(queue, kernel, 1, NULL, global_work_size, NULL, 0, NULL, &kernel_completion));
printf ("=14=\n");
	CL_CHECK(clWaitForEvents(1, &kernel_completion));
printf ("=15=\n");
	CL_CHECK(clReleaseEvent(kernel_completion));
printf ("=16=\n");

	printf("Result:");
	for (int i=0; i<NUM_DATA; i++) {
		int data;
		CL_CHECK(clEnqueueReadBuffer(queue, output_buffer, CL_TRUE, i*sizeof(int), sizeof(int), &data, 0, NULL, NULL));
		printf(" %d", data);
	}
	printf("\n");

	CL_CHECK(clReleaseMemObject(input_buffer));
printf ("=17=\n");
	CL_CHECK(clReleaseMemObject(output_buffer));
printf ("=18=\n");

	CL_CHECK(clReleaseKernel(kernel));
printf ("=19=\n");
	CL_CHECK(clReleaseProgram(program));
printf ("=20=\n");
	CL_CHECK(clReleaseContext(context));
printf ("=21=\n");

	return 0;
}

#if 0
size_t strlen(const char *s)
{
    size_t n = 0;
    const char *s0 = s;

    printf ("%p \n", s0);
    if ((int)s == 1)
        return 0;
    while (*s++)
        n++;
    printf ("'%s' -> %zu \n", s0, n);
    return n;
}
#endif
