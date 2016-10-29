
#include <jni.h>
#include <android/bitmap.h>
#include <android/log.h>

#include <string>
#include <cstring>
#include <sstream>
#include <vector>

/*
 * needed for loadProgram function
 */
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>


#include <sys/time.h>

#include <CL/cl.h>


// Commonly-defined shortcuts for LogCat output from native C applications.
#define  LOG_TAG    "AndroidBasic"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)


/* Container for all OpenCL-specific objects used in the sample.
 *
 * The container consists of the following parts:
 *   - Regular OpenCL objects, used in almost each
 *     OpenCL application.
 *   - Specific OpenCL objects - buffers, used in this
 *     particular sample.
 *
 * For convenience, collect all objects in one structure.
 * Avoid global variables and make easier the process of passing
 * all arguments in functions.
 */
struct OpenCLObjects
{
    // Regular OpenCL objects:
    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;
    cl_program program;
    cl_kernel kernel;

    // Objects that are specific for this sample.
    bool isInputBufferInitialized;
    cl_mem inputBuffer;
    cl_mem outputBuffer;
};

// Hold all OpenCL objects.
OpenCLObjects openCLObjects;

/*
 * Load the program out of the file in to a string for opencl compiling.
 */
inline std::string loadProgram(std::string input)
{
	std::ifstream stream(input.c_str());
	if (!stream.is_open()) {
		LOGE("Cannot open input file\n");
		exit(1);
	}
	return std::string( std::istreambuf_iterator<char>(stream),
						(std::istreambuf_iterator<char>()));
}

/* This function helps to create informative messages in
 * case when OpenCL errors occur. The function returns a string
 * representation for an OpenCL error code.
 * For example, "CL_DEVICE_NOT_FOUND" instead of "-1".
 */
const char* opencl_error_to_str (cl_int error)
{
#define CASE_CL_CONSTANT(NAME) case NAME: return #NAME;

    // Suppose that no combinations are possible.
    switch(error)
    {
        CASE_CL_CONSTANT(CL_SUCCESS)
        CASE_CL_CONSTANT(CL_DEVICE_NOT_FOUND)
        CASE_CL_CONSTANT(CL_DEVICE_NOT_AVAILABLE)
        CASE_CL_CONSTANT(CL_COMPILER_NOT_AVAILABLE)
        CASE_CL_CONSTANT(CL_MEM_OBJECT_ALLOCATION_FAILURE)
        CASE_CL_CONSTANT(CL_OUT_OF_RESOURCES)
        CASE_CL_CONSTANT(CL_OUT_OF_HOST_MEMORY)
        CASE_CL_CONSTANT(CL_PROFILING_INFO_NOT_AVAILABLE)
        CASE_CL_CONSTANT(CL_MEM_COPY_OVERLAP)
        CASE_CL_CONSTANT(CL_IMAGE_FORMAT_MISMATCH)
        CASE_CL_CONSTANT(CL_IMAGE_FORMAT_NOT_SUPPORTED)
        CASE_CL_CONSTANT(CL_BUILD_PROGRAM_FAILURE)
        CASE_CL_CONSTANT(CL_MAP_FAILURE)
        CASE_CL_CONSTANT(CL_MISALIGNED_SUB_BUFFER_OFFSET)
        CASE_CL_CONSTANT(CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST)
        CASE_CL_CONSTANT(CL_INVALID_VALUE)
        CASE_CL_CONSTANT(CL_INVALID_DEVICE_TYPE)
        CASE_CL_CONSTANT(CL_INVALID_PLATFORM)
        CASE_CL_CONSTANT(CL_INVALID_DEVICE)
        CASE_CL_CONSTANT(CL_INVALID_CONTEXT)
        CASE_CL_CONSTANT(CL_INVALID_QUEUE_PROPERTIES)
        CASE_CL_CONSTANT(CL_INVALID_COMMAND_QUEUE)
        CASE_CL_CONSTANT(CL_INVALID_HOST_PTR)
        CASE_CL_CONSTANT(CL_INVALID_MEM_OBJECT)
        CASE_CL_CONSTANT(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR)
        CASE_CL_CONSTANT(CL_INVALID_IMAGE_SIZE)
        CASE_CL_CONSTANT(CL_INVALID_SAMPLER)
        CASE_CL_CONSTANT(CL_INVALID_BINARY)
        CASE_CL_CONSTANT(CL_INVALID_BUILD_OPTIONS)
        CASE_CL_CONSTANT(CL_INVALID_PROGRAM)
        CASE_CL_CONSTANT(CL_INVALID_PROGRAM_EXECUTABLE)
        CASE_CL_CONSTANT(CL_INVALID_KERNEL_NAME)
        CASE_CL_CONSTANT(CL_INVALID_KERNEL_DEFINITION)
        CASE_CL_CONSTANT(CL_INVALID_KERNEL)
        CASE_CL_CONSTANT(CL_INVALID_ARG_INDEX)
        CASE_CL_CONSTANT(CL_INVALID_ARG_VALUE)
        CASE_CL_CONSTANT(CL_INVALID_ARG_SIZE)
        CASE_CL_CONSTANT(CL_INVALID_KERNEL_ARGS)
        CASE_CL_CONSTANT(CL_INVALID_WORK_DIMENSION)
        CASE_CL_CONSTANT(CL_INVALID_WORK_GROUP_SIZE)
        CASE_CL_CONSTANT(CL_INVALID_WORK_ITEM_SIZE)
        CASE_CL_CONSTANT(CL_INVALID_GLOBAL_OFFSET)
        CASE_CL_CONSTANT(CL_INVALID_EVENT_WAIT_LIST)
        CASE_CL_CONSTANT(CL_INVALID_EVENT)
        CASE_CL_CONSTANT(CL_INVALID_OPERATION)
        CASE_CL_CONSTANT(CL_INVALID_GL_OBJECT)
        CASE_CL_CONSTANT(CL_INVALID_BUFFER_SIZE)
        CASE_CL_CONSTANT(CL_INVALID_MIP_LEVEL)
        CASE_CL_CONSTANT(CL_INVALID_GLOBAL_WORK_SIZE)
        CASE_CL_CONSTANT(CL_INVALID_PROPERTY)

    default:
        return "UNKNOWN ERROR CODE";
    }

#undef CASE_CL_CONSTANT
}


