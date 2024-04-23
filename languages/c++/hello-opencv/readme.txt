Hello OpenCV
************

A simple C++ project to illustrate starting off on a OpenCV project.
I use CMake since that simplifies the compilation process.

Install
*******

Make sure you have the necessary packages installed to be able
to build and run a OpenCV project using CMake.

These commands should do the necessary:

$ sudo apt-get update
$ sudo apt-get install g++ libopencv-dev cmake

Build
*****

CMake reads the CMakeLists.txt we have in the project and
creates a Makefile that can be used to build the project.

We first create a directory to store build files:

$ mkdir build

Run CMake from the build directory:

$ cd build
$ cmake ..

To build the project:

$ make

Run
***

Go to the bin directory and try:

$ ./hello-opencv

You must be able to see a funny picture in a window.

***
