Here you can find a demo of Meson build script in a 'Hello World' style.
A test is also included.

    hello.c             -- application sources
    meson.build         -- build script for application
    tests/meson.build   -- build script for tests
    tests/simple        -- test example

Build:

    meson setup build
    meson compile -C build

Run tests:

    meson test -C build