/* The following macro is used after each OpenCL call
 * to check if OpenCL error occurs. In the case when ERR != CL_SUCCESS
 * the macro forms an error message with OpenCL error code mnemonic,
 * puts it to LogCat, and returns from a caller function.
 *
 * The approach helps to implement consistent error handling tactics
 * because it is important to catch OpenCL errors as soon as
 * possible to avoid missing the origin of the problem.
 *
 * You may chose a different way to do that. The macro is
 * simple and context-specific as it assumes you use it in a function
 * that doesn't have a return value, so it just returns in the end.
 */
#define SAMPLE_CHECK_ERRORS(ERR)                                                      \
    if(ERR != CL_SUCCESS)                                                             \
    {                                                                                 \
        LOGE                                                                          \
        (                                                                             \
            "OpenCL error with code %s happened in file %s at line %d. Exiting.\n",   \
            opencl_error_to_str(ERR), __FILE__, __LINE__                              \
        );                                                                            \
                                                                                      \
        return;                                                                       \
    }


void initOpenCL
(
    JNIEnv* env,
    jobject thisObject,
    jstring kernelName,
    cl_device_type required_device_type,
    OpenCLObjects& openCLObjects
)
{
    /*
     * This function picks and creates all necessary OpenCL objects
     * to be used at each filter iteration. The objects are:
     * OpenCL platform, device, context, command queue, program,
     * and kernel.
     *
     * Almost all of these steps need to be performed in all
	 * OpenCL applications before the actual compute kernel calls
     * are performed.
     *
     * For convenience, in this application all basic OpenCL objects
     * are stored in the OpenCLObjects structure,
     * so, this function populates fields of this structure,
     * which is passed as parameter openCLObjects.
     * Consider reviewing the fields before going further.
     * The structure definition is in the beginning of this file.
     */

    using namespace std;

    // Will be used at each effect iteration,
    // and means that you haven't yet initialized
    // the inputBuffer object.
    openCLObjects.isInputBufferInitialized = false;

    // Search for the Intel OpenCL platform.
    // Platform name includes "Intel" as a substring, consider this
    // method to be a recommendation for Intel OpenCL platform search.
    const char* required_platform_subname = "PowerVR";

    // The following variable stores return codes for all OpenCL calls.
    // In the code it is used with the SAMPLE_CHECK_ERRORS macro defined
    // before this function.
    cl_int err = CL_SUCCESS;

    /* -----------------------------------------------------------------------
     * Step 1: Query for all available OpenCL platforms on the system.
     * Enumerate all platforms and pick one which name has
     * required_platform_subname as a sub-string.
     */

    cl_uint num_of_platforms = 0;
    // Get total number of the available platforms.
    err = clGetPlatformIDs(0, 0, &num_of_platforms);
    SAMPLE_CHECK_ERRORS(err);
    LOGD("Number of available platforms: %u", num_of_platforms);

    vector<cl_platform_id> platforms(num_of_platforms);
    // Get IDs for all platforms.
    err = clGetPlatformIDs(num_of_platforms, &platforms[0], 0);
    SAMPLE_CHECK_ERRORS(err);

    //cl_uint selected_platform_index = num_of_platforms;

    LOGD("Platform names:");

    cl_uint i = 0;
        // Get the length for the i-th platform name.
        size_t platform_name_length = 0;
        err = clGetPlatformInfo(
            platforms[i],
            CL_PLATFORM_NAME,
            0,
            0,
            &platform_name_length
        );
        SAMPLE_CHECK_ERRORS(err);

        // Get the name itself for the i-th platform.
        vector<char> platform_name(platform_name_length);
        err = clGetPlatformInfo(
            platforms[i],
            CL_PLATFORM_NAME,
            platform_name_length,
            &platform_name[0],
            0
        );
        SAMPLE_CHECK_ERRORS(err);

    //selected_platform_index = 0;
    openCLObjects.platform = platforms[0];


    /* -----------------------------------------------------------------------
     * Step 2: Create context with a device of the specified type.
     * Required device type is passed as function argument required_device_type.
     * Use this function to create context for any CPU or GPU OpenCL device.
     */

    cl_context_properties context_props[] = {
        CL_CONTEXT_PLATFORM,
        cl_context_properties(openCLObjects.platform),
        0
    };

    openCLObjects.context =
        clCreateContextFromType
        (
            context_props,
            required_device_type,
            0,
            0,
            &err
        );
    SAMPLE_CHECK_ERRORS(err);

    /* -----------------------------------------------------------------------
     * Step 3: Query for OpenCL device that was used for context creation.
     */

    err = clGetContextInfo
    (
        openCLObjects.context,
        CL_CONTEXT_DEVICES,
        sizeof(openCLObjects.device),
        &openCLObjects.device,
        0
    );
    SAMPLE_CHECK_ERRORS(err);

    /* -----------------------------------------------------------------------
     * Step 4: Create OpenCL program from its source code.
     * The file name is passed bij java.
     * Convert the jstring to const char* and append the needed directory path.
     */
    const char* fileName = env->GetStringUTFChars(kernelName, 0);
    std::string fileDir;
    fileDir.append("/data/data/com.example.inversefilter/app_execdir/");
    fileDir.append(fileName);
    fileDir.append(".cl");
    std::string kernelSource = loadProgram(fileDir);
    //std::string to const char* needed for the clCreateProgramWithSource function
    const char* kernelSourceChar = kernelSource.c_str();

    openCLObjects.program =
        clCreateProgramWithSource
        (
            openCLObjects.context,
            1,
            &kernelSourceChar,
            0,
            &err
        );

    SAMPLE_CHECK_ERRORS(err);

    /* -----------------------------------------------------------------------
     * Step 5: Build the program.
     * During creation a program is not built. Call the build function explicitly.
     * This example utilizes the create-build sequence, still other options are applicable,
     * for example, when a program consists of several parts, some of which are libraries.
     * Consider using clCompileProgram and clLinkProgram as alternatives.
     * Also consider looking into a dedicated chapter in the OpenCL specification
     * for more information on applicable alternatives and options.
     */

    err = clBuildProgram(openCLObjects.program, 0, 0, 0, 0, 0);

    if(err == CL_BUILD_PROGRAM_FAILURE)
    {
        size_t log_length = 0;
        err = clGetProgramBuildInfo(
            openCLObjects.program,
            openCLObjects.device,
            CL_PROGRAM_BUILD_LOG,
            0,
            0,
            &log_length
        );
        SAMPLE_CHECK_ERRORS(err);

        vector<char> log(log_length);

        err = clGetProgramBuildInfo(
            openCLObjects.program,
            openCLObjects.device,
            CL_PROGRAM_BUILD_LOG,
            log_length,
            &log[0],
            0
        );
        SAMPLE_CHECK_ERRORS(err);

        LOGE
        (
            "Error happened during the build of OpenCL program.\nBuild log:%s",
            &log[0]
        );

        return;
    }

    /* -----------------------------------------------------------------------
     * Step 6: Extract kernel from the built program.
     * An OpenCL program consists of kernels. Each kernel can be called (enqueued) from
     * the host part of an application.
     * First create a kernel to call it from the existing program.
     * Creating a kernel via clCreateKernel is similar to obtaining an entry point of a specific function
     * in an OpenCL program.
     */

    openCLObjects.kernel = clCreateKernel(openCLObjects.program, "inverseKernel", &err);
    SAMPLE_CHECK_ERRORS(err);

    /* -----------------------------------------------------------------------
     * Step 7: Create command queue.
     * OpenCL kernels are enqueued for execution to a particular device through
     * special objects called command queues. Command queue provides ordering
     * of calls and other OpenCL commands.
     * This sample uses a simple in-order OpenCL command queue that doesn't
     * enable execution of two kernels in parallel on a target device.
     */

    openCLObjects.queue =
        clCreateCommandQueue
        (
            openCLObjects.context,
            openCLObjects.device,
            0,    // Creating queue properties, refer to the OpenCL specification for details.
            &err
        );
    SAMPLE_CHECK_ERRORS(err);

    // -----------------------------------------------------------------------

    LOGD("initOpenCL finished successfully");
}


