Prerequisites for Ubuntu:

    sudo apt install meson ninja-build libgtk-3-dev

Prerequisites for Mac OS:

    brew install meson ninja gtk+3

Build:

    meson setup build
    cd build
    ninja

Run:

    ./demo
