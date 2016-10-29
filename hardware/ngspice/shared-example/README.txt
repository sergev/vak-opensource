Source code and project files for a console master program to call ngspice.so.

    main.c          - compiles to console app that links to ngspice.so
                      dynamically during runtime

    sharedspice.h   - include file for the ngspice.so API

To build ngspice.so:

    (1) Download ngspice sources from https://sourceforge.net/projects/ngspice/

    (2) Configure and build:

            cd ngspice
            ./autogen.sh
            mkdir build
            cd build
            ../configure --with-ngshared CFLAGS="-O3"
            make

    (3) Result is ngspice/build/src/.libs/libngspice.so.0.0.0