extern "C" void Java_com_example_inversefilter_MainActivity_initOpenCL
(
    JNIEnv* env,
    jobject thisObject,
    jstring kernelName
)
{
    initOpenCL
    (
        env,
        thisObject,
        kernelName,
        CL_DEVICE_TYPE_GPU,
        openCLObjects
    );
}


void shutdownOpenCL (OpenCLObjects& openCLObjects)
{
    /* Release all OpenCL objects.
     * This is a regular sequence of calls to deallocate
     * all created OpenCL resources in bootstrapOpenCL.
     *
     * You can call these deallocation procedures in the middle
     * of your application execution (not at the end) if you don't
     * need OpenCL runtime any more.
     * Use deallocation, for example, to free memory or recreate
     * OpenCL objects with different parameters.
     *
     * Calling deallocation in the end of application
     * execution might be not so useful, as upon killing
     * an application, which is a common thing in the Android OS,
     * all OpenCL resources are deallocated automatically.
     */

    cl_int err = CL_SUCCESS;

    if(openCLObjects.isInputBufferInitialized)
    {
        err = clReleaseMemObject(openCLObjects.inputBuffer);
        SAMPLE_CHECK_ERRORS(err);
    }

    err = clReleaseKernel(openCLObjects.kernel);
    SAMPLE_CHECK_ERRORS(err);

    err = clReleaseProgram(openCLObjects.program);
    SAMPLE_CHECK_ERRORS(err);

    err = clReleaseCommandQueue(openCLObjects.queue);
    SAMPLE_CHECK_ERRORS(err);

    err = clReleaseContext(openCLObjects.context);
    SAMPLE_CHECK_ERRORS(err);

    /* There is no procedure to deallocate OpenCL devices or
     * platforms as both are not created at the startup,
     * but queried from the OpenCL runtime.
     */
}


