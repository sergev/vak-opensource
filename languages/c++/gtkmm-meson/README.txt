Prerequisites for Ubuntu:

    sudo apt install meson ninja-build libgtkmm-3-dev

Prerequisites for Mac OS:

    brew install meson ninja gtkmm3

Build:

    meson setup build
    cd build
    ninja

Run:

    ./demo