extern "C" void Java_com_example_inversefilter_MainActivity_shutdownOpenCL
(
    JNIEnv* env,
    jobject thisObject
)
{
    shutdownOpenCL(openCLObjects);
    LOGD("shutdownOpenCL(openCLObjects) was called");
}


/*
 * Effect step.
 * This function is called each time you need to process the image.
 *
 * The function consists of the following parts:
 *   - reading input image content if changed
 *   - running OpenCL kernel on the input image
 *   - reading results of image processing
 */
void nativeInverseOpenCL
(
    JNIEnv* env,
    jobject thisObject,
    OpenCLObjects& openCLObjects,
    jobject inputBitmap,
    jobject outputBitmap
)
{
    using namespace std;

    /* Using basic native Android SDK routines,
     * you query dimensions of an input image
     * and store it in bitmapInfo. This is not OpenCL-specific.
     */
    AndroidBitmapInfo bitmapInfo;
    AndroidBitmap_getInfo(env, inputBitmap, &bitmapInfo);

    /* Calculate the buffer size in bytes, which is enough to
     * hold entire input or output image.
     */
    size_t bufferSize = bitmapInfo.height * bitmapInfo.stride;

    /* Also calculate row pith _in_pixels_ for easy access
     * of each pixel inside of an OpenCL kernel.
     */
    cl_uint rowPitch = bitmapInfo.stride / 4;

    /* The following variables define inner and outer
     * radius for circle that is drawn on the image.
     */

    cl_int err = CL_SUCCESS;

    // In this application the input buffer is allocated and initialized on demand
    // with the first call or when the Java side requests an update of the input image.

        if(openCLObjects.isInputBufferInitialized)
        {
            /* If this is not the first time, you need to deallocate the previously
             * allocated buffer as the new buffer will be allocated in
             * the next statements.
             *
             * It is important to remember that unlike Java, there is no
             * garbage collector for OpenCL objects, so deallocate all resources
             * explicitly to avoid running out of memory.
             *
             * It is especially important in case of image buffers,
             * because they are relatively large and even one lost buffer
             * can significantly limit free resources for the application.
             */

            err = clReleaseMemObject(openCLObjects.inputBuffer);
            SAMPLE_CHECK_ERRORS(err);
        }

        LOGD("Creating input buffer in OpenCL");

        /* You receive input buffer as a Java bitmap.
         * To use it in the native part, lock it and obtain a native pointer
         * to the area with pixels.
         */
        void* inputPixels = 0;
        AndroidBitmap_lockPixels(env, inputBitmap, &inputPixels);

        /* Create a new OpenCL buffer object and copy the input image
         * content to it.
         *
         * As the kernel uses this buffer for reading only,
         * create the buffer with the CL_MEM_READ_ONLY flag.
         * Always set minimal read/write flags for buffers, as using the flags
         * enables the runtime to organize data copying better, which
         * might provide better performance.
         *
         * Use CL_MEM_COPY_HOST_PTR here, because the buffer
         * should be populated with bytes at inputPixels.
         */

        openCLObjects.inputBuffer =
            clCreateBuffer
            (
                openCLObjects.context,
                CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                bufferSize,   // Buffer size in bytes.
                inputPixels,  // Bytes for initialization.
                &err
            );
        SAMPLE_CHECK_ERRORS(err);

        openCLObjects.isInputBufferInitialized = true;

        // Do not forget to unlock pixels in the bitmap object.
        AndroidBitmap_unlockPixels(env, inputBitmap);


    /* Output buffer is created with using the CL_MEM_USE_HOST_PTR
     * flag and providing pointer to locked pixels in the output
     * image.
     *
     * CL_MEM_USE_HOST_PTR indicates that the application requests
     * the OpenCL implementation to use the memory that is referenced
     * by outputPixels as the storage bits for the memory object.
     *
     * OpenCL implementations are allowed to cache buffer contents pointed
     * to by host_ptr in device memory. This cached copy can be used when
     * kernels are executed on a device.
     */
    void* outputPixels = 0;
    AndroidBitmap_lockPixels(env, outputBitmap, &outputPixels);

    cl_mem outputBuffer =
        clCreateBuffer
        (
            openCLObjects.context,
            CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR,
            bufferSize,    // Buffer size in bytes, same as the input buffer.
            outputPixels,  // Area, above which the buffer is created.
            &err
        );
    SAMPLE_CHECK_ERRORS(err);

    /* Passing arguments into OpenCL kernel.
     *
     * Consider comparing the sequence of clSetKernelArg calls
     * with the order of arguments in stepKernel OpenCL kernel defined
     * in assets/step.cl.
     */

    err = clSetKernelArg(openCLObjects.kernel, 0, sizeof(openCLObjects.inputBuffer), &openCLObjects.inputBuffer);
    SAMPLE_CHECK_ERRORS(err);

    err = clSetKernelArg(openCLObjects.kernel, 1, sizeof(outputBuffer), &outputBuffer);
    SAMPLE_CHECK_ERRORS(err);

    err = clSetKernelArg(openCLObjects.kernel, 2, sizeof(cl_uint), &rowPitch);
    SAMPLE_CHECK_ERRORS(err);


    /* Define the global iteration space for clEnqueueNDRangeKernel.
     * Every work-item running a kernel processes a single pixel
     * in the image. The global iteration space (globalSize)
     * is two-dimensional and exactly matches the image dimensions.
     *
     * To learn more about the OpenCL NDRange and find details on how
     * kernels are executed on OpenCL devices, refer to the
     * OpenCL specification, chapter 3.2 Execution Model.
     */
    size_t globalSize[2] = { bitmapInfo.width, bitmapInfo.height };

    /* Measure the OpenCL kernel execution time to define
     * the performance improvement you gain using OpenCL in your
     * application.
     *
     * To obtain the kernel execution time, measure the time spent
     * starting with the call to clEnqueueNDRangeKernel and ending
     * with the call to clFinish.
     *
     * clFinish is necessary to measure the entire time spent in the
     * kernel, measuring just clEnqueueNDRangeKernel is not enough,
     * as this call doesn't guarantee that kernel is finished.
     * clEnqueueNDRangeKernel enqueues a new command in the OpenCL
     * command queue and doesn't wait until it ends. clFinish waits
     * until all commands in command queue are finished, which is
     * exactly needed to measure the kernel execution time.
     * gettimeofday gives enough resolution and suits well here.
     */

    timeval start;
    timeval end;

    gettimeofday(&start, NULL);

    err =
        clEnqueueNDRangeKernel
        (
            openCLObjects.queue,
            openCLObjects.kernel,
            2,
            0,
            globalSize,
            0,
            0, 0, 0
        );
    SAMPLE_CHECK_ERRORS(err);

    err = clFinish(openCLObjects.queue);
    SAMPLE_CHECK_ERRORS(err);

    gettimeofday(&end, NULL);

    /* The start and end timestamps are obtained, now calculate
     * the elapsed time interval in seconds and print it out in
     * LogCat.
     */

    float ndrangeDuration =
        (end.tv_sec + end.tv_usec * 1e-6) - (start.tv_sec + start.tv_usec * 1e-6);

    LOGD("NDRangeKernel time: %f", ndrangeDuration);

    /* The last part of this function: getting processed results back.
     * As you used the CL_MEM_USE_HOST_PTR flag while creating the output buffer,
     * use the map-unmap sequence to update the original memory area with output
     * image.
     */
    err = clEnqueueReadBuffer (openCLObjects.queue,
    		outputBuffer,
    		true,
    		0,
    		bufferSize,
    		outputPixels,
    		0,
    		0,
    		0);
    SAMPLE_CHECK_ERRORS(err);

    // Call clFinish to guarantee that the output region is updated.
    err = clFinish(openCLObjects.queue);
    SAMPLE_CHECK_ERRORS(err);

    err = clReleaseMemObject(outputBuffer);
    SAMPLE_CHECK_ERRORS(err);

    // Make the output content be visible at the Java side by unlocking
    // pixels in the output bitmap object.
    AndroidBitmap_unlockPixels(env, outputBitmap);

    LOGD("nativeInverseOpenCL ends successfully");
}

extern "C" void Java_com_example_inversefilter_MainActivity_nativeInverseOpenCL
(
    JNIEnv* env,
    jobject thisObject,
    jobject inputBitmap,
    jobject outputBitmap
)
{
    nativeInverseOpenCL
    (
        env,
        thisObject,
        openCLObjects,
        inputBitmap,
        outputBitmap
    );
}
